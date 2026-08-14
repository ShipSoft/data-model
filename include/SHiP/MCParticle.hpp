#pragma once

#include <array>
#include <cstdint>

namespace SHiP {

/// Monte Carlo particle — input to simulation
struct MCParticle {
  std::int32_t pdgCode{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum (px, py, pz) [GeV/c]
  double energy{0};                         ///< Total energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t motherId{-1};                ///< Index of mother (-1 = primary)
  std::int32_t status{1};                   ///< Status code (1 = stable)

  bool operator==(MCParticle const&) const = default;
};

}  // namespace SHiP
