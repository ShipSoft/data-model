#pragma once

#include <array>
#include <cstdint>

#include "SHiP/SimHit.hpp"

namespace SHiP {

/// Reconstructed hit
struct RecHit {
  std::int32_t detectorId{0};
  std::int32_t trackId{0};
  std::int32_t pdgCode{0};
  std::array<double, 3> position{0, 0, 0};  ///< Hit position [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum at hit [GeV/c]
  double energyDeposit{0};                  ///< Energy deposited [GeV]
  double time{0};                           ///< Global time [ns]
  double pathLength{0};                     ///< Step length [mm]
};

inline RecHit fromSimHit(SimHit const& sp) {
  return {.detectorId = sp.detectorId,
          .trackId = sp.trackId,
          .pdgCode = sp.pdgCode,
          .position = sp.position,
          .momentum = sp.momentum,
          .energyDeposit = sp.energyDeposit,
          .time = sp.time,
          .pathLength = sp.pathLength};
}

}  // namespace SHiP
