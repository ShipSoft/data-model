#pragma once

#include "SHiP/MCParticle.hpp"
#include "SHiP/RecParticle.hpp"
#include "SHiP/SimHit.hpp"
#include "SHiP/SimParticle.hpp"
#include "SHiP/Units.hpp"

/// Quantity views over the persisted data classes.
///
/// The data classes stay plain-double PODs (ROOT streaming cannot handle
/// mp-units members); these free functions convert to and from the canonical
/// quantity types so computation code never touches an implicit unit. All
/// conversions are bitwise (the storage unit is the quantity's unit).
namespace ship::view {

// --- MCParticle -----------------------------------------------------------
[[nodiscard]] inline Vec3<Length> vertex(SHiP::MCParticle const& p) {
  return vecOf<Length>(p.vertex);
}
[[nodiscard]] inline Vec3<Momentum> momentum(SHiP::MCParticle const& p) {
  return vecOf<Momentum>(p.momentum);
}
/// Total energy.
[[nodiscard]] inline Energy energy(SHiP::MCParticle const& p) {
  return quantityOf<Energy>(p.energy);
}
[[nodiscard]] inline Time time(SHiP::MCParticle const& p) {
  return quantityOf<Time>(p.time);
}
inline void set_vertex(SHiP::MCParticle& p, Vec3<Length> const& v) {
  p.vertex = raw(v);
}
inline void set_momentum(SHiP::MCParticle& p, Vec3<Momentum> const& v) {
  p.momentum = raw(v);
}
inline void set_energy(SHiP::MCParticle& p, Energy e) { p.energy = raw(e); }
inline void set_time(SHiP::MCParticle& p, Time t) { p.time = raw(t); }

// --- SimHit ---------------------------------------------------------------
[[nodiscard]] inline Vec3<Length> position(SHiP::SimHit const& h) {
  return vecOf<Length>(h.position);
}
[[nodiscard]] inline Vec3<Momentum> momentum(SHiP::SimHit const& h) {
  return vecOf<Momentum>(h.momentum);
}
[[nodiscard]] inline Energy energy_deposit(SHiP::SimHit const& h) {
  return quantityOf<Energy>(h.energy_deposit);
}
[[nodiscard]] inline Time time(SHiP::SimHit const& h) {
  return quantityOf<Time>(h.time);
}
[[nodiscard]] inline Length path_length(SHiP::SimHit const& h) {
  return quantityOf<Length>(h.path_length);
}
inline void set_position(SHiP::SimHit& h, Vec3<Length> const& v) {
  h.position = raw(v);
}
inline void set_momentum(SHiP::SimHit& h, Vec3<Momentum> const& v) {
  h.momentum = raw(v);
}
inline void set_energy_deposit(SHiP::SimHit& h, Energy e) {
  h.energy_deposit = raw(e);
}
inline void set_time(SHiP::SimHit& h, Time t) { h.time = raw(t); }
inline void set_path_length(SHiP::SimHit& h, Length l) {
  h.path_length = raw(l);
}

// --- SimParticle ----------------------------------------------------------
[[nodiscard]] inline Vec3<Length> vertex(SHiP::SimParticle const& p) {
  return vecOf<Length>(p.vertex);
}
[[nodiscard]] inline Vec3<Length> endpoint(SHiP::SimParticle const& p) {
  return vecOf<Length>(p.endpoint);
}
[[nodiscard]] inline Vec3<Momentum> momentum(SHiP::SimParticle const& p) {
  return vecOf<Momentum>(p.momentum);
}
/// Initial kinetic energy.
[[nodiscard]] inline Energy energy(SHiP::SimParticle const& p) {
  return quantityOf<Energy>(p.energy);
}
[[nodiscard]] inline Time time(SHiP::SimParticle const& p) {
  return quantityOf<Time>(p.time);
}
inline void set_vertex(SHiP::SimParticle& p, Vec3<Length> const& v) {
  p.vertex = raw(v);
}
inline void set_endpoint(SHiP::SimParticle& p, Vec3<Length> const& v) {
  p.endpoint = raw(v);
}
inline void set_momentum(SHiP::SimParticle& p, Vec3<Momentum> const& v) {
  p.momentum = raw(v);
}
inline void set_energy(SHiP::SimParticle& p, Energy e) { p.energy = raw(e); }
inline void set_time(SHiP::SimParticle& p, Time t) { p.time = raw(t); }

// --- RecParticle ----------------------------------------------------------
[[nodiscard]] inline Vec3<Length> vertex(SHiP::RecParticle const& p) {
  return vecOf<Length>(p.vertex);
}
[[nodiscard]] inline Vec3<Length> endpoint(SHiP::RecParticle const& p) {
  return vecOf<Length>(p.endpoint);
}
[[nodiscard]] inline Vec3<Momentum> momentum(SHiP::RecParticle const& p) {
  return vecOf<Momentum>(p.momentum);
}
/// Initial kinetic energy.
[[nodiscard]] inline Energy energy(SHiP::RecParticle const& p) {
  return quantityOf<Energy>(p.energy);
}
[[nodiscard]] inline Time time(SHiP::RecParticle const& p) {
  return quantityOf<Time>(p.time);
}
/// Impact parameter wrt the primary vertex.
[[nodiscard]] inline Length ip_pv(SHiP::RecParticle const& p) {
  return quantityOf<Length>(p.ip_pv);
}
inline void set_vertex(SHiP::RecParticle& p, Vec3<Length> const& v) {
  p.vertex = raw(v);
}
inline void set_endpoint(SHiP::RecParticle& p, Vec3<Length> const& v) {
  p.endpoint = raw(v);
}
inline void set_momentum(SHiP::RecParticle& p, Vec3<Momentum> const& v) {
  p.momentum = raw(v);
}
inline void set_energy(SHiP::RecParticle& p, Energy e) { p.energy = raw(e); }
inline void set_time(SHiP::RecParticle& p, Time t) { p.time = raw(t); }
inline void set_ip_pv(SHiP::RecParticle& p, Length l) { p.ip_pv = raw(l); }

}  // namespace ship::view
