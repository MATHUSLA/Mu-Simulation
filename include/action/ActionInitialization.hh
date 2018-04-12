#ifndef MU__ACTION_ACTIONINITIALIZATION_HH
#define MU__ACTION_ACTIONINITIALIZATION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4VUserActionInitialization.hh"

namespace MATHUSLA { namespace MU {

class ActionInitialization : public G4VUserActionInitialization {
public:
  ActionInitialization(const std::string& generator="");

  void BuildForMaster() const;
  void Build() const;
private:
  std::string _generator;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__ACTION_ACTIONINITIALIZATION_HH */
