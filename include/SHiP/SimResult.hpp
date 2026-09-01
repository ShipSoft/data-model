#pragma once

#include <vector>

#include "Rtypes.h"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"

namespace SHiP {

/// Bundled result from one Geant4 event
struct SimResult {
  std::vector<SimHit> hits;
  std::vector<SimParticle> particles;

  bool operator==(SimResult const&) const = default;
  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(SimResult, 2);
};

}  // namespace SHiP
