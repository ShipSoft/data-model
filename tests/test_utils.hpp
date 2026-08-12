#pragma once

#include <concepts>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "SHiP/EventHeader.hpp"
#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/SimResult.hpp"

namespace SHiP::test {

/// Build MCParticles with distinctive non-default values in every member.
inline std::vector<MCParticle> makeMCParticles(int offset) {
  std::vector<MCParticle> v;
  for (int i = 0; i < 3; ++i) {
    MCParticle p;
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

/// Build SimHits with distinctive non-default values in every member.
inline std::vector<SimHit> makeSimHits(int offset) {
  std::vector<SimHit> v;
  for (int i = 0; i < 3; ++i) {
    SimHit h;
    h.detector_id = 1000 + 10 * i + offset;
    h.track_id = 42 + i + offset;
    h.pdg_code = -13 + 2 * i;
    h.position = {10.5 + i + offset, -20.25 + i, 3000.75 + i};
    h.momentum = {1.125 + i, -2.25 + i, 30.5 + i + offset};
    h.energy_deposit = 0.0625 + i + offset;
    h.time = 25.375 + i;
    h.path_length = 0.5 + i + offset;
    v.push_back(h);
  }
  return v;
}

/// Build SimParticles with distinctive non-default values in every member.
inline std::vector<SimParticle> makeSimParticles(int offset) {
  std::vector<SimParticle> v;
  for (int i = 0; i < 3; ++i) {
    SimParticle p;
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

/// Build RecParticles with distinctive non-default values in every member.
inline std::vector<RecParticle> makeRecParticles(int offset) {
  std::vector<RecParticle> v;
  for (auto const& sp : makeSimParticles(offset)) {
    RecParticle p = fromSimParticle(sp);
    p.ip_pv = 0.875 + p.track_id;
    v.push_back(p);
  }
  return v;
}

/// Build a SimResult holding non-trivial hit and particle collections.
inline SimResult makeSimResult(int offset) {
  return {.hits = makeSimHits(offset + 5),
          .particles = makeSimParticles(offset + 5)};
}

/// Default comparison used by check(): any type with operator== works as-is.
/// A type without operator== can still opt in by defining its own
/// equal(T const&, T const&) overload, which is preferred via ADL / overload
/// resolution (a non-template overload beats this constrained template).
template <typename T>
  requires std::equality_comparable<T>
bool equal(T const& a, T const& b) {
  return a == b;
}

/// Compare expected vs. read-back values, reporting PASS/FAIL.
template <typename T>
bool check(std::string const& label, T const& expected, T const& actual) {
  bool const ok = equal(expected, actual);
  std::cout << label << ": " << (ok ? "PASS" : "FAIL") << '\n';
  return ok;
}

}  // namespace SHiP::test
