#include "physics/Generator.hh"

#include <cmath>

#include "Geant4/Randomize.hh"

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

Generator::Generator(G4int id, G4double pT, G4double eta, G4double phi)
    : G4VUserPrimaryGeneratorAction(), _id(id), _pT(pT), _eta(eta), _phi(phi) {}

G4PrimaryVertex* Generator::DefaultVertex() {
  return new G4PrimaryVertex(0, 0, 100*m, 0);
}

G4PrimaryParticle* Generator::CreateParticle(G4int id, G4double pT, G4double eta, G4double phi) {
  return new G4PrimaryParticle(
    id, pT*std::sinh(eta), pT*std::sin(phi), -pT*std::cos(phi));
}

void Generator::GeneratePrimaries(G4Event* event) {
  static const auto particle = CreateParticle(_id, _pT, _eta, _phi);
  static const auto momentum = particle->GetMomentum();
  auto vertex = DefaultVertex();
  vertex->SetPrimary(new G4PrimaryParticle(
    _id, momentum.x(), momentum.y(), momentum.z()));
  event->AddPrimaryVertex(vertex);
}

RangeGenerator::RangeGenerator(G4int id, G4double pT, G4double eta, G4double phi)
    : _id(id), _pT_min(pT), _pT_max(-1), _eta_min(0), _eta_max(eta),
      _phi_min(-phi), _phi_max(phi) {}

RangeGenerator::RangeGenerator(G4int id, G4double pT_min,  G4double pT_max,
                                         G4double eta_min, G4double eta_max,
                                         G4double phi_min, G4double phi_max)
    : _id(id), _pT_min(pT_min), _pT_max(pT_max), _eta_min(eta_min),
      _eta_max(eta_max), _phi_min(phi_min), _phi_max(phi_max) {}

void RangeGenerator::GeneratePrimaries(G4Event* event) {
  auto vertex = DefaultVertex();
  _pT  = _pT_max < 0 ? _pT_min : G4RandFlat::shoot(_pT_min, _pT_max);
  _eta = G4RandFlat::shoot(_eta_min, _eta_max);
  _phi = G4RandFlat::shoot(_phi_min, _phi_max);
  vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  event->AddPrimaryVertex(vertex);
}

} } /* namespace MATHUSLA::MU */
