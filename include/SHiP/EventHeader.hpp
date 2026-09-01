#pragma once

#include <cstdint>

#include "Rtypes.h"

namespace SHiP {

/// Per-event metadata — one record per event
struct EventHeader {
  double weight{1.0};  ///< Event weight (e.g. P_DIS / nReplicas)
  std::int64_t original_event_id{
      -1};  ///< Originating event id, e.g. the muon
            ///< event that seeded this replica
            ///< (-1 = none). Event-level provenance,
            ///< distinct from MCParticle::mother_id,
            ///< which links particles within an event.

  bool operator==(EventHeader const&) const = default;
  // Explicit class version: required for RNTuple I/O rules
  // (root-project/root#23146); bump on any layout change.
  ClassDefNV(EventHeader, 2);
};

}  // namespace SHiP
