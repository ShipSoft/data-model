#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the SBT (Surround Background Tagger)
struct SBTHit {
  RecHit recHit;  ///< The reconstructed hit
};

}  // namespace SHiP
