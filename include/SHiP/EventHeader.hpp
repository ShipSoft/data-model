#pragma once

#include <cstdint>
#include <limits>

namespace SHiP {

// The event weight must be a 64-bit IEEE-754 binary64 value for portable
// I/O. std::float64_t would state this in the type system, but cling does
// not implement __STDCPP_FLOAT64_T__, so it cannot appear in a class that
// needs a ROOT dictionary; this assertion gives the same guarantee.
static_assert(std::numeric_limits<double>::is_iec559 && sizeof(double) == 8,
              "EventHeader::weight requires IEEE-754 binary64 double");

/// Per-event metadata — one record per event
struct EventHeader {
  double weight{1.0};                ///< Event weight (e.g. P_DIS / nReplicas)
  std::int64_t originalEventId{-1};  ///< Originating event id, e.g. the muon
                                     ///< event that seeded this replica
                                     ///< (-1 = none). Event-level provenance,
                                     ///< distinct from MCParticle::motherId,
                                     ///< which links particles within an event.
};

}  // namespace SHiP
