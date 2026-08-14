#pragma once

#include <array>
#include <cstdint>

namespace SHiP {

/// Simulation hit from Geant4 sensitive detectors
struct SimHit {
  std::int32_t detectorId{0};
  std::int32_t geometryNodeId{0};
  std::int32_t trackId{0};
  std::int32_t pdgCode{0};
  std::array<double, 3> position{0, 0, 0};  ///< Hit position [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum at hit [GeV/c]
  double energyDeposit{0};                  ///< Energy deposited [GeV]
  double time{0};                           ///< Global time [ns]
  double pathLength{0};                     ///< Step length [mm]

  bool operator==(SimHit const&) const = default;
};

}  // namespace SHiP
