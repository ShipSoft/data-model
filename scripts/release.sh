#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) CERN for the benefit of the SHiP Collaboration
#
# Cut a release: bump CMakeLists.txt VERSION, regenerate CHANGELOG.md via
# git-cliff, create a release commit and an annotated tag. Does NOT push.

set -euo pipefail

usage() {
    cat <<'EOF'
Usage: scripts/release.sh <version>

  <version>   semver without leading 'v', e.g. 0.2.0

The script must be run from a clean working tree. It will:
  1. bump the VERSION line in CMakeLists.txt
  2. bump version and date-released in CITATION.cff (if present)
  3. bump the [package] version in pixi.toml (if present)
  4. write the frozen compatibility reference file
     tests/data/reference_v<version>.root, refresh the schema snapshot and
     run the test suite (requires pixi)
  5. regenerate CHANGELOG.md with `git cliff --tag v<version>`
  6. create commit `chore(release): v<version>`
  7. create annotated tag `v<version>`

Pushing is left to the operator:
  git push origin <branch> && git push origin v<version>
EOF
}

if [[ $# -ne 1 ]]; then
    usage >&2
    exit 64
fi

case "$1" in
    -h|--help) usage; exit 0 ;;
esac

VERSION="$1"
TAG="v${VERSION}"

if ! [[ "${VERSION}" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "error: version must match MAJOR.MINOR.PATCH (got: ${VERSION})" >&2
    exit 64
fi

REPO_ROOT="$(git rev-parse --show-toplevel)"
cd "${REPO_ROOT}"

if ! git cliff --version >/dev/null 2>&1; then
    echo "error: 'git cliff' not available; install git-cliff (https://git-cliff.org/)" >&2
    exit 69
fi

if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "error: working tree is dirty; commit or stash changes first" >&2
    exit 65
fi

BRANCH="$(git rev-parse --abbrev-ref HEAD)"
if [[ "${BRANCH}" != "main" ]]; then
    echo "warning: on branch '${BRANCH}', not 'main'" >&2
fi

if git rev-parse --verify --quiet "refs/tags/${TAG}" >/dev/null; then
    echo "error: tag ${TAG} already exists" >&2
    exit 65
fi

CMAKE_FILE="CMakeLists.txt"
# Match the project's three-component VERSION token. Tolerates both styles:
#   project(Foo VERSION 0.1.0 LANGUAGES CXX)               # inline
#   project(Foo                                            # multi-line
#       VERSION 0.1.0 ...
#   )
# `cmake_minimum_required(VERSION 3.21)` is not matched because its version is
# only two-component.
VERSION_RE='(^|[[:space:](])VERSION[[:space:]]+[0-9]+\.[0-9]+\.[0-9]+'
if ! grep -qE "${VERSION_RE}" "${CMAKE_FILE}"; then
    echo "error: could not find VERSION line in ${CMAKE_FILE}" >&2
    exit 70
fi

sed -i -E "s/((^|[[:space:](])VERSION[[:space:]]+)[0-9]+\.[0-9]+\.[0-9]+/\1${VERSION}/" "${CMAKE_FILE}"

if ! grep -qE "(^|[[:space:](])VERSION[[:space:]]+${VERSION//./\\.}([[:space:])]|$)" "${CMAKE_FILE}"; then
    echo "error: failed to update VERSION in ${CMAKE_FILE}" >&2
    git checkout -- "${CMAKE_FILE}"
    exit 70
fi

CITATION_FILE="CITATION.cff"
if [[ -f "${CITATION_FILE}" ]]; then
    sed -i -E "s/^version: .*/version: ${VERSION}/" "${CITATION_FILE}"
    sed -i -E "s/^date-released: .*/date-released: \"$(date -u +%Y-%m-%d)\"/" "${CITATION_FILE}"
fi

# Bump the [package] version in pixi.toml so the source dependency and the
# conda recipe stay in lockstep with the tag. Guarding on a top-level version
# key makes this a clean no-op where pixi.toml has no [package] section.
# Anchored at column 0, it only matches the top-level `version = "X.Y.Z"` key,
# not the inline `version =` fields of [package.build]/host-dependency tables.
PIXI_FILE="pixi.toml"
if [[ -f "${PIXI_FILE}" ]] && grep -qE '^version = "[0-9]+\.[0-9]+\.[0-9]+"' "${PIXI_FILE}"; then
    sed -i -E "s/^version = \"[0-9]+\.[0-9]+\.[0-9]+\"/version = \"${VERSION}\"/" "${PIXI_FILE}"
    if ! grep -qE "^version = \"${VERSION//./\\.}\"$" "${PIXI_FILE}"; then
        echo "error: failed to update version in ${PIXI_FILE}" >&2
        git checkout -- "${CMAKE_FILE}" "${PIXI_FILE}"
        [[ -f "${CITATION_FILE}" ]] && git checkout -- "${CITATION_FILE}"
        exit 70
    fi
fi

# Freeze the compatibility reference file for this release: written from
# exactly the code being tagged, with the ROOT version pinned in pixi.lock at
# this moment — capturing both the schema and the writing ROOT (see
# tests/data/README.md). The snapshot regen should be a no-op if CI was
# green; the test run gates the release on the compat suite, including the
# new file (picked up via the CMake glob).
if ! command -v pixi >/dev/null 2>&1; then
    echo "error: 'pixi' not available; needed to write the compatibility reference file" >&2
    exit 69
fi
REF_FILE="tests/data/reference_${TAG}.root"
if [[ -e "${REF_FILE}" ]]; then
    echo "error: ${REF_FILE} already exists; frozen reference files are never rewritten" >&2
    echo "hint: a file left over from an interrupted release attempt is safe to remove" >&2
    exit 65
fi
pixi run build
# The compat suite is only built when build/ was configured with testing on;
# fail here rather than on a bare "No such file" further down.
if [[ ! -x ./build/tests/write_reference ]]; then
    echo "error: ./build/tests/write_reference missing; reconfigure build/ with BUILD_TESTING=ON" >&2
    exit 70
fi
# Write atomically: never leave a partial file at the frozen path (the
# exists-check above would otherwise block a retry after a failed write).
REF_TMP="${REF_FILE}.tmp"
# The file stays provisional until the snapshot regen and the test suite have
# both passed: on failure remove it, together with any partial temporary, so
# no unvalidated file is left behind and the exists-check above does not block
# a retry. It has to sit at its frozen path while the tests run, because the
# compat suite discovers reference files through the CMake glob.
trap 'rm -f "${REF_TMP}" "${REF_FILE}"' EXIT
pixi run ./build/tests/write_reference "${REF_TMP}"
mv "${REF_TMP}" "${REF_FILE}"
pixi run update-schema-snapshot
pixi run test
trap - EXIT  # validated from here on: keep the frozen file

git cliff --tag "${TAG}" -o CHANGELOG.md

git add "${CMAKE_FILE}" CHANGELOG.md "${REF_FILE}" tests/data/schema_snapshot.txt
if [[ -f "${CITATION_FILE}" ]]; then
    git add "${CITATION_FILE}"
fi
if [[ -f "${PIXI_FILE}" ]] && grep -qE "^version = \"${VERSION//./\\.}\"$" "${PIXI_FILE}"; then
    git add "${PIXI_FILE}"
fi
git commit -m "chore(release): ${TAG}"
git tag -a "${TAG}" -m "Release ${TAG}"

cat <<EOF

Release ${TAG} prepared on branch '${BRANCH}'.

Next steps:
  git push origin ${BRANCH}
  git push origin ${TAG}

(or:  git push --follow-tags origin ${BRANCH})
EOF
