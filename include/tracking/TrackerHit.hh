#ifndef TRACKERHIT_HH
#define TRACKERHIT_HH
#pragma once

#include "Geant4/tls.hh"
#include "Geant4/G4VHit.hh"
#include "Geant4/G4THitsCollection.hh"
#include "Geant4/G4Allocator.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA {

class TrackerHit : public G4VHit {
 public:
  TrackerHit();
  TrackerHit(const TrackerHit&);
  virtual ~TrackerHit();

  const TrackerHit& operator=(const TrackerHit&);
  G4int operator==(const TrackerHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  virtual void Draw();
  virtual void Print();

  void SetTrackID(G4int track)     { fTrackID = track;   }
  void SetChamberNb(G4int chamber) { fChamber = chamber; }
  void SetEdep(G4double e)         { fDeposit = e;       }
  void SetTime(G4double t)         { fTime = t;          }
  void SetPos(G4ThreeVector xyz)   { fPos = xyz;         }

  G4int GetTrackID() const { return fTrackID; }
  G4int GetChamber() const { return fChamber; }
  G4double GetDeposit() const { return fDeposit; }
  G4double GetTime() const { return fTime; }
  G4ThreeVector GetPos() const { return fPos; }

 private:
  G4int fTrackID;
  G4int fChamber;
  G4double fDeposit;
  G4double fTime;
  G4ThreeVector fPos;
};

typedef G4THitsCollection<TrackerHit> TrackerHitsCollection;

extern G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator;

inline void* TrackerHit::operator new(size_t) {
  if (!TrackerHitAllocator) TrackerHitAllocator = new G4Allocator<TrackerHit>;
  return TrackerHitAllocator->MallocSingle();
}

inline void TrackerHit::operator delete(void* hit) {
  TrackerHitAllocator->FreeSingle(reinterpret_cast<TrackerHit*>(hit));
}

} /* namespace MATHUSLA */

#endif /* TRACKERHIT_HH */
