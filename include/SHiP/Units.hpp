#pragma once

#include <mp-units/systems/hep.h>  // exports framework.h; includes systems/si.h

#include <array>

/// Canonical ShipSoft unit vocabulary.
///
/// Storage convention (matches the data-class doxygen contracts and Key4hep):
/// positions and lengths [mm], momenta [GeV/c], energies [GeV], times [ns].
/// Persisted classes keep plain doubles in these units; computation code
/// works with the quantity types below and converts at the boundaries (see
/// SHiP/QuantityView.hpp).
///
/// NEVER `using namespace ship::units` in a translation unit that includes
/// CLHEP or Geant4 headers: G4SystemOfUnits.hh injects global `mm`, `GeV`,
/// `tesla`, ... that collide with these names. Always qualify.
namespace ship {

namespace units {

// Length
inline constexpr auto mm = mp_units::si::milli<mp_units::si::metre>;
inline constexpr auto cm = mp_units::si::centi<mp_units::si::metre>;
inline constexpr auto m = mp_units::si::metre;

// Time
inline constexpr auto ns = mp_units::si::nano<mp_units::si::second>;
inline constexpr auto s = mp_units::si::second;

// Angle
inline constexpr auto rad = mp_units::si::radian;

// Energy
inline constexpr auto MeV = mp_units::hep::unit_symbols::MeV;
inline constexpr auto GeV = mp_units::hep::unit_symbols::GeV;

// Speed of light as a named unit ("c", exact by SI definition). Building
// momentum, mass and time-of-flight units from it keeps every conversion
// factor derived rather than hand-typed.
inline constexpr auto c = mp_units::hep::speed_of_light;

// Momentum and mass
inline constexpr auto GeV_per_c = GeV / c;
inline constexpr auto GeV_per_c2 = GeV / mp_units::square(c);

// Time of flight per unit length (Pythia8 production time, lifetime cuts)
inline constexpr auto mm_per_c = mm / c;

// Magnetic flux density
inline constexpr auto tesla = mp_units::si::tesla;

}  // namespace units

/// Canonical quantity types: double representation, storage unit baked in.
using Length = mp_units::quantity<units::mm, double>;
using Time = mp_units::quantity<units::ns, double>;
using Energy = mp_units::quantity<units::GeV, double>;
using Momentum = mp_units::quantity<units::GeV_per_c, double>;
using Mass = mp_units::quantity<units::GeV_per_c2, double>;
using Angle = mp_units::quantity<units::rad, double>;
using MagneticField = mp_units::quantity<units::tesla, double>;

template <typename Q>
using Vec3 = std::array<Q, 3>;

/// Wrap a raw scalar, interpreting it in Q's canonical unit (bitwise).
template <typename Q>
[[nodiscard]] constexpr Q quantityOf(double v) {
  return v * Q::reference;
}

/// Unwrap a scalar quantity to a raw double in Q's canonical unit (bitwise).
template <typename Q>
[[nodiscard]] constexpr double raw(Q q) {
  return q.numerical_value_in(Q::unit);
}

/// Wrap a raw 3-vector, interpreting it in Q's canonical unit (bitwise).
template <typename Q>
[[nodiscard]] constexpr Vec3<Q> vecOf(std::array<double, 3> const& v) {
  return {v[0] * Q::reference, v[1] * Q::reference, v[2] * Q::reference};
}

/// Unwrap a quantity 3-vector to raw doubles in Q's canonical unit (bitwise).
template <typename Q>
[[nodiscard]] constexpr std::array<double, 3> raw(Vec3<Q> const& v) {
  return {v[0].numerical_value_in(Q::unit), v[1].numerical_value_in(Q::unit),
          v[2].numerical_value_in(Q::unit)};
}

}  // namespace ship
