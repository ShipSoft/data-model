#pragma once
#include "Rtypes.h"
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the timing detector
struct TimeDetHit {
  RecHit rec_hit;  ///< The reconstructed hit

  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(TimeDetHit, 2);
};

}  // namespace SHiP
