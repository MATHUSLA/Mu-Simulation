#ifdef G4MULTITHREADED
#include "Geant4/G4MTRunManager.hh"
#endif

#include "Geant4/G4RunManager.hh"
#include "Geant4/FTFP_BERT.hh"
#include "Geant4/G4StepLimiterPhysics.hh"
#include "Geant4/G4UIExecutive.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4VisExecutive.hh"

#include "action/ActionInitialization.hh"
#include "detector/Construction.hh"
#include "physics/Units.hh"
#include "util/CommandLineParser.hh"

using Option = MATHUSLA::MU::CommandLineOption;

auto help_opt   = new Option('h', "help",   "Muon Simulation", Option::NoArguments);
auto gen_opt    = new Option('g', "gen",    "Generator",       Option::RequiredArguments);
auto script_opt = new Option('s', "script", "Custom Script",   Option::RequiredArguments);
auto events_opt = new Option('e', "events", "Event Count",     Option::RequiredArguments);
auto vis_opt    = new Option('v', "vis",    "Visualization",   Option::NoArguments);
auto quiet_opt  = new Option('q', "quiet",  "Quiet Mode",      Option::NoArguments);

auto thread_opt = new Option('j', "threads",
  "Multi-Threading Mode: Specify Optional number of threads (default: 2)",
  Option::OptionalArguments);

int main(int argc, char* argv[]) {
  MATHUSLA::MU::CommandLineParser::parse(argv, {
    help_opt, gen_opt, script_opt, events_opt, vis_opt, quiet_opt, thread_opt});

  G4UIExecutive* ui = nullptr;
  if (argc == 1 || vis_opt->count) {
    ui = new G4UIExecutive(argc, argv);
    vis_opt->count = 1;
  }

  if (script_opt->argument && events_opt->argument) {
    G4cout << "[FATAL ERROR] Incompatible Arguments: "
           << "a script OR an event count can be provided, but not both.\n";
    exit(EXIT_FAILURE);
  }

  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(time(0));

  #ifdef G4MULTITHREADED
    if (thread_opt->argument) {
      auto opt = std::string(thread_opt->argument);
      if (opt == "on") {
        thread_opt->count = 2;
      } else if (opt == "off" || opt == "0") {
        thread_opt->count = 1;
      } else {
        thread_opt->count = std::stoi(opt);
      }
    } else if (!thread_opt->count) {
      thread_opt->count = 2;
    }

    auto runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(thread_opt->count);

    if (thread_opt->count > 1) {
      G4cout << "Running " << thread_opt->count << " Threads.\n";
    } else {
      G4cout << "Running 1 Thread.\n";
    }
  #else
    auto runManager = new G4RunManager;
    G4cout << "Running in Single Threaded Mode.\n";
  #endif

  runManager->SetPrintProgress(1000);
  runManager->SetRandomNumberStore(false);

  MATHUSLA::MU::DefineNewUnits();

  auto physicsList = new FTFP_BERT;
  physicsList->RegisterPhysics(new G4StepLimiterPhysics);

  runManager->SetUserInitialization(physicsList);
  runManager->SetUserInitialization(new MATHUSLA::MU::Construction);

  auto generator = !gen_opt->argument ? "basic" : gen_opt->argument;
  runManager->SetUserInitialization(
    new MATHUSLA::MU::ActionInitialization(generator));

  auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();

  UImanager->ApplyCommand("/run/initialize");
  UImanager->ApplyCommand("/control/macroPath scripts/");
  UImanager->ApplyCommand("/control/saveHistory scripts/G4History");

  if (quiet_opt->count) {
    UImanager->ApplyCommand("/control/execute settings/quiet");
  } else {
    UImanager->ApplyCommand("/control/execute settings/verbose");
  }

  if (vis_opt->count) {
    UImanager->ApplyCommand("/control/execute settings/init_vis");
    if (ui->IsGUI())
      UImanager->ApplyCommand("/control/execute settings/init_gui");
  }

  if (script_opt->argument) {
    UImanager->ApplyCommand("/control/execute " + G4String(script_opt->argument));
  } else if (events_opt->argument) {
    UImanager->ApplyCommand("/run/beamOn " + G4String(events_opt->argument));
  }

  if (ui) {
    ui->SessionStart();
    delete ui;
  }

  delete visManager;
  delete runManager;
  return 0;
}
