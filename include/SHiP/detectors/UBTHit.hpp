#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the UBT (Upstream Background Tagger)
struct UBTHit {
  RecHit rec_hit;  ///< The reconstructed hit
};

}  // namespace SHiP
