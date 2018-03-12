#ifndef TRACKING_PROTOTYPEHIT_HH
#define TRACKING_PROTOTYPEHIT_HH
#pragma once

#include <iostream>

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4LorentzVector.hh"
#include "Geant4/G4VHit.hh"
#include "Geant4/tls.hh"

namespace MATHUSLA { namespace MU {

class PrototypeHit : public G4VHit {
public:
  PrototypeHit(const G4String& particle,
               const G4int track,
               const G4int parent,
               const G4String& chamber,
               const G4double ionizing,
               const G4double nonionizing,
               const G4LorentzVector position,
               const G4LorentzVector momentum);

  inline G4bool operator==(const PrototypeHit& rhs) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();
  void Print(std::ostream& os);

  inline const G4String& GetParticleName()       const { return _particle;    }
  inline G4int           GetTrackID()            const { return _trackID;     }
  inline G4int           GetParentID()           const { return _parentID;    }
  inline const G4String& GetChamberID()          const { return _chamberID;   }
  inline G4double        GetIonizingDeposit()    const { return _ionizing;    }
  inline G4double        GetNonIonizingDeposit() const { return _nonionizing; }
  inline G4double        GetTotalDeposit()       const { return _ionizing
                                                              + _nonionizing; }
  inline G4LorentzVector GetPosition()           const { return _position;    }
  inline G4LorentzVector GetMomentum()           const { return _momentum;    }

private:
  G4String        _particle;
  G4int           _trackID;
  G4int           _parentID;
  G4String        _chamberID;
  G4double        _ionizing;
  G4double        _nonionizing;
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

#endif /* TRACKING_PROTOTYPEHIT_HH */
