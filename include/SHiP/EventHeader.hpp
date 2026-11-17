#pragma once

#include <cstdint>

namespace SHiP {

/// Per-event metadata — one record per event
struct EventHeader {
  double weight{1.0};  ///< Event weight (e.g. P_DIS / nReplicas)

  /// Originating event id, e.g. the muon event that seeded this replica
  /// (-1 = none). Event-level provenance, distinct from
  /// MCParticle::mother_id, which links particles within an event.
#ifdef __APPLE__
  // std::int64_t is long long on macOS but long on Linux. ROOT computes the
  // class checksum from the member type names, so declare long here to keep
  // the schema identical to Linux. long is 64 bits on macOS (LP64).
  static_assert(sizeof(long) == 8);
  long original_event_id{-1};
#else
  std::int64_t original_event_id{-1};
#endif

  bool operator==(EventHeader const&) const = default;
};

}  // namespace SHiP
