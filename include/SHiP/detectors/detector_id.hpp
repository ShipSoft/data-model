#pragma once

#include <cstdint>

namespace SHiP {

enum class detector_id : std::int32_t {
  UpstreamTagger = 0,
  SurroundTagger = 1,
  StrawTubes = 2,
  Calorimeter = 3,
  TimingDetector = 4
};

}  // namespace SHiP
