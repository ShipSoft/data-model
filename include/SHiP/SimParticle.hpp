#pragma once

#include <array>
#include <cstdint>

#include "Rtypes.h"

namespace SHiP {

/// Tracked particle from simulation
struct SimParticle {
  std::int32_t track_id{0};
  std::int32_t parent_id{0};
  std::int32_t pdg_code{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> endpoint{0, 0, 0};  ///< End point [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Initial momentum [GeV/c]
  double energy{0};                         ///< Initial kinetic energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t creator_process{0};

  bool operator==(SimParticle const&) const = default;
  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(SimParticle, 2);
};

}  // namespace SHiP
