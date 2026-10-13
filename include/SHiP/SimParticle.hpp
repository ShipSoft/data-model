#pragma once

#include <array>
#include <cstdint>

namespace SHiP {

/// Tracked particle from simulation
///
/// `weight` is the particle's own weight, on top of EventHeader::weight, as
/// for MCParticle: a primary takes the weight of its MCParticle, a secondary
/// that of the particle it descends from. Data written before the field
/// existed should read back as 1, but reads back as 0 in a std::vector
/// through ROOT 6.40 (see the known issue in LinkDef.h).
struct SimParticle {
  std::int32_t track_id{0};
  std::int32_t parent_id{0};
  std::int32_t pdg_code{0};
  std::array<double, 3> vertex{0, 0, 0};    ///< Production vertex [mm]
  std::array<double, 3> endpoint{0, 0, 0};  ///< End point [mm]
  std::array<double, 3> momentum{0, 0, 0};  ///< Initial momentum [GeV/c]
  double energy{0};                         ///< Initial kinetic energy [GeV]
  double time{0};                           ///< Production time [ns]
  std::int32_t creator_process{0};
  double weight{1.0};  ///< Particle weight, times EventHeader::weight

  bool operator==(SimParticle const&) const = default;
};

}  // namespace SHiP
