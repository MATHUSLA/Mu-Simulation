#ifndef TRACKERHIT_HH
#define TRACKERHIT_HH
#pragma once

#include "Geant4/G4Allocator.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4VHit.hh"
#include "Geant4/tls.hh"

namespace MATHUSLA { namespace MU {

class TrackerHit : public G4VHit {
 public:
  explicit TrackerHit(G4Step*);
  explicit TrackerHit(G4String, G4int, G4int, G4double, G4double, G4ThreeVector);

  G4int operator==(const TrackerHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  virtual void Draw();
  virtual void Print();

  G4String GetParticleName()  const { return fParticleName; }
  G4int GetTrackID()          const { return fTrackID;      }
  G4int GetChamberID()        const { return fChamberID;    }
  G4double GetEnergyDeposit() const { return fDeposit;      }
  G4double GetTime()          const { return fTime;         }
  G4ThreeVector GetPosition() const { return fPosition;     }

 private:
  G4String      fParticleName;
  G4int         fTrackID;
  G4int         fChamberID;
  G4double      fDeposit;
  G4double      fTime;
  G4ThreeVector fPosition;
};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

//______________________________________________________________________________
// inline functions

inline G4int TrackerHit::operator==(const TrackerHit& rhs) const {
    return this == &rhs;
}

extern G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator;

inline void* TrackerHit::operator new(size_t) {
  if (!TrackerHitAllocator) TrackerHitAllocator = new G4Allocator<TrackerHit>;
  return TrackerHitAllocator->MallocSingle();
}

inline void TrackerHit::operator delete(void* hit) {
  TrackerHitAllocator->FreeSingle(static_cast<TrackerHit*>(hit));
}

} } /* namespace MATHUSLA::MU */

#endif /* TRACKERHIT_HH */
