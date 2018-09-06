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
  const G4ThreeVector p_unit() const;
  const G4ThreeVector p() const;

  void set_pT(double pT);
  void set_eta(double eta);
  void set_phi(double phi);
  void set_pseudo_lorentz_triplet(double pT,
                                  double eta,
                                  double phi);
  void set_pseudo_lorentz_triplet(const PseudoLorentzTriplet& triplet);

  void set_p(double new_px,
             double new_py,
             double new_pz);
  void set_p(const G4ThreeVector& new_p);
};
//----------------------------------------------------------------------------------------------

//__Momentum Vertex Particle____________________________________________________________________
struct Particle : BasicParticle {
  double t, x, y, z;

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
      : Particle(identifier, 0.0L, x_position, y_position, z_position, x_momentum, y_momentum, z_momentum) {}

  Particle(const BasicParticle& basic_particle)
      : Particle(basic_particle.id, 0, 0, 0, 0, basic_particle.px, basic_particle.py, basic_particle.pz) {}

  void set_vertex(double new_x,
                  double new_y,
                  double new_z);
  void set_vertex(double new_t,
                  double new_x,
                  double new_y,
                  double new_z);
};
//----------------------------------------------------------------------------------------------

namespace Filter { /////////////////////////////////////////////////////////////////////////////



} /* namespace Filter */ ///////////////////////////////////////////////////////////////////////

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_PARTICLE_HH */
