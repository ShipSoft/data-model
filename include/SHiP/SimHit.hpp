#pragma once

#include <array>
#include <cstdint>

#include "Rtypes.h"

namespace SHiP {

/// Simulation hit from Geant4 sensitive detectors
struct SimHit {
  std::int32_t detector_id{0};
  std::int32_t geometry_node_id{0};
  std::int32_t track_id{0};
  std::int32_t pdg_code{0};
  std::array<double, 3> position{0, 0, 0};  ///< Hit position [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum at hit [GeV/c]
  double energy_deposit{0};                 ///< Energy deposited [GeV]
  double time{0};                           ///< Global time [ns]
  double path_length{0};                    ///< Step length [mm]

  bool operator==(SimHit const&) const = default;

  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(SimHit, 2);
};

}  // namespace SHiP
