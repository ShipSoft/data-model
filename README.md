# SHiP Data Model

Event data model for the SHiP experiment, providing C++ data classes with ROOT dictionary support.

## Documentation

An [automatic class reference](https://shipsoft.github.io/data-model/) is built using Doxygen from comments in the C++ code.

## Data classes

| Header | Class | Category |
|--------|-------|----------|
| `SHiP/EventHeader.hpp` | `SHiP::EventHeader` | Event metadata |
| `SHiP/MCParticle.hpp` | `SHiP::MCParticle` | MC / generation |
| `SHiP/SimHit.hpp` | `SHiP::SimHit` | Simulation |
| `SHiP/SimParticle.hpp` | `SHiP::SimParticle` | Simulation |
| `SHiP/SimResult.hpp` | `SHiP::SimResult` | Simulation |
| `SHiP/RecParticle.hpp` | `SHiP::RecParticle` | Reconstruction |

All classes are plain structs with default member initialisers, suitable for ROOT I/O via RNTuple or TTree.

## Building

```bash
cmake -B build
cmake --build build
```

## Installing

```bash
cmake --install build --prefix /your/install/prefix
```

Downstream packages consume this via CMake:

```cmake
find_package(SHiPDataModel REQUIRED)
target_link_libraries(your_target PRIVATE SHiP::SHiPDataModel)
```

## Backward compatibility

Files written by released versions must stay readable: CI reads a frozen
reference RNTuple file per release (`tests/data/reference_v*.root`) with the
current code, and a committed schema snapshot fails CI on any schema change
until it is deliberately regenerated. See
[`tests/data/README.md`](tests/data/README.md) for the policy and what to do
when these tests fail.

## Dependencies

- ROOT 6.36+ (Core, RIO)

## Licence

LGPL-3.0-or-later. See `LICENSES/` for the full text.
