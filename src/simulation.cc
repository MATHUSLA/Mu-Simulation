#ifdef G4MULTITHREADED
#include "Geant4/G4MTRunManager.hh"
#else
#include "Geant4/G4RunManager.hh"
#endif

#include "Geant4/FTFP_BERT.hh"
#include "Geant4/G4StepLimiterPhysics.hh"
#include "Geant4/G4UIExecutive.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4VisExecutive.hh"

#include "action/ActionInitialization.hh"
#include "detector/Construction.hh"
#include "physics/Units.hh"

int main(int argc, char** argv) {
  G4UIExecutive* ui = 0;
  if (argc == 1) ui = new G4UIExecutive(argc, argv);

  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(time(0));

  #ifdef G4MULTITHREADED
    auto runManager = new G4MTRunManager;
  #else
    auto runManager = new G4RunManager;
  #endif

  MATHUSLA::MU::DefineNewUnits();

  auto physicsList = new FTFP_BERT;
  physicsList->RegisterPhysics(new G4StepLimiterPhysics);

  runManager->SetUserInitialization(physicsList);
  runManager->SetUserInitialization(new MATHUSLA::MU::Construction);
  runManager->SetUserInitialization(new MATHUSLA::MU::ActionInitialization);

  auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();

  UImanager->ApplyCommand("/control/macroPath scripts/");
  UImanager->ApplyCommand("/control/saveHistory scripts/G4History");
  UImanager->ApplyCommand("/run/initialize");
  UImanager->ApplyCommand("/control/execute verbosity.mac");

  if (!ui) {
    UImanager->ApplyCommand("/control/execute " + G4String(argv[1]));
  } else {
    UImanager->ApplyCommand("/control/execute vis.mac");
    if (ui->IsGUI()) UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
