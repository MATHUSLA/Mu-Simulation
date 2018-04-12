#ifndef MU__TRACKING_PROTOTYPEHIT_HH
#define MU__TRACKING_PROTOTYPEHIT_HH
#pragma once

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4LorentzVector.hh"
#include "Geant4/G4VHit.hh"

namespace MATHUSLA { namespace MU {

class PrototypeHit : public G4VHit {
public:
  PrototypeHit(const std::string& particle,
               const int track,
               const int parent,
               const std::string& chamber,
               const double deposit,
               const G4LorentzVector position,
               const G4LorentzVector momentum);

  inline G4bool operator==(const PrototypeHit& rhs) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

  inline const std::string& GetParticleName() const { return _particle;  }
  inline int                GetTrackID()      const { return _trackID;   }
  inline int                GetParentID()     const { return _parentID;  }
  inline const std::string& GetChamberID()    const { return _chamberID; }
  inline double             GetDeposit()      const { return _deposit;   }
  inline G4LorentzVector    GetPosition()     const { return _position;  }
  inline G4LorentzVector    GetMomentum()     const { return _momentum;  }

private:
  std::string     _particle;
  int             _trackID;
  int             _parentID;
  std::string     _chamberID;
  double          _deposit;
  G4LorentzVector _position;
  G4LorentzVector _momentum;
};

using PrototypeHC = G4THitsCollection<PrototypeHit>;

extern G4ThreadLocal G4Allocator<PrototypeHit>* PrototypeHitAllocator;

inline G4bool PrototypeHit::operator==(const PrototypeHit& rhs) const {
  return this == &rhs;
}

inline void* PrototypeHit::operator new(size_t) {
  if (!PrototypeHitAllocator)
    PrototypeHitAllocator = new G4Allocator<PrototypeHit>;
  return PrototypeHitAllocator->MallocSingle();
}

inline void PrototypeHit::operator delete(void* hit) {
  PrototypeHitAllocator->FreeSingle(static_cast<PrototypeHit*>(hit));
}

} } /* namespace MATHUSLA::MU */

#endif /* MU__TRACKING_PROTOTYPEHIT_HH */
