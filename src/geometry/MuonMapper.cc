#include "geometry/MuonMapper.hh"

#include "Geant4/G4NistManager.hh"
#include "Geant4/G4VProcess.hh"
#include "Geant4/G4RunManager.hh"

#include "action.hh"
#include "analysis.hh"
#include "util/time.hh"

namespace MATHUSLA { namespace MU {

namespace MuonMapper { /////////////////////////////////////////////////////////////////////////

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__MuonMapper Sensitive Material_______________________________________________________________
G4LogicalVolume* _box;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

namespace Material { ///////////////////////////////////////////////////////////////////////////
auto Stopper = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
} /* namespace Material */ /////////////////////////////////////////////////////////////////////

//__MuonMapper Data Variables___________________________________________________________________
const std::string& Detector::DataPrefix = "mu_map_event";
const std::vector<std::string>& Detector::DataKeys = {
  "R", "KE", "TimestampDate", "TimestampTime"};
//----------------------------------------------------------------------------------------------

//__Detector Constructor________________________________________________________________________
Detector::Detector() : G4VSensitiveDetector("MATHUSLA/MU/MuonMapper") {
  collectionName.insert("MuonMapper_HC");
  _box->SetSensitiveDetector(this);
}
//----------------------------------------------------------------------------------------------

//__Initalize Event_____________________________________________________________________________
void Detector::Initialize(G4HCofThisEvent*) {}
//----------------------------------------------------------------------------------------------

//__Hit Processing______________________________________________________________________________
G4bool Detector::ProcessHits(G4Step* step, G4TouchableHistory*) {
  const auto pre_step = step->GetPreStepPoint();
  const auto track = step->GetTrack();

  if (track->GetParticleDefinition()->GetParticleName() != "mu-") return false;

  const auto process = pre_step->GetProcessDefinedStep();
  const auto process_name = process->GetProcessName();
  if (process_name == "Transportation" && track->GetVolume() == track->GetNextVolume()) {

    return Analysis::FillNTuple(DataPrefix, 0, {
      (track->GetPosition() - G4ThreeVector(0, 0, 100*m)).mag() / cm,
      track->GetKineticEnergy() / MeV,
      std::stod(util::time::GetDate()),
      std::stod(util::time::GetTime())});
  }
  return false;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  constexpr double box_height = 1*m;
  _box = Construction::BoxVolume("StopperBox",
    200*m, 200*m, box_height,
    MuonMapper::Material::Stopper,
    Construction::SensitiveAttributes());

  return Construction::PlaceVolume(_box, world,
    G4Translate3D(100*m, 100*m, -0.5*box_height));
}
//----------------------------------------------------------------------------------------------

} /* namespace MuonMapper */ ///////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
