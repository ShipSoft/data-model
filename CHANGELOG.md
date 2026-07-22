# Changelog

All notable changes to this project will be documented in this file.

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

- List RecParticle in README data classes table
- Link Doxygen API reference from README
- Add CONTRIBUTING with prek hook setup
- Align SPDX guidance with aggregate REUSE metadata

### Styling

- Reorder RecParticle.hpp includes to satisfy cpplint
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

### Styling

- Apply clang-format, gersemi and pre-commit fixes

### Miscellaneous

- Record formatting commit in git-blame-ignore-revs
- Add git-cliff configuration for changelog generation
- Add release automation via git-cliff
