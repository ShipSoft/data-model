#pragma once

#include <array>
#include <cstdint>

namespace SHiP {

/// Tracked particle from simulation
struct SimParticle {
  std::int32_t trackId{0};
  std::int32_t parentId{0};
  std::int32_t pdgCode{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> endpoint{0, 0, 0};  ///< End point [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Initial momentum [GeV/c]
  double energy{0};                         ///< Initial kinetic energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t creatorProcess{0};

  bool operator==(SimParticle const&) const = default;
};

}  // namespace SHiP
