#pragma once

#include <vector>

#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"

namespace SHiP {

/// Bundled result from one Geant4 event
struct SimResult {
  std::vector<SimHit> hits;
  std::vector<SimParticle> particles;

  bool operator==(SimResult const&) const = default;
};

}  // namespace SHiP
