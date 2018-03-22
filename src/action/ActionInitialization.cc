#include "action/ActionInitialization.hh"

#include "action/RunAction.hh"
#include "action/EventAction.hh"
#include "action/SteppingAction.hh"
#include "action/TrackingAction.hh"
#include "action/StackingAction.hh"

#include "physics/Generator.hh"
#include "physics/PythiaGenerator.hh"
#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

ActionInitialization::ActionInitialization(const G4String& generator)
    : G4VUserActionInitialization(), _generator(generator) {}

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction);
}

void ActionInitialization::Build() const {
  SetUserAction(new RunAction());
  SetUserAction(new EventAction(100));
  SetUserAction(new SteppingAction());
  SetUserAction(new TrackingAction());
  SetUserAction(new StackingAction());

  Generator* generator;
  if (_generator == "pythia" || _generator == "pythia8") {
    auto pythia = new Pythia8::Pythia;
    pythia->readString("Print:quiet = on");
    pythia->readString("Next:numberCount = 10000");
    pythia->readString("Stat:showErrors = off");
    pythia->readString("Beams:eCM = 13000.");
    pythia->readString("WeakSingleBoson:ffbar2W = on");
    pythia->readString("24:onMode = off");
    pythia->readString("24:onIfAny = 13");
    pythia->init();
    generator = new PythiaGenerator(pythia, {13, 60, 0.07, -4, 4});
  } else if (_generator == "range") {
    generator = new RangeGenerator(13, 65*GeVperC, 0, 0);
  } else {
    generator = new Generator(13, 100*GeVperC, 0, 0);
  }

  SetUserAction(generator);
}

} } /* namespace MATHUSLA::MU */
