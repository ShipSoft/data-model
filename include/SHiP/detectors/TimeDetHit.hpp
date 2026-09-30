#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the timing detector
struct TimeDetHit {
  RecHit rec_hit;  ///< The reconstructed hit
};

}  // namespace SHiP
