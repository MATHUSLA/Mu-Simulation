#ifndef ACTION_RUNACTION_HH
#define ACTION_RUNACTION_HH
#pragma once

#include <fstream>

#include "Geant4/globals.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4Run.hh"

#include "util/FileIO.hh"
#include "util/Time.hh"

namespace MATHUSLA { namespace MU {

class RunAction : public G4UserRunAction {
public:
  RunAction();

  G4Run* GenerateRun();

  void BeginOfRunAction(const G4Run* run);
  void FillPerEvent(G4double deposit);
  void EndOfRunAction(const G4Run* run);

  static std::ofstream& GetStream() {
    static std::ofstream _file(
      "data/" + Time::GetDate() + "/" + Time::GetTime() + "/output.dat");
    return _file;
  }

};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_RUNACTION_HH */
