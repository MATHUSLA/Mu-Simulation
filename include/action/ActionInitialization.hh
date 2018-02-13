#ifndef ACTION_ACTIONINITIALIZATION_HH
#define ACTION_ACTIONINITIALIZATION_HH
#pragma once

#include "Geant4/G4VUserActionInitialization.hh"

namespace MATHUSLA { namespace MU {

class ActionInitialization : public G4VUserActionInitialization {
 public:
  ActionInitialization();
  virtual ~ActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_ACTIONINITIALIZATION_HH */
