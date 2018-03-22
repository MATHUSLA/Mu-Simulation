#include "physics/PythiaGenerator.hh"

#include "Geant4/G4Event.hh"
#include "Geant4/G4PhysicalConstants.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4TransportationManager.hh"

#include "detector/Construction.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

Pythia8::Particle* PythiaFilter::find(Pythia8::Event& event) {
  for (int i = 0; i < event.size(); ++i) {
    const auto& particle = event[i];
    if (particle.idAbs() == id
        && particle.pT() >= pT
        && abs(particle.eta()) <= eta) {
      auto angle = particle.phi() * 180/pi;
      if (phi_min <= angle && angle <= phi_max)
        return &event[i];
    }
  }
  return nullptr;
}

PythiaGenerator::PythiaGenerator(Pythia8::Pythia* pythia,
                                 const PythiaFilter& filter)
    : _pythia(pythia), _filter(filter) {
  if (!_pythia) {
    G4cout << "PythiaGenerator:\n  Pythia Generator not specified!\n";
    G4RunManager::GetRunManager()->AbortRun();
  }
}

void PythiaGenerator::GeneratePrimaries(G4Event* event) {
  Pythia8::Particle* particle = nullptr;
  G4ThreeVector position;

  uint_fast64_t counter = 0;
  while (true) {
    ++counter;
    if (!_pythia->next()) continue;
    particle = _filter.find(_pythia->process);
    if (particle) break;
  }

  G4cout << "\n\nPythiaGenerator:\n"
         << "  Attempt " << counter << " | Filtered Particle: "
         << particle->idAbs() << " "
         << particle->pT()    << " "
         << particle->eta()   << " "
         << particle->phi()   << "\n\n\n";

  // PythiaGenerator:
  //   Attempt 1266345 | Filtered Particle: 13 79.9106 0.00330305 0.0404396

  auto vertex = DefaultVertex();
  auto momentum
    = G4ThreeVector(particle->pz(), particle->py(), -particle->px())*GeVperC;

  vertex->SetPrimary(new G4PrimaryParticle(
    particle->id(), momentum.x(), momentum.y(), momentum.z()));

  event->AddPrimaryVertex(vertex);
}

} } /* namespace MATHUSLA::MU */
