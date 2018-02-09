#include "detector/DetectorConstruction.hh"

#include "Geant4/G4AutoDelete.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4GlobalMagFieldMessenger.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4SDParticleFilter.hh"
#include "Geant4/G4SolidStore.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4UserLimits.hh"
#include "Geant4/G4VisAttributes.hh"

#include "detector/TrackerSD.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = 0;

DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
      fNbOfChambers(0),
      fLogicTarget(NULL),
      fLogicChamber(NULL),
      fTargetMaterial(NULL),
      fChamberMaterial(NULL),
      fStepLimit(NULL),
      fCheckOverlaps(true) {

  fMessenger = new DetectorMessenger(this);

  fNbOfChambers = 5;
  fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
}

DetectorConstruction::~DetectorConstruction() {
  delete[] fLogicChamber;
  delete   fStepLimit;
  delete   fMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  DefineMaterials();
  return DefineVolumes();
}

void DetectorConstruction::DefineMaterials() {
  // Material definition
  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Lead defined using NIST Manager
  fTargetMaterial = nistManager->FindOrBuildMaterial("G4_Pb");

  // Xenon gas defined using NIST Manager
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Xe");

  // Define Elements
  G4Element* H  = new G4Element("Hydrogen", "H",  1.,     1.01*g/mole);
  G4Element* C  = new G4Element("Carbon",   "C",  6.,    12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen", "N",  7.,    14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen",   "O",  8.,    16.00*g/mole);
  G4Element* Al = new G4Element("Aluminum", "Al", 13.,  26.981*g/mole);
  G4Element* Si = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
  G4Element* Ca = new G4Element("Calcium",  "Ca", 20.,  40.078*g/mole);

  Air = new G4Material("Air", 1.290*mg/cm3, 2);
  Air->AddElement(N, 0.7);
  Air->AddElement(O, 0.3);

  Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Scintillator->AddElement(C, 9);
  Scintillator->AddElement(H, 10);

  const G4int nSci = 1;
  G4double eSci[nSci] = { 3.10*eV };
  G4double rSci[nSci] = { 1.58    };

  G4MaterialPropertiesTable* sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Scintillator->SetMaterialPropertiesTable(sciProp);

  CaCO3 = new G4Material("CaCO3", 2.71*g/cm3, 3);
  CaCO3->AddElement(C,  1);
  CaCO3->AddElement(Ca, 1);
  CaCO3->AddElement(O,  3);

  Kaolinite = new G4Material("Clay", 2.65*g/cm3, 4);
  Kaolinite->AddElement(Al, 2);
  Kaolinite->AddElement(Si, 2);
  Kaolinite->AddElement(O,  9);
  Kaolinite->AddElement(H,  4);

  Marl = new G4Material("Marl", 2.46*g/cm3, 2);
  Marl->AddMaterial(Kaolinite, 35*perCent);
  Marl->AddMaterial(CaCO3,     65*perCent);

  SiO2 = new G4Material("Quartz", 2.445*g/cm3, 2);
  SiO2->AddElement(Si, 46.743*perCent);
  SiO2->AddElement(O,  53.257*perCent);

  Mix = new G4Material("Mix", 2.54*g/cm3, 2);
  Mix->AddMaterial(Marl, 50*perCent);
  Mix->AddMaterial(SiO2, 50*perCent);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << "\n";
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes() {
  G4NistManager* nist  = G4NistManager::Instance();
  G4Material* trap_mat = nist->FindOrBuildMaterial("G4_Al");
  G4Material* pmt      = nist->FindOrBuildMaterial("G4_C");

  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(12000.0);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm\n";

  G4Box* worldS = new G4Box("world", 12000.0*cm, 12000.0*cm, 12000.0*cm);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldS, Air, "World");
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                // no rotation
                        G4ThreeVector(),  // at (0,0,0)
                        worldLV,          // its logical volume
                        "World",          // its name
                        0,                // its mother  volume
                        false,            // no boolean operations
                        0,                // copy number
                        fCheckOverlaps);  // checking overlaps

  // Earth Material

  G4Box* solidSandstone = new G4Box("Sandstone", 10000.0*cm, 10000.0*cm, 2265.0*cm);
  G4LogicalVolume* logSandstone = new G4LogicalVolume(solidSandstone, SiO2, "Sandstone");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 2265*cm-10000*cm),
                    logSandstone,
                    "Sandstone",
                    worldLV,
                    false,
                    0);

  G4Box* solidMarl = new G4Box("Marl", 10000.0*cm, 10000.0*cm, 912.5*cm);
  G4LogicalVolume* logMarl = new G4LogicalVolume(solidMarl, Marl, "Marl");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 5442.5*cm-10000*cm),
                    logMarl,
                    "Marl",
                    worldLV,
                    false,
                    0);

  G4Box* solidMix = new G4Box("Mix", 10000.0*cm, 10000.0*cm, 1822.5*cm);
  G4LogicalVolume* logMix = new G4LogicalVolume(solidMix, Mix, "Mix");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, 8177.5*cm-10000*cm),
                    logMix,
                    "Mix",
                    worldLV,
                    false,
                    0);

  // *************
  // TO DELETE ...
  // *************

  // Sizes of the principal geometrical components (solids)

  G4double chamberSpacing = 80*cm;  // from chamber center to center!

  G4double chamberWidth = 20.0*cm;  // width of the chambers
  G4double targetLength =  5.0*cm;  // full length of Target

  G4double trackerLength = (fNbOfChambers+1)*chamberSpacing;

  G4double worldLength = 1.2 * (2*targetLength + trackerLength);

  G4double targetRadius = 0.5*targetLength;   // Radius of Target
  targetLength          = 0.5*targetLength;   // Half length of the Target
  G4double trackerSize  = 0.5*trackerLength;  // Half length of the Tracker

  // Target

  G4ThreeVector positionTarget = G4ThreeVector(0, 0, -(targetLength+trackerSize));

  G4Tubs* targetS = new G4Tubs("target", 0., targetRadius, targetLength, 0.*deg, 360.*deg);

  fLogicTarget = new G4LogicalVolume(targetS, fTargetMaterial, "Target", 0, 0, 0);

  new G4PVPlacement(0,                // no rotation
                    positionTarget,   // at (x,y,z)
                    fLogicTarget,     // its logical volume
                    "Target",         // its name
                    worldLV,          // its mother volume
                    false,            // no boolean operations
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

  G4cout << "Target is " << 2*targetLength/cm << " cm of "
         << fTargetMaterial->GetName() << "\n";

  // Tracker

  G4ThreeVector positionTracker = G4ThreeVector(0, 0, 10250.0*cm-10000*cm);

  G4Tubs* trackerS = new G4Tubs("tracker", 0, trackerSize, trackerSize, 0.*deg, 360.*deg);
  G4LogicalVolume* trackerLV = new G4LogicalVolume(trackerS, Air, "Tracker", 0, 0, 0);

  new G4PVPlacement(0,                // no rotation
                    positionTracker,  // at (x,y,z)
                    trackerLV,        // its logical volume
                    "Tracker",        // its name
                    worldLV,          // its mother  volume
                    false,            // no boolean operations
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes

  G4VisAttributes* boxVisAtt     = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));

  worldLV->SetVisAttributes(boxVisAtt);
  fLogicTarget->SetVisAttributes(boxVisAtt);
  trackerLV->SetVisAttributes(boxVisAtt);

  // Tracker segments

  G4cout << "There are " << fNbOfChambers << " chambers in the tracker region.\n"
         << "The chambers are " << chamberWidth/cm << " cm of "
         << fChamberMaterial->GetName() << "\n"
         << "The distance between chamber is " << chamberSpacing/cm << " cm\n";

  G4double firstPosition = -trackerSize + chamberSpacing;
  G4double firstLength   = trackerLength/10;
  G4double lastLength    = trackerLength;

  G4double halfWidth = 0.4 * chamberWidth;
  G4double rmaxFirst = 0.5 * firstLength;

  G4double rmaxIncr = 0.0;
  if (fNbOfChambers > 0) {
    rmaxIncr =  0.5 * (lastLength-firstLength)/(fNbOfChambers-1);
    if (chamberSpacing  < chamberWidth) {
       G4Exception("DetectorConstruction::DefineVolumes()",
                   "InvalidSetup", FatalException,
                   "Width>Spacing");
    }
  }

  for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
      G4double Zposition = firstPosition + copyNo * chamberSpacing;
      G4double rmax = rmaxFirst + copyNo * rmaxIncr;

      G4Tubs* chamberS = new G4Tubs("Chamber_solid",
                                    0,
                                    rmax,
                                    halfWidth,
                                    0.*deg,
                                    360.*deg);

      fLogicChamber[copyNo] = new G4LogicalVolume(chamberS, fChamberMaterial, "Chamber_LV", 0, 0, 0);
      fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);

      new G4PVPlacement(0,                               // no rotation
                        G4ThreeVector(0, 0, Zposition),  // at (x,y,z)
                        fLogicChamber[copyNo],           // its logical volume
                        "Chamber_PV",                    // its name
                        trackerLV,                       // its mother  volume
                        false,                           // no boolean operations
                        copyNo,                          // copy number
                        fCheckOverlaps);                 // checking overlaps
  }

  G4double maxStep = 0.5 * chamberWidth;
  fStepLimit = new G4UserLimits(maxStep);
  trackerLV->SetUserLimits(fStepLimit);

  // *************
  // END TO DELETE
  // *************

  return worldPV;
}

void DetectorConstruction::ConstructSDandField() {
  TrackerSD* detector = new TrackerSD("MATHUSLA/MU/TrackerSD", "TrackerHC");
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);

  // Setting detector to all logical volumes with the same name
  // of "Chamber_LV".
  SetSensitiveDetector("Chamber_LV", detector, true);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

G4RotationMatrix* DetectorConstruction::AddMatrix(G4double th1,
                                                  G4double phi1,
                                                  G4double th2,
                                                  G4double phi2,
                                                  G4double th3,
                                                  G4double phi3) {
  G4double sinth1 = std::sin(th1);
  G4double costh1 = std::cos(th1);
  G4double sinth2 = std::sin(th2);
  G4double costh2 = std::cos(th2);
  G4double sinth3 = std::sin(th3);
  G4double costh3 = std::cos(th3);

  G4double sinph1 = std::sin(phi1);
  G4double cosph1 = std::cos(phi1);
  G4double sinph2 = std::sin(phi2);
  G4double cosph2 = std::cos(phi2);
  G4double sinph3 = std::sin(phi3);
  G4double cosph3 = std::cos(phi3);

  // xprime axis coordinates
  CLHEP::Hep3Vector xprime(sinth1 * cosph1, sinth1 * sinph1, costh1);
  // yprime axis coordinates
  CLHEP::Hep3Vector yprime(sinth2 * cosph2, sinth2 * sinph2, costh2);
  // zprime axis coordinates
  CLHEP::Hep3Vector zprime(sinth3 * cosph3, sinth3 * sinph3, costh3);

  G4RotationMatrix *rotMat = new G4RotationMatrix();
  rotMat->rotateAxes(xprime, yprime, zprime);
  if (*rotMat == G4RotationMatrix()) {
    delete rotMat;
    rotMat = 0;
  } else {
    rotMat->invert();
  }

  return rotMat;
}

void DetectorConstruction::SetTargetMaterial(G4String materialName) {
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTargetMaterial = pttoMaterial;
        if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
        G4cout << "\n----> The target is made of " << materialName << "\n";
     } else {
        G4cout << "\n-->  WARNING from SetTargetMaterial : "
               << materialName
               << " not found\n";
     }
  }
}

void DetectorConstruction::SetChamberMaterial(G4String materialName) {
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fChamberMaterial != pttoMaterial) {
     if (pttoMaterial) {
        fChamberMaterial = pttoMaterial;
        for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
            if (fLogicChamber[copyNo])
                fLogicChamber[copyNo]->SetMaterial(fChamberMaterial);
        }
        G4cout << "\n----> The chambers are made of " << materialName << "\n";
     } else {
        G4cout << "\n-->  WARNING from SetChamberMaterial : "
               << materialName
               << " not found\n";
     }
  }
}

void DetectorConstruction::SetMaxStep(G4double maxStep) {
  if (fStepLimit && (maxStep > 0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps) {
  fCheckOverlaps = checkOverlaps;
}

} } /* namespace MATHUSLA::MU */
