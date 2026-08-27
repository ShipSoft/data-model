#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Rtypes.h"
#include "SHiP/RecHit.hpp"
#include "SHiP/SimParticle.hpp"

namespace SHiP {

/// Reconstructed particle
struct RecParticle {
  std::int32_t track_id{0};
  std::int32_t parent_id{0};
  std::int32_t pdg_code{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> endpoint{0, 0, 0};  ///< End point [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Initial momentum [GeV/c]
  std::vector<RecHit> hits{};
  double energy{0};  ///< Initial kinetic energy [GeV]
  double time{0};    ///< Production time [ns]
  std::int32_t creator_process{0};
  double ip_pv{0};  ///< IP wrt to the PV (at 0,0,0) [mm]

  bool operator==(RecParticle const&) const = default;
  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(RecParticle, 2);
};

inline RecParticle fromSimParticle(SimParticle const& sp) {
  return {
      .track_id = sp.track_id,
      .parent_id = sp.parent_id,
      .pdg_code = sp.pdg_code,
      .vertex = sp.vertex,
      .endpoint = sp.endpoint,
      .momentum = sp.momentum,
      .energy = sp.energy,
      .time = sp.time,
      .creator_process = sp.creator_process,
      .ip_pv = 0.0,
  };
}

}  // namespace SHiP
