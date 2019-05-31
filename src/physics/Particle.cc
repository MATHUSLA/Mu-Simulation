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
#include <Geant4/G4SystemOfUnits.hh>

#include "geometry/Cavern.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Convert Momentum to Pseudo-Lorentz Triplet__________________________________________________
const PseudoLorentzTriplet Convert(const G4ThreeVector& momentum) {
  const auto magnitude = momentum.mag();
  if (magnitude == 0)
    return {};
  const auto pxpz = Cavern::rotate_from_P1(momentum.x(), momentum.z());
  const auto eta = std::atanh(pxpz.first / magnitude);
  return {static_cast<double>(magnitude / std::cosh(eta)),
          static_cast<double>(eta),
          static_cast<double>(std::atan2(momentum.y(), -pxpz.second))};
}
//----------------------------------------------------------------------------------------------

//__Convert Pseudo-Lorentz Triplet to Momentum__________________________________________________
const G4ThreeVector Convert(const PseudoLorentzTriplet& triplet) {
  const auto pxpz = Cavern::rotate_to_P1(triplet.pT * std::sinh(triplet.eta),
                                        -triplet.pT * std::cos(triplet.phi));
  return G4ThreeVector(static_cast<double>(pxpz.first),
                       triplet.pT * std::sin(triplet.phi),
                       static_cast<double>(pxpz.second));
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
  const auto magnitude = p_mag();
  if (magnitude == 0)
    return 0;
  const auto eta = std::atanh(Cavern::rotate_from_P1_x(px, pz) / magnitude);
  return static_cast<double>(magnitude / std::cosh(eta));
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle ETA______________________________________________________________________
double BasicParticle::eta() const {
  const auto magnitude = p_mag();
  if (magnitude == 0)
    return 0;
  return static_cast<double>(std::atanh(Cavern::rotate_from_P1_x(px, pz) / magnitude));
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle PHI______________________________________________________________________
double BasicParticle::phi() const {
  return static_cast<double>(std::atan2(py, -Cavern::rotate_from_P1_z(px, pz)));
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
  return e() - mass();
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Total Energy_____________________________________________________________
double BasicParticle::e() const {
  return std::hypot(p_mag(), mass());
}
//----------------------------------------------------------------------------------------------

//__Get Basic Particle Total Momentum___________________________________________________________
double BasicParticle::p_mag() const {
  return G4ThreeVector(px, py, pz).mag();
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

//__Set Basic Particle PT_______________________________________________________________________
void BasicParticle::set_pT(double new_pT) {
  set_pseudo_lorentz_triplet(new_pT, eta(), phi());
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle ETA______________________________________________________________________
void BasicParticle::set_eta(double new_eta) {
  set_pseudo_lorentz_triplet(PseudoLorentzTriplet{pT(), new_eta, phi()});
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle PHI______________________________________________________________________
void BasicParticle::set_phi(double new_phi) {
  set_pseudo_lorentz_triplet(PseudoLorentzTriplet{pT(), eta(), new_phi});
}
//----------------------------------------------------------------------------------------------


//__Set Basic Particle PseudoLorentzTriplet_____________________________________________________
void BasicParticle::set_pseudo_lorentz_triplet(double new_pT,
                                               double new_eta,
                                               double new_phi) {
  set_pseudo_lorentz_triplet(PseudoLorentzTriplet{new_pT, new_eta, new_phi});
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle PseudoLorentzTriplet_____________________________________________________
void BasicParticle::set_pseudo_lorentz_triplet(const PseudoLorentzTriplet& triplet) {
  set_p(Convert(triplet));
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Kinetic Energy___________________________________________________________
void BasicParticle::set_ke(double new_ke) {
  set_p(p_unit() * std::sqrt(new_ke * (new_ke + 2.0L * mass())));
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Total Momentum___________________________________________________________
void BasicParticle::set_p_mag(double magnitude) {
  set_p(magnitude * p_unit());
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Momentum Unit Vector_____________________________________________________
void BasicParticle::set_p_unit(double pu_x,
                               double pu_y,
                               double pu_z) {
  set_p_unit({pu_x, pu_y, pu_z});
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Momentum Unit Vector_____________________________________________________
void BasicParticle::set_p_unit(const G4ThreeVector& new_p_unit) {
  const auto magnitude = p_mag();
  set_p((!magnitude ? 1.0L : magnitude) * new_p_unit.unit());
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Momentum Vector__________________________________________________________
void BasicParticle::set_p(double new_px,
                          double new_py,
                          double new_pz) {
  px = new_px;
  py = new_py;
  pz = new_pz;
}
//----------------------------------------------------------------------------------------------

//__Set Basic Particle Momentum Vector__________________________________________________________
void BasicParticle::set_p(const G4ThreeVector& new_p) {
  set_p(new_p.x(), new_p.y(), new_p.z());
}
//----------------------------------------------------------------------------------------------

//__Set Particle Vertex_________________________________________________________________________
void Particle::set_vertex(double new_x,
                          double new_y,
                          double new_z) {
  x = new_x;
  y = new_y;
  z = new_z;
}
//----------------------------------------------------------------------------------------------

//__Set Particle Vertex_________________________________________________________________________
void Particle::set_vertex(double new_t,
                          double new_x,
                          double new_y,
                          double new_z) {
  t = new_t;
  set_vertex(new_x, new_y, new_z);
}
//----------------------------------------------------------------------------------------------

//__Set Particle Vertex_________________________________________________________________________
void Particle::set_vertex(double new_t,
                          const G4ThreeVector& vertex) {
  set_vertex(new_t, vertex.x(), vertex.y(), vertex.z());
}
//----------------------------------------------------------------------------------------------

//__Set Particle Vertex_________________________________________________________________________
void Particle::set_vertex(const G4ThreeVector& vertex) {
  set_vertex(vertex.x(), vertex.y(), vertex.z());
}
//----------------------------------------------------------------------------------------------

//__Add Momentum and Vertex Particle To Event___________________________________________________
void AddParticle(const Particle& particle,
                 G4Event& event) {
  const auto vertex = new G4PrimaryVertex(particle.x, particle.y, particle.z, particle.t);
  vertex->SetPrimary(new G4PrimaryParticle(particle.id, particle.px, particle.py, particle.pz));
  event.AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
