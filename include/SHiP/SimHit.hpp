#pragma once

#include <array>
#include <cstdint>

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
};

}  // namespace SHiP
