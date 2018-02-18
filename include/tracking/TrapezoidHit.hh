#ifndef TRACKING_TRACKERHIT_HH
#define TRACKING_TRACKERHIT_HH
#pragma once

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4VHit.hh"
#include "Geant4/tls.hh"

namespace MATHUSLA { namespace MU {

class TrapezoidHit : public G4VHit {
 public:
  TrapezoidHit(const G4String& particle,
               const G4int track,
               const G4String& chamber,
               const G4double deposit,
               const G4double time,
               const G4ThreeVector position,
               const G4double energy,
               const G4ThreeVector momentum);

  inline G4bool operator==(const TrapezoidHit& rhs) const;

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

using TrapezoidHC = G4THitsCollection<TrapezoidHit>;

extern G4ThreadLocal G4Allocator<TrapezoidHit>* TrapezoidHitAllocator;

inline G4bool TrapezoidHit::operator==(const TrapezoidHit& rhs) const {
  return this == &rhs;
}

inline void* TrapezoidHit::operator new(size_t) {
  if (!TrapezoidHitAllocator)
    TrapezoidHitAllocator = new G4Allocator<TrapezoidHit>;
  return TrapezoidHitAllocator->MallocSingle();
}

inline void TrapezoidHit::operator delete(void* hit) {
  TrapezoidHitAllocator->FreeSingle(static_cast<TrapezoidHit*>(hit));
}

} } /* namespace MATHUSLA::MU */

#endif /* TRACKING_TRACKERHIT_HH */
