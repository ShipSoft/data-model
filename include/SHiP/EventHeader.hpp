#pragma once

#include <cstdint>

namespace SHiP {

/// Per-event metadata — one record per event
struct EventHeader {
  double weight{1.0};                ///< Event weight (e.g. P_DIS / nReplicas)
  std::int64_t originalEventId{-1};  ///< Originating event id, e.g. the muon
                                     ///< event that seeded this replica
                                     ///< (-1 = none). Event-level provenance,
                                     ///< distinct from MCParticle::motherId,
                                     ///< which links particles within an event.

  bool operator==(EventHeader const&) const = default;
};

}  // namespace SHiP
