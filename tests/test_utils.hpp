#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

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
    p.pdgCode = 11 + 100 * i + offset;
    p.vertex = {1.5 + i + offset, -2.25 + i, 3.75 + i};
    p.momentum = {0.125 + i, -0.25 + i, 40.5 + i + offset};
    p.energy = 40.625 + i + offset;
    p.time = 0.375 + i;
    p.motherId = i - 1;
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
    h.detectorId = 1000 + 10 * i + offset;
    h.trackId = 42 + i + offset;
    h.pdgCode = -13 + 2 * i;
    h.position = {10.5 + i + offset, -20.25 + i, 3000.75 + i};
    h.momentum = {1.125 + i, -2.25 + i, 30.5 + i + offset};
    h.energyDeposit = 0.0625 + i + offset;
    h.time = 25.375 + i;
    h.pathLength = 0.5 + i + offset;
    v.push_back(h);
  }
  return v;
}

/// Build SimParticles with distinctive non-default values in every member.
inline std::vector<SimParticle> makeSimParticles(int offset) {
  std::vector<SimParticle> v;
  for (int i = 0; i < 3; ++i) {
    SimParticle p;
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

/// Build RecParticles with distinctive non-default values in every member.
inline std::vector<RecParticle> makeRecParticles(int offset) {
  std::vector<RecParticle> v;
  for (auto const& sp : makeSimParticles(offset)) {
    RecParticle p = fromSimParticle(sp);
    p.ipPV = 0.875 + p.trackId;
    v.push_back(p);
  }
  return v;
}

/// Build a SimResult holding non-trivial hit and particle collections.
inline SimResult makeSimResult(int offset) {
  return {.hits = makeSimHits(offset + 5),
          .particles = makeSimParticles(offset + 5)};
}

inline bool equal(MCParticle const& a, MCParticle const& b) {
  return a.pdgCode == b.pdgCode && a.vertex == b.vertex &&
         a.momentum == b.momentum && a.energy == b.energy && a.time == b.time &&
         a.motherId == b.motherId && a.status == b.status;
}

inline bool equal(SimHit const& a, SimHit const& b) {
  return a.detectorId == b.detectorId && a.trackId == b.trackId &&
         a.pdgCode == b.pdgCode && a.position == b.position &&
         a.momentum == b.momentum && a.energyDeposit == b.energyDeposit &&
         a.time == b.time && a.pathLength == b.pathLength;
}

inline bool equal(SimParticle const& a, SimParticle const& b) {
  return a.trackId == b.trackId && a.parentId == b.parentId &&
         a.pdgCode == b.pdgCode && a.vertex == b.vertex &&
         a.endpoint == b.endpoint && a.momentum == b.momentum &&
         a.energy == b.energy && a.time == b.time &&
         a.creatorProcess == b.creatorProcess;
}

inline bool equal(RecParticle const& a, RecParticle const& b) {
  return a.trackId == b.trackId && a.parentId == b.parentId &&
         a.pdgCode == b.pdgCode && a.vertex == b.vertex &&
         a.endpoint == b.endpoint && a.momentum == b.momentum &&
         a.energy == b.energy && a.time == b.time &&
         a.creatorProcess == b.creatorProcess && a.ipPV == b.ipPV;
}

template <typename T>
bool equal(std::vector<T> const& a, std::vector<T> const& b) {
  if (a.size() != b.size()) {
    return false;
  }
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (!equal(a[i], b[i])) {
      return false;
    }
  }
  return true;
}

inline bool equal(SimResult const& a, SimResult const& b) {
  return equal(a.hits, b.hits) && equal(a.particles, b.particles);
}

/// Compare expected vs. read-back values, reporting PASS/FAIL.
template <typename T>
bool check(std::string const& label, T const& expected, T const& actual) {
  bool const ok = equal(expected, actual);
  std::cout << label << ": " << (ok ? "PASS" : "FAIL") << '\n';
  return ok;
}

}  // namespace SHiP::test
