/*
 * include/tracking.hh
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MU__TRACKING_HH
#define MU__TRACKING_HH
#pragma once

#include <ostream>

#include <Geant4/G4Allocator.hh>
#include <Geant4/G4THitsCollection.hh>
#include <Geant4/G4LorentzVector.hh>
#include <Geant4/G4VHit.hh>
#include <Geant4/G4Step.hh>
#include <Geant4/G4Event.hh>
#include <Geant4/G4HCofThisEvent.hh>
#include <Geant4/G4VSensitiveDetector.hh>
#include <Geant4/G4ParticleDefinition.hh>

#include "analysis.hh"
#include "physics/Particle.hh"

namespace MATHUSLA { namespace MU {

namespace Tracking { ///////////////////////////////////////////////////////////////////////////

//__Hit Class Forward Declaration_______________________________________________________________
class Hit;
//----------------------------------------------------------------------------------------------

//__Hit Allocator Declaration___________________________________________________________________
extern G4ThreadLocal G4Allocator<Hit>* HitAllocator;
//----------------------------------------------------------------------------------------------

//__Hit Collection Definition___________________________________________________________________
using HitCollection = G4THitsCollection<Hit>;
//----------------------------------------------------------------------------------------------

//__Hit Class Definition________________________________________________________________________
class Hit : public G4VHit {
public:
  Hit(const G4ParticleDefinition* particle,
      const int track,
      const int parent,
      const std::string& chamber,
      const double deposit,
      const G4LorentzVector position,
      const G4LorentzVector momentum);

  Hit(const G4Step* step, bool post=true);

  void Draw();
  void Print(std::ostream& os=std::cout) const;
  void Print();

  const std::string&     GetParticleName() const { return _particle->GetParticleName(); }
  int                    GetPDGEncoding()  const { return _particle->GetPDGEncoding();  }
  int                    GetTrackID()      const { return _trackID;                     }
  int                    GetParentID()     const { return _parentID;                    }
  const std::string&     GetChamberID()    const { return _chamberID;                   }
  double                 GetDeposit()      const { return _deposit;                     }
  const G4LorentzVector& GetPosition()     const { return _position;                    }
  const G4LorentzVector& GetMomentum()     const { return _momentum;                    }

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
  const G4ParticleDefinition* _particle;
  int _trackID;
  int _parentID;
  std::string _chamberID;
  double _deposit;
  G4LorentzVector _position;
  G4LorentzVector _momentum;
};
//----------------------------------------------------------------------------------------------

//__Stream Hit__________________________________________________________________________________
std::ostream& operator<<(std::ostream& os,
                         const Hit& hit);
//----------------------------------------------------------------------------------------------

//__Stream Hit Collection_______________________________________________________________________
std::ostream& operator<<(std::ostream& os,
                         const HitCollection& hits);
//----------------------------------------------------------------------------------------------

//__Add Hit Collection to Detector______________________________________________________________
HitCollection* GenerateHitCollection(G4VSensitiveDetector* detector,
                                     G4HCofThisEvent* event);
//----------------------------------------------------------------------------------------------

//__Convert HitCollection to Analysis Form______________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const HitCollection* collection);
//----------------------------------------------------------------------------------------------

//__Convert HitCollection to Analysis Form______________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const HitCollection* collection,
                                                      const Analysis::ROOT::NameToDataMap& map);
//----------------------------------------------------------------------------------------------

//__Convert G4Event to Analysis Form____________________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const G4Event* event);
//----------------------------------------------------------------------------------------------

//__Convert ParticleVector to Analysis Form_____________________________________________________
const Analysis::ROOT::DataEntryList ConvertToAnalysis(const Physics::ParticleVector& particles);
//----------------------------------------------------------------------------------------------

} /* namespace Tracking */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__TRACKING_HH */
