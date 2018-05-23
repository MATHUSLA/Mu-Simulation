#include "Geant4/G4MTRunManager.hh"
#include "Geant4/FTFP_BERT.hh"
#include "Geant4/G4StepLimiterPhysics.hh"
#include "Geant4/G4UIExecutive.hh"
#include "Geant4/G4UImanager.hh"
#include "Geant4/G4VisExecutive.hh"

#include "action.hh"
#include "detector/Construction.hh"
#include "physics/Units.hh"

#include "util/command_line_parser.hh"
#include "util/error.hh"

//__Main Function: Simulation___________________________________________________________________
int main(int argc, char* argv[]) {
  using namespace MATHUSLA;
  using namespace MATHUSLA::MU;

  using util::cli::option;

  option help_opt   ('h', "help",   "MATHUSLA Muon Simulation", option::no_arguments);
  option gen_opt    ('g', "gen",    "Generator",                option::required_arguments);
  option det_opt    ('d', "det",    "Detector",                 option::required_arguments);
  option script_opt ('s', "script", "Custom Script",            option::required_arguments);
  option events_opt ('e', "events", "Event Count",              option::required_arguments);
  option vis_opt    ('v', "vis",    "Visualization",            option::no_arguments);
  option quiet_opt  ('q', "quiet",  "Quiet Mode",               option::no_arguments);
  option thread_opt ('j', "threads",
    "Multi-Threading Mode: Specify Optional number of threads (default: 2)",
    option::optional_arguments);

  util::cli::parse(argv,
    {&help_opt, &gen_opt, &det_opt, &script_opt, &events_opt, &vis_opt, &quiet_opt, &thread_opt});

  G4UIExecutive* ui = nullptr;
  if (argc == 1 || vis_opt.count) {
    ui = new G4UIExecutive(argc, argv);
    vis_opt.count = 1;
  }

  util::error::exit_when(script_opt.argument && events_opt.argument,
    "[FATAL ERROR] Incompatible Arguments: ",
    "A script OR an event count can be provided, but not both.\n");

  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  G4Random::setTheSeed(time(nullptr));

  #ifdef G4MULTITHREADED
    if (thread_opt.argument) {
      auto opt = std::string(thread_opt.argument);
      if (opt == "on") {
        thread_opt.count = 2;
      } else if (opt == "off" || opt == "0") {
        thread_opt.count = 1;
      } else {
        try {
          thread_opt.count = std::stoi(opt);
        } catch(...) {
          thread_opt.count = 2;
        }
      }
    } else if (!thread_opt.count) {
      thread_opt.count = 2;
    }

    auto run = new G4MTRunManager;
    run->SetNumberOfThreads(thread_opt.count);

    if (thread_opt.count > 1) {
      std::cout << "Running " << thread_opt.count << " Threads.\n";
    } else {
      std::cout << "Running 1 Thread.\n";
    }
  #else
    auto run = new G4RunManager;
    std::cout << "Running in Single Threaded Mode.\n";
  #endif

  run->SetPrintProgress(1000);
  run->SetRandomNumberStore(false);

  Units::Define();

  auto physics = new FTFP_BERT;
  physics->RegisterPhysics(new G4StepLimiterPhysics);
  run->SetUserInitialization(physics);

  auto detector = det_opt.argument ? det_opt.argument : "Prototype";
  run->SetUserInitialization(new Construction::Builder(detector));

  auto generator = gen_opt.argument ? gen_opt.argument : "basic";
  run->SetUserInitialization(new ActionInitialization(generator));

  auto vis = new G4VisExecutive("Quiet");
  vis->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();

  UImanager->ApplyCommand("/run/initialize");
  UImanager->ApplyCommand("/control/macroPath scripts/");
  UImanager->ApplyCommand("/control/saveHistory scripts/G4History");
  UImanager->ApplyCommand(quiet_opt.count ? "/control/execute settings/quiet"
                                          : "/control/execute settings/verbose");

  if (vis_opt.count) {
    UImanager->ApplyCommand("/control/execute settings/init_vis");
    if (ui->IsGUI())
      UImanager->ApplyCommand("/control/execute settings/init_gui");
  }

  if (script_opt.argument) {
    UImanager->ApplyCommand("/control/execute " + std::string(script_opt.argument));
  } else if (events_opt.argument) {
    UImanager->ApplyCommand("/run/beamOn " + std::string(events_opt.argument));
  }

  if (ui) {
    ui->SessionStart();
    delete ui;
  }

  delete vis;
  delete run;
  return 0;
}
//----------------------------------------------------------------------------------------------
