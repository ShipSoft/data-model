/// Enforce the MCParticle mother-index invariants documented in
/// include/SHiP/MCParticle.hpp.
///
/// The rest of the suite compares read-back data with the very generator that
/// produced it, so a recipe that drifts out of spec would still agree with
/// itself. These checks pin the recipes against the contract instead, and the
/// negative cases keep the predicate itself honest: a validator that accepted
/// everything would pass the positive checks alone.

#include <iostream>
#include <string>
#include <vector>

#include "SHiP/MCParticle.hpp"
#include "reference_values.hpp"
#include "test_utils.hpp"

namespace {

/// A minimal valid 3-particle collection to mutate into each violation.
std::vector<SHiP::MCParticle> makeValid() {
  std::vector<SHiP::MCParticle> v(3);
  v[0].motherId = -1;
  v[1].motherId = 0;
  v[1].mothers = {0, 2};
  v[2].motherId = 1;
  v[2].mothers = {1, 0};
  return v;
}

/// Assert that `mutate` turns the valid collection into a rejected one.
template <typename Mutate>
bool rejects(std::string const& what, Mutate mutate) {
  auto v = makeValid();
  mutate(v);
  return SHiP::test::check("rejects " + what, false,
                           SHiP::mothersAreConsistent(v));
}

}  // namespace

int main() {
  bool ok = true;

  // The baseline must pass, or every negative case below is vacuous.
  ok &= SHiP::test::check("accepts a valid collection", true,
                          SHiP::mothersArePopulated(makeValid()));

  // Both canonical recipes satisfy the contract, at every offset the suite
  // uses: entries for the two top-level entries and the +5 simResult shift.
  for (int offset : {0, 1, 5, 6}) {
    std::string const suffix = " (offset " + std::to_string(offset) + ")";
    ok &= SHiP::test::check(
        "test_utils recipe" + suffix, true,
        SHiP::mothersArePopulated(SHiP::test::makeMCParticles(offset)));
    ok &= SHiP::test::check(
        "reference_values recipe" + suffix, true,
        SHiP::mothersArePopulated(SHiP::ref::makeMCParticles(offset)));
  }

  // One violation per documented rule.
  ok &= rejects("an out-of-range mother index",
                [](auto& v) { v[1].mothers = {0, 7}; });
  ok &= rejects("a -1 sentinel inside the list", [](auto& v) {
    v[1].motherId = -1;
    v[1].mothers = {-1};
  });
  ok &= rejects("a first element that is not motherId",
                [](auto& v) { v[1].mothers = {2, 0}; });
  ok &= rejects("a duplicated mother", [](auto& v) { v[1].mothers = {0, 0}; });
  ok &= rejects("an entry that is its own mother", [](auto& v) {
    v[2].motherId = 2;
    v[2].mothers = {2};
  });
  ok &= rejects("an out-of-range motherId", [](auto& v) {
    v[0].motherId = 5;
    v[0].mothers.clear();
  });

  // Pre-v0.5.0 read-back shape: the field did not exist, so `mothers` comes
  // back empty beside a valid `motherId`. That is consistent but not
  // populated — the distinction the compat tests rely on.
  auto legacy = makeValid();
  for (auto& p : legacy) {
    p.mothers.clear();
  }
  ok &= SHiP::test::check("accepts legacy empty mothers", true,
                          SHiP::mothersAreConsistent(legacy));
  ok &= SHiP::test::check("legacy mothers are not populated", false,
                          SHiP::mothersArePopulated(legacy));

  std::cout << (ok ? "All MCParticle mother invariant tests passed"
                   : "MCParticle mother invariant tests FAILED")
            << '\n';
  return ok ? 0 : 1;
}
