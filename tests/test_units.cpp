/// Tests for the canonical unit vocabulary: exact conversion factors, bitwise
/// POD round-trips through the quantity views, and compile-time dimension
/// safety. Returns non-zero on failure.

#include <array>
#include <cmath>
#include <cstdio>
#include <string>

#include "SHiP/MCParticle.hpp"
#include "SHiP/QuantityView.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/Units.hpp"

namespace su = ship::units;

namespace {

int failures = 0;

void expect(bool ok, std::string const& what) {
  if (!ok) {
    std::printf("FAIL: %s\n", what.c_str());
    ++failures;
  }
}

// Energy and momentum have different dimensions: mixing them must not compile.
template <typename E, typename P>
concept AddsWithMomentum = requires(E e, P p) { e + p; };
static_assert(!AddsWithMomentum<ship::Energy, ship::Momentum>,
              "energy + momentum must be a compile error");
static_assert(AddsWithMomentum<ship::Energy, ship::Energy>);

// Canonical types store in the canonical units (no hidden conversion).
static_assert(ship::Length::unit == su::mm);
static_assert(ship::Time::unit == su::ns);
static_assert(ship::Energy::unit == su::GeV);

}  // namespace

int main() {
  // mm/c -> ns uses the exact SI definition of c; compare against the
  // hand-derived factor 1 / 299.792458 (the constant FairShip hardcoded).
  double const mm_per_c_in_ns = (1.0 * su::mm_per_c).numerical_value_in(su::ns);
  double const reference = 1.0 / 299.792458;
  expect(std::abs(mm_per_c_in_ns - reference) <= 1e-16,
         "1 mm/c in ns matches 1/299.792458");

  // Exact power-of-ten factors.
  expect((1.0 * su::GeV).numerical_value_in(su::MeV) == 1000.0,
         "GeV -> MeV is exactly 1000");
  expect((1.0 * su::m).numerical_value_in(su::mm) == 1000.0,
         "m -> mm is exactly 1000");
  expect((1.0 * su::s).numerical_value_in(su::ns) == 1e9,
         "s -> ns is exactly 1e9");

  // vecOf/raw round-trip must be bitwise (storage unit == quantity unit).
  std::array<double, 3> const v{1.0000000000000002, -2.5e-17, 3.9e12};
  expect(ship::raw(ship::vecOf<ship::Length>(v)) == v,
         "Length vecOf/raw round-trip is bitwise");
  expect(ship::raw(ship::vecOf<ship::Momentum>(v)) == v,
         "Momentum vecOf/raw round-trip is bitwise");

  // Quantity views round-trip the PODs bitwise.
  SHiP::MCParticle mc;
  mc.vertex = {1.1, 2.2, 3.3};
  mc.momentum = {0.1, 0.2, 40.0};
  mc.energy = 40.001;
  mc.time = 5.5;
  SHiP::MCParticle copy;
  ship::view::setVertex(copy, ship::view::vertex(mc));
  ship::view::setMomentum(copy, ship::view::momentum(mc));
  ship::view::setEnergy(copy, ship::view::energy(mc));
  ship::view::setTime(copy, ship::view::time(mc));
  expect(copy.vertex == mc.vertex && copy.momentum == mc.momentum &&
             copy.energy == mc.energy && copy.time == mc.time,
         "MCParticle view round-trip is bitwise");

  SHiP::SimHit hit;
  hit.position = {-0.5, 0.0, 12000.0};
  hit.momentum = {0.01, -0.02, 3.0};
  hit.energyDeposit = 0.00123;
  hit.time = 7.25;
  hit.pathLength = 0.3;
  SHiP::SimHit hitCopy;
  ship::view::setPosition(hitCopy, ship::view::position(hit));
  ship::view::setMomentum(hitCopy, ship::view::momentum(hit));
  ship::view::setEnergyDeposit(hitCopy, ship::view::energyDeposit(hit));
  ship::view::setTime(hitCopy, ship::view::time(hit));
  ship::view::setPathLength(hitCopy, ship::view::pathLength(hit));
  expect(hitCopy.position == hit.position && hitCopy.momentum == hit.momentum &&
             hitCopy.energyDeposit == hit.energyDeposit &&
             hitCopy.time == hit.time && hitCopy.pathLength == hit.pathLength,
         "SimHit view round-trip is bitwise");

  // A deliberately non-canonical input converts, rather than reinterprets.
  auto const z = 1.5 * su::m;
  expect(ship::Length{z}.numerical_value_in(su::mm) == 1500.0,
         "1.5 m stored as Length is 1500 mm");

  if (failures == 0) {
    std::printf("all unit tests passed\n");
  }
  return failures;
}
