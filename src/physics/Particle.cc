/*
 * src/physics/Particle.cc
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

#include "physics/Particle.hh"

#include <Geant4/G4ParticleDefinition.hh>
#include <Geant4/G4ParticleTable.hh>

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Convert Momentum to Pseudo-Lorentz Triplet__________________________________________________
const PseudoLorentzTriplet Convert(const G4ThreeVector& momentum) {
  const auto magnitude = momentum.mag();
  if (magnitude == 0)
    return {};
  const auto eta = std::atanh(momentum.x() / magnitude);
  return {magnitude / std::cosh(eta), eta, std::atan2(momentum.y(), -momentum.z())};
}
//----------------------------------------------------------------------------------------------

//__Convert Pseudo-Lorentz Triplet to Momentum__________________________________________________
const G4ThreeVector Convert(const PseudoLorentzTriplet& triplet) {
  return G4ThreeVector(
     triplet.pT * std::sinh(triplet.eta),
     triplet.pT * std::sin(triplet.phi),
    -triplet.pT * std::cos(triplet.phi));
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Get Particle Definition from Geant4_________________________________________________________
G4ParticleDefinition* _get_particle_def(int id) {
  return G4ParticleTable::GetParticleTable()->FindParticle(id);
}
//----------------------------------------------------------------------------------------------

//__Get Particle Property from Definition_______________________________________________________
template<class Function, class Output>
Output _get_particle_property(int id,
                              Function f,
                              Output default_value={}) {
  return id == 0 ? default_value : f(_get_particle_def(id));
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Get Mass of Particle from ID________________________________________________________________
double GetParticleMass(int id) {
  return _get_particle_property(id, [](const auto& def) { return def->GetPDGMass(); }, 0);
}
//----------------------------------------------------------------------------------------------

//__Get Charge of Particle from ID______________________________________________________________
double GetParticleCharge(int id) {
  return _get_particle_property(id, [](const auto& def) { return def->GetPDGCharge(); }, 0);
}
//----------------------------------------------------------------------------------------------

//__Get Name of Particle from ID________________________________________________________________
const std::string GetParticleName(int id) {
  return _get_particle_property(id, [](const auto& def) { return def->GetParticleName(); }, G4String(""));
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle PT_______________________________________________________________________
double BasicParticle::pT() const {
  const auto magnitude = G4ThreeVector(px, py, pz).mag();
  if (magnitude == 0)
    return 0;
  return magnitude / std::cosh(std::atanh(px / magnitude));
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle ETA______________________________________________________________________
double BasicParticle::eta() const {
  const auto magnitude = G4ThreeVector(px, py, pz).mag();
  if (magnitude == 0)
    return 0;
  return std::atanh(px / magnitude);
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle PHI______________________________________________________________________
double BasicParticle::phi() const {
  return std::atan2(py, -pz);
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle PseudoLorentzTriplet_____________________________________________________
const PseudoLorentzTriplet BasicParticle::pseudo_lorentz_triplet() const {
  return Convert(G4ThreeVector(px, py, pz));
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Name_____________________________________________________________________
const std::string BasicParticle::name() const {
  return GetParticleName(id);
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Charge___________________________________________________________________
double BasicParticle::charge() const {
  return GetParticleCharge(id);
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Mass_____________________________________________________________________
double BasicParticle::mass() const {
  return GetParticleMass(id);
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Kinetic Energy___________________________________________________________
double BasicParticle::ke() const {
  return 0;
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Momentum Unit Vector_____________________________________________________
const G4ThreeVector BasicParticle::p_unit() const {
  return G4ThreeVector(px, py, pz).unit();
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Momentum Vector__________________________________________________________
const G4ThreeVector BasicParticle::p() const {
  return G4ThreeVector(px, py, pz);
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
