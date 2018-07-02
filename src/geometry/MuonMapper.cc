#include "geometry/MuonMapper.hh"

#include <Geant4/G4NistManager.hh>
#include <Geant4/G4VProcess.hh>

#include "action.hh"
#include "analysis.hh"
#include "geometry/Earth.hh"

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
const std::string& Detector::DataName = "mu_map";
const Analysis::ROOT::DataKeyList Detector::DataKeys{
  "R", "logB"};
const Analysis::ROOT::DataKeyTypeList Detector::DataKeyTypes{
  Analysis::ROOT::DataKeyType::Single, Analysis::ROOT::DataKeyType::Single};
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
  try {
    if (track->GetParticleDefinition()->GetParticleName() != "mu-")
      return false;

    constexpr auto mass = 105.658369L;
    const auto process = pre_step->GetProcessDefinedStep();
    const auto process_name = process->GetProcessName();
    if (process_name == "Transportation" && track->GetVolume() == track->GetNextVolume()) {
      const auto kinetic = track->GetKineticEnergy() / MeV; // from GeV correction
      /*
      Analysis::ROOT::FillNTuple(DataPrefix, 0, {
        (track->GetPosition() - G4ThreeVector(0, 0, 100*m)).mag() / m,
        static_cast<double>(std::log10(std::sqrt(kinetic*kinetic + 2*kinetic*mass) / mass))});
      */
      track->SetTrackStatus(fStopAndKill);
      return true;
    }
  } catch (...) {}
  return false;
}
//----------------------------------------------------------------------------------------------

//__Post-Event Processing_______________________________________________________________________
void Detector::EndOfEvent(G4HCofThisEvent*) {}
//----------------------------------------------------------------------------------------------

//__Build Detector______________________________________________________________________________
G4VPhysicalVolume* Detector::Construct(G4LogicalVolume* world) {
  constexpr double box_height =   1*m;
  constexpr double box_width  =  20*m;
  constexpr double box_length = 400*m;
  constexpr double x_shift    =   0*m;

  _box = Construction::BoxVolume("StopperBox",
    box_length, box_width, box_height,
    MuonMapper::Material::Stopper,
    Construction::SensitiveAttributes());

  return Construction::PlaceVolume(_box, world,
    G4Translate3D(x_shift + 0.5 * box_length, 0, -0.5*box_height));
}
//----------------------------------------------------------------------------------------------

//__Build Earth for Detector____________________________________________________________________
G4VPhysicalVolume* Detector::ConstructEarth(G4LogicalVolume* world) {
  return Earth::Construct(world);
}
//----------------------------------------------------------------------------------------------

} /* namespace MuonMapper */ ///////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
