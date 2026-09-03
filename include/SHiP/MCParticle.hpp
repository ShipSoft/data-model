#pragma once

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

}  // namespace SHiP
