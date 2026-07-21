#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace SHiP {

/// Track fit result
struct TrackFitResult {
  std::int32_t nMeas{0};
  std::int32_t fitStatus{1};  // 0 = success, 1 = failure
  double chi2{0};
  std::int32_t ndf{0};
  double qoverp{0};
  double phi{0};
  double theta{0};
  double time{0};
  std::array<double, 3> refLoc{0, 0, 0};
  std::vector<double> inputMeasurementsX{};
  std::vector<double> inputMeasurementsY{};
  std::vector<double> fittedMeasurementsX{};
  std::vector<double> fittedMeasurementsY{};
  std::vector<double> residualsX{};
  std::vector<double> residualsY{};
};
}  // namespace SHiP
