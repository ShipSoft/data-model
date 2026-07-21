#pragma once
#include "SHiP/RecHit.hpp"

namespace SHiP {

/// Hit reconstructed by a layer of the straw tubes spectrometer
struct StrawTubesHit {
  RecHit recHit;  ///< The reconstructed hit
};

}  // namespace SHiP
