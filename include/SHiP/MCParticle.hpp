#pragma once

#include <array>
#include <cstdint>

#include "Rtypes.h"

namespace SHiP {

/// Monte Carlo particle — input to simulation
struct MCParticle {
  std::int32_t pdg_code{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum (px, py, pz) [GeV/c]
  double energy{0};                         ///< Total energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t mother_id{-1};               ///< Index of mother (-1 = primary)
  std::int32_t status{1};                   ///< Status code (1 = stable)

  bool operator==(MCParticle const&) const = default;
  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(MCParticle, 2);
};

}  // namespace SHiP
