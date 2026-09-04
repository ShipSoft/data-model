#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

namespace SHiP {

/// Monte Carlo particle — input to simulation
///
/// Mother indices refer to the *same* collection (this event's vector of
/// MCParticle), not to the generator's own record: a producer that filters
/// the record must remap them. -1 means "no mother".
///
/// An entry can have several mothers — a hadron from string fragmentation,
/// the products of a hard 2 -> n process, two partons joined into one.
/// `mothers` is the complete, resolved list: Pythia8's Particle::motherList()
/// provides exactly it, derived from the raw mother1/mother2 pair *and* the
/// native status, which is what distinguishes an inclusive range of mothers
/// from two distinct ones. `motherId` is its first element, or -1 when the
/// entry has no mother.
///
/// An empty `mothers` means either that the entry has no mothers or that it
/// was written before v0.5.0, when the field did not exist. `motherId` stays
/// the portable single-mother accessor across all versions.
///
/// Taken together, for every entry `i` of a collection of `N` particles:
///   - `motherId` is -1 or in `[0, N)`;
///   - every element of `mothers` is in `[0, N)` — the -1 sentinel belongs to
///     `motherId` alone and never appears in the list;
///   - a non-empty `mothers` starts with `motherId`;
///   - the elements of `mothers` are distinct, and none of them is `i`.
/// mothersAreConsistent() checks exactly this; mothersArePopulated() adds the
/// requirement that current-schema data actually fill the list. Mothers are
/// deliberately *not* required to precede their daughters: a producer that
/// filters a generator record and remaps indices may reorder it.
///
/// `status` follows the HepMC convention — the codes Pythia8's statusHepMC()
/// produces, the same set EDM4hep uses for generatorStatus:
///   - 0: an empty entry, to be skipped unconditionally
///   - 1: final state, i.e. not decayed *by the generator*; these are the
///        entries a detector simulation tracks, and they may well be unstable
///   - 2: a decayed Standard Model hadron, tau or muon
///   - 3: a documentation entry
///   - 4: an incoming beam particle
///   - 11-200: an intermediate entry, generator-dependent classification
struct MCParticle {
  std::int32_t pdgCode{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Momentum (px, py, pz) [GeV/c]
  double energy{0};                         ///< Total energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t motherId{-1};                ///< First mother (-1 = none)
  std::vector<std::int32_t> mothers{};      ///< All mothers (see above)
  std::int32_t status{1};                   ///< HepMC status (see above)

  bool operator==(MCParticle const&) const = default;
};

/// True when the mother indices of a whole collection are self-consistent,
/// i.e. satisfy the four invariants documented above.
///
/// An empty `mothers` is always accepted: it means either that the entry has
/// no mothers, or that the data was written before v0.5.0, when the field did
/// not exist. Use mothersArePopulated() to additionally require the list on
/// current-schema data.
inline bool mothersAreConsistent(std::vector<MCParticle> const& particles) {
  auto const size = static_cast<std::int32_t>(particles.size());
  for (std::int32_t i = 0; i < size; ++i) {
    auto const& mothers = particles[i].mothers;
    if (particles[i].motherId < -1 || particles[i].motherId >= size) {
      return false;
    }
    if (mothers.empty()) {
      continue;
    }
    if (mothers.front() != particles[i].motherId) {
      return false;
    }
    for (auto m = mothers.begin(); m != mothers.end(); ++m) {
      if (*m < 0 || *m >= size || *m == i) {
        return false;
      }
      // Mother lists hold a handful of entries at most, so scanning the
      // prefix already seen beats allocating a set.
      if (std::find(mothers.begin(), m, *m) != m) {
        return false;
      }
    }
  }
  return true;
}

/// True when, on top of mothersAreConsistent(), every entry that has a mother
/// carries the full list — `motherId >= 0` implies a non-empty `mothers`.
///
/// Only current-schema data can satisfy this: files written before v0.5.0 have
/// no `mothers` field at all, so they read back with an empty list next to a
/// perfectly valid `motherId`.
inline bool mothersArePopulated(std::vector<MCParticle> const& particles) {
  for (auto const& p : particles) {
    if (p.motherId >= 0 && p.mothers.empty()) {
      return false;
    }
  }
  return mothersAreConsistent(particles);
}

}  // namespace SHiP
