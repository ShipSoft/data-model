#!/usr/bin/env bash
# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) CERN for the benefit of the SHiP Collaboration
#
# One-time backfill of the frozen compatibility reference files for releases
# that predate the compatibility suite (see tests/data/README.md). For each
# tag: build the tag's dictionary library, compile the CURRENT
# tests/write_reference.cpp against the TAG's headers (the old headers define
# the on-disk shape; reference_values.hpp adapts via __has_include and member
# detection), and write tests/data/reference_<tag>.root.
#
# Run from the repo root inside the pixi environment:
#   pixi run bash scripts/backfill_reference_files.sh
#
# Kept for provenance; releases from v0.5.0 on write their reference file at
# release time via scripts/release.sh instead.

set -euo pipefail

REPO="$(git rev-parse --show-toplevel)"
mkdir -p "${REPO}/tests/data"

for tag in v0.1.0 v0.2.0 v0.3.0 v0.4.0; do
    out="${REPO}/tests/data/reference_${tag}.root"
    if [[ -e "${out}" ]]; then
        echo "skip ${tag}: ${out} already exists (frozen files are never rewritten)"
        continue
    fi
    wt="$(mktemp -d)/data-model-${tag}"
    tmp="${out}.tmp"
    # Never leave a partial file at the frozen path (the skip-check above
    # would treat it as frozen) or a registered worktree behind on failure.
    trap 'rm -f "${tmp}"; git worktree remove --force "${wt}" 2>/dev/null || true' EXIT
    git worktree add "${wt}" "${tag}"
    cmake -S "${wt}" -B "${wt}/build" -G Ninja \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=OFF
    cmake --build "${wt}/build" -j
    # Tag headers first on the include path; reference_values.hpp resolves
    # next to write_reference.cpp, i.e. to the current repo's copy.
    "${CXX:-g++}" -std=c++23 -I "${wt}/include" $(root-config --cflags) \
        "${REPO}/tests/write_reference.cpp" -o "${wt}/build/write_reference" \
        $(root-config --libs) -lROOTNTuple \
        -L "${wt}/build" -Wl,--no-as-needed -lSHiPDataModel -Wl,--as-needed \
        -Wl,-rpath,"${wt}/build"
    "${wt}/build/write_reference" "${tmp}"
    mv "${tmp}" "${out}"
    git worktree remove --force "${wt}"
    trap - EXIT
done
