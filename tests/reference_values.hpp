#pragma once

/// Canonical value recipe for the frozen compatibility reference files.
///
/// This header must compile unmodified against the headers of every released
/// tag: headers added in later versions are guarded with __has_include, and
/// members added in later versions are guarded with `if constexpr (requires
/// ...)` inside function templates (a discarded branch of a non-template
/// function would still be type-checked against old headers). Members renamed
/// from camelCase to snake_case after v0.5.0 are accessed through the
/// SHiP::ref::field accessors, which pick whichever spelling the headers
/// declare.
///
/// The values are part of the on-disk compatibility contract documented in
/// tests/data/README.md: for members that existed at v0.1.0 they are identical
/// to SHiP::test::make* in test_utils.hpp; members added later get their own
/// distinctive values here so that reference files exercise them.

#include <vector>

#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"
#if __has_include("SHiP/EventHeader.hpp")
#include "SHiP/EventHeader.hpp"
#define SHIP_REF_HAS_EVENT_HEADER 1
#endif
#if __has_include("SHiP/TrackFitResult.hpp")
#include "SHiP/TrackFitResult.hpp"
#define SHIP_REF_HAS_TRACK_FIT_RESULT 1
#endif
#if __has_include("SHiP/detectors/UBTHit.hpp")
#include "SHiP/detectors/UBTHit.hpp"
#define SHIP_REF_HAS_UBT_HIT 1
#endif

namespace SHiP::ref {

namespace field {

// Resolve a member renamed from camelCase to snake_case to whichever spelling
// the headers being compiled against declare. A function template on the
// object, so only the selected branch is instantiated even when the caller's
// object type is not dependent.
#define SHIP_REF_RENAMED(snake, camel)     \
  template <typename T>                    \
  constexpr auto& snake(T& o) {            \
    if constexpr (requires { o.snake; }) { \
      return o.snake;                      \
    } else {                               \
      return o.camel;                      \
    }                                      \
  }
SHIP_REF_RENAMED(pdg_code, pdgCode)
SHIP_REF_RENAMED(mother_id, motherId)
SHIP_REF_RENAMED(detector_id, detectorId)
SHIP_REF_RENAMED(track_id, trackId)
SHIP_REF_RENAMED(energy_deposit, energyDeposit)
SHIP_REF_RENAMED(path_length, pathLength)
SHIP_REF_RENAMED(geometry_node_id, geometryNodeId)
SHIP_REF_RENAMED(parent_id, parentId)
SHIP_REF_RENAMED(creator_process, creatorProcess)
SHIP_REF_RENAMED(ip_pv, ipPV)
SHIP_REF_RENAMED(n_meas, nMeas)
SHIP_REF_RENAMED(fit_status, fitStatus)
SHIP_REF_RENAMED(q_over_p, qoverp)
SHIP_REF_RENAMED(ref_loc, refLoc)
SHIP_REF_RENAMED(input_measurements_x, inputMeasurementsX)
SHIP_REF_RENAMED(input_measurements_y, inputMeasurementsY)
SHIP_REF_RENAMED(fitted_measurements_x, fittedMeasurementsX)
SHIP_REF_RENAMED(fitted_measurements_y, fittedMeasurementsY)
SHIP_REF_RENAMED(residuals_x, residualsX)
SHIP_REF_RENAMED(residuals_y, residualsY)
SHIP_REF_RENAMED(rec_hit, recHit)
#undef SHIP_REF_RENAMED

}  // namespace field

constexpr int kEntries = 2;

template <typename Particle = SHiP::MCParticle>
std::vector<Particle> makeMCParticles(int offset) {
  std::vector<Particle> v;
  for (int i = 0; i < 3; ++i) {
    Particle p;
    field::pdg_code(p) = 11 + 100 * i + offset;
    p.vertex = {1.5 + i + offset, -2.25 + i, 3.75 + i};
    p.momentum = {0.125 + i, -0.25 + i, 40.5 + i + offset};
    p.energy = 40.625 + i + offset;
    p.time = 0.375 + i;
    field::mother_id(p) = i - 1;
    if constexpr (requires { p.mothers; }) {  // added in v0.5.0
      // Indices refer to this same 3-element collection. Entry 0 has no mother,
      // so its list stays empty and mother_id stays -1; later entries lead with
      // mother_id and add a second in-range index for the multi-mother case.
      if (i > 0) {
        p.mothers = {i - 1, (i + 1) % 3};
      }
    }
    p.status = 1 + i + offset;
    v.push_back(p);
  }
  return v;
}

template <typename Hit = SHiP::SimHit>
std::vector<Hit> makeSimHits(int offset) {
  std::vector<Hit> v;
  for (int i = 0; i < 3; ++i) {
    Hit h;
    field::detector_id(h) = 1000 + 10 * i + offset;
    field::track_id(h) = 42 + i + offset;
    field::pdg_code(h) = -13 + 2 * i;
    h.position = {10.5 + i + offset, -20.25 + i, 3000.75 + i};
    h.momentum = {1.125 + i, -2.25 + i, 30.5 + i + offset};
    field::energy_deposit(h) = 0.0625 + i + offset;
    h.time = 25.375 + i;
    field::path_length(h) = 0.5 + i + offset;
    if constexpr (
        requires { h.geometry_node_id; } ||
        requires { h.geometryNodeId; }) {  // added in v0.4.0
      field::geometry_node_id(h) = 900 + 7 * i + offset;
    }
    v.push_back(h);
  }
  return v;
}

template <typename Particle = SHiP::SimParticle>
std::vector<Particle> makeSimParticles(int offset) {
  std::vector<Particle> v;
  for (int i = 0; i < 3; ++i) {
    Particle p;
    field::track_id(p) = 7 + i + offset;
    field::parent_id(p) = 6 + i;
    field::pdg_code(p) = 211 - 2 * i + offset;
    p.vertex = {0.5 + i + offset, -1.25 + i, 2.75 + i};
    p.endpoint = {100.5 + i, -200.25 + i + offset, 5000.75 + i};
    p.momentum = {3.125 + i, -4.25 + i, 50.5 + i + offset};
    p.energy = 51.625 + i + offset;
    p.time = 12.375 + i;
    field::creator_process(p) = 2 + i + offset;
    v.push_back(p);
  }
  return v;
}

template <typename Particle = SHiP::RecParticle>
std::vector<Particle> makeRecParticles(int offset) {
  std::vector<Particle> v;
  for (auto const& sp : makeSimParticles(offset)) {
    Particle p;
    field::track_id(p) = field::track_id(sp);
    field::parent_id(p) = field::parent_id(sp);
    field::pdg_code(p) = field::pdg_code(sp);
    p.vertex = sp.vertex;
    p.endpoint = sp.endpoint;
    p.momentum = sp.momentum;
    p.energy = sp.energy;
    p.time = sp.time;
    field::creator_process(p) = field::creator_process(sp);
    field::ip_pv(p) = 0.875 + field::track_id(sp);
    if constexpr (requires { p.hits; }) {  // added in v0.3.0
      using HitType = typename decltype(Particle{}.hits)::value_type;
      for (auto const& sh : makeSimHits(offset + 2)) {
        HitType rh;
        field::detector_id(rh) = field::detector_id(sh);
        field::track_id(rh) = field::track_id(sh);
        field::pdg_code(rh) = field::pdg_code(sh);
        rh.position = sh.position;
        rh.momentum = sh.momentum;
        field::energy_deposit(rh) = field::energy_deposit(sh);
        rh.time = sh.time;
        field::path_length(rh) = field::path_length(sh);
        p.hits.push_back(rh);
      }
    }
    v.push_back(p);
  }
  return v;
}

template <typename Result = SHiP::SimResult>
Result makeSimResult(int offset) {
  Result r;
  r.hits = makeSimHits(offset + 5);
  r.particles = makeSimParticles(offset + 5);
  return r;
}

#ifdef SHIP_REF_HAS_EVENT_HEADER
template <typename Header = SHiP::EventHeader>
Header makeEventHeader(int entry) {
  Header h;
  h.weight = 0.125 + entry;
  h.original_event_id = 7000 + entry;
  return h;
}
#endif

#ifdef SHIP_REF_HAS_TRACK_FIT_RESULT
template <typename Fit = SHiP::TrackFitResult>
std::vector<Fit> makeTrackFitResults(int offset) {
  std::vector<Fit> v;
  for (int i = 0; i < 3; ++i) {
    Fit f;
    field::n_meas(f) = 4 + i + offset;
    field::fit_status(f) = i % 2;
    f.chi2 = 3.5 + i + offset;
    f.ndf = 2 + i;
    field::q_over_p(f) = -0.03125 * (1 + i + offset);
    f.phi = 0.25 + i;
    f.theta = 0.0625 + i + offset;
    f.time = 17.375 + i;
    field::ref_loc(f) = {1.25 + i + offset, -3.5 + i, 2500.75 + i};
    // The vectors differ in length between members and between elements, so a
    // swapped or truncated vector cannot pass for the right one.
    for (int m = 0; m < 2 + i; ++m) {
      field::input_measurements_x(f).push_back(10.5 + m + offset);
      field::input_measurements_y(f).push_back(-10.25 + m);
      field::fitted_measurements_x(f).push_back(10.625 + m + offset);
      field::fitted_measurements_y(f).push_back(-10.125 + m);
    }
    for (int m = 0; m < 1 + i; ++m) {
      field::residuals_x(f).push_back(-0.125 + m);
      field::residuals_y(f).push_back(0.0625 * (m + 1) + offset);
    }
    v.push_back(f);
  }
  return v;
}
#endif

#ifdef SHIP_REF_HAS_UBT_HIT
template <typename Wrapper = SHiP::UBTHit>
std::vector<Wrapper> makeUBTHits(int offset) {
  std::vector<Wrapper> v;
  for (auto const& sh : makeSimHits(offset + 3)) {
    Wrapper w;
    auto& rh = field::rec_hit(w);
    field::detector_id(rh) = field::detector_id(sh);
    field::track_id(rh) = field::track_id(sh);
    field::pdg_code(rh) = field::pdg_code(sh);
    rh.position = sh.position;
    rh.momentum = sh.momentum;
    field::energy_deposit(rh) = field::energy_deposit(sh);
    rh.time = sh.time;
    field::path_length(rh) = field::path_length(sh);
    v.push_back(w);
  }
  return v;
}
#endif

}  // namespace SHiP::ref
