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
               const G4double ionizing,
               const G4double nonionizing,
               const G4double time,
               const G4ThreeVector position,
               const G4double energy,
               const G4ThreeVector momentum);

  inline G4bool operator==(const PrototypeHit& rhs) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  void Draw();
  void Print();

  inline const G4String& GetParticleName()       const { return _particle_name; }
  inline G4int           GetTrackID()            const { return _trackID;      }
  inline const G4String& GetChamberID()          const { return _chamberID;    }
  inline G4double        GetIonizingDeposit()    const { return _ionizing;     }
  inline G4double        GetNonIonizingDeposit() const { return _nonionizing;  }
  inline G4double        GetTotalDeposit()       const { return _ionizing
                                                              + _nonionizing;  }
  inline G4double        GetTime()               const { return _time;         }
  inline G4ThreeVector   GetPosition()           const { return _position;     }
  inline G4double        GetEnergy()             const { return _energy;       }
  inline G4ThreeVector   GetMomentum()           const { return _momentum;     }

private:
  G4String      _particle_name;
  G4int         _trackID;
  G4String      _chamberID;
  G4double      _ionizing;
  G4double      _nonionizing;
  G4double      _time;
  G4ThreeVector _position;
  G4double      _energy;
  G4ThreeVector _momentum;
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
