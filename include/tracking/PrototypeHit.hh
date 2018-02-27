#ifndef TRACKING_PROTOTYPEHIT_HH
#define TRACKING_PROTOTYPEHIT_HH
#pragma once

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4VHit.hh"
#include "Geant4/tls.hh"

namespace MATHUSLA { namespace MU {

class PrototypeHit : public G4VHit {
 public:
  PrototypeHit(const G4String& particle,
               const G4int track,
               const G4String& chamber,
               const G4double deposit,
               const G4double time,
               const G4ThreeVector position,
               const G4double energy,
               const G4ThreeVector momentum);

  inline G4bool operator==(const PrototypeHit& rhs) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

  inline const G4String& GetParticleName() const { return fParticleName; }
  inline G4int           GetTrackID()      const { return fTrackID;      }
  inline const G4String& GetChamberID()    const { return fChamberID;    }
  inline G4double        GetDeposit()      const { return fDeposit;      }
  inline G4double        GetTime()         const { return fTime;         }
  inline G4ThreeVector   GetPosition()     const { return fPosition;     }
  inline G4double        GetEnergy()       const { return fEnergy;       }
  inline G4ThreeVector   GetMomentum()     const { return fMomentum;     }

 private:
  G4String      fParticleName;
  G4int         fTrackID;
  G4String      fChamberID;
  G4double      fDeposit;
  G4double      fTime;
  G4ThreeVector fPosition;
  G4double      fEnergy;
  G4ThreeVector fMomentum;
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
