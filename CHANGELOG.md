# Changelog

All notable changes to this project will be documented in this file.

## [0.6.0] - 2026-10-13

### Bug fixes

- *(release)* Tag the release only after it lands on main
- Keep files written before the field renames readable
- Stop reads of camelCase detector-wrapper data with a Fatal error
- *(tests)* Strip -/! LinkDef suffixes when extracting class names
- *(build)* Put the pstl include directory on the dictionary target

### Refactor

- [**breaking**] Use snake_case for data-model field names
- [**breaking**] Use snake_case for the free functions in the public headers
- *(tests)* Derive dictionary classes without hardcoding the namespace
- [**breaking**] Put the unit vocabulary in the SHiP namespace

### Documentation

- Say which rename rules the compat suite can actually check

### Testing

- Add a frozen v0.5.0 fixture for TrackFitResult and the wrappers
- Cover SimHit::geometry_node_id in round-trip tests
- Compile every public header standalone
- Mark the pre-v0.5.0 compat reads as expected failures
- Keep the reference recipe compiling against camelCase tags

### Miscellaneous

- Update pixi lock file
- Enforce the header naming convention with clang-tidy
## [0.5.0] - 2026-09-29

### Features

- Give all persistent classes an explicit class version
- Record all mothers of an MCParticle
- Check the MCParticle mother-index invariants

### Bug fixes

- *(release)* Roll back on any failure, fail on unbumpable versions
- Let a failed git diff fail the clang-tidy-diff task
- *(tests)* Keep the mothers recipe within its own collection

### Refactor

- *(tests)* Compare via operator== in test::check

### Documentation

- Adopt AI policy
- Document the backward-compatibility policy
- Describe what the clang-tidy gate actually does, and fix the local check

### Testing

- Add version-agnostic reference-file writer
- Freeze reference files for v0.1.0-v0.4.0 and main
- Read frozen reference files with the current model
- Gate schema changes behind a committed snapshot
- Open reference files through TFile before attaching the reader

### Miscellaneous

- Grant config-sync job explicit permissions
- Treat ROOT files as binary in git
- Sync shared configs
- Update pixi lock file
- Gate merges on uniform aggregator checks
- Check C++ against the Core Guidelines with clang-tidy

### Build

- Export C++ standard requirement from installed targets
- Pin BUILD_TESTING=ON in the configure task
- Freeze compatibility artefacts when cutting a release
## [0.4.0] - 2026-08-12

### Features

- Add geometryNodeId to SimHit structure
- *(data-model)* Add EventHeader for per-event weight and provenance (#17)

### Bug fixes

- Add mp-units to package host dependencies

### Miscellaneous

- Update pixi lock file
## [0.3.0] - 2026-07-22

### Features

- Add reconstructed particle

### Bug fixes

- Pin host root_base to a C++23 ROOT build
## [0.2.0] - 2026-07-16

### Features

- *(doxygen)* Add API documentation via Doxygen and GitHub Pages
- *(lint)* Enforce conventional commits with commitizen
- Enable consumption as a pixi source dependency
- Add canonical unit vocabulary via mp-units

### Refactor

- Drop redundant CMAKE_CXX_STANDARD from pixi build args

### Documentation

- Link Doxygen API reference from README
- Add CONTRIBUTING with prek hook setup
- Align SPDX guidance with aggregate REUSE metadata

### Styling

- Reformat CMakeLists.txt with gersemi 0.27.7

### Testing

- Add TTree and RNTuple round-trip tests
- Add SimParticle and RecParticle view round-trips

### Miscellaneous

- Lint with prek via pixi
- Enable Renovate via shared preset
- Build and test via shared pixi workflow
- Add concurrency control and scheduled lock updates
## [0.1.0] - 2026-06-19

### Features

- Add project infrastructure
- Add SHiP event data model with ROOT dictionary
- Add `RecParticle`

### Documentation

- Add data model hierarchy diagrams
- List RecParticle in README data classes table

### Styling

- Apply clang-format, gersemi and pre-commit fixes
- Reorder RecParticle.hpp includes to satisfy cpplint

### Miscellaneous

- Record formatting commit in git-blame-ignore-revs
- Add git-cliff configuration for changelog generation
- Add release automation via git-cliff
