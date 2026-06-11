#pragma once

#include <array>
#include <cstdint>

#include "SHiP/SimParticle.hpp"

namespace SHiP {

/// Reconstructed particle
struct RecParticle {
  std::int32_t trackId{0};
  std::int32_t parentId{0};
  std::int32_t pdgCode{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> endpoint{0, 0, 0};  ///< End point [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Initial momentum [GeV/c]
  double energy{0};                         ///< Initial kinetic energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t creatorProcess{0};
  double ipPV{0};  ///< IP wrt to the PV (at 0,0,0) [mm]
};

inline RecParticle fromSimParticle(SimParticle const& sp) {
  return {
      .trackId = sp.trackId,
      .parentId = sp.parentId,
      .pdgCode = sp.pdgCode,
      .vertex = sp.vertex,
      .endpoint = sp.endpoint,
      .momentum = sp.momentum,
      .energy = sp.energy,
      .time = sp.time,
      .creatorProcess = sp.creatorProcess,
      .ipPV = 0.0,
  };
}

}  // namespace SHiP
