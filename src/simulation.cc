#ifdef G4MULTITHREADED
#include "Geant4/G4MTRunManager.hh"
#else
#include "Geant4/G4RunManager.hh"
#endif

#include "Geant4/G4UImanager.hh"
#include "Geant4/FTFP_BERT.hh"
#include "Geant4/G4StepLimiterPhysics.hh"
#include "Geant4/Randomize.hh"
#include "Geant4/G4VisExecutive.hh"
#include "Geant4/G4UIExecutive.hh"

#include "detector/DetectorConstruction.hh"
#include "action/ActionInitialization.hh"

int main(int argc, char** argv) {
  // Detect interactive mode (if no arguments) and define UI session
  G4UIExecutive* ui = 0;
  if (argc == 1) {
    ui = new G4UIExecutive(argc, argv);
  }

  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  // Set mandatory initialization classes
  runManager->SetUserInitialization(new MATHUSLA::DetectorConstruction());

  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  physicsList->RegisterPhysics(new G4StepLimiterPhysics());
  runManager->SetUserInitialization(physicsList);

  // Set user action classes
  runManager->SetUserInitialization(new MATHUSLA::ActionInitialization());

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  UImanager->ApplyCommand("/control/macroPath scripts/");

  // Process macro or start UI session

  if (!ui) {  // batch mode
    UImanager->ApplyCommand("/control/execute " + G4String(argv[1]));
  } else {  // interactive mode
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
