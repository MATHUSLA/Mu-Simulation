#ifndef REGIONINFO_HH
#define REGIONINFO_HH
#pragma once

#include "Geant4/G4VUserRegionInformation.hh"

namespace MATHUSLA {

class RegionInfo : public G4VUserRegionInformation {
 public:
  RegionInfo();
  ~RegionInfo();

  enum {
    
  };

  virtual void Print() const;

};

} /* namespace MATHUSLA */

#endif /* REGIONINFO_HH */