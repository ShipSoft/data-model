#pragma once

#include <array>
#include <cstdint>

#include "SHiP/SimHit.hpp"

namespace SHiP {

/// Reconstructed hit
struct RecHit {
  std::int32_t detector_id{0};
  std::int32_t track_id{0};
  std::int32_t pdg_code{0};
  std::array<double, 3> position{0, 0, 0};  ///< Hit position [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum at hit [GeV/c]
  double energy_deposit{0};                 ///< Energy deposited [GeV]
  double time{0};                           ///< Global time [ns]
  double path_length{0};                    ///< Step length [mm]

  bool operator==(RecHit const&) const = default;
};

inline RecHit fromSimHit(SimHit const& sp) {
  return {.detector_id = sp.detector_id,
          .track_id = sp.track_id,
          .pdg_code = sp.pdg_code,
          .position = sp.position,
          .momentum = sp.momentum,
          .energy_deposit = sp.energy_deposit,
          .time = sp.time,
          .path_length = sp.path_length};
}

}  // namespace SHiP
