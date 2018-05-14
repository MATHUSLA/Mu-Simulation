#ifndef MU__TRACKING_HH
#define MU__TRACKING_HH
#pragma once

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4LorentzVector.hh"
#include "Geant4/G4VHit.hh"

namespace MATHUSLA { namespace MU {

namespace Tracking { ///////////////////////////////////////////////////////////////////////////

class Hit;

using HitCollection = G4THitsCollection<Hit>;

extern G4ThreadLocal G4Allocator<Hit>* HitAllocator;

class Hit : public G4VHit {
public:
  Hit(const std::string& particle,
      const int track,
      const int parent,
      const std::string& chamber,
      const double deposit,
      const G4LorentzVector position,
      const G4LorentzVector momentum);

  void Draw();
  void Print();

  const std::string&     GetParticleName() const { return _particle;  }
  int                    GetTrackID()      const { return _trackID;   }
  int                    GetParentID()     const { return _parentID;  }
  const std::string&     GetChamberID()    const { return _chamberID; }
  double                 GetDeposit()      const { return _deposit;   }
  const G4LorentzVector& GetPosition()     const { return _position;  }
  const G4LorentzVector& GetMomentum()     const { return _momentum;  }

  bool operator==(const Hit& rhs) const {
    return this == &rhs;
  }

  void* operator new(size_t) {
    if (!HitAllocator) HitAllocator = new G4Allocator<Hit>;
    return HitAllocator->MallocSingle();
  }

  void operator delete(void* hit) {
    HitAllocator->FreeSingle(static_cast<Hit*>(hit));
  }

private:
  std::string     _particle;
  int             _trackID;
  int             _parentID;
  std::string     _chamberID;
  double          _deposit;
  G4LorentzVector _position;
  G4LorentzVector _momentum;
};

} /* namespace Tracking */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__TRACKING_HH */
