/*
 * include/physics/Particle.hh
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MU__PHYSICS_PARTICLE_HH
#define MU__PHYSICS_PARTICLE_HH
#pragma once

#include <Geant4/G4ThreeVector.hh>
#include <Geant4/G4Event.hh>

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Pseudo-Lorentz Invariant Triplet____________________________________________________________
struct PseudoLorentzTriplet { double pT, eta, phi; };
//----------------------------------------------------------------------------------------------

//__Equality Between Pseudo Lorentz Triplet_____________________________________________________
constexpr bool operator==(const PseudoLorentzTriplet& left,
                          const PseudoLorentzTriplet& right) {
  return left.pT == right.pT && left.eta == right.eta && left.phi == right.phi;
}
//----------------------------------------------------------------------------------------------

//__Convert Momentum to Pseudo-Lorentz Triplet__________________________________________________
const PseudoLorentzTriplet Convert(const G4ThreeVector& momentum);
//----------------------------------------------------------------------------------------------

//__Convert Pseudo-Lorentz Triplet to Momentum__________________________________________________
const G4ThreeVector Convert(const PseudoLorentzTriplet& triplet);
//----------------------------------------------------------------------------------------------

//__Get Mass of Particle from ID________________________________________________________________
double GetParticleMass(int id);
//----------------------------------------------------------------------------------------------

//__Get Charge of Particle from ID______________________________________________________________
double GetParticleCharge(int id);
//----------------------------------------------------------------------------------------------

//__Get Name of Particle from ID________________________________________________________________
const std::string GetParticleName(int id);
//----------------------------------------------------------------------------------------------

//__Basic Momentum Particle_____________________________________________________________________
struct BasicParticle {
  int id;
  double px, py, pz;

  BasicParticle() = default;

  BasicParticle(int identifier)
      : BasicParticle(identifier, 0, 0, 0) {}

  BasicParticle(int identifier,
                double x_momentum,
                double y_momentum,
                double z_momentum)
      : id(identifier), px(x_momentum), py(y_momentum), pz(z_momentum) {}

  double pT() const;
  double eta() const;
  double phi() const;
  const PseudoLorentzTriplet pseudo_lorentz_triplet() const;

  const std::string name() const;

  double charge() const;
  double mass() const;
  double ke() const;
  double e() const;
  double p_mag() const;
  const G4ThreeVector p_unit() const;
  const G4ThreeVector p() const;

  void set_pT(double new_pT);
  void set_eta(double new_eta);
  void set_phi(double new_phi);
  void set_pseudo_lorentz_triplet(double new_pT,
                                  double new_eta,
                                  double new_phi);
  void set_pseudo_lorentz_triplet(const PseudoLorentzTriplet& triplet);

  void set_ke(double new_ke);

  void set_p_mag(double magnitude);
  void set_p_unit(double pu_x,
                  double pu_y,
                  double pu_z);
  void set_p_unit(const G4ThreeVector& new_p_unit);
  void set_p(double new_px,
             double new_py,
             double new_pz);
  void set_p(const G4ThreeVector& new_p);
};
//----------------------------------------------------------------------------------------------

//__Collection of Basic Particles_______________________________________________________________
using BasicParticleVector = std::vector<BasicParticle>;
//----------------------------------------------------------------------------------------------

//__Collection of Particles at a Vertex_________________________________________________________
struct Vertex {
  double t, x, y, z;
  BasicParticleVector particles;
};
//----------------------------------------------------------------------------------------------

//__Momentum Vertex Particle____________________________________________________________________
struct Particle : BasicParticle {
  double t, x, y, z;

  Particle() = default;

  Particle(int identifier)
      : Particle(identifier, 0, 0, 0, 0, 0, 0, 0) {}

  Particle(int identifier,
           double t_position,
           double x_position,
           double y_position,
           double z_position)
      : Particle(identifier, t_position, x_position, y_position, z_position, 0, 0, 0) {}

  Particle(int identifier,
           double x_position,
           double y_position,
           double z_position)
      : Particle(identifier, 0, x_position, y_position, z_position) {}

  Particle(int identifier,
           double t_position,
           double x_position,
           double y_position,
           double z_position,
           double x_momentum,
           double y_momentum,
           double z_momentum)
      : BasicParticle(identifier, x_momentum, y_momentum, z_momentum),
        t(t_position), x(x_position), y(y_position), z(z_position) {}

  Particle(int identifier,
           double x_position,
           double y_position,
           double z_position,
           double x_momentum,
           double y_momentum,
           double z_momentum)
      : Particle(identifier, 0, x_position, y_position, z_position, x_momentum, y_momentum, z_momentum) {}

  Particle(const BasicParticle& basic_particle)
      : Particle(basic_particle.id, 0, 0, 0, 0, basic_particle.px, basic_particle.py, basic_particle.pz) {}

  void set_vertex(double new_x,
                  double new_y,
                  double new_z);
  void set_vertex(double new_t,
                  double new_x,
                  double new_y,
                  double new_z);
  void set_vertex(double new_t,
                  const G4ThreeVector& vertex);
  void set_vertex(const G4ThreeVector& vertex);
};
//----------------------------------------------------------------------------------------------

//__Collection of Particles_____________________________________________________________________
using ParticleVector = std::vector<Particle>;
//----------------------------------------------------------------------------------------------

//__Add Momentum and Vertex Particle To Event___________________________________________________
void AddParticle(const Particle& particle,
                 G4Event& event);
//----------------------------------------------------------------------------------------------

namespace Filter { /////////////////////////////////////////////////////////////////////////////

//__Sign Enum___________________________________________________________________________________
enum class Sign : signed char {
  Negative = -1,
  Zero     =  0,
  Positive =  1
};
//----------------------------------------------------------------------------------------------

//__Convert to Sign Enum________________________________________________________________________
template<class T>
constexpr Sign to_sign(const T& t) {
  return t > 0 ? Sign::Positive : (t == 0 ? Sign::Zero : Sign::Negative);
}
//----------------------------------------------------------------------------------------------

//__Disjunction Filter Type_____________________________________________________________________
template<class ...Ts>
struct Any {
  template<class P>
  bool operator()(const P&) const { return true; }
};
template<class T, class ...Ts>
struct Any<T, Ts...> : T, Any<Ts...> {
  Any(T&& t,
      Ts&& ...ts) : T(std::forward<T>(t)), Any<Ts...>(std::forward<Ts>(ts)...) {}
  template<class P>
  bool operator()(const P& particle) const {
    return T::operator()(particle) || Any<Ts...>::operator()(particle);
  }
};
//----------------------------------------------------------------------------------------------

//__Conjuction Filter Type______________________________________________________________________
template<class ...Ts>
struct All {
  template<class P>
  bool operator()(const P&) const { return true; }
};
template<class T, class ...Ts>
struct All<T, Ts...> : T, All<Ts...> {
  All(T&& t,
      Ts&& ...ts) : T(std::forward<T>(t)), All<Ts...>(std::forward<Ts>(ts)...) {}
  template<class P>
  bool operator()(const P& particle) const {
    return T::operator()(particle) && All<Ts...>::operator()(particle);
  }
};
//----------------------------------------------------------------------------------------------

//__ID Filter Type______________________________________________________________________________
struct Id {
  int id;
  Id(int identifier) : id(identifier) {}
  template<class P>
  bool operator()(const P& particle) const {
    return particle.id == id;
  }
};
//----------------------------------------------------------------------------------------------

//__Pseudo-Lorentz Particle Filter______________________________________________________________
struct PseudoLorentz {
  PseudoLorentzTriplet min, max;
  PseudoLorentz(const PseudoLorentzTriplet& min_value,
                const PseudoLorentzTriplet& max_value)
      : min(min_value), max(max_value) {}

  template<class P>
  bool operator()(const P& particle) const {
    const auto triplet = particle.pseudo_lorentz_triplet();
    bool match_pT = true, match_eta = true, match_phi = true;
    if (min.pT  || max.pT)  match_pT  = (min.pT  <= triplet.pT)  && (triplet.pT  <= max.pT);
    if (min.eta || max.eta) match_eta = (min.eta <= triplet.eta) && (triplet.eta <= max.eta);
    if (min.phi || max.phi) match_phi = (min.phi <= triplet.phi) && (triplet.phi <= max.phi);
    return match_pT && match_eta && match_phi;
  }
};
//----------------------------------------------------------------------------------------------

} /* namespace Filter */ ///////////////////////////////////////////////////////////////////////

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_PARTICLE_HH */
