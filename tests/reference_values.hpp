#pragma once

/// Canonical value recipe for the frozen compatibility reference files.
///
/// This header must compile unmodified against the headers of every released
/// tag: headers added in later versions are guarded with __has_include, and
/// members added in later versions are guarded with `if constexpr (requires
/// ...)` inside function templates (a discarded branch of a non-template
/// function would still be type-checked against old headers).
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

constexpr int kEntries = 2;

template <typename Particle = SHiP::MCParticle>
std::vector<Particle> makeMCParticles(int offset) {
  std::vector<Particle> v;
  for (int i = 0; i < 3; ++i) {
    Particle p;
    p.pdgCode = 11 + 100 * i + offset;
    p.vertex = {1.5 + i + offset, -2.25 + i, 3.75 + i};
    p.momentum = {0.125 + i, -0.25 + i, 40.5 + i + offset};
    p.energy = 40.625 + i + offset;
    p.time = 0.375 + i;
    p.motherId = i - 1;
    if constexpr (requires { p.mothers; }) {  // added in v0.5.0
      // Indices refer to this same 3-element collection. Entry 0 has no mother,
      // so its list stays empty and motherId stays -1; later entries lead with
      // motherId and add a second in-range index for the multi-mother case.
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
    h.detectorId = 1000 + 10 * i + offset;
    h.trackId = 42 + i + offset;
    h.pdgCode = -13 + 2 * i;
    h.position = {10.5 + i + offset, -20.25 + i, 3000.75 + i};
    h.momentum = {1.125 + i, -2.25 + i, 30.5 + i + offset};
    h.energyDeposit = 0.0625 + i + offset;
    h.time = 25.375 + i;
    h.pathLength = 0.5 + i + offset;
    if constexpr (requires { h.geometryNodeId; }) {  // added in v0.4.0
      h.geometryNodeId = 900 + 7 * i + offset;
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
    p.trackId = 7 + i + offset;
    p.parentId = 6 + i;
    p.pdgCode = 211 - 2 * i + offset;
    p.vertex = {0.5 + i + offset, -1.25 + i, 2.75 + i};
    p.endpoint = {100.5 + i, -200.25 + i + offset, 5000.75 + i};
    p.momentum = {3.125 + i, -4.25 + i, 50.5 + i + offset};
    p.energy = 51.625 + i + offset;
    p.time = 12.375 + i;
    p.creatorProcess = 2 + i + offset;
    v.push_back(p);
  }
  return v;
}

template <typename Particle = SHiP::RecParticle>
std::vector<Particle> makeRecParticles(int offset) {
  std::vector<Particle> v;
  for (auto const& sp : makeSimParticles(offset)) {
    Particle p;
    p.trackId = sp.trackId;
    p.parentId = sp.parentId;
    p.pdgCode = sp.pdgCode;
    p.vertex = sp.vertex;
    p.endpoint = sp.endpoint;
    p.momentum = sp.momentum;
    p.energy = sp.energy;
    p.time = sp.time;
    p.creatorProcess = sp.creatorProcess;
    p.ipPV = 0.875 + sp.trackId;
    if constexpr (requires { p.hits; }) {  // added in v0.3.0
      using HitType = typename decltype(Particle{}.hits)::value_type;
      for (auto const& sh : makeSimHits(offset + 2)) {
        HitType rh;
        rh.detectorId = sh.detectorId;
        rh.trackId = sh.trackId;
        rh.pdgCode = sh.pdgCode;
        rh.position = sh.position;
        rh.momentum = sh.momentum;
        rh.energyDeposit = sh.energyDeposit;
        rh.time = sh.time;
        rh.pathLength = sh.pathLength;
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
    f.nMeas = 4 + i + offset;
    f.fitStatus = i % 2;
    f.chi2 = 3.5 + i + offset;
    f.ndf = 2 + i;
    f.qoverp = -0.03125 * (1 + i + offset);
    f.phi = 0.25 + i;
    f.theta = 0.0625 + i + offset;
    f.time = 17.375 + i;
    f.refLoc = {1.25 + i + offset, -3.5 + i, 2500.75 + i};
    // The vectors differ in length between members and between elements, so a
    // swapped or truncated vector cannot pass for the right one.
    for (int m = 0; m < 2 + i; ++m) {
      f.inputMeasurementsX.push_back(10.5 + m + offset);
      f.inputMeasurementsY.push_back(-10.25 + m);
      f.fittedMeasurementsX.push_back(10.625 + m + offset);
      f.fittedMeasurementsY.push_back(-10.125 + m);
    }
    for (int m = 0; m < 1 + i; ++m) {
      f.residualsX.push_back(-0.125 + m);
      f.residualsY.push_back(0.0625 * (m + 1) + offset);
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
    w.recHit.detectorId = sh.detectorId;
    w.recHit.trackId = sh.trackId;
    w.recHit.pdgCode = sh.pdgCode;
    w.recHit.position = sh.position;
    w.recHit.momentum = sh.momentum;
    w.recHit.energyDeposit = sh.energyDeposit;
    w.recHit.time = sh.time;
    w.recHit.pathLength = sh.pathLength;
    v.push_back(w);
  }
  return v;
}
#endif

}  // namespace SHiP::ref
