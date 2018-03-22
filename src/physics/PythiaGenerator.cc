#include "physics/PythiaGenerator.hh"

#include "Geant4/G4Event.hh"
#include "Geant4/G4PhysicalConstants.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4TransportationManager.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Pythia8::Particle* PythiaFilter::find(Pythia8::Event& event) {
  for (int i = 0; i < event.size(); ++i) {
    const auto& particle = event[i];
    if (particle.idAbs() == id
        && particle.pT() >= pT
        && abs(particle.eta()) <= eta) {
      auto angle = particle.phi();
      if (phi_min * pi/180 <= angle && angle <= phi_max * pi/180)
        return &event[i];
    }
  }
  return nullptr;
}

PythiaGenerator::PythiaGenerator(Pythia8::Pythia* pythia,
                                 const PythiaFilter& filter)
    : _hepmcevt(nullptr), _pythia(pythia), _filter(filter), _hepmc_converter() {
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
    if (particle) {
      position = G4ThreeVector(
        particle->zProd(), particle->yProd(), -particle->xProd() + 100*m);

      if (world_solid->Inside(position*mm) != kInside) continue;
      else break;
    }
  }

  G4cout << "\n\nPythiaGenerator:\n"
         << "  Attempt " << counter << " | Filtered Particle: "
         << particle->idAbs() << " "
         << particle->pT()    << " "
         << particle->eta()   << " "
         << particle->phi()   << "\n\n\n";

  auto vertex = new G4PrimaryVertex(position, particle->tProd()*mm/c_light);

  vertex->SetPrimary(new G4PrimaryParticle(
    particle->id(), particle->pz(), particle->py(), -particle->px()));

  event->AddPrimaryVertex(vertex);
}

} } /* namespace MATHUSLA::MU */
