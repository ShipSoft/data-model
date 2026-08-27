#pragma once
#include "Rtypes.h"
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by the calorimeter
struct CaloHit {
  RecHit rec_hit;  ///< The reconstructed hit

  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(CaloHit, 2);
};

}  // namespace SHiP
