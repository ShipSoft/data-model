#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the timing detector
struct TimeDetHit {
  RecHit recHit;  ///< The reconstructed hit
};

}  // namespace SHiP
