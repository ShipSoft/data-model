#pragma once
#include "Rtypes.h"
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the UBT (Upstream Background Tagger)
struct UBTHit {
  RecHit rec_hit;  ///< The reconstructed hit

  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(UBTHit, 2);
};

}  // namespace SHiP
