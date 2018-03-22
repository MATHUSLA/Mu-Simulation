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

void PythiaGenerator::GeneratePrimaryVertex(G4Event* event) {
  static const auto world_solid
    = Construction::WorldVolume->GetLogicalVolume()->GetSolid();

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

  // first result:
  //
  // PythiaGenerator:
  //   Attempt 74678 | Filtered Particle: 13 86.518 -0.0352423 -0.285994
  //

  // old code:
  /*
  // create the HepMC event from Pythia
  delete _hepmcevt;
  _hepmcevt = new HepMC::GenEvent;
  _hepmc_converter.fill_next_event(*_pythia, _hepmcevt);

  // convert HepMC to G4Event
  for (auto v_iter = _hepmcevt->vertices_begin();
            v_iter != _hepmcevt->vertices_end(); ++v_iter) {
    auto vertex = *v_iter;

    auto is_real_vertex = false;
    for (auto p_iter = vertex->particles_begin(HepMC::children);
              p_iter != vertex->particles_end(HepMC::children); ++p_iter) {
      if (!(*p_iter)->end_vertex() && (*p_iter)->is_undecayed()) {
        is_real_vertex = true;
        break;
      }
    }
    if (!is_real_vertex) continue;

    // check world boundary
    auto pos = vertex->position();
    G4LorentzVector xvtx(pos.z(), pos.y(), -pos.x(), pos.t());
    if (_world->Inside(xvtx.vect()*mm) != kInside) continue;

    auto g4vtx = new G4PrimaryVertex(
      xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm+100*m, xvtx.t()*mm/c_light);

    for (auto p_iter = vertex->particles_begin(HepMC::children);
              p_iter != vertex->particles_end(HepMC::children); ++p_iter) {
      auto particle = *p_iter;

      if (particle->status() != 1) continue;

      pos = particle->momentum();
      auto momentum = G4ThreeVector(pos.pz(), pos.py(), -pos.px());

      auto xbyz = momentum.x()/momentum.z();
      auto ybyz = momentum.y()/momentum.z();

      G4double cut = 0.2;

      if (momentum.z() > 0) continue;
      if (!(-cut < xbyz && xbyz < cut && -cut < ybyz && ybyz < cut)) continue;

      g4vtx->SetPrimary(new G4PrimaryParticle(
        particle->pdg_id(), momentum.x()*GeV, momentum.y()*GeV, momentum.z()*GeV));
    }
    event->AddPrimaryVertex(g4vtx);
  }
  */
}

PythiaGeneratorAction::PythiaGeneratorAction() {
  auto pythia = new Pythia8::Pythia;
  pythia->readString("Beams:eCM = 13000.");
  pythia->readString("WeakSingleBoson:ffbar2W = on");
  pythia->readString("24:onMode = off");
  pythia->readString("24:onIfAny = 13");
  pythia->init();
  PythiaFilter filter = {13, 60, 0.2, -25, 25};
  _p8generator = PythiaGenerator(pythia, filter);
}

void PythiaGeneratorAction::GeneratePrimaries(G4Event* event) {
  _p8generator.GeneratePrimaryVertex(event);
}

} } /* namespace MATHUSLA::MU */
