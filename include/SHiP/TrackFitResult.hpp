#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Rtypes.h"

namespace SHiP {

/// Track fit result
struct TrackFitResult {
  std::int32_t n_meas{0};
  std::int32_t fit_status{1};  // 0 = success, 1 = failure
  double chi2{0};
  std::int32_t ndf{0};
  double q_over_p{0};
  double phi{0};
  double theta{0};
  double time{0};
  std::array<double, 3> ref_loc{0, 0, 0};
  std::vector<double> input_measurements_x{};
  std::vector<double> input_measurements_y{};
  std::vector<double> fitted_measurements_x{};
  std::vector<double> fitted_measurements_y{};
  std::vector<double> residuals_x{};
  std::vector<double> residuals_y{};

  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(TrackFitResult, 2);
};
}  // namespace SHiP
