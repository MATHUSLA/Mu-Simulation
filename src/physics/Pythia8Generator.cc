#include "physics/Pythia8Generator.hh"

#include "Geant4/G4Event.hh"
#include "Geant4/G4TransportationManager.hh"
#include "Geant4/G4PhysicalConstants.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4RunManager.hh"

namespace MATHUSLA { namespace MU {

Pythia8Generator::Pythia8Generator(Pythia8::Pythia* pythia)
    : _hepmcevt(nullptr), _world(nullptr), _pythia(pythia), _hepmc_converter() {
}

void Pythia8Generator::GeneratePrimaryVertex(G4Event* event) {
  if (!_pythia) {
    G4cout << "Pythia8Generator: Pythia8 Generator not specified\n";
    G4RunManager::GetRunManager()->AbortRun();
  }

  if (!_world) _world = G4TransportationManager::GetTransportationManager()
                        ->GetNavigatorForTracking()
                        ->GetWorldVolume()
                        ->GetLogicalVolume()
                        ->GetSolid();

  // get the next pythia event
  constexpr int attempts = 10;
  int counter = 0;
  while (!_pythia->next() && counter++ <= attempts) {}

  if (counter >= attempts) {
    G4cout << "Pythia8Generator: no generated particles. run terminated...\n";
    G4RunManager::GetRunManager()->AbortRun();
    return;
  }

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
      if (!(*p_iter)->end_vertex() && (*p_iter)->is_undecayed()) {  // change to "is_undecayed()"status() == 1
        is_real_vertex = true;
        break;
      }
    }
    if (!is_real_vertex) continue;

    // check world boundary
    auto pos = vertex->position();
    G4LorentzVector xvtx(pos.x(), pos.y(), pos.z(), pos.t());
    if (_world->Inside(xvtx.vect()*mm) != kInside) continue;

    auto g4vtx = new G4PrimaryVertex(
      xvtx.x()*mm, xvtx.y()*mm, xvtx.z()*mm+100*m, xvtx.t()*mm/c_light);

    for (auto p_iter = vertex->particles_begin(HepMC::children);
              p_iter != vertex->particles_end(HepMC::children); ++p_iter) {
      auto particle = *p_iter;

      if (particle->status() != 1) continue;

      pos = particle->momentum();

      g4vtx->SetPrimary(new G4PrimaryParticle(
        particle->pdg_id(), pos.px()*GeV, pos.py()*GeV, pos.pz()*GeV));
    }
    event->AddPrimaryVertex(g4vtx);
  }
}

Pythia8GeneratorAction::Pythia8GeneratorAction() {
  auto pythia = new Pythia8::Pythia;
  pythia->readString("Beams:eCM = 13000.");
  pythia->readString("WeakSingleBoson:all = on");
  pythia->readString("PhaseSpace:pTHatMin = 20.");
  pythia->readString("23:onMode = 0");
  pythia->readString("24:onMode = 0");
  pythia->readString("23:onIfAll = 13");
  pythia->readString("24:onIfAll = 13");
  pythia->init();
  _p8generator = Pythia8Generator(pythia);
}

void Pythia8GeneratorAction::GeneratePrimaries(G4Event* event) {
  _p8generator.GeneratePrimaryVertex(event);
}

} } /* namespace MATHUSLA::MU */
