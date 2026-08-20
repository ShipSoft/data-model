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

namespace SHiP::ref {

constexpr int kEntries = 2;

template <typename Particle = SHiP::MCParticle>
std::vector<Particle> makeMCParticles(int offset) {
  std::vector<Particle> v;
  for (int i = 0; i < 3; ++i) {
    Particle p;
    p.pdg_code = 11 + 100 * i + offset;
    p.vertex = {1.5 + i + offset, -2.25 + i, 3.75 + i};
    p.momentum = {0.125 + i, -0.25 + i, 40.5 + i + offset};
    p.energy = 40.625 + i + offset;
    p.time = 0.375 + i;
    p.mother_id = i - 1;
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
    h.detector_id = 1000 + 10 * i + offset;
    h.track_id = 42 + i + offset;
    h.pdg_code = -13 + 2 * i;
    h.position = {10.5 + i + offset, -20.25 + i, 3000.75 + i};
    h.momentum = {1.125 + i, -2.25 + i, 30.5 + i + offset};
    h.energy_deposit = 0.0625 + i + offset;
    h.time = 25.375 + i;
    h.path_length = 0.5 + i + offset;
    if constexpr (requires { h.geometry_node_id; }) {  // added in v0.4.0
      h.geometry_node_id = 900 + 7 * i + offset;
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
    p.track_id = 7 + i + offset;
    p.parent_id = 6 + i;
    p.pdg_code = 211 - 2 * i + offset;
    p.vertex = {0.5 + i + offset, -1.25 + i, 2.75 + i};
    p.endpoint = {100.5 + i, -200.25 + i + offset, 5000.75 + i};
    p.momentum = {3.125 + i, -4.25 + i, 50.5 + i + offset};
    p.energy = 51.625 + i + offset;
    p.time = 12.375 + i;
    p.creator_process = 2 + i + offset;
    v.push_back(p);
  }
  return v;
}

template <typename Particle = SHiP::RecParticle>
std::vector<Particle> makeRecParticles(int offset) {
  std::vector<Particle> v;
  for (auto const& sp : makeSimParticles(offset)) {
    Particle p;
    p.track_id = sp.track_id;
    p.parent_id = sp.parent_id;
    p.pdg_code = sp.pdg_code;
    p.vertex = sp.vertex;
    p.endpoint = sp.endpoint;
    p.momentum = sp.momentum;
    p.energy = sp.energy;
    p.time = sp.time;
    p.creator_process = sp.creator_process;
    p.ip_pv = 0.875 + sp.track_id;
    if constexpr (requires { p.hits; }) {  // added in v0.3.0
      using HitType = typename decltype(Particle{}.hits)::value_type;
      for (auto const& sh : makeSimHits(offset + 2)) {
        HitType rh;
        rh.detector_id = sh.detector_id;
        rh.track_id = sh.track_id;
        rh.pdg_code = sh.pdg_code;
        rh.position = sh.position;
        rh.momentum = sh.momentum;
        rh.energy_deposit = sh.energy_deposit;
        rh.time = sh.time;
        rh.path_length = sh.path_length;
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

}  // namespace SHiP::ref
