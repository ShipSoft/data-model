#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the calorimeter
struct CaloHit {
  RecHit recHit;  ///< The reconstructed hit
};

}  // namespace SHiP
