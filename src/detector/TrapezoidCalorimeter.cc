#include "detector/TrapezoidCalorimeter.hh"

#include "Geant4/G4Trap.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4Box.hh"

#include "detector/Construction.hh"
#include "detector/PMT.hh"

namespace MATHUSLA { namespace MU {

G4Material* TrapezoidCalorimeter::Material::Scintillator = 0;
G4Material* TrapezoidCalorimeter::Material::Aluminum     = 0;

void TrapezoidCalorimeter::DefineMaterials() {
  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  const G4int nSci = 1;
  G4double eSci[nSci] = { 3.10*eV };
  G4double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);

  Material::Aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
}

G4VPhysicalVolume* TrapezoidCalorimeter::Construct(G4LogicalVolume* world) {
  auto Calorimeter = Construction::Volume(
    new G4Box("TrapezoidCalorimeter", 1000.0*cm, 1000.0*cm, 1000.0*cm),
    Construction::Material::Air);
  auto Cylinder = Construction::Volume(
    new G4Tubs("PMT",
      PMT::innerRadius,
      PMT::outerRadius,
      0.5 * PMT::hz,
      PMT::startAngle,
      PMT::spanningAngle),
    PMT::Material);

    // BOTTOM DETECTOR //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // env 1 //

      const unsigned int nc_1(5);
      G4double edge_1(35.3*cm);
      std::string childNames_1[nc_1] = {"B8", "B9", "B10", "B11", "B12"};
      G4double heights_1[nc_1] = {
        66.8484225245044*cm,
        37.8707804735234*cm,
        42.3673111366067*cm,
        55.8569031258564*cm,
        64.8499644520229*cm
      };
      std::string env1Name("Envelope1");

      G4double toth_1(0);
      for (unsigned int k=0; k<nc_1; ++k) toth_1 += heights_1[k];
      G4double cfac = tan(0.5*PMT::angle);
      G4double bl1_1 = 0.5*edge_1;
      G4double bl2_1 = bl1_1 + toth_1*cfac;
      G4double h1_1  = 0.5*PMT::thickness;

      auto solid1 = new G4Trap(env1Name, 0.5*toth_1, 0, 0, 0.5*PMT::envthickness, bl1_1, bl1_1, 0, 0.5*PMT::envthickness, bl2_1, bl2_1, 0);
      auto logE_1 = new G4LogicalVolume(solid1, Construction::Material::Air, env1Name);
      G4double zpos1 = -0.5*toth_1;
      G4double ypos1 = 0;
      for (unsigned int k=0; k<nc_1; ++k) {
        if (k == 0) {
          ypos1 = 13*cm;
        } else if (k == 1) {
          ypos1 = 3*cm;
        } else if (k == 2) {
          ypos1 = 8*cm;
        } else if (k == 3) {
          ypos1 = 13*cm;
        } else if (k == 4) {
          ypos1 = 18*cm;
        }
        zpos1 += 0.5*heights_1[k];
        bl2_1   = bl1_1 + heights_1[k]*cfac;

        auto  outersolid1a = new G4Trap("outerchildNames_1[k]", 0.5*heights_1[k], 0, 0, h1_1, bl1_1, bl1_1, 0, h1_1, bl2_1, bl2_1, 0);
        auto  innersolid1a = new G4Trap("innerchildNames_1[k]", 0.5*heights_1[k] - 0.1*cm, 0, 0, h1_1 - 0.1*cm, bl1_1 - 0.1*cm, bl1_1 - 0.1*cm, 0, h1_1 - 0.1*cm, bl2_1 - 0.1*cm, bl2_1 - 0.1*cm, 0);
        G4SubtractionSolid *hollow1 = new G4SubtractionSolid("Hollow 1",outersolid1a,innersolid1a);
        auto   child_1 = new G4LogicalVolume(hollow1, PMT::Material, "Hollow 1");

        new G4PVPlacement(0, G4ThreeVector(0, ypos1, zpos1), child_1, childNames_1[k],
              logE_1, false, 0);

        zpos1 += 0.5*heights_1[k];
        bl1_1   = bl2_1;
      }

      //Now the modules in mother
      G4double bl_1 = 0.5*tan(0.5*PMT::angle)*toth_1 + edge_1;
      G4double xpos_1 = 0.6*bl_1;
      G4double ypos_1 = -19*cm;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_1, ypos_1, -250.0*cm), logE_1, env1Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos1p = -0.54*toth_1;
      G4double zpos1p = 0;
      G4double xpos1p = -6*cm;
      for (unsigned int k=0; k<nc_1; ++k) {
        if (k == 0) {
          zpos1p = 13*cm;
        } else if (k == 1) {
          zpos1p = 3*cm;
        } else if (k == 2) {
          zpos1p = 8*cm;
        } else if (k == 3) {
          zpos1p = 13*cm;
        } else if (k == 4) {
          zpos1p = 18*cm;
        }
        ypos1p += heights_1[k];

        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos1p, ypos1p, zpos1p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_1x(0);
      const unsigned int nc_1x(5);
      std::string childNames_1x[nc_1x] = {"B8", "B9", "B10", "B11", "B12"};
      G4double    heights_1x[nc_1x]    =               {66.8484225245044*cm - 0.3*cm,
            37.8707804735234*cm - 0.3*cm, 42.3673111366067*cm - 0.3*cm,
            55.8569031258564*cm - 0.3*cm, 64.8499644520229*cm - 0.3*cm};
      std::string env1xName("Envelope1x");

      toth_1x = toth_1 - 0.3*cm;
      G4double bl1_1x  = 0.5*edge_1 - 0.15*cm;
      G4double bl2_1x  = bl1_1x + toth_1x*cfac;
      G4double h1_1x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid1x = new G4Trap(env1xName, 0.5*toth_1x, 0, 0, 0.5*PMT::envthickness, bl1_1x, bl1_1x, 0, 0.5*PMT::envthickness, bl2_1x, bl2_1x, 0);
      auto   logE_1x = new G4LogicalVolume(solid1x, Construction::Material::Air, env1xName);
      G4double zpos1x = -0.5*toth_1x;
      G4double ypos1x = 0;

      for (unsigned int k=0; k<nc_1x; ++k) {
        if (k == 0) {
          ypos1x = 13*cm;
        } else if (k == 1) {
          ypos1x = 3*cm;
        } else if (k == 2) {
          ypos1x = 8*cm;
        } else if (k == 3) {
          ypos1x = 13*cm;
        } else if (k == 4) {
          ypos1x = 18*cm;
        }
        bl2_1x   = bl1_1x + heights_1x[k]*cfac;
        zpos1x += 0.5*heights_1x[k] + 0.15*cm;

        auto  solid1xa = new G4Trap(childNames_1x[k], 0.5*heights_1x[k], 0, 0, h1_1x, bl1_1x, bl1_1x, 0, h1_1x, bl2_1x, bl2_1x, 0);
        auto   child_1x = new G4LogicalVolume(solid1xa, Material::Scintillator, childNames_1x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos1x, zpos1x), child_1x, childNames_1x[k],
              logE_1x, false, 0);
        zpos1x += 0.5*heights_1x[k] + 0.15*cm;
        bl1_1x   = bl2_1x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_1x, env1xName,
              logE_1, false, 0);

    // env 2 //

      const unsigned int nc_2(5);
      G4double edge_2(31*cm);
      std::string childNames_2[nc_2] = {"B7", "B8", "B9", "B10", "B11"};
      G4double    heights_2[nc_2]    = {55.5571344149842*cm,
            66.8484225245044*cm, 37.8707804735234*cm,
            42.3673111366067*cm, 55.8569031258564*cm};
      std::string env2Name("Envelope2");

      G4double toth_2(0);
      for (unsigned int k=0; k<nc_2; ++k) toth_2 += heights_2[k];
      G4double bl1_2  = 0.5*edge_2;
      G4double bl2_2  = bl1_2 + toth_2*cfac;
      G4double h1_2  = 0.5*PMT::thickness;

      auto solid2 = new G4Trap(env2Name, 0.5*toth_2, 0, 0, 0.5*PMT::envthickness, bl1_2, bl1_2, 0, 0.5*PMT::envthickness, bl2_2, bl2_2, 0);
      auto   logE_2 = new G4LogicalVolume(solid2, Construction::Material::Air, env2Name);
      G4double zpos2 = -0.5*toth_2;
      G4double ypos2 = 0;
      for (unsigned int k=0; k<nc_2; ++k) {
        if (k == 0) {
          ypos2 = 13*cm;
        } else if (k == 1) {
          ypos2 = 8*cm;
        } else if (k == 2) {
          ypos2 = 3*cm;
        } else if (k == 3) {
          ypos2 = 18*cm;
        } else if (k == 4) {
          ypos2 = 8*cm;
        }
        zpos2 += 0.5*heights_2[k];
        bl2_2   = bl1_2 + heights_2[k]*cfac;

        auto  outersolid2a = new G4Trap("outerchildNames_2[k]", 0.5*heights_2[k], 0, 0, h1_2, bl1_2, bl1_2, 0, h1_2, bl2_2, bl2_2, 0);
        auto  innersolid2a = new G4Trap("innerchildNames_2[k]", 0.5*heights_2[k] - 0.1*cm, 0, 0, h1_2 - 0.1*cm, bl1_2 - 0.1*cm, bl1_2 - 0.1*cm, 0, h1_2 - 0.1*cm, bl2_2 - 0.1*cm, bl2_2 - 0.1*cm, 0);
        G4SubtractionSolid *hollow2 = new G4SubtractionSolid("Hollow 2",outersolid2a,innersolid2a);
        auto   child_2 = new G4LogicalVolume(hollow2, PMT::Material, "Hollow 2");

        new G4PVPlacement(0, G4ThreeVector(0, ypos2, zpos2), child_2, childNames_2[k],
              logE_2, false, 0);
        zpos2 += 0.5*heights_2[k];
        bl1_2   = bl2_2;
      }

      //Now the modules in mother
      G4double bl_2 = 0.5*tan(0.5*PMT::angle)*toth_2 + edge_2;
      G4double xpos_2 = 0.47*bl_2 + 2.1*xpos_1;
      G4double ypos_2 = ypos_1 + 0.4*(toth_1 - toth_2);
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_2, ypos_2, -250.0*cm), logE_2, env2Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos2p = 0.41*toth_2;
      G4double zpos2p = 0;
      G4double xpos2p = xpos1p + 2.15*bl_1 + PMT::trx;
      for (unsigned int k=0; k<nc_2; ++k) {
        if (k == 0) {
          zpos2p = 13*cm;
        } else if (k == 1) {
          zpos2p = 8*cm;
        } else if (k == 2) {
          zpos2p = 3*cm;
        } else if (k == 3) {
          zpos2p = 18*cm;
        } else if (k == 4) {
          zpos2p = 8*cm;
        }
        ypos2p += -heights_2[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos2p, ypos2p, zpos2p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_2x(0);
      const unsigned int nc_2x(5);
      std::string childNames_2x[nc_2x] = {"B7", "B8", "B9", "B10", "B11"};
      G4double    heights_2x[nc_2x]    =               {55.5571344149842*cm - 0.3*cm,
            66.8484225245044*cm - 0.3*cm, 37.8707804735234*cm - 0.3*cm,
            42.3673111366067*cm - 0.3*cm, 55.8569031258564*cm - 0.3*cm};
      std::string env2xName("Envelope2x");

      toth_2x = toth_2 - 0.3*cm;
      G4double bl1_2x  = 0.5*edge_2 - 0.15*cm;
      G4double bl2_2x  = bl1_2x + toth_2x*cfac;
      G4double h1_2x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid2x = new G4Trap(env2xName, 0.5*toth_2x, 0, 0, 0.5*PMT::envthickness, bl1_2x, bl1_2x, 0, 0.5*PMT::envthickness, bl2_2x, bl2_2x, 0);
      auto   logE_2x = new G4LogicalVolume(solid2x, Construction::Material::Air, env2xName);
      G4double zpos2x = -0.5*toth_2x;
      G4double ypos2x = 0;

      for (unsigned int k=0; k<nc_2x; ++k) {
        if (k == 0) {
          ypos2x = 13*cm;
        } else if (k == 1) {
          ypos2x = 8*cm;
        } else if (k == 2) {
          ypos2x = 3*cm;
        } else if (k == 3) {
          ypos2x = 18*cm;
        } else if (k == 4) {
          ypos2x = 8*cm;
        }
        bl2_2x   = bl1_2x + heights_2x[k]*cfac;
        zpos2x += 0.5*heights_2x[k] + 0.15*cm;

        auto  solid2xa = new G4Trap(childNames_2x[k], 0.5*heights_2x[k], 0, 0, h1_2x, bl1_2x, bl1_2x, 0, h1_2x, bl2_2x, bl2_2x, 0);
        auto   child_2x = new G4LogicalVolume(solid2xa, Material::Scintillator, childNames_2x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos2x, zpos2x), child_2x, childNames_2x[k],
              logE_2x, false, 0);
        zpos2x += 0.5*heights_2x[k] + 0.15*cm;
        bl1_2x   = bl2_2x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_2x, env2xName,
              logE_2, false, 0);

    // env 3 //

      const unsigned int nc_3(4);
      G4double edge_3(42.8*cm);
      std::string childNames_3[nc_3] = {"C8", "C9", "C10", "C11"};
      G4double    heights_3[nc_3]    = {81.9367809717393*cm, 46.8638417996899*cm,
            52.2596785953898*cm, 69.2465722114821*cm};
      std::string env3Name("Envelope3");

      G4double toth_3(0);
      for (unsigned int k=0; k<nc_3; ++k) toth_3 += heights_3[k];
      G4double bl1_3  = 0.5*edge_3;
      G4double bl2_3  = bl1_3 + toth_3*cfac;
      G4double h1_3  = 0.5*PMT::thickness;

      auto  solid3 = new G4Trap(env3Name, 0.5*toth_3, 0, 0, 0.5*PMT::envthickness, bl1_3, bl1_3, 0, 0.5*PMT::envthickness, bl2_3, bl2_3, 0);
      auto   logE_3 = new G4LogicalVolume(solid3, Construction::Material::Air, env3Name);
      G4double zpos3 = -0.5*toth_3;
      G4double ypos3 = 0;
      for (unsigned int k=0; k<nc_3; ++k) {
        if (k == 0) {
          ypos3 = 3*cm;
        } else if (k == 1) {
          ypos3 = 18*cm;
        } else if (k == 2) {
          ypos3 = 13*cm;
        } else if (k == 3) {
          ypos3 = 18*cm;
        }
        zpos3 += 0.5*heights_3[k];
        bl2_3   = bl1_3 + heights_3[k]*cfac;

        auto  outersolid3a = new G4Trap("outerchildNames_3[k]", 0.5*heights_3[k], 0, 0, h1_3, bl1_3, bl1_3, 0, h1_3, bl2_3, bl2_3, 0);
        auto  innersolid3a = new G4Trap("innerchildNames_3[k]", 0.5*heights_3[k] - 0.1*cm, 0, 0, h1_3 - 0.1*cm, bl1_3 - 0.1*cm, bl1_3 - 0.1*cm, 0, h1_3 - 0.1*cm, bl2_3 - 0.1*cm, bl2_3 - 0.1*cm, 0);
        G4SubtractionSolid *hollow3 = new G4SubtractionSolid("Hollow 3",outersolid3a,innersolid3a);
        auto   child_3 = new G4LogicalVolume(hollow3, PMT::Material, "Hollow 3");

        new G4PVPlacement(0, G4ThreeVector(0, ypos3, zpos3), child_3, childNames_3[k],
              logE_3, false, 0);
        zpos3 += 0.5*heights_3[k];
        bl1_3   = bl2_3;
      }

      //Now the modules in mother
      G4double bl_3 = 0.5*tan(0.5*PMT::angle)*toth_3 + edge_3;
      G4double xpos_3 = -0.53*bl_3;
      G4double ypos_3 = -10.086815 - 0.5*(toth_1 - toth_3);
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_3, ypos_3, -250.0*cm), logE_3, env3Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos3p = 0.425*toth_3;
      G4double zpos3p = 0;
      G4double xpos3p = xpos1p + PMT::trx;
      for (unsigned int k=0; k<nc_3; ++k) {
        if (k == 0) {
          zpos3p = 13*cm;
        } else if (k == 1) {
          zpos3p = 8*cm;
        } else if (k == 2) {
          zpos3p = 3*cm;
        } else if (k == 3) {
          zpos3p = 18*cm;
        } else if (k == 4) {
          zpos3p = 8*cm;
        }
        ypos3p += -heights_3[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos3p, ypos3p, zpos3p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }


      // Inside //

      G4double toth_3x(0);
      const unsigned int nc_3x(4);
      std::string childNames_3x[nc_3x] = {"C8", "C9", "C10", "C11"};
      G4double    heights_3x[nc_3x]    = {81.9367809717393*cm - 0.3*cm, 46.8638417996899*cm - 0.3*cm,
            52.2596785953898*cm - 0.3*cm, 69.2465722114821*cm - 0.3*cm};
      std::string env3xName("Envelope3x");

      toth_3x = toth_3 - 0.3*cm;
      G4double bl1_3x  = 0.5*edge_3 - 0.15*cm;
      G4double bl2_3x  = bl1_3x + toth_3x*cfac;
      G4double h1_3x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid3x = new G4Trap(env3xName, 0.5*toth_3x, 0, 0, 0.5*PMT::envthickness, bl1_3x, bl1_3x, 0, 0.5*PMT::envthickness, bl2_3x, bl2_3x, 0);
      auto   logE_3x = new G4LogicalVolume(solid3x, Construction::Material::Air, env3xName);
      G4double zpos3x = -0.5*toth_3x;
      G4double ypos3x = 0;

      for (unsigned int k=0; k<nc_3x; ++k) {
        if (k == 0) {
          ypos3x = 3*cm;
        } else if (k == 1) {
          ypos3x = 18*cm;
        } else if (k == 2) {
          ypos3x = 13*cm;
        } else if (k == 3) {
          ypos3x = 18*cm;
        }
        bl2_3x   = bl1_3x + heights_3x[k]*cfac;
        zpos3x += 0.5*heights_3x[k] + 0.15*cm;

        auto  solid3xa = new G4Trap(childNames_3x[k], 0.5*heights_3x[k], 0, 0, h1_3x, bl1_3x, bl1_3x, 0, h1_3x, bl2_3x, bl2_3x, 0);
        auto   child_3x = new G4LogicalVolume(solid3xa, Material::Scintillator, childNames_3x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos3x, zpos3x), child_3x, childNames_3x[k],
              logE_3x, false, 0);
        zpos3x += 0.5*heights_3x[k] + 0.15*cm;
        bl1_3x   = bl2_3x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_3x, env3xName,
              logE_3, false, 0);

    // env 4 //

      const unsigned int nc_4(4);
      G4double edge_4 = edge_3;
      G4double toth_4 = toth_3;
      std::string childNames_4[nc_4] = {"C8", "C9", "C10", "C11"};
      G4double    heights_4[nc_4]    = {81.9367809717393*cm, 46.8638417996899*cm,
            52.2596785953898*cm, 69.2465722114821*cm};
      std::string env4Name("Envelope4");

      G4double bl1_4  = 0.5*edge_4;
      G4double bl2_4  = bl1_4 + toth_4*cfac;
      G4double h1_4  = 0.5*PMT::thickness;

      auto  solid4 = new G4Trap(env4Name, 0.5*toth_4, 0, 0, 0.5*PMT::envthickness, bl1_4, bl1_4, 0, 0.5*PMT::envthickness, bl2_4, bl2_4, 0);
      auto   logE_4 = new G4LogicalVolume(solid4, Construction::Material::Air, env4Name);
      G4double zpos4 = -0.5*toth_4;
      G4double ypos4 = 0;
      for (unsigned int k=0; k<nc_4; ++k) {
        if (k == 0) {
          ypos4 = 3*cm;
        } else if (k == 1) {
          ypos4 = 8*cm;
        } else if (k == 2) {
          ypos4 = 13*cm;
        } else if (k == 3) {
          ypos4 = 18*cm;
        }
        zpos4 += 0.5*heights_4[k];
        bl2_4   = bl1_4 + heights_4[k]*cfac;

        auto  outersolid4a = new G4Trap("outerchildNames_4[k]", 0.5*heights_4[k], 0, 0, h1_4, bl1_4, bl1_4, 0, h1_4, bl2_4, bl2_4, 0);
        auto  innersolid4a = new G4Trap("innerchildNames_4[k]", 0.5*heights_4[k] - 0.1*cm, 0, 0, h1_4 - 0.1*cm, bl1_4 - 0.1*cm, bl1_4 - 0.1*cm, 0, h1_4 - 0.1*cm, bl2_4 - 0.1*cm, bl2_4 - 0.1*cm, 0);
        G4SubtractionSolid *hollow4 = new G4SubtractionSolid("Hollow 4",outersolid4a,innersolid4a);
        auto   child_4 = new G4LogicalVolume(hollow4, PMT::Material, "Hollow 4");

        new G4PVPlacement(0, G4ThreeVector(0, ypos4, zpos4), child_4, childNames_4[k],
              logE_4, false, 0);
        zpos4 += 0.5*heights_4[k];
        bl1_4   = bl2_4;
      }

      //Now the modules in mother
      G4double bl_4 = 0.5*tan(0.5*PMT::angle)*toth_4 + edge_4;
      G4double xpos_4 = -0.53*bl_4 + 2.1*xpos_3;
      G4double ypos_4 = ypos_3 + 2.0*cm;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_4, ypos_4, -250.0*cm), logE_4, env4Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos4p = -0.5*toth_4;
      G4double zpos4p = 0;
      G4double xpos4p = xpos3p - 2.5*bl_3;
      for (unsigned int k=0; k<nc_4; ++k) {
        if (k == 0) {
          zpos4p = 13*cm;
        } else if (k == 1) {
          zpos4p = 3*cm;
        } else if (k == 2) {
          zpos4p = 8*cm;
        } else if (k == 3) {
          zpos4p = 13*cm;
        } else if (k == 4) {
          zpos4p = 18*cm;
        }
        ypos4p += heights_4[k];

        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos4p, ypos4p, zpos4p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_4x(0);
      const unsigned int nc_4x(4);
      std::string childNames_4x[nc_4x] = {"C8", "C9", "C10", "C11"};
      G4double    heights_4x[nc_4x]    = {81.9367809717393*cm - 0.3*cm, 46.8638417996899*cm - 0.3*cm,
            52.2596785953898*cm - 0.3*cm, 69.2465722114821*cm - 0.3*cm};
      std::string env4xName("Envelope4x");

      toth_4x = toth_4 - 0.3*cm;
      G4double bl1_4x  = 0.5*edge_4 - 0.15*cm;
      G4double bl2_4x  = bl1_4x + toth_4x*cfac;
      G4double h1_4x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid4x = new G4Trap(env4xName, 0.5*toth_4x, 0, 0, 0.5*PMT::envthickness, bl1_4x, bl1_4x, 0, 0.5*PMT::envthickness, bl2_4x, bl2_4x, 0);
      auto   logE_4x = new G4LogicalVolume(solid4x, Construction::Material::Air, env4xName);
      G4double zpos4x = -0.5*toth_4x;
      G4double ypos4x = 0;

      for (unsigned int k=0; k<nc_4x; ++k) {
        if (k == 0) {
          ypos4x = 3*cm;
        } else if (k == 1) {
          ypos4x = 8*cm;
        } else if (k == 2) {
          ypos4x = 13*cm;
        } else if (k == 3) {
          ypos4x = 18*cm;
        }
        bl2_4x   = bl1_4x + heights_4x[k]*cfac;
        zpos4x += 0.5*heights_4x[k] + 0.15*cm;

        auto  solid4xa = new G4Trap(childNames_4x[k], 0.5*heights_4x[k], 0, 0, h1_4x, bl1_4x, bl1_4x, 0, h1_4x, bl2_4x, bl2_4x, 0);
        auto   child_4x = new G4LogicalVolume(solid4xa, Material::Scintillator, childNames_4x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos4x, zpos4x), child_4x, childNames_4x[k],
              logE_4x, false, 0);
        zpos4x += 0.5*heights_4x[k] + 0.15*cm;
        bl1_4x   = bl2_4x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_4x, env4xName,
              logE_4, false, 0);

    // env 5 //

      const unsigned int nc_5(6);
      G4double edge_5(20.3*cm);
      std::string childNames_5[nc_5] = {"C2", "C3", "C4", "C5", "C6", "C7"};
      G4double    heights_5[nc_5]    = {32.4749436778235*cm,
            36.9714743409067*cm, 42.6670798474789*cm,
            49.5617601975399*cm, 57.8553611983379*cm,
            68.9468035006099*cm};
      std::string env5Name("Envelope5");

      G4double toth_5(0);
      for (unsigned int k=0; k<nc_5; ++k) toth_5 += heights_5[k];
      G4double bl1_5  = 0.5*edge_5;
      G4double bl2_5  = bl1_5 + toth_5*cfac;
      G4double h1_5  = 0.5*PMT::thickness;

      auto  solid5 = new G4Trap(env5Name, 0.5*toth_5, 0, 0, 0.5*PMT::envthickness, bl1_5, bl1_5, 0, 0.5*PMT::envthickness, bl2_5, bl2_5, 0);
      auto   logE_5 = new G4LogicalVolume(solid5, Construction::Material::Air, env5Name);
      G4double zpos5 = -0.5*toth_5;
      G4double ypos5 = 0;
      for (unsigned int k=0; k<nc_5; ++k) {
        if (k == 0) {
          ypos5 = 3*cm;
        } else if (k == 1) {
          ypos5 = 13*cm;
        } else if (k == 2) {
          ypos5 = 8*cm;
        } else if (k == 3) {
          ypos5 = 13*cm;
        } else if (k == 4) {
          ypos5 = 3*cm;
        } else if (k == 5) {
          ypos5 = 8*cm;
        }
        zpos5 += 0.5*heights_5[k];
        bl2_5   = bl1_5 + heights_5[k]*cfac;

        auto  outersolid5a = new G4Trap("outerchildNames_5[k]", 0.5*heights_5[k], 0, 0, h1_5, bl1_5, bl1_5, 0, h1_5, bl2_5, bl2_5, 0);
        auto  innersolid5a = new G4Trap("innerchildNames_5[k]", 0.5*heights_5[k] - 0.1*cm, 0, 0, h1_5 - 0.1*cm, bl1_5 - 0.1*cm, bl1_5 - 0.1*cm, 0, h1_5 - 0.1*cm, bl2_5 - 0.1*cm, bl2_5 - 0.1*cm, 0);
        G4SubtractionSolid *hollow5 = new G4SubtractionSolid("Hollow 5",outersolid5a,innersolid5a);
        auto   child_5 = new G4LogicalVolume(hollow5, PMT::Material, "Hollow 5");

        new G4PVPlacement(0, G4ThreeVector(0, ypos5, zpos5), child_5, childNames_5[k],
              logE_5, false, 0);
        zpos5 += 0.5*heights_5[k];
        bl1_5   = bl2_5;
      }

      //Now the modules in mother
      G4double bl_5 = 0.5*tan(0.5*PMT::angle)*toth_5 + edge_5;
      G4double xpos_5 = 0.5*bl_5 + 1.08*xpos_2 + 0.5*bl_2;
      G4double ypos_5 = 5*cm;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_5, ypos_5, -250.0*cm), logE_5, env5Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos5p = -0.455*toth_5;
      G4double zpos5p = 0;
      G4double xpos5p = xpos2p - PMT::trx;
      for (unsigned int k=0; k<nc_5; ++k) {
        if (k == 0) {
          zpos5p = 13*cm;
        } else if (k == 1) {
          zpos5p = 3*cm;
        } else if (k == 2) {
          zpos5p = 8*cm;
        } else if (k == 3) {
          zpos5p = 13*cm;
        } else if (k == 4) {
          zpos5p = 18*cm;
        }
        ypos5p += heights_5[k];

        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos5p, ypos5p, zpos5p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_5x(0);
      const unsigned int nc_5x(6);
      std::string childNames_5x[nc_5x] = {"C2", "C3", "C4", "C5", "C6", "C7"};
      G4double    heights_5x[nc_5x]    =               {32.4749436778235*cm - 0.3*cm,
            36.9714743409067*cm - 0.3*cm, 42.6670798474789*cm - 0.3*cm,
            49.5617601975399*cm - 0.3*cm, 57.8553611983379*cm - 0.3*cm,
            68.9468035006099*cm - 0.3*cm};
      std::string env5xName("Envelope5x");

      toth_5x = toth_5 - 0.3*cm;
      G4double bl1_5x  = 0.5*edge_5 - 0.15*cm;
      G4double bl2_5x  = bl1_5x + toth_5x*cfac;
      G4double h1_5x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid5x = new G4Trap(env5xName, 0.5*toth_5x, 0, 0, 0.5*PMT::envthickness, bl1_5x, bl1_5x, 0, 0.5*PMT::envthickness, bl2_5x, bl2_5x, 0);
      auto   logE_5x = new G4LogicalVolume(solid5x, Construction::Material::Air, env5xName);
      G4double zpos5x = -0.5*toth_5x;
      G4double ypos5x = 0;

      for (unsigned int k=0; k<nc_5x; ++k) {
        if (k == 0) {
          ypos5x = 3*cm;
        } else if (k == 1) {
          ypos5x = 13*cm;
        } else if (k == 2) {
          ypos5x = 8*cm;
        } else if (k == 3) {
          ypos5x = 13*cm;
        } else if (k == 4) {
          ypos5x = 3*cm;
        } else if (k == 5) {
          ypos5x = 8*cm;
        }
        bl2_5x   = bl1_5x + heights_5x[k]*cfac;
        zpos5x += 0.5*heights_5x[k] + 0.15*cm;

        auto  solid5xa = new G4Trap(childNames_5x[k], 0.5*heights_5x[k], 0, 0, h1_5x, bl1_5x, bl1_5x, 0, h1_5x, bl2_5x, bl2_5x, 0);
        auto   child_5x = new G4LogicalVolume(solid5xa, Material::Scintillator, childNames_5x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos5x, zpos5x), child_5x, childNames_5x[k],
              logE_5x, false, 0);
        zpos5x += 0.5*heights_5x[k] + 0.15*cm;
        bl1_5x   = bl2_5x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_5x, env5xName,
              logE_5, false, 0);

      // env 6 //

      const unsigned int nc_6(6);
      G4double edge_6(20.3*cm);
      std::string childNames_6[nc_6] = {"C2", "C3", "C4", "C5", "C6", "C7"};
      G4double    heights_6[nc_6]    = {32.4749436778235*cm,
            36.9714743409067*cm, 42.6670798474789*cm,
            49.5617601975399*cm, 57.8553611983379*cm,
            68.9468035006099*cm};
      std::string env6Name("Envelope6");

      G4double toth_6(0);
      for (unsigned int k=0; k<nc_6; ++k) toth_6 += heights_6[k];
      G4double bl1_6  = 0.5*edge_6;
      G4double bl2_6  = bl1_6 + toth_6*cfac;
      G4double h1_6  = 0.5*PMT::thickness;

      auto  solid6 = new G4Trap(env6Name, 0.5*toth_6, 0, 0, 0.5*PMT::envthickness, bl1_6, bl1_6, 0, 0.5*PMT::envthickness, bl2_6, bl2_6, 0);
      auto   logE_6 = new G4LogicalVolume(solid6, Construction::Material::Air, env6Name);
      G4double zpos6 = -0.5*toth_6;
      G4double ypos6 = 0;
      for (unsigned int k=0; k<nc_6; ++k) {
        if (k == 0) {
          ypos6 = 3*cm;
        } else if (k == 1) {
          ypos6 = 13*cm;
        } else if (k == 2) {
          ypos6 = 18*cm;
        } else if (k == 3) {
          ypos6 = 8*cm;
        } else if (k == 4) {
          ypos6 = 3*cm;
        } else if (k == 5) {
          ypos6 = 18*cm;
        }
        zpos6 += 0.5*heights_6[k];
        bl2_6   = bl1_6 + heights_6[k]*cfac;

        auto  outersolid6a = new G4Trap("outerchildNames_6[k]", 0.5*heights_6[k], 0, 0, h1_6, bl1_6, bl1_6, 0, h1_6, bl2_6, bl2_6, 0);
        auto  innersolid6a = new G4Trap("innerchildNames_6[k]", 0.5*heights_6[k] - 0.1*cm, 0, 0, h1_6 - 0.1*cm, bl1_6 - 0.1*cm, bl1_6 - 0.1*cm, 0, h1_6 - 0.1*cm, bl2_6 - 0.1*cm, bl2_6 - 0.1*cm, 0);
        G4SubtractionSolid *hollow6 = new G4SubtractionSolid("Hollow 6",outersolid6a,innersolid6a);
        auto   child_6 = new G4LogicalVolume(hollow6, PMT::Material, "Hollow 6");

        new G4PVPlacement(0, G4ThreeVector(0, ypos6, zpos6), child_6, childNames_6[k],
              logE_6, false, 0);
        zpos6 += 0.5*heights_6[k];
        bl1_6   = bl2_6;
      }

      //Now the modules in mother
      G4double xpos_6 = xpos_5 + 1.2*bl_5;
      G4double ypos_6 = ypos_5;
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_6, ypos_6, -250.0*cm), logE_6, env6Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos6p = 0.49*toth_6;
      G4double zpos6p = 0;
      G4double xpos6p = xpos5p + 2.45*bl_5 + PMT::trx;
      for (unsigned int k=0; k<nc_6; ++k) {
        if (k == 0) {
          zpos6p = 13*cm;
        } else if (k == 1) {
          zpos6p = 8*cm;
        } else if (k == 2) {
          zpos6p = 3*cm;
        } else if (k == 3) {
          zpos6p = 18*cm;
        } else if (k == 4) {
          zpos6p = 8*cm;
        }
        ypos6p += -heights_6[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos6p, ypos6p, zpos6p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_6x(0);
      const unsigned int nc_6x(6);
      std::string childNames_6x[nc_6x] = {"C2", "C3", "C4", "C5", "C6", "C7"};
      G4double    heights_6x[nc_6x]    =               {32.4749436778235*cm - 0.3*cm,
            36.9714743409067*cm - 0.3*cm, 42.6670798474789*cm - 0.3*cm,
            49.5617601975399*cm - 0.3*cm, 57.8553611983379*cm - 0.3*cm,
            68.9468035006099*cm - 0.3*cm};
      std::string env6xName("Envelope6x");

      toth_6x = toth_6 - 0.3*cm;
      G4double bl1_6x  = 0.5*edge_6 - 0.15*cm;
      G4double bl2_6x  = bl1_6x + toth_6x*cfac;
      G4double h1_6x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid6x = new G4Trap(env6xName, 0.5*toth_6x, 0, 0, 0.5*PMT::envthickness, bl1_6x, bl1_6x, 0, 0.5*PMT::envthickness, bl2_6x, bl2_6x, 0);
      auto   logE_6x = new G4LogicalVolume(solid6x, Construction::Material::Air, env6xName);
      G4double zpos6x = -0.5*toth_6x;
      G4double ypos6x = 0;

      for (unsigned int k=0; k<nc_6x; ++k) {
        if (k == 0) {
          ypos6x = 3*cm;
        } else if (k == 1) {
          ypos6x = 13*cm;
        } else if (k == 2) {
          ypos6x = 18*cm;
        } else if (k == 3) {
          ypos6x = 8*cm;
        } else if (k == 4) {
          ypos6x = 3*cm;
        } else if (k == 5) {
          ypos6x = 18*cm;
        }
        bl2_6x   = bl1_6x + heights_6x[k]*cfac;
        zpos6x += 0.5*heights_6x[k] + 0.15*cm;

        auto  solid6xa = new G4Trap(childNames_6x[k], 0.5*heights_6x[k], 0, 0, h1_6x, bl1_6x, bl1_6x, 0, h1_6x, bl2_6x, bl2_6x, 0);
        auto   child_6x = new G4LogicalVolume(solid6xa, Material::Scintillator, childNames_6x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos6x, zpos6x), child_6x, childNames_6x[k],
              logE_6x, false, 0);
        zpos6x += 0.5*heights_6x[k] + 0.15*cm;
        bl1_6x   = bl2_6x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_6x, env6xName,
              logE_6, false, 0);

    // env 7 //

      const unsigned int nc_7(6);
      G4double edge_7(15.1*cm);
      std::string childNames_7[nc_7] = {"B1", "B2", "B3", "B4", "B5", "B6"};
      G4double    heights_7[nc_7]    = {23.3819594480329*cm,
            26.4795694603792*cm, 30.2766397980939*cm,
            34.6732475575531*cm, 40.4687759677493*cm,
            46.963764703314*cm};
      std::string env7Name("Envelope7");

      G4double toth_7(0);
      for (unsigned int k=0; k<nc_7; ++k) toth_7 += heights_7[k];
      G4double bl1_7  = 0.5*edge_7;
      G4double bl2_7  = bl1_7 + toth_7*cfac;
      G4double h1_7  = 0.5*PMT::thickness;

      auto  solid7 = new G4Trap(env7Name, 0.5*toth_7, 0, 0, 0.5*PMT::envthickness, bl1_7, bl1_7, 0, 0.5*PMT::envthickness, bl2_7, bl2_7, 0);
      auto   logE_7 = new G4LogicalVolume(solid7, Construction::Material::Air, env7Name);
      G4double zpos7 = -0.5*toth_7;
      G4double ypos7 = 0;
      for (unsigned int k=0; k<nc_7; ++k) {
        if (k == 0) {
          ypos7 = 3*cm;
        } else if (k == 1) {
          ypos7 = 8*cm;
        } else if (k == 2) {
          ypos7 = 3*cm;
        } else if (k == 3) {
          ypos7 = 8*cm;
        } else if (k == 4) {
          ypos7 = 3*cm;
        } else if (k == 5) {
          ypos7 = 18*cm;
        }
        zpos7 += 0.5*heights_7[k];
        bl2_7  = bl1_7 + heights_7[k]*cfac;

        auto  outersolid7a = new G4Trap("outerchildNames_7[k]", 0.5*heights_7[k], 0, 0, h1_7, bl1_7, bl1_7, 0, h1_7, bl2_7, bl2_7, 0);
        auto  innersolid7a = new G4Trap("innerchildNames_7[k]", 0.5*heights_7[k] - 0.1*cm, 0, 0, h1_7 - 0.1*cm, bl1_7 - 0.1*cm, bl1_7 - 0.1*cm, 0, h1_7 - 0.1*cm, bl2_7 - 0.1*cm, bl2_7 - 0.1*cm, 0);
        G4SubtractionSolid *hollow7 = new G4SubtractionSolid("Hollow 7",outersolid7a,innersolid7a);
        auto   child_7 = new G4LogicalVolume(hollow7, PMT::Material, "Hollow 7");

        new G4PVPlacement(0, G4ThreeVector(0, ypos7, zpos7), child_7, childNames_7[k],
              logE_7, false, 0);
        zpos7 += 0.5*heights_7[k];
        bl1_7   = bl2_7;
      }

      //Now the modules in mother
      G4double bl_7 = 0.5*tan(0.5*PMT::angle)*toth_7 + edge_7;
      G4double xpos_7 = -0.4*bl_7 + 4.3*xpos_3;
      G4double ypos_7 = ypos_3 + 0.54*50.0613747156602*cm;
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7, ypos_7, -250.0*cm), logE_7, env7Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos7p = 0.55*toth_7;
      G4double zpos7p = 0;
      G4double xpos7p = xpos4p + PMT::trx;
      for (unsigned int k=0; k<nc_7; ++k) {
        if (k == 0) {
          zpos7p = 13*cm;
        } else if (k == 1) {
          zpos7p = 8*cm;
        } else if (k == 2) {
          zpos7p = 3*cm;
        } else if (k == 3) {
          zpos7p = 18*cm;
        } else if (k == 4) {
          zpos7p = 8*cm;
        }
        ypos7p += -heights_7[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos7p, ypos7p, zpos7p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_7x(0);
      const unsigned int nc_7x(6);
      std::string childNames_7x[nc_7x] = {"B1", "B2", "B3", "B4", "B5", "B6"};
      G4double    heights_7x[nc_7x]    =               {23.3819594480329*cm - 0.3*cm,
            26.4795694603792*cm - 0.3*cm, 30.2766397980939*cm - 0.3*cm,
            34.6732475575531*cm - 0.3*cm, 40.4687759677493*cm - 0.3*cm,
            46.963764703314*cm  - 0.3*cm};
      std::string env7xName("Envelope7x");

      toth_7x = toth_7 - 0.3*cm;
      G4double bl1_7x  = 0.5*edge_7 - 0.15*cm;
      G4double bl2_7x  = bl1_7x + toth_7x*cfac;
      G4double h1_7x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid7x = new G4Trap(env7xName, 0.5*toth_7x, 0, 0, 0.5*PMT::envthickness, bl1_7x, bl1_7x, 0, 0.5*PMT::envthickness, bl2_7x, bl2_7x, 0);
      auto   logE_7x = new G4LogicalVolume(solid7x, Construction::Material::Air, env7xName);
      G4double zpos7x = -0.5*toth_7x;
      G4double ypos7x = 0;

      for (unsigned int k=0; k<nc_7x; ++k) {
        if (k == 0) {
          ypos7x = 3*cm;
        } else if (k == 1) {
          ypos7x = 8*cm;
        } else if (k == 2) {
          ypos7x = 3*cm;
        } else if (k == 3) {
          ypos7x = 8*cm;
        } else if (k == 4) {
          ypos7x = 3*cm;
        } else if (k == 5) {
          ypos7x = 18*cm;
        }
        bl2_7x   = bl1_7x + heights_7x[k]*cfac;
        zpos7x += 0.5*heights_7x[k] + 0.15*cm;

        auto  solid7xa = new G4Trap(childNames_7x[k], 0.5*heights_7x[k], 0, 0, h1_7x, bl1_7x, bl1_7x, 0, h1_7x, bl2_7x, bl2_7x, 0);
        auto   child_7x = new G4LogicalVolume(solid7xa, Material::Scintillator, childNames_7x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos7x, zpos7x), child_7x, childNames_7x[k],
              logE_7x, false, 0);
        zpos7x += 0.5*heights_7x[k] + 0.15*cm;
        bl1_7x   = bl2_7x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_7x, env7xName,
              logE_7, false, 0);

    // env 8 //

      const unsigned int nc_8(6);
      G4double edge_8(15.1*cm);
      std::string childNames_8[nc_8] = {"B1", "B2", "B3", "B4", "B5", "B6"};
      G4double    heights_8[nc_8]    = {23.3819594480329*cm,
            26.4795694603792*cm, 30.2766397980939*cm,
            34.6732475575531*cm, 40.4687759677493*cm,
            46.963764703314*cm};
      std::string env8Name("Envelope8");

      G4double toth_8(0);
      for (unsigned int k=0; k<nc_8; ++k) toth_8 += heights_8[k];
      G4double bl1_8  = 0.5*edge_8;
      G4double bl2_8  = bl1_8 + toth_8*cfac;
      G4double h1_8  = 0.5*PMT::thickness;

      auto  solid8 = new G4Trap(env8Name, 0.5*toth_8, 0, 0, 0.5*PMT::envthickness, bl1_8, bl1_8, 0, 0.5*PMT::envthickness, bl2_8, bl2_8, 0);
      auto   logE_8 = new G4LogicalVolume(solid8, Construction::Material::Air, env8Name);
      G4double zpos8 = -0.5*toth_8;
      G4double ypos8 = 0;
      for (unsigned int k=0; k<nc_8; ++k) {
        if (k == 0) {
          ypos8 = 3*cm;
        } else if (k == 1) {
          ypos8 = 13*cm;
        } else if (k == 2) {
          ypos8 = 8*cm;
        } else if (k == 3) {
          ypos8 = 13*cm;
        } else if (k == 4) {
          ypos8 = 18*cm;
        } else if (k == 5) {
          ypos8 = 13*cm;
        }
        zpos8 += 0.5*heights_8[k];
        bl2_8   = bl1_8 + heights_8[k]*cfac;

        auto  outersolid8a = new G4Trap("outerchildNames_8[k]", 0.5*heights_8[k], 0, 0, h1_8, bl1_8, bl1_8, 0, h1_8, bl2_8, bl2_8, 0);
        auto  innersolid8a = new G4Trap("innerchildNames_8[k]", 0.5*heights_8[k] - 0.1*cm, 0, 0, h1_8 - 0.1*cm, bl1_8 - 0.1*cm, bl1_8 - 0.1*cm, 0, h1_8 - 0.1*cm, bl2_8 - 0.1*cm, bl2_8 - 0.1*cm, 0);
        G4SubtractionSolid *hollow8 = new G4SubtractionSolid("Hollow 8",outersolid8a,innersolid8a);
        auto   child_8 = new G4LogicalVolume(hollow8, PMT::Material, "Hollow 8");

        new G4PVPlacement(0, G4ThreeVector(0, ypos8, zpos8), child_8, childNames_8[k],
              logE_8, false, 0);
        zpos8 += 0.5*heights_8[k];
        bl1_8   = bl2_8;
      }

      //Now the modules in mother
      G4double bl_8 = 0.5*tan(0.5*PMT::angle)*toth_8 + edge_8;
      G4double xpos_8 = -1.2*bl_8 + xpos_7;
      G4double ypos_8 = ypos_3 + 0.55*49.0613747156602*cm;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_8, ypos_8, -250.0*cm), logE_8, env8Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos8p = -0.38*toth_8;
      G4double zpos8p = 0;
      G4double xpos8p = xpos7p - 3.1*bl_7;
      for (unsigned int k=0; k<nc_8; ++k) {
        if (k == 0) {
          zpos8p = 13*cm;
        } else if (k == 1) {
          zpos8p = 3*cm;
        } else if (k == 2) {
          zpos8p = 8*cm;
        } else if (k == 3) {
          zpos8p = 13*cm;
        } else if (k == 4) {
          zpos8p = 18*cm;
        }
        ypos8p += heights_8[k];

        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos8p, ypos8p, zpos8p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_8x(0);
      const unsigned int nc_8x(6);
      std::string childNames_8x[nc_8x] = {"B1", "B2", "B3", "B4", "B5", "B6"};
      G4double    heights_8x[nc_8x]    =               {23.3819594480329*cm - 0.3*cm,
            26.4795694603792*cm - 0.3*cm, 30.2766397980939*cm - 0.3*cm,
            34.6732475575531*cm - 0.3*cm, 40.4687759677493*cm - 0.3*cm,
            46.963764703314*cm  - 0.3*cm};
      std::string env8xName("Envelope8x");

      toth_8x = toth_8 - 0.3*cm;
      G4double bl1_8x  = 0.5*edge_8 - 0.15*cm;
      G4double bl2_8x  = bl1_8x + toth_8x*cfac;
      G4double h1_8x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid8x = new G4Trap(env8xName, 0.5*toth_8x, 0, 0, 0.5*PMT::envthickness, bl1_8x, bl1_8x, 0, 0.5*PMT::envthickness, bl2_8x, bl2_8x, 0);
      auto   logE_8x = new G4LogicalVolume(solid8x, Construction::Material::Air, env8xName);
      G4double zpos8x = -0.5*toth_8x;
      G4double ypos8x = 0;

      for (unsigned int k=0; k<nc_8x; ++k) {
        if (k == 0) {
          ypos8x = 3*cm;
        } else if (k == 1) {
          ypos8x = 13*cm;
        } else if (k == 2) {
          ypos8x = 8*cm;
        } else if (k == 3) {
          ypos8x = 13*cm;
        } else if (k == 4) {
          ypos8x = 18*cm;
        } else if (k == 5) {
          ypos8x = 13*cm;
        }
        bl2_8x  = bl1_8x + heights_8x[k]*cfac;
        zpos8x += 0.5*heights_8x[k] + 0.15*cm;

        auto  solid8xa = new G4Trap(childNames_8x[k], 0.5*heights_8x[k], 0, 0, h1_8x, bl1_8x, bl1_8x, 0, h1_8x, bl2_8x, bl2_8x, 0);
        auto   child_8x = new G4LogicalVolume(solid8xa, Material::Scintillator, childNames_8x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos8x, zpos8x), child_8x, childNames_8x[k],
              logE_8x, false, 0);
        zpos8x += 0.5*heights_8x[k] + 0.15*cm;
        bl1_8x   = bl2_8x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_8x, env8xName,
              logE_8, false, 0);

    // A9 //

      G4double edgeA9(25.8*cm);
      G4double bl1_A9 = 0.5*edgeA9;
      G4double bl2_A9 = 0.5*29.5*cm;
      G4double heightA9 = 50.0613747156602*cm;
      G4double h1_A9 = 0.5*PMT::thickness;

      auto  outersolidA9 = new G4Trap("outerA9", 0.5*heightA9, 0, 0, h1_A9, bl1_A9, bl1_A9, 0, h1_A9, bl2_A9, bl2_A9, 0);
      auto  innersolidA9 = new G4Trap("innerA9", 0.5*heightA9 - 0.1*cm, 0, 0, h1_A9 - 0.1*cm, bl1_A9 - 0.1*cm, bl1_A9 - 0.1*cm, 0, h1_A9 - 0.1*cm, bl2_A9 - 0.1*cm, bl2_A9 - 0.1*cm, 0);
      G4SubtractionSolid *hollowA9 = new G4SubtractionSolid("Hollow A9",outersolidA9,innersolidA9);
      auto   solidlogA9 = new G4LogicalVolume(hollowA9, PMT::Material, "A9");

      G4double xpos_A9 = 1.02*xpos_7;
      G4double ypos_A9 = -0.54*toth_7;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_A9, ypos_A9, -237.0*cm), solidlogA9, "A9",
              Calorimeter, false, 0);


      G4double edgeA9x(25.8*cm);
      G4double bl1_A9x = 0.5*edgeA9x - 0.15*cm;
      G4double bl2_A9x = 0.5*29.5*cm - 0.15*cm;
      G4double heightA9x = 50.0613747156602*cm - 0.3*cm;
      G4double h1_A9x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidA9x = new G4Trap("A9x", 0.5*heightA9x, 0, 0, h1_A9x, bl1_A9x, bl1_A9x, 0, h1_A9x, bl2_A9x, bl2_A9x, 0);
      auto   solidlogA9x = new G4LogicalVolume(solidA9x, Material::Scintillator, "A9x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_A9, ypos_A9, -237.0*cm), solidlogA9x, "A9x",
              Calorimeter, false, 0);

      G4double ypos_A9p = ypos7p - heightA9;
      G4double xpos_A9p = xpos7p;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_A9p, ypos_A9p, -237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A8_1 //

      G4double edge_8_1(22.3*cm);
      G4double bl1_8_1 = 0.5*edge_8_1;
      G4double bl2_8_1 = 0.5*25.6*cm;
      G4double height8_1 = 40.9683904858696*cm;
      G4double h1_8_1 = 0.5*PMT::thickness;

      auto  outersolid8_1 = new G4Trap("outerA8_1", 0.5*height8_1, 0, 0, h1_8_1, bl1_8_1, bl1_8_1, 0, h1_8_1, bl2_8_1, bl2_8_1, 0);
      auto  innersolid8_1 = new G4Trap("innerA8_1", 0.5*height8_1 - 0.1*cm, 0, 0, h1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, 0, h1_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow8_1 = new G4SubtractionSolid("Hollow A8_1",outersolid8_1,innersolid8_1);
      auto   solidlog8_1 = new G4LogicalVolume(hollow8_1, PMT::Material, "A8_1");

      G4double xpos_8_1 = 1.125*xpos_2;
      G4double ypos_8_1 = 0.519*toth_2;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_8_1, ypos_8_1, -247.0*cm), solidlog8_1, "A8_1",
              Calorimeter, false, 0);

      G4double edge_8_1x(22.3*cm);
      G4double bl1_8_1x = 0.5*edge_8_1x - 0.15*cm;
      G4double bl2_8_1x = 0.5*25.6*cm - 0.15*cm;
      G4double height8_1x = 40.9683904858696*cm - 0.3*cm;
      G4double h1_8_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solid8_1x = new G4Trap("A8_1x", 0.5*height8_1x, 0, 0, h1_8_1x, bl1_8_1x, bl1_8_1x, 0, h1_8_1x, bl2_8_1x, bl2_8_1x, 0);
      auto   solidlog8_1x = new G4LogicalVolume(solid8_1x, Material::Scintillator, "A8_1x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_8_1, ypos_8_1, -247.0*cm), solidlog8_1x, "A8_1x",
              Calorimeter, false, 0);

      G4double xpos_8_1p = xpos2p;
      G4double ypos_8_1p = ypos_8_1 - 0.65*height8_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_8_1p, ypos_8_1p, -247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A8_2 //

      auto  outersolid8_2 = new G4Trap("outerA8_2", 0.5*height8_1, 0, 0, h1_8_1, bl1_8_1, bl1_8_1, 0, h1_8_1, bl2_8_1, bl2_8_1, 0);
      auto  innersolid8_2 = new G4Trap("innerA8_2", 0.5*height8_1 - 0.1*cm, 0, 0, h1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, 0, h1_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow8_2 = new G4SubtractionSolid("Hollow A8_2",outersolid8_2,innersolid8_2);
      auto   solidlog8_2 = new G4LogicalVolume(hollow8_2, PMT::Material, "A8_2");

      G4double bl_8_2 = 0.5*tan(0.5*PMT::angle)*height8_1 + edge_8_1;
      G4double xpos_8_2 = xpos_8_1 - 1.05*bl_8_2;
      G4double ypos_8_2 = ypos_8_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_8_2, ypos_8_2, -242.0*cm), solidlog8_2, "A8_2",
              Calorimeter, false, 0);

      auto  solid8_2x = new G4Trap("A8_2x", 0.5*height8_1x, 0, 0, h1_8_1x, bl1_8_1x, bl1_8_1x, 0, h1_8_1x, bl2_8_1x, bl2_8_1x, 0);
      auto   solidlog8_2x = new G4LogicalVolume(solid8_2x, Material::Scintillator, "A8_2x");
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_8_2, ypos_8_2, -242.0*cm), solidlog8_2x, "A8_2x",
              Calorimeter, false, 0);

      G4double xpos_8_2p = xpos_8_1p - 2.1*bl_8_2 - PMT::trx;
      G4double ypos_8_2p = ypos_8_1p + height8_1 + PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_8_2p, ypos_8_2p, -242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A8_3 //

      auto  outersolid8_3 = new G4Trap("outerA8_3", 0.5*height8_1, 0, 0, h1_8_1, bl1_8_1, bl1_8_1, 0, h1_8_1, bl2_8_1, bl2_8_1, 0);
      auto  innersolid8_3 = new G4Trap("innerA8_3", 0.5*height8_1 - 0.1*cm, 0, 0, h1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, 0, h1_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow8_3 = new G4SubtractionSolid("Hollow A8_3",outersolid8_3,innersolid8_3);
      auto   solidlog8_3 = new G4LogicalVolume(hollow8_3, PMT::Material, "A8_3");

      G4double xpos_8_3 = xpos_8_2 - 1.05*bl_8_2;
      G4double ypos_8_3 = ypos_8_1 + 1*cm;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_8_3, ypos_8_3, -247.0*cm), solidlog8_3, "A8_3",
              Calorimeter, false, 0);

      auto  solid8_3x = new G4Trap("A8_3x", 0.5*height8_1x, 0, 0, h1_8_1x, bl1_8_1x, bl1_8_1x, 0, h1_8_1x, bl2_8_1x, bl2_8_1x, 0);
      auto   solidlog8_3x = new G4LogicalVolume(solid8_3x, Material::Scintillator, "A8_3x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_8_3, ypos_8_3, -247.0*cm), solidlog8_3x, "A8_3x",
              Calorimeter, false, 0);

      G4double ypos_8_3p = ypos_8_1p;
      G4double xpos_8_3p = xpos_8_2p + PMT::trx;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_8_3p, ypos_8_3p, -247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A8_4 //

      auto  outersolid8_4 = new G4Trap("outerA8_4", 0.5*height8_1, 0, 0, h1_8_1, bl1_8_1, bl1_8_1, 0, h1_8_1, bl2_8_1, bl2_8_1, 0);
      auto  innersolid8_4 = new G4Trap("innerA8_4", 0.5*height8_1 - 0.1*cm, 0, 0, h1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, bl1_8_1 - 0.1*cm, 0, h1_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, bl2_8_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow8_4 = new G4SubtractionSolid("Hollow A8_4",outersolid8_4,innersolid8_4);
      auto   solidlog8_4 = new G4LogicalVolume(hollow8_4, PMT::Material, "A8_4");

      G4double xpos_8_4 = xpos_8_3 - 1.05*bl_8_2;
      G4double ypos_8_4 = 1.01*ypos_8_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_8_4, ypos_8_4, -237.0*cm), solidlog8_4, "A8_4",
              Calorimeter, false, 0);

      auto  solid8_4x = new G4Trap("A8_4x", 0.5*height8_1x, 0, 0, h1_8_1x, bl1_8_1x, bl1_8_1x, 0, h1_8_1x, bl2_8_1x, bl2_8_1x, 0);
      auto   solidlog8_4x = new G4LogicalVolume(solid8_4x, Material::Scintillator, "A8_4x");
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_8_4, ypos_8_4, -237.0*cm), solidlog8_4x, "A8_4x",
              Calorimeter, false, 0);

      G4double ypos_8_4p = ypos_8_2p;
      G4double xpos_8_4p = xpos_8_3p - 2*bl_8_2 - PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_8_4p, ypos_8_4p, -237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7_1 //

      G4double edge_7_1(19.8*cm);
      G4double bl1_7_1 = 0.5*edge_7_1;
      G4double bl2_7_1 = 0.5*22.3*cm;
      G4double height7_1 = 34.1736330394327*cm;
      G4double h1_7_1 = 0.5*PMT::thickness;

      auto  outersolid7_1 = new G4Trap("outerA7_1", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_1 = new G4Trap("innerA7_1", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_1 = new G4SubtractionSolid("Hollow A7_1",outersolid7_1,innersolid7_1);
      auto   solidlog7_1 = new G4LogicalVolume(hollow7_1, PMT::Material, "A7_1");

      G4double xpos_7_1 = xpos_8_4 - 0.95*bl_8_2;
      G4double ypos_7_1 = 0.99*ypos_8_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_1, ypos_7_1, -232.0*cm), solidlog7_1, "A7_1",
              Calorimeter, false, 0);

      G4double edge_7_1x(19.8*cm);
      G4double bl1_7_1x = 0.5*edge_7_1x - 0.15*cm;
      G4double bl2_7_1x = 0.5*22.3*cm - 0.15*cm;
      G4double height7_1x = 34.1736330394327*cm - 0.3*cm;
      G4double h1_7_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solid7_1x = new G4Trap("A7_1x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_1x = new G4LogicalVolume(solid7_1x, Material::Scintillator, "A7_1x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_1, ypos_7_1, -232.0*cm), solidlog7_1x, "A7_1x",
              Calorimeter, false, 0);

      G4double ypos_7_1p = ypos_7_1 - 0.75*height7_1;
      G4double xpos_7_1p = xpos_8_4p + 0.9*PMT::trx;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_7_1p, ypos_7_1p, -232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7_2 //

      auto  outersolid7_2 = new G4Trap("outerA7_2", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_2 = new G4Trap("innerA7_2", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_2 = new G4SubtractionSolid("Hollow A7_2",outersolid7_2,innersolid7_2);
      auto   solidlog7_2 = new G4LogicalVolume(hollow7_2, PMT::Material, "A7_2");

      G4double bl_7_2 = 0.5*tan(0.5*PMT::angle)*height7_1 + edge_7_1;
      G4double xpos_7_2 = xpos_7_1 - bl_7_2;
      G4double ypos_7_2 = 0.99*ypos_8_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_2, ypos_7_2, -242.0*cm), solidlog7_2, "A7_2",
              Calorimeter, false, 0);

      auto  solid7_2x = new G4Trap("A7_2x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_2x = new G4LogicalVolume(solid7_2x, Material::Scintillator, "A7_2x");
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_2, ypos_7_2, -242.0*cm), solidlog7_2x, "A7_2x",
              Calorimeter, false, 0);

      G4double ypos_7_2p = ypos_8_4p - 0.7*(height8_1 - height7_1);
      G4double xpos_7_2p = xpos_7_1p - 2*bl_7_2 - PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_7_2p, ypos_7_2p, -242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

      // A7_3 //

      auto  outersolid7_3 = new G4Trap("outerA7_3", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_3 = new G4Trap("innerA7_3", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_3 = new G4SubtractionSolid("Hollow A7_3",outersolid7_3,innersolid7_3);
      auto   solidlog7_3 = new G4LogicalVolume(hollow7_3, PMT::Material, "A7_3");

      G4double xpos_7_3 = xpos_7_2 - bl_7_2;
      G4double ypos_7_3 = ypos_8_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_3, ypos_7_3, -237.0*cm), solidlog7_3, "A7_3",
              Calorimeter, false, 0);

      auto  solid7_3x = new G4Trap("A7_3x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_3x = new G4LogicalVolume(solid7_3x, Material::Scintillator, "A7_3x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_3, ypos_7_3, -237.0*cm), solidlog7_3x, "A7_3x",
              Calorimeter, false, 0);

      G4double ypos_7_3p = 1.015*ypos_7_1p;
      G4double xpos_7_3p = xpos_7_2p + PMT::trx;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_7_3p, ypos_7_3p, -237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7_4 //

      auto  outersolid7_4 = new G4Trap("outerA7_4", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_4 = new G4Trap("innerA7_4", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_4 = new G4SubtractionSolid("Hollow A7_4",outersolid7_4,innersolid7_4);
      auto   solidlog7_4 = new G4LogicalVolume(hollow7_4, PMT::Material, "A7_4");

      G4double xpos_7_4 = xpos_7_3 - bl_7_2;
      G4double ypos_7_4 = ypos_8_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_4, ypos_7_4, -247.0*cm), solidlog7_4, "A7_4",
              Calorimeter, false, 0);

      auto  solid7_4x = new G4Trap("A7_4x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_4x = new G4LogicalVolume(solid7_4x, Material::Scintillator, "A7_4x");
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_4, ypos_7_4, -247.0*cm), solidlog7_4x, "A7_4x",
              Calorimeter, false, 0);

      G4double ypos_7_4p = 1.01*ypos_7_2p;
      G4double xpos_7_4p = xpos_7_2p - 2*bl_7_2;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_7_4p, ypos_7_4p, -247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7_5 //

      auto  outersolid7_5 = new G4Trap("outerA7_5", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_5 = new G4Trap("innerA7_5", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_5 = new G4SubtractionSolid("Hollow A7_5",outersolid7_5,innersolid7_5);
      auto   solidlog7_5 = new G4LogicalVolume(hollow7_5, PMT::Material, "A7_5");

      G4double xpos_7_5 = xpos_7_4 - bl_7_2;
      G4double ypos_7_5 = 1.01*ypos_8_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_5, ypos_7_5, -242.0*cm), solidlog7_5, "A7_5",
              Calorimeter, false, 0);

      auto  solid7_5x = new G4Trap("A7_5x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_5x = new G4LogicalVolume(solid7_5x, Material::Scintillator, "A7_5x");
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_7_5, ypos_7_5, -242.0*cm), solidlog7_5x, "A7_5x",
              Calorimeter, false, 0);

      G4double ypos_7_5p = 1.01*ypos_7_3p;
      G4double xpos_7_5p = xpos_7_4p + PMT::trx;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_7_5p, ypos_7_5p, -242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7_6 //

      auto  outersolid7_6 = new G4Trap("outerA7_6", 0.5*height7_1, 0, 0, h1_7_1, bl1_7_1, bl1_7_1, 0, h1_7_1, bl2_7_1, bl2_7_1, 0);
      auto  innersolid7_6 = new G4Trap("innerA7_6", 0.5*height7_1 - 0.1*cm, 0, 0, h1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, bl1_7_1 - 0.1*cm, 0, h1_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, bl2_7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollow7_6 = new G4SubtractionSolid("Hollow A7_6",outersolid7_6,innersolid7_6);
      auto   solidlog7_6 = new G4LogicalVolume(hollow7_6, PMT::Material, "A7_6");

      G4double xpos_7_6 = xpos_7_5 - bl_7_2;
      G4double ypos_7_6 = 1.011*ypos_8_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_6, ypos_7_6, -232.0*cm), solidlog7_6, "A7_6",
              Calorimeter, false, 0);

      auto  solid7_6x = new G4Trap("A7_6x", 0.5*height7_1x, 0, 0, h1_7_1x, bl1_7_1x, bl1_7_1x, 0, h1_7_1x, bl2_7_1x, bl2_7_1x, 0);
      auto   solidlog7_6x = new G4LogicalVolume(solid7_6x, Material::Scintillator, "A7_6x");
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_7_6, ypos_7_6, -232.0*cm), solidlog7_6x, "A7_6x",
              Calorimeter, false, 0);

      G4double ypos_7_6p = 1.01*ypos_7_4p;
      G4double xpos_7_6p = xpos_7_4p - 2*bl_7_2;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_7_6p, ypos_7_6p, -232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // TOP DETECTOR // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // env 9 //

      const unsigned int nc_9(4);
      G4double edge_9(27*cm);
      std::string childNames_9[nc_9] = {"B6", "B7", "B8", "B9"};
      G4double    heights_9[nc_9]    = {46.963764703314*cm,
            55.5571344149842*cm, 66.8484225245044*cm,
            37.8707804735234*cm};
      std::string env9Name("Envelope9");

      G4double toth_9(0);
      for (unsigned int k=0; k<nc_9; ++k) toth_9 += heights_9[k];
      G4double bl1_9  = 0.5*edge_9;
      G4double bl2_9  = bl1_9 + toth_9*cfac;
      G4double h1_9  = 0.5*PMT::thickness;

      auto  solid9 = new G4Trap(env9Name, 0.5*toth_9, 0, 0, 0.5*PMT::envthickness, bl1_9, bl1_9, 0, 0.5*PMT::envthickness, bl2_9, bl2_9, 0);
      auto   logE_9 = new G4LogicalVolume(solid9, Construction::Material::Air, env9Name);
      G4double zpos9 = -0.5*toth_9;
      G4double ypos9 = 0;
      for (unsigned int k=0; k<nc_9; ++k) {
        if (k == 0) {
          ypos9 = -8*cm;
        } else if (k == 1) {
          ypos9 = -3*cm;
        } else if (k == 2) {
          ypos9 = -8*cm;
        } else if (k == 3) {
          ypos9 = -3*cm;
        }
        zpos9 += 0.5*heights_9[k];
        bl2_9   = bl1_9 + heights_9[k]*cfac;

        auto  outersolid9a = new G4Trap("outerchildNames_9[k]", 0.5*heights_9[k], 0, 0, h1_9, bl1_9, bl1_9, 0, h1_9, bl2_9, bl2_9, 0);
        auto  innersolid9a = new G4Trap("innerchildNames_9[k]", 0.5*heights_9[k] - 0.1*cm, 0, 0, h1_9 - 0.1*cm, bl1_9 - 0.1*cm, bl1_9 - 0.1*cm, 0, h1_9 - 0.1*cm, bl2_9 - 0.1*cm, bl2_9 - 0.1*cm, 0);
        G4SubtractionSolid *hollow9 = new G4SubtractionSolid("Hollow 9",outersolid9a,innersolid9a);
        auto   child_9 = new G4LogicalVolume(hollow9, PMT::Material, "Hollow 9");

        new G4PVPlacement(0, G4ThreeVector(0, ypos9, zpos9), child_9, childNames_9[k],
              logE_9, false, 0);

        zpos9 += 0.5*heights_9[k];
        bl1_9   = bl2_9;
      }

      //Now the modules in mother
      G4double bl_9 = 0.5*tan(0.5*PMT::angle)*toth_9 + edge_9;
      G4double xpos_9 = 65.86*cm + 0.6*bl_9;
      G4double ypos_9 = -42.8707804735234*cm;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_9, ypos_9, 250.0*cm), logE_9, env9Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos9p = -0.67*toth_9;
      G4double zpos9p = 0;
      G4double xpos9p = xpos_9 - 0.82*bl_9;
      for (unsigned int k=0; k<nc_9; ++k) {
        if (k == 0) {
          zpos9p = -8*cm;
        } else if (k == 1) {
          zpos9p = -3*cm;
        } else if (k == 2) {
          zpos9p = -8*cm;
        } else if (k == 3) {
          zpos9p = -3*cm;
        }
        ypos9p += heights_9[k];

        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos9p, ypos9p, zpos9p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_9x(0);
      const unsigned int nc_9x(4);
      std::string childNames_9x[nc_9x] = {"B6", "B7", "B8", "B9"};
      G4double    heights_9x[nc_9x]    =    {46.963764703314*cm - 0.3*cm,
            55.5571344149842*cm - 0.3*cm, 66.8484225245044*cm - 0.3*cm,
            37.8707804735234*cm - 0.3*cm};
      std::string env9xName("Envelope9x");

      toth_9x = toth_9 - 0.3*cm;
      G4double bl1_9x  = 0.5*edge_9 - 0.15*cm;
      G4double bl2_9x  = bl1_9x + toth_9x*cfac;
      G4double h1_9x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid9x = new G4Trap(env9xName, 0.5*toth_9x, 0, 0, 0.5*PMT::envthickness, bl1_9x, bl1_9x, 0, 0.5*PMT::envthickness, bl2_9x, bl2_9x, 0);
      auto   logE_9x = new G4LogicalVolume(solid9x, Construction::Material::Air, env9xName);
      G4double zpos9x = -0.5*toth_9x;
      G4double ypos9x = 0;

      for (unsigned int k=0; k<nc_9x; ++k) {
        if (k == 0) {
          ypos9x = -8*cm;
        } else if (k == 1) {
          ypos9x = -3*cm;
        } else if (k == 2) {
          ypos9x = -8*cm;
        } else if (k == 3) {
          ypos9x = -3*cm;
        }
        bl2_9x   = bl1_9x + heights_9x[k]*cfac;
        zpos9x += 0.5*heights_9x[k] + 0.15*cm;

        auto  solid9xa = new G4Trap(childNames_9x[k], 0.5*heights_9x[k], 0, 0, h1_9x, bl1_9x, bl1_9x, 0, h1_9x, bl2_9x, bl2_9x, 0);
        auto   child_9x = new G4LogicalVolume(solid9xa, Material::Scintillator, childNames_9x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos9x, zpos9x), child_9x, childNames_9x[k],
              logE_9x, false, 0);
        zpos9x += 0.5*heights_9x[k] + 0.15*cm;
        bl1_9x   = bl2_9x + 0.3*cm*cfac;

      }
      //Now the modules in mother
        new G4PVPlacement(0, G4ThreeVector(), logE_9x, env9xName,
              logE_9, false, 0);

    // env 10 //

      const unsigned int nc_10(3);
      G4double edge_10(32.8*cm);
      std::string childNames_10[nc_10] = {"C6", "C7", "C8"};
      G4double    heights_10[nc_10]    = {57.8553611983379*cm,
            68.9468035006099*cm, 81.9367809717393*cm};
      std::string env10Name("Envelope10");

      G4double toth_10(0);
      for (unsigned int k=0; k<nc_10; ++k) toth_10 += heights_10[k];
      G4double bl1_10  = 0.5*edge_10;
      G4double bl2_10  = bl1_10 + toth_10*cfac;
      G4double h1_10  = 0.5*PMT::thickness;

      auto  solid10 = new G4Trap(env10Name, 0.5*toth_10, 0, 0, 0.5*PMT::envthickness, bl1_10, bl1_10, 0, 0.5*PMT::envthickness, bl2_10, bl2_10, 0);
      auto   logE_10 = new G4LogicalVolume(solid10, Construction::Material::Air, env10Name);

      G4double zpos10 = -0.5*toth_10;
      G4double ypos10 = 0;
      for (unsigned int k=0; k<nc_10; ++k) {
        if (k == 0) {
          ypos10 = -18*cm;
        } else if (k == 1) {
          ypos10 = -13*cm;
        } else if (k == 2) {
          ypos10 = -18*cm;
        }
        zpos10 += 0.5*heights_10[k];
        bl2_10   = bl1_10 + heights_10[k]*cfac;

        auto  outersolid10a = new G4Trap("outerchildNames_10[k]", 0.5*heights_10[k], 0, 0, h1_10, bl1_10, bl1_10, 0, h1_10, bl2_10, bl2_10, 0);
        auto  innersolid10a = new G4Trap("innerchildNames_10[k]", 0.5*heights_10[k] - 0.1*cm, 0, 0, h1_10 - 0.1*cm, bl1_10 - 0.1*cm, bl1_10 - 0.1*cm, 0, h1_10 - 0.1*cm, bl2_10 - 0.1*cm, bl2_10 - 0.1*cm, 0);
        G4SubtractionSolid *hollow10 = new G4SubtractionSolid("Hollow 10",outersolid10a,innersolid10a);
        auto   child_10 = new G4LogicalVolume(hollow10, PMT::Material, "Hollow 10");

        new G4PVPlacement(0, G4ThreeVector(0, ypos10, zpos10), child_10, childNames_10[k],
              logE_10, false, 0);
        zpos10 += 0.5*heights_10[k];
        bl1_10  = bl2_10;
      }

      //Now the modules in mother
      G4double bl_10 = 0.5*tan(0.5*PMT::angle)*toth_10 + edge_10;
      G4double xpos_10 = xpos_9 + 0.5*bl_9 + 0.6*bl_10;
      G4double ypos_10 = 1.05*ypos_9;
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_10, ypos_10, 250.0*cm), logE_10, env10Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos10p = 0.245*toth_10;
      G4double zpos10p = 0;
      G4double xpos10p = xpos9p + 2*bl_9 + 2*PMT::trx;
      for (unsigned int k=0; k<nc_10; ++k) {
        if (k == 0) {
          zpos10p = -18*cm;
        } else if (k == 1) {
          zpos10p = -13*cm;
        } else if (k == 2) {
          zpos10p = -18*cm;
        }
        ypos10p += -heights_10[k];

        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos10p, ypos10p, zpos10p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_10x(0);
      const unsigned int nc_10x(3);
      std::string childNames_10x[nc_10x] = {"C6", "C7", "C8"};
      G4double    heights_10x[nc_10x]    = {57.8553611983379*cm - 0.3*cm,
            68.9468035006099*cm - 0.3*cm, 81.9367809717393*cm - 0.3*cm};
      std::string env10xName("Envelope10x");

      toth_10x = toth_10 - 0.3*cm;
      G4double bl1_10x  = 0.5*edge_10 - 0.15*cm;
      G4double bl2_10x  = bl1_10x + toth_10x*cfac;
      G4double h1_10x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid10x = new G4Trap(env10xName, 0.5*toth_10x, 0, 0, 0.5*PMT::envthickness, bl1_10x, bl1_10x, 0, 0.5*PMT::envthickness, bl2_10x, bl2_10x, 0);
      auto   logE_10x = new G4LogicalVolume(solid10x, Construction::Material::Air, env10xName);
      G4double zpos10x = -0.5*toth_10x;
      G4double ypos10x = 0;

      for (unsigned int k=0; k<nc_10x; ++k) {
        if (k == 0) {
          ypos10x = -18*cm;
        } else if (k == 1) {
          ypos10x = -13*cm;
        } else if (k == 2) {
          ypos10x = -18*cm;
        }
        bl2_10x   = bl1_10x + heights_10x[k]*cfac;
        zpos10x += 0.5*heights_10x[k] + 0.15*cm;

        auto  solid10xa = new G4Trap(childNames_10x[k], 0.5*heights_10x[k], 0, 0, h1_10x, bl1_10x, bl1_10x, 0, h1_10x, bl2_10x, bl2_10x, 0);
        auto   child_10x = new G4LogicalVolume(solid10xa, Material::Scintillator, childNames_10x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos10x, zpos10x), child_10x, childNames_10x[k],
              logE_10x, false, 0);
        zpos10x += 0.5*heights_10x[k] + 0.15*cm;
        bl1_10x   = bl2_10x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_10x, env10xName,
              logE_10, false, 0);

    // B9_1 //

      G4double edge_B9_1(40.7*cm);
      G4double bl1_B9_1 = 0.5*edge_B9_1;
      G4double bl2_B9_1 = 0.5*43.6*cm;
      G4double heightB9_1 = 37.8707804735234*cm;
      G4double h1_B9_1 = 0.5*PMT::thickness;

      auto  outersolidB9_1 = new G4Trap("outerB9_1", 0.5*heightB9_1, 0, 0, h1_B9_1, bl1_B9_1, bl1_B9_1, 0, h1_B9_1, bl2_B9_1, bl2_B9_1, 0);
      auto  innersolidB9_1 = new G4Trap("innerB9_1", 0.5*heightB9_1 - 0.1*cm, 0, 0, h1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, 0, h1_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB9_1 = new G4SubtractionSolid("Hollow B9_1",outersolidB9_1,innersolidB9_1);
      auto   solidlogB9_1 = new G4LogicalVolume(hollowB9_1, PMT::Material, "B9_1");

      G4double xpos_B9_1 = 1.04*xpos_9;
      G4double ypos_B9_1 = 0.385*toth_9;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B9_1, ypos_B9_1, 242.0*cm), solidlogB9_1, "B9_1",
              Calorimeter, false, 0);

      G4double edge_B9_1x(40.7*cm);
      G4double bl1_B9_1x = 0.5*edge_B9_1x - 0.15*cm;
      G4double bl2_B9_1x = 0.5*43.6*cm - 0.15*cm;
      G4double heightB9_1x = 37.8707804735234*cm - 0.3*cm;
      G4double h1_B9_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidB9_1x = new G4Trap("B9_1x", 0.5*heightB9_1x, 0, 0, h1_B9_1x, bl1_B9_1x, bl1_B9_1x, 0, h1_B9_1x, bl2_B9_1x, bl2_B9_1x, 0);
      auto   solidlogB9_1x = new G4LogicalVolume(solidB9_1x, Material::Scintillator, "B9_1x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B9_1, ypos_B9_1, 242.0*cm), solidlogB9_1x, "B9_1x",
              Calorimeter, false, 0);

      G4double xpos_B9_1p = xpos9p;
      G4double ypos_B9_1p = ypos9p + heightB9_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B9_1p, ypos_B9_1p, 242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B9_2 //

      auto  outersolidB9_2 = new G4Trap("outerB9_2", 0.5*heightB9_1, 0, 0, h1_B9_1, bl1_B9_1, bl1_B9_1, 0, h1_B9_1, bl2_B9_1, bl2_B9_1, 0);
      auto  innersolidB9_2 = new G4Trap("innerB9_2", 0.5*heightB9_1 - 0.1*cm, 0, 0, h1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, 0, h1_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB9_2 = new G4SubtractionSolid("Hollow B9_2",outersolidB9_2,innersolidB9_2);
      auto   solidlogB9_2 = new G4LogicalVolume(hollowB9_2, PMT::Material, "B9_2");

      G4double xpos_B9_2 = 1.04*xpos_9;
      G4double ypos_B9_2 = ypos_B9_1 + heightB9_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B9_2, ypos_B9_2, 247.0*cm), solidlogB9_2, "B9_2",
              Calorimeter, false, 0);

      auto  solidB9_2x = new G4Trap("B9_2x", 0.5*heightB9_1x, 0, 0, h1_B9_1x, bl1_B9_1x, bl1_B9_1x, 0, h1_B9_1x, bl2_B9_1x, bl2_B9_1x, 0);
      auto   solidlogB9_2x = new G4LogicalVolume(solidB9_2x, Material::Scintillator, "B9_2x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B9_2, ypos_B9_2, 247.0*cm), solidlogB9_2x, "B9_2x",
              Calorimeter, false, 0);

      G4double xpos_B9_2p = xpos_B9_1p;
      G4double ypos_B9_2p = ypos_B9_1p + heightB9_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B9_2p, ypos_B9_2p, 247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B9_3 //

      auto  outersolidB9_3 = new G4Trap("outerB9_3", 0.5*heightB9_1, 0, 0, h1_B9_1, bl1_B9_1, bl1_B9_1, 0, h1_B9_1, bl2_B9_1, bl2_B9_1, 0);
      auto  innersolidB9_3 = new G4Trap("innerB9_3", 0.5*heightB9_1 - 0.1*cm, 0, 0, h1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, 0, h1_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB9_3 = new G4SubtractionSolid("Hollow B9_3",outersolidB9_3,innersolidB9_3);
      auto   solidlogB9_3 = new G4LogicalVolume(hollowB9_3, PMT::Material, "B9_3");

      G4double bl_B9_1 = 0.5*tan(0.5*PMT::angle)*heightB9_1 + edge_B9_1;
      G4double xpos_B9_3 = 1.04*xpos_9 + 1.02*bl_B9_1;
      G4double ypos_B9_3 = 0.98*ypos_B9_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B9_3, ypos_B9_3, 237.0*cm), solidlogB9_3, "B9_3",
              Calorimeter, false, 0);

      auto  solidB9_3x = new G4Trap("B9_3x", 0.5*heightB9_1x, 0, 0, h1_B9_1x, bl1_B9_1x, bl1_B9_1x, 0, h1_B9_1x, bl2_B9_1x, bl2_B9_1x, 0);
      auto   solidlogB9_3x = new G4LogicalVolume(solidB9_3x, Material::Scintillator, "B9_3x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B9_3, ypos_B9_3, 237.0*cm), solidlogB9_3x, "B9_3x",
              Calorimeter, false, 0);

      G4double xpos_B9_3p = xpos_B9_1p + 2.05*bl_B9_1 + PMT::trx;
      G4double ypos_B9_3p = ypos_B9_1p - heightB9_1 - 1.2*PMT::trx;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B9_3p, ypos_B9_3p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B9_4 //

      auto  outersolidB9_4 = new G4Trap("outerB9_4", 0.5*heightB9_1, 0, 0, h1_B9_1, bl1_B9_1, bl1_B9_1, 0, h1_B9_1, bl2_B9_1, bl2_B9_1, 0);
      auto  innersolidB9_4 = new G4Trap("innerB9_4", 0.5*heightB9_1 - 0.1*cm, 0, 0, h1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, bl1_B9_1 - 0.1*cm, 0, h1_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, bl2_B9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB9_4 = new G4SubtractionSolid("Hollow B9_4",outersolidB9_4,innersolidB9_4);
      auto   solidlogB9_4 = new G4LogicalVolume(hollowB9_4, PMT::Material, "B9_4");

      G4double xpos_B9_4 = xpos_B9_3;
      G4double ypos_B9_4 = 0.99*ypos_B9_2;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B9_4, ypos_B9_4, 232.0*cm), solidlogB9_4, "B9_4",
              Calorimeter, false, 0);

      auto  solidB9_4x = new G4Trap("B9_4x", 0.5*heightB9_1x, 0, 0, h1_B9_1x, bl1_B9_1x, bl1_B9_1x, 0, h1_B9_1x, bl2_B9_1x, bl2_B9_1x, 0);
      auto   solidlogB9_4x = new G4LogicalVolume(solidB9_4x, Material::Scintillator, "B9_4x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B9_4, ypos_B9_4, 232.0*cm), solidlogB9_4x, "B9_4x",
              Calorimeter, false, 0);

      G4double xpos_B9_4p = xpos_B9_3p;
      G4double ypos_B9_4p = ypos_B9_3p + heightB9_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B9_4p, ypos_B9_4p, 232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B11_1 //

      G4double edge_B11_1(47.1*cm);
      G4double bl1_B11_1 = 0.5*edge_B11_1;
      G4double bl2_B11_1 = 0.5*51.2*cm;
      G4double heightB11_1 = 55.8569031258564*cm;
      G4double h1_B11_1 = 0.5*PMT::thickness;

      auto  outersolidB11_1 = new G4Trap("outerB11_1", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_1 = new G4Trap("innerB11_1", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_1 = new G4SubtractionSolid("Hollow B11_1",outersolidB11_1,innersolidB11_1);
      auto   solidlogB11_1 = new G4LogicalVolume(hollowB11_1, PMT::Material, "B11_1");

      G4double bl_B11_1 = 0.5*tan(0.5*PMT::angle)*heightB11_1 + edge_B11_1;
      G4double xpos_B11_1 = xpos_9 - 0.53*bl_9 - 0.53*bl_B11_1;
      G4double ypos_B11_1 = -147.24*cm + 0.5*heightB11_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_1, ypos_B11_1, 237.0*cm), solidlogB11_1, "B11_1",
              Calorimeter, false, 0);

      G4double edge_B11_1x(47.1*cm);
      G4double bl1_B11_1x = 0.5*edge_B11_1x - 0.15*cm;
      G4double bl2_B11_1x = 0.5*51.2*cm - 0.15*cm;
      G4double heightB11_1x = 55.8569031258564*cm - 0.3*cm;
      G4double h1_B11_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidB11_1x = new G4Trap("B11_1x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_1x = new G4LogicalVolume(solidB11_1x, Material::Scintillator, "B11_1x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_1, ypos_B11_1, 237.0*cm), solidlogB11_1x, "B11_1x",
              Calorimeter, false, 0);

      G4double xpos_B11_1p = xpos_B9_2p + PMT::trx;
      G4double ypos_B11_1p = 0.26*toth_10 - toth_10;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B11_1p, ypos_B11_1p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B11_2 //

      auto  outersolidB11_2 = new G4Trap("outerB11_2", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_2 = new G4Trap("innerB11_2", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_2 = new G4SubtractionSolid("Hollow B11_2",outersolidB11_2,innersolidB11_2);
      auto   solidlogB11_2 = new G4LogicalVolume(hollowB11_2, PMT::Material, "B11_2");

      G4double xpos_B11_2 = xpos_B11_1;
      G4double ypos_B11_2 = ypos_B11_1 + heightB11_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_2, ypos_B11_2, 232.0*cm), solidlogB11_2, "B11_2",
              Calorimeter, false, 0);

      auto  solidB11_2x = new G4Trap("B11_2x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_2x = new G4LogicalVolume(solidB11_2x, Material::Scintillator, "B11_2x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_2, ypos_B11_2, 232.0*cm), solidlogB11_2x, "B11_2x",
              Calorimeter, false, 0);

      G4double xpos_B11_2p = xpos_B11_1p;
      G4double ypos_B11_2p = ypos_B11_1p + heightB11_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B11_2p, ypos_B11_2p, 232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B11_3 //

      auto  outersolidB11_3 = new G4Trap("outerB11_3", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_3 = new G4Trap("innerB11_3", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_3 = new G4SubtractionSolid("Hollow B11_3",outersolidB11_3,innersolidB11_3);
      auto   solidlogB11_3 = new G4LogicalVolume(hollowB11_3, PMT::Material, "B11_3");

      G4double xpos_B11_3 = xpos_B11_1;
      G4double ypos_B11_3 = ypos_B11_2 + heightB11_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_3, ypos_B11_3, 237.0*cm), solidlogB11_3, "B11_3",
              Calorimeter, false, 0);

      auto  solidB11_3x = new G4Trap("B11_3x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_3x = new G4LogicalVolume(solidB11_3x, Material::Scintillator, "B11_3x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B11_3, ypos_B11_3, 237.0*cm), solidlogB11_3x, "B11_3x",
              Calorimeter, false, 0);

      G4double xpos_B11_3p = xpos_B11_1p;
      G4double ypos_B11_3p = ypos_B11_2p + heightB11_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B11_3p, ypos_B11_3p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B11_4 //

      auto  outersolidB11_4 = new G4Trap("outerB11_4", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_4 = new G4Trap("innerB11_4", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_4 = new G4SubtractionSolid("Hollow B11_4",outersolidB11_4,innersolidB11_4);
      auto   solidlogB11_4 = new G4LogicalVolume(hollowB11_4, PMT::Material, "B11_4");

      G4double xpos_B11_4 = xpos_B11_1 - 1.019*bl_B11_1;
      G4double ypos_B11_4 = 0.985*ypos_B11_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_4, ypos_B11_4, 242.0*cm), solidlogB11_4, "B11_4",
              Calorimeter, false, 0);

      auto  solidB11_4x = new G4Trap("B11_4x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_4x = new G4LogicalVolume(solidB11_4x, Material::Scintillator, "B11_4x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_4, ypos_B11_4, 242.0*cm), solidlogB11_4x, "B11_4x",
              Calorimeter, false, 0);

      G4double xpos_B11_4p = xpos_B11_1p - 2*bl_B11_1 - PMT::trx;
      G4double ypos_B11_4p = 0.96*ypos_B11_2p + PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B11_4p, ypos_B11_4p, 242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B11_5 //

      auto  outersolidB11_5 = new G4Trap("outerB11_5", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_5 = new G4Trap("innerB11_5", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_5 = new G4SubtractionSolid("Hollow B11_5",outersolidB11_5,innersolidB11_5);
      auto   solidlogB11_5 = new G4LogicalVolume(hollowB11_5, PMT::Material, "B11_5");

      G4double xpos_B11_5 = xpos_B11_4;
      G4double ypos_B11_5 = ypos_B11_4 + heightB11_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_5, ypos_B11_5, 247.0*cm), solidlogB11_5, "B11_5",
              Calorimeter, false, 0);

      auto  solidB11_5x = new G4Trap("B11_5x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_5x = new G4LogicalVolume(solidB11_5x, Material::Scintillator, "B11_5x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_5, ypos_B11_5, 247.0*cm), solidlogB11_5x, "B11_5x",
              Calorimeter, false, 0);

      G4double xpos_B11_5p = xpos_B11_4p;
      G4double ypos_B11_5p = ypos_B11_4p + heightB11_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B11_5p, ypos_B11_5p, 247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

      // B11_6 //

      auto  outersolidB11_6 = new G4Trap("outerB11_6", 0.5*heightB11_1, 0, 0, h1_B11_1, bl1_B11_1, bl1_B11_1, 0, h1_B11_1, bl2_B11_1, bl2_B11_1, 0);
      auto  innersolidB11_6 = new G4Trap("innerB11_6", 0.5*heightB11_1 - 0.1*cm, 0, 0, h1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, bl1_B11_1 - 0.1*cm, 0, h1_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, bl2_B11_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB11_6 = new G4SubtractionSolid("Hollow B11_6",outersolidB11_6,innersolidB11_6);
      auto   solidlogB11_6 = new G4LogicalVolume(hollowB11_6, PMT::Material, "B11_6");

      G4double xpos_B11_6 = xpos_B11_4;
      G4double ypos_B11_6 = ypos_B11_5 + heightB11_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_6, ypos_B11_6, 242.0*cm), solidlogB11_6, "B11_6",
              Calorimeter, false, 0);

      auto  solidB11_6x = new G4Trap("B11_6x", 0.5*heightB11_1x, 0, 0, h1_B11_1x, bl1_B11_1x, bl1_B11_1x, 0, h1_B11_1x, bl2_B11_1x, bl2_B11_1x, 0);
      auto   solidlogB11_6x = new G4LogicalVolume(solidB11_6x, Material::Scintillator, "B11_6x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B11_6, ypos_B11_6, 242.0*cm), solidlogB11_6x, "B11_6x",
              Calorimeter, false, 0);

      G4double xpos_B11_6p = xpos_B11_5p;
      G4double ypos_B11_6p = ypos_B11_5p + heightB11_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B11_6p, ypos_B11_6p, 242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C7_1 //

      G4double edge_C7_1(37.5*cm);
      G4double bl1_C7_1 = 0.5*edge_C7_1;
      G4double bl2_C7_1 = 0.5*42*cm;
      G4double heightC7_1 = 68.9468035006099*cm;
      G4double h1_C7_1 = 0.5*PMT::thickness;

      auto  outersolidC7_1 = new G4Trap("outerC7_1", 0.5*heightC7_1, 0, 0, h1_C7_1, bl1_C7_1, bl1_C7_1, 0, h1_C7_1, bl2_C7_1, bl2_C7_1, 0);
      auto  innersolidC7_1 = new G4Trap("innerC7_1", 0.5*heightC7_1 - 0.1*cm, 0, 0, h1_C7_1 - 0.1*cm, bl1_C7_1 - 0.1*cm, bl1_C7_1 - 0.1*cm, 0, h1_C7_1 - 0.1*cm, bl2_C7_1 - 0.1*cm, bl2_C7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC7_1 = new G4SubtractionSolid("Hollow C7_1",outersolidC7_1,innersolidC7_1);
      auto   solidCalorimeter7_1 = new G4LogicalVolume(hollowC7_1, PMT::Material, "C7_1");

      G4double xpos_C7_1 = 1.1*xpos_B11_1;
      G4double ypos_C7_1 = ypos_B11_3 + 0.5*heightB11_1 + 0.5*heightC7_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C7_1, ypos_C7_1, 232.0*cm), solidCalorimeter7_1, "C7_1",
              Calorimeter, false, 0);

      G4double edge_C7_1x(37.5*cm);
      G4double bl1_C7_1x = 0.5*edge_C7_1x - 0.15*cm;
      G4double bl2_C7_1x = 0.5*42*cm - 0.15*cm;
      G4double heightC7_1x = 68.9468035006099*cm - 0.3*cm;
      G4double h1_C7_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidC7_1x = new G4Trap("C7_1x", 0.5*heightC7_1x, 0, 0, h1_C7_1x, bl1_C7_1x, bl1_C7_1x, 0, h1_C7_1x, bl2_C7_1x, bl2_C7_1x, 0);
      auto   solidCalorimeter7_1x = new G4LogicalVolume(solidC7_1x, Material::Scintillator, "C7_1x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C7_1, ypos_C7_1, 232.0*cm), solidCalorimeter7_1x, "C7_1x",
              Calorimeter, false, 0);

      G4double xpos_C7_1p = xpos_B11_1p;
      G4double ypos_C7_1p = ypos_B11_3p + 0.96*heightB11_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_C7_1p, ypos_C7_1p, 232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C7_2 //

      auto  outersolidC7_2 = new G4Trap("outerC7_2", 0.5*heightC7_1, 0, 0, h1_C7_1, bl1_C7_1, bl1_C7_1, 0, h1_C7_1, bl2_C7_1, bl2_C7_1, 0);
      auto  innersolidC7_2 = new G4Trap("innerC7_2", 0.5*heightC7_1 - 0.1*cm, 0, 0, h1_C7_1 - 0.1*cm, bl1_C7_1 - 0.1*cm, bl1_C7_1 - 0.1*cm, 0, h1_C7_1 - 0.1*cm, bl2_C7_1 - 0.1*cm, bl2_C7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC7_2 = new G4SubtractionSolid("Hollow C7_2",outersolidC7_2,innersolidC7_2);
      auto   solidCalorimeter7_2 = new G4LogicalVolume(hollowC7_2, PMT::Material, "C7_2");

      G4double xpos_C7_2 = xpos_C7_1 - 1.06*edge_C7_1;
      G4double ypos_C7_2 = 1.025*ypos_C7_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C7_2, ypos_C7_2, 247.0*cm), solidCalorimeter7_2, "C7_2",
              Calorimeter, false, 0);

      auto  solidC7_2x = new G4Trap("C7_2x", 0.5*heightC7_1x, 0, 0, h1_C7_1x, bl1_C7_1x, bl1_C7_1x, 0, h1_C7_1x, bl2_C7_1x, bl2_C7_1x, 0);
      auto   solidCalorimeter7_2x = new G4LogicalVolume(solidC7_2x, Material::Scintillator, "C7_2x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C7_2, ypos_C7_2, 247.0*cm), solidCalorimeter7_2x, "C7_2x",
              Calorimeter, false, 0);

      G4double xpos_C7_2p = xpos_C7_1p - 2.1*edge_C7_1 - PMT::trx;
      G4double ypos_C7_2p = ypos_C7_1p + 1.08*heightC7_1 + PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_C7_2p, ypos_C7_2p, 247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B12-1_1 //

      G4double edge_B12_1(43.5*cm);
      G4double bl1_B12_1 = 0.5*edge_B12_1;
      G4double bl2_B12_1 = 0.5*48.6*cm;
      G4double heightB12_1 = 64.8499644520229*cm;
      G4double h1_B12_1 = 0.5*PMT::thickness;

      auto  outersolidB12_1 = new G4Trap("outerB12_1", 0.5*heightB12_1, 0, 0, h1_B12_1, bl1_B12_1, bl1_B12_1, 0, h1_B12_1, bl2_B12_1, bl2_B12_1, 0);
      auto  innersolidB12_1 = new G4Trap("innerB12_1", 0.5*heightB12_1 - 0.1*cm, 0, 0, h1_B12_1 - 0.1*cm, bl1_B12_1 - 0.1*cm, bl1_B12_1 - 0.1*cm, 0, h1_B12_1 - 0.1*cm, bl2_B12_1 - 0.1*cm, bl2_B12_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB12_1 = new G4SubtractionSolid("Hollow B12_1",outersolidB12_1,innersolidB12_1);
      auto   solidlogB12_1 = new G4LogicalVolume(hollowB12_1, PMT::Material, "B12_1");

      G4double bl_B12_1 = 0.5*tan(0.5*PMT::angle)*heightB12_1 + edge_B12_1;
      G4double xpos_B12_1 = 0.93*xpos_C7_1;
      G4double ypos_B12_1 = ypos_C7_1 + 0.5*heightC7_1 + 0.5*heightB12_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B12_1, ypos_B12_1, 237.0*cm), solidlogB12_1, "B12_1",
              Calorimeter, false, 0);

      G4double edge_B12_1x(43.5*cm);
      G4double bl1_B12_1x = 0.5*edge_B12_1x - 0.15*cm;
      G4double bl2_B12_1x = 0.5*48.6*cm - 0.15*cm;
      G4double heightB12_1x = 64.8499644520229*cm - 0.3*cm;
      G4double h1_B12_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidB12_1x = new G4Trap("B12_1x", 0.5*heightB12_1x, 0, 0, h1_B12_1x, bl1_B12_1x, bl1_B12_1x, 0, h1_B12_1x, bl2_B12_1x, bl2_B12_1x, 0);
      auto   solidlogB12_1x = new G4LogicalVolume(solidB12_1x, Material::Scintillator, "B12_1x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_B12_1, ypos_B12_1, 237.0*cm), solidlogB12_1x, "B12_1x",
              Calorimeter, false, 0);

      G4double xpos_B12_1p = xpos_C7_1p;
      G4double ypos_B12_1p = ypos_C7_1p + 1.01*heightC7_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_B12_1p, ypos_B12_1p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // B7_1 //

      G4double edge_B7_1(31*cm);
      G4double bl1_B7_1 = 0.5*edge_B7_1;
      G4double bl2_B7_1 = 0.5*35*cm;
      G4double heightB7_1 = 55.5571344149842*cm;
      G4double h1_B7_1 = 0.5*PMT::thickness;

      auto  outersolidB7_1 = new G4Trap("outerB7_1", 0.5*heightB7_1, 0, 0, h1_B7_1, bl1_B7_1, bl1_B7_1, 0, h1_B7_1, bl2_B7_1, bl2_B7_1, 0);
      auto  innersolidB7_1 = new G4Trap("innerB7_1", 0.5*heightB7_1 - 0.1*cm, 0, 0, h1_B7_1 - 0.1*cm, bl1_B7_1 - 0.1*cm, bl1_B7_1 - 0.1*cm, 0, h1_B7_1 - 0.1*cm, bl2_B7_1 - 0.1*cm, bl2_B7_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowB7_1 = new G4SubtractionSolid("Hollow B7_1",outersolidB7_1,innersolidB7_1);
      auto   solidlogB7_1 = new G4LogicalVolume(hollowB7_1, PMT::Material, "B7_1");

      G4double bl_B7_1 = 0.5*tan(0.5*PMT::angle)*heightB7_1 + edge_B7_1;
      G4double xpos_B7_1 = xpos_B12_1 - 0.5*bl_B12_1 - 0.55*bl_B7_1;
      G4double ypos_B7_1 = ypos_B12_1 - 0.35*(heightB12_1 - heightB7_1);

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B7_1, ypos_B7_1, 232.0*cm), solidlogB7_1, "B7_1",
              Calorimeter, false, 0);

      G4double edge_B7_1x(31*cm);
      G4double bl1_B7_1x = 0.5*edge_B7_1x - 0.15*cm;
      G4double bl2_B7_1x = 0.5*35*cm - 0.15*cm;
      G4double heightB7_1x = 55.5571344149842*cm - 0.3*cm;
      G4double h1_B7_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidB7_1x = new G4Trap("B7_1x", 0.5*heightB7_1x, 0, 0, h1_B7_1x, bl1_B7_1x, bl1_B7_1x, 0, h1_B7_1x, bl2_B7_1x, bl2_B7_1x, 0);
      auto   solidlogB7_1x = new G4LogicalVolume(solidB7_1x, Material::Scintillator, "B7_1x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_B7_1, ypos_B7_1, 232.0*cm), solidlogB7_1x, "B7_1x",
              Calorimeter, false, 0);

      G4double xpos_B7_1p = xpos_C7_2p;
      G4double ypos_B7_1p = ypos_C7_2p + heightB7_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_B7_1p, ypos_B7_1p, 232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // env 11 // Flip env 11 upside down

      const unsigned int nc_11(3);
      G4double edge_11(25.8*cm);
      std::string childNames_11[nc_11] = {"A9", "A10", "A11"};
      G4double    heights_11[nc_11]    = {50.0613747156602*cm,
            34.2735559430568*cm, 39.2697011242604*cm};
      std::string env11Name("Envelope11");

      G4double toth_11(0);
      for (unsigned int k=0; k<nc_11; ++k) toth_11 += heights_11[k];
      G4double bl1_11  = 0.5*edge_11;
      G4double bl2_11  = bl1_11 + toth_11*cfac;
      G4double h1_11  = 0.5*PMT::thickness;

      auto  solid11 = new G4Trap(env11Name, 0.5*toth_11, 0, 0, 0.5*PMT::envthickness, bl1_11, bl1_11, 0, 0.5*PMT::envthickness, bl2_11, bl2_11, 0);
      auto   logE_11 = new G4LogicalVolume(solid11, Construction::Material::Air, env11Name);
      G4double zpos11 = -0.5*toth_11;
      G4double ypos11 = 0;
      for (unsigned int k=0; k<nc_11; ++k) {
        if (k == 0) {
          ypos11 = -3*cm;
        } else if (k == 1) {
          ypos11 = -8*cm;
        } else if (k == 2) {
          ypos11 = -13*cm;
        }
        zpos11 += 0.5*heights_11[k];
        bl2_11   = bl1_11 + heights_11[k]*cfac;

        auto  outersolid11a = new G4Trap("outerchildNames_11[k]", 0.5*heights_11[k], 0, 0, h1_11, bl1_11, bl1_11, 0, h1_11, bl2_11, bl2_11, 0);
        auto  innersolid11a = new G4Trap("innerchildNames_11[k]", 0.5*heights_11[k] - 0.1*cm, 0, 0, h1_11 - 0.1*cm, bl1_11 - 0.1*cm, bl1_11 - 0.1*cm, 0, h1_11 - 0.1*cm, bl2_11 - 0.1*cm, bl2_11 - 0.1*cm, 0);
        G4SubtractionSolid *hollow11 = new G4SubtractionSolid("Hollow 11",outersolid11a,innersolid11a);
        auto   child_11 = new G4LogicalVolume(hollow11, PMT::Material, "Hollow 11");

        new G4PVPlacement(0, G4ThreeVector(0, ypos11, zpos11), child_11, childNames_11[k],
              logE_11, false, 0);
        zpos11 += 0.5*heights_11[k];
        bl1_11  = bl2_11;
      }

      //Now the modules in mother
      G4double bl_11 = 0.5*tan(0.5*PMT::angle)*toth_11 + edge_11;
      G4double xpos_11 = xpos_B7_1 - 0.5*bl_B7_1 - 0.55*bl_11;
      G4double ypos_11 = ypos_C7_2 + 0.415*heightC7_1;
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_11, ypos_11, 250.0*cm), logE_11, env11Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos11p = 1.135*toth_11;
      G4double zpos11p = 0;
      G4double xpos11p = xpos_B7_1 - 1.9*bl_11;
      //G4double phiz_5 = 90.*deg - PMT::anglep - 0.5*PMT::angle;
      //G4double phix_5 = phiz_5 + 90.0*deg;
      //G4RotationMatrix* PMT::rot_5 = AddMatrix(90*deg, phix_5, 0, 0, 90*deg, phiz_5);

      for (unsigned int k=0; k<nc_11; ++k) {
        if (k == 0) {
          zpos11p = -3*cm;
        } else if (k == 1) {
          zpos11p = -8*cm;
        } else if (k == 2) {
          zpos11p = -13*cm;
        }
        ypos11p += -heights_11[k];
        new G4PVPlacement(PMT::rot_5, G4ThreeVector(xpos11p, ypos11p, zpos11p), Cylinder, "pmt",
              Calorimeter, false, 0);
        xpos11p += -1.5*heights_11[k]*cfac;
      }

      // Inside //

      G4double toth_11x(0);
      const unsigned int nc_11x(3);
      std::string childNames_11x[nc_11x] = {"A9", "A10", "A11"};
      G4double    heights_11x[nc_11x]    = {50.0613747156602*cm - 0.3*cm,
            34.2735559430568*cm - 0.3*cm, 39.2697011242604*cm - 0.3*cm};
      std::string env11xName("Envelope11x");

      toth_11x = toth_11 - 0.3*cm;
      G4double bl1_11x  = 0.5*edge_11 - 0.15*cm;
      G4double bl2_11x  = bl1_11x + toth_11x*cfac;
      G4double h1_11x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid11x = new G4Trap(env11xName, 0.5*toth_11x, 0, 0, 0.5*PMT::envthickness, bl1_11x, bl1_11x, 0, 0.5*PMT::envthickness, bl2_11x, bl2_11x, 0);
      auto   logE_11x = new G4LogicalVolume(solid11x, Construction::Material::Air, env11xName);
      G4double zpos11x = -0.5*toth_11x;
      G4double ypos11x = 0;

      for (unsigned int k=0; k<nc_11x; ++k) {
        if (k == 0) {
          ypos11x = -3*cm;
        } else if (k == 1) {
          ypos11x = -8*cm;
        } else if (k == 2) {
          ypos11x = -13*cm;
        }
        bl2_11x   = bl1_11x + heights_11x[k]*cfac;
        zpos11x += 0.5*heights_11x[k] + 0.15*cm;

        auto  solid11xa = new G4Trap(childNames_11x[k], 0.5*heights_11x[k], 0, 0, h1_11x, bl1_11x, bl1_11x, 0, h1_11x, bl2_11x, bl2_11x, 0);
        auto   child_11x = new G4LogicalVolume(solid11xa, Material::Scintillator, childNames_11x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos11x, zpos11x), child_11x, childNames_11x[k],
              logE_11x, false, 0);
        zpos11x += 0.5*heights_11x[k] + 0.15*cm;
        bl1_11x   = bl2_11x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_11x, env11xName,
              logE_11, false, 0);

    // env 12 //

      const unsigned int nc_12(3);
      std::string childNames_12[nc_12] = {"A9", "A10", "A11"};
      G4double    heights_12[nc_12]    = {50.0613747156602*cm,
            34.2735559430568*cm, 39.2697011242604*cm};
      std::string env12Name("Envelope12");

      G4double toth_12(0);
      for (unsigned int k=0; k<nc_12; ++k) toth_12 += heights_12[k];
      G4double bl1_12  = 0.5*edge_11;
      G4double bl2_12  = bl1_12 + toth_12*cfac;
      G4double h1_12  = 0.5*PMT::thickness;

      auto  solid12 = new G4Trap(env12Name, 0.5*toth_12, 0, 0, 0.5*PMT::envthickness, bl1_12, bl1_12, 0, 0.5*PMT::envthickness, bl2_12, bl2_12, 0);
      auto   logE_12 = new G4LogicalVolume(solid12, Construction::Material::Air, env12Name);
      G4double zpos12 = -0.5*toth_12;
      G4double ypos12 = 0;
      for (unsigned int k=0; k<nc_12; ++k) {
        if (k == 0) {
          ypos12 = -18*cm;
        } else if (k == 1) {
          ypos12 = -13*cm;
        } else if (k == 2) {
          ypos12 = -18*cm;
        }
        zpos12 += 0.5*heights_12[k];
        bl2_12   = bl1_12 + heights_12[k]*cfac;

        auto  outersolid12a = new G4Trap("outerchildNames_12[k]", 0.5*heights_12[k], 0, 0, h1_12, bl1_12, bl1_12, 0, h1_12, bl2_12, bl2_12, 0);
        auto  innersolid12a = new G4Trap("innerchildNames_12[k]", 0.5*heights_12[k] - 0.1*cm, 0, 0, h1_12 - 0.1*cm, bl1_12 - 0.1*cm, bl1_12 - 0.1*cm, 0, h1_12 - 0.1*cm, bl2_12 - 0.1*cm, bl2_12 - 0.1*cm, 0);
        G4SubtractionSolid *hollow12 = new G4SubtractionSolid("Hollow 12",outersolid12a,innersolid12a);
        auto   child_12 = new G4LogicalVolume(hollow12, PMT::Material, "Hollow 12");

        new G4PVPlacement(0, G4ThreeVector(0, ypos12, zpos12), child_12, childNames_12[k],
              logE_12, false, 0);
        zpos12 += 0.5*heights_12[k];
        bl1_12  = bl2_12;
      }

      //Now the modules in mother
      G4double xpos_12 = xpos_11 - 1.1*bl_11;
      G4double ypos_12 = 1.015*ypos_11;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_12, ypos_12, 250.0*cm), logE_12, env12Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos12p = ypos_B11_3 + 40.0613747156602*cm;
      G4double zpos12p = 0;
      G4double xpos12p = xpos_B7_1 - 3*bl_11;

      for (unsigned int k=0; k<nc_12; ++k) {
        if (k == 0) {
          zpos12p = -18*cm;
        } else if (k == 1) {
          zpos12p = -13*cm;
        } else if (k == 2) {
          zpos12p = -18*cm;
        }
        ypos12p += heights_12[k];
        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos12p, ypos12p, zpos12p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_12x(0);
      const unsigned int nc_12x(3);
      std::string childNames_12x[nc_12x] = {"A9", "A10", "A11"};
      G4double    heights_12x[nc_12x]    = {50.0613747156602*cm - 0.3*cm,
            34.2735559430568*cm - 0.3*cm, 39.2697011242604*cm - 0.3*cm};
      std::string env12xName("Envelope12x");

      toth_12x = toth_12 - 0.3*cm;
      G4double bl1_12x  = 0.5*edge_11 - 0.15*cm;
      G4double bl2_12x  = bl1_12x + toth_12x*cfac;
      G4double h1_12x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid12x = new G4Trap(env12xName, 0.5*toth_12x, 0, 0, 0.5*PMT::envthickness, bl1_12x, bl1_12x, 0, 0.5*PMT::envthickness, bl2_12x, bl2_12x, 0);
      auto   logE_12x = new G4LogicalVolume(solid12x, Construction::Material::Air, env12xName);
      G4double zpos12x = -0.5*toth_12x;
      G4double ypos12x = 0;

      for (unsigned int k=0; k<nc_12x; ++k) {
        if (k == 0) {
          ypos12x = -18*cm;
        } else if (k == 1) {
          ypos12x = -13*cm;
        } else if (k == 2) {
          ypos12x = -18*cm;
        }
        bl2_12x   = bl1_12x + heights_12x[k]*cfac;
        zpos12x += 0.5*heights_12x[k] + 0.15*cm;

        auto  solid12xa = new G4Trap(childNames_12x[k], 0.5*heights_12x[k], 0, 0, h1_12x, bl1_12x, bl1_12x, 0, h1_12x, bl2_12x, bl2_12x, 0);
        auto   child_12x = new G4LogicalVolume(solid12xa, Material::Scintillator, childNames_12x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos12x, zpos12x), child_12x, childNames_12x[k],
              logE_12x, false, 0);
        zpos12x += 0.5*heights_12x[k] + 0.15*cm;
        bl1_12x   = bl2_12x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_12x, env12xName,
              logE_12, false, 0);

    // env 13 // Flip env 13 upside down

      const unsigned int nc_13(3);
      G4double edge_13(19*cm);
      std::string childNames_13[nc_13] = {"B3", "B4", "B5"};
      G4double    heights_13[nc_13]    = {30.2766397980939*cm,
            34.6732475575531*cm, 40.4687759677493*cm};
      std::string env13Name("Envelope13");

      G4double toth_13(0);
      for (unsigned int k=0; k<nc_13; ++k) toth_13 += heights_13[k];
      G4double bl1_13  = 0.5*edge_13;
      G4double bl2_13  = bl1_13 + toth_13*cfac;
      G4double h1_13  = 0.5*PMT::thickness;

      auto  solid13 = new G4Trap(env13Name, 0.5*toth_13, 0, 0, 0.5*PMT::envthickness, bl1_13, bl1_13, 0, 0.5*PMT::envthickness, bl2_13, bl2_13, 0);
      auto   logE_13 = new G4LogicalVolume(solid13, Construction::Material::Air, env13Name);
      G4double zpos13 = -0.5*toth_13;
      G4double ypos13 = 0;
      for (unsigned int k=0; k<nc_13; ++k) {
        if (k == 0) {
          ypos13 = -3*cm;
        } else if (k == 1) {
          ypos13 = -8*cm;
        } else if (k == 2) {
          ypos13 = -13*cm;
        }
        zpos13 += 0.5*heights_13[k];
        bl2_13   = bl1_13 + heights_13[k]*cfac;

        auto  outersolid13a = new G4Trap("outerchildNames_13[k]", 0.5*heights_13[k], 0, 0, h1_13, bl1_13, bl1_13, 0, h1_13, bl2_13, bl2_13, 0);
        auto  innersolid13a = new G4Trap("innerchildNames_13[k]", 0.5*heights_13[k] - 0.1*cm, 0, 0, h1_13 - 0.1*cm, bl1_13 - 0.1*cm, bl1_13 - 0.1*cm, 0, h1_13 - 0.1*cm, bl2_13 - 0.1*cm, bl2_13 - 0.1*cm, 0);
        G4SubtractionSolid *hollow13 = new G4SubtractionSolid("Hollow 13",outersolid13a,innersolid13a);
        auto   child_13 = new G4LogicalVolume(hollow13, PMT::Material, "Hollow 13");

        new G4PVPlacement(0, G4ThreeVector(0, ypos13, zpos13), child_13, childNames_13[k],
              logE_13, false, 0);
        zpos13 += 0.5*heights_13[k];
        bl1_13  = bl2_13;
      }

      //Now the modules in mother
      G4double bl_13 = 0.5*tan(0.5*PMT::angle)*toth_13 + edge_13;
      G4double xpos_13 = xpos_12 - 0.55*bl_11 - 0.55*bl_13;
      G4double ypos_13 = ypos_11 - 0.39*(toth_12 - toth_13);
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_13, ypos_13, 250.0*cm), logE_13, env13Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos13p = toth_11;
      G4double zpos13p = 0;
      G4double xpos13p = xpos12p - 1.55*PMT::trx;

      for (unsigned int k=0; k<nc_13; ++k) {
        if (k == 0) {
          zpos13p = -3*cm;
        } else if (k == 1) {
          zpos13p = -8*cm;
        } else if (k == 2) {
          zpos13p = -13*cm;
        }
        ypos13p += -heights_13[k];
        xpos13p += -1.5*heights_13[k]*cfac;
        new G4PVPlacement(PMT::rot_5, G4ThreeVector(xpos13p, ypos13p, zpos13p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_13x(0);
      const unsigned int nc_13x(3);
      std::string childNames_13x[nc_13x] = {"B3", "B4", "B5"};
      G4double    heights_13x[nc_13x]    = {30.2766397980939*cm - 0.3*cm,
            34.6732475575531*cm - 0.3*cm, 40.468775967749*cm - 0.3*cm};
      std::string env13xName("Envelope13x");

      toth_13x = toth_13 - 0.3*cm;
      G4double bl1_13x  = 0.5*edge_13 - 0.15*cm;
      G4double bl2_13x  = bl1_13x + toth_13x*cfac;
      G4double h1_13x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid13x = new G4Trap(env13xName, 0.5*toth_13x, 0, 0, 0.5*PMT::envthickness, bl1_13x, bl1_13x, 0, 0.5*PMT::envthickness, bl2_13x, bl2_13x, 0);
      auto   logE_13x = new G4LogicalVolume(solid13x, Construction::Material::Air, env13xName);
      G4double zpos13x = -0.5*toth_13x;
      G4double ypos13x = 0;

      for (unsigned int k=0; k<nc_13x; ++k) {
        if (k == 0) {
          ypos13x = -3*cm;
        } else if (k == 1) {
          ypos13x = -8*cm;
        } else if (k == 2) {
          ypos13x = -13*cm;
        }
        bl2_13x   = bl1_13x + heights_13x[k]*cfac;
        zpos13x += 0.5*heights_13x[k] + 0.15*cm;

        auto  solid13xa = new G4Trap(childNames_13x[k], 0.5*heights_13x[k], 0, 0, h1_13x, bl1_13x, bl1_13x, 0, h1_13x, bl2_13x, bl2_13x, 0);
        auto   child_13x = new G4LogicalVolume(solid13xa, Material::Scintillator, childNames_13x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos13x, zpos13x), child_13x, childNames_13x[k],
              logE_13x, false, 0);
        zpos13x += 0.5*heights_13x[k] + 0.15*cm;
        bl1_13x   = bl2_13x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_13x, env13xName,
              logE_13, false, 0);

    // env 14 //

      const unsigned int nc_14(3);
      std::string childNames_14[nc_14] = {"B3", "B4", "B5"};
      G4double    heights_14[nc_14]    = {30.2766397980939*cm,
            34.6732475575531*cm, 40.4687759677493*cm};
      std::string env14Name("Envelope14");

      G4double toth_14(0);
      for (unsigned int k=0; k<nc_14; ++k) toth_14 += heights_14[k];
      G4double bl1_14  = 0.5*edge_13;
      G4double bl2_14  = bl1_14 + toth_14*cfac;
      G4double h1_14  = 0.5*PMT::thickness;

      auto  solid14 = new G4Trap(env14Name, 0.5*toth_14, 0, 0, 0.5*PMT::envthickness, bl1_14, bl1_14, 0, 0.5*PMT::envthickness, bl2_14, bl2_14, 0);
      auto   logE_14 = new G4LogicalVolume(solid14, Construction::Material::Air, env14Name);
      G4double zpos14 = -0.5*toth_14;
      G4double ypos14 = 0;
      for (unsigned int k=0; k<nc_14; ++k) {
        if (k == 0) {
          ypos14 = -18*cm;
        } else if (k == 1) {
          ypos14 = -3*cm;
        } else if (k == 2) {
          ypos14 = -18*cm;
        }
        zpos14 += 0.5*heights_14[k];
        bl2_14   = bl1_14 + heights_14[k]*cfac;

        auto  outersolid14a = new G4Trap("outerchildNames_14[k]", 0.5*heights_14[k], 0, 0, h1_14, bl1_14, bl1_14, 0, h1_14, bl2_14, bl2_14, 0);
        auto  innersolid14a = new G4Trap("innerchildNames_14[k]", 0.5*heights_14[k] - 0.1*cm, 0, 0, h1_14 - 0.1*cm, bl1_14 - 0.1*cm, bl1_14 - 0.1*cm, 0, h1_14 - 0.1*cm, bl2_14 - 0.1*cm, bl2_14 - 0.1*cm, 0);
        G4SubtractionSolid *hollow14 = new G4SubtractionSolid("Hollow 14",outersolid14a,innersolid14a);
        auto   child_14 = new G4LogicalVolume(hollow14, PMT::Material, "Hollow 14");

        new G4PVPlacement(0, G4ThreeVector(0, ypos14, zpos14), child_14, childNames_14[k],
              logE_14, false, 0);
        zpos14 += 0.5*heights_14[k];
        bl1_14  = bl2_14;
      }

      //Now the modules in mother
      G4double xpos_14 = xpos_13 - 1.1*bl_13;
      G4double ypos_14 = 1.01*ypos_13;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_14, ypos_14, 250.0*cm), logE_14, env14Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos14p = 0.26*toth_11;
      G4double zpos14p = 0;
      G4double xpos14p = xpos12p - 2.4*PMT::trx - bl2_14;

      for (unsigned int k=0; k<nc_14; ++k) {
        if (k == 0) {
          zpos14p = -18*cm;
        } else if (k == 1) {
          zpos14p = -3*cm;
        } else if (k == 2) {
          zpos14p = -18*cm;
        }
        ypos14p += heights_14[k];
        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos14p, ypos14p, zpos14p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_14x(0);
      const unsigned int nc_14x(3);
      std::string childNames_14x[nc_14x] = {"B3", "B4", "B5"};
      G4double    heights_14x[nc_14x]    = {30.2766397980939*cm - 0.3*cm,
            34.6732475575531*cm - 0.3*cm, 40.468775967749*cm - 0.3*cm};
      std::string env14xName("Envelope14x");

      toth_14x = toth_14 - 0.3*cm;
      G4double bl1_14x  = 0.5*edge_13 - 0.15*cm;
      G4double bl2_14x  = bl1_14x + toth_14x*cfac;
      G4double h1_14x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid14x = new G4Trap(env14xName, 0.5*toth_14x, 0, 0, 0.5*PMT::envthickness, bl1_14x, bl1_14x, 0, 0.5*PMT::envthickness, bl2_14x, bl2_14x, 0);
      auto   logE_14x = new G4LogicalVolume(solid14x, Construction::Material::Air, env14xName);
      G4double zpos14x = -0.5*toth_14x;
      G4double ypos14x = 0;

      for (unsigned int k=0; k<nc_14x; ++k) {
        if (k == 0) {
          ypos14x = -18*cm;
        } else if (k == 1) {
          ypos14x = -3*cm;
        } else if (k == 2) {
          ypos14x = -18*cm;
        }
        bl2_14x   = bl1_14x + heights_14x[k]*cfac;
        zpos14x += 0.5*heights_14x[k] + 0.15*cm;

        auto  solid14xa = new G4Trap(childNames_14x[k], 0.5*heights_14x[k], 0, 0, h1_14x, bl1_14x, bl1_14x, 0, h1_14x, bl2_14x, bl2_14x, 0);
        auto   child_14x = new G4LogicalVolume(solid14xa, Material::Scintillator, childNames_14x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos14x, zpos14x), child_14x, childNames_14x[k],
              logE_14x, false, 0);
        zpos14x += 0.5*heights_14x[k] + 0.15*cm;
        bl1_14x   = bl2_14x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_14x, env14xName,
              logE_14, false, 0);

    // env 15 //

      const unsigned int nc_15(2);
      G4double edge_15(22.9*cm);
      std::string childNames_15[nc_15] = {"C3", "C4"};
      G4double    heights_15[nc_15]    = {36.9714743409067*cm,
            42.6670798474789*cm};
      std::string env15Name("Envelope15");

      G4double toth_15(0);
      for (unsigned int k=0; k<nc_15; ++k) toth_15 += heights_15[k];
      G4double bl1_15  = 0.5*edge_15;
      G4double bl2_15  = bl1_15 + toth_15*cfac;
      G4double h1_15  = 0.5*PMT::thickness;

      auto  solid15 = new G4Trap(env15Name, 0.5*toth_15, 0, 0, 0.5*PMT::envthickness, bl1_15, bl1_15, 0, 0.5*PMT::envthickness, bl2_15, bl2_15, 0);
      auto   logE_15 = new G4LogicalVolume(solid15, Construction::Material::Air, env15Name);
      G4double zpos15 = -0.5*toth_15;
      G4double ypos15 = 0;
      for (unsigned int k=0; k<nc_15; ++k) {
        if (k == 0) {
          ypos15 = -13*cm;
        } else if (k == 1) {
          ypos15 = -18*cm;
        }
        zpos15 += 0.5*heights_15[k];
        bl2_15   = bl1_15 + heights_15[k]*cfac;

        auto  outersolid15a = new G4Trap("outerchildNames_15[k]", 0.5*heights_15[k], 0, 0, h1_15, bl1_15, bl1_15, 0, h1_15, bl2_15, bl2_15, 0);
        auto  innersolid15a = new G4Trap("innerchildNames_15[k]", 0.5*heights_15[k] - 0.1*cm, 0, 0, h1_15 - 0.1*cm, bl1_15 - 0.1*cm, bl1_15 - 0.1*cm, 0, h1_15 - 0.1*cm, bl2_15 - 0.1*cm, bl2_15 - 0.1*cm, 0);
        G4SubtractionSolid *hollow15 = new G4SubtractionSolid("Hollow 15",outersolid15a,innersolid15a);
        auto   child_15 = new G4LogicalVolume(hollow15, PMT::Material, "Hollow 15");

        new G4PVPlacement(0, G4ThreeVector(0, ypos15, zpos15), child_15, childNames_15[k],
              logE_15, false, 0);
        zpos15 += 0.5*heights_15[k];
        bl1_15  = bl2_15;
      }

      //Now the modules in mother
      G4double bl_15 = 0.5*tan(0.5*PMT::angle)*toth_15 + edge_15;
      G4double xpos_15 = xpos_B11_4 - 0.5*bl_B11_1 - 0.55*bl_15;
      G4double ypos_15 = ypos_B11_4 + 7.5*cm;
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_15, ypos_15, 250.0*cm), logE_15, env15Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos15p = ypos_B11_4p + 3.*cm;
      G4double zpos15p = 0;
      G4double xpos15p = xpos_B11_4p + PMT::trx;

      for (unsigned int k=0; k<nc_15; ++k) {
        if (k == 0) {
          zpos15p = -13*cm;
        } else if (k == 1) {
          zpos15p = -18*cm;
        }
        ypos15p += -heights_15[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos15p, ypos15p, zpos15p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_15x(0);
      const unsigned int nc_15x(2);
      std::string childNames_15x[nc_15x] = {"C3", "C4"};
      G4double    heights_15x[nc_15x]    = {36.9714743409067*cm - 0.3*cm,
            42.6670798474789*cm - 0.3*cm};
      std::string env15xName("Envelope15x");

      toth_15x = toth_15 - 0.3*cm;
      G4double bl1_15x  = 0.5*edge_15 - 0.15*cm;
      G4double bl2_15x  = bl1_15x + toth_15x*cfac;
      G4double h1_15x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid15x = new G4Trap(env15xName, 0.5*toth_15x, 0, 0, 0.5*PMT::envthickness, bl1_15x, bl1_15x, 0, 0.5*PMT::envthickness, bl2_15x, bl2_15x, 0);
      auto   logE_15x = new G4LogicalVolume(solid15x, Construction::Material::Air, env15xName);
      G4double zpos15x = -0.5*toth_15x;
      G4double ypos15x = 0;

      for (unsigned int k=0; k<nc_15x; ++k) {
        if (k == 0) {
          ypos15x = -13*cm;
        } else if (k == 1) {
          ypos15x = -18*cm;
        }

        bl2_15x   = bl1_15x + heights_15x[k]*cfac;
        zpos15x += 0.5*heights_15x[k] + 0.15*cm;

        auto  solid15xa = new G4Trap(childNames_15x[k], 0.5*heights_15x[k], 0, 0, h1_15x, bl1_15x, bl1_15x, 0, h1_15x, bl2_15x, bl2_15x, 0);
        auto   child_15x = new G4LogicalVolume(solid15xa, Material::Scintillator, childNames_15x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos15x, zpos15x), child_15x, childNames_15x[k],
              logE_15x, false, 0);
        zpos15x += 0.5*heights_15x[k] + 0.15*cm;
        bl1_15x   = bl2_15x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_15x, env15xName,
              logE_15, false, 0);

      // env 16 //

      const unsigned int nc_16(2);
      std::string childNames_16[nc_16] = {"C3", "C4"};
      G4double    heights_16[nc_16]    = {36.9714743409067*cm,
            42.6670798474789*cm};
      std::string env16Name("Envelope16");

      G4double toth_16(0);
      for (unsigned int k=0; k<nc_16; ++k) toth_16 += heights_16[k];
      G4double bl1_16  = 0.5*edge_15;
      G4double bl2_16  = bl1_16 + toth_16*cfac;
      G4double h1_16  = 0.5*PMT::thickness;

      auto  solid16 = new G4Trap(env16Name, 0.5*toth_16, 0, 0, 0.5*PMT::envthickness, bl1_16, bl1_16, 0, 0.5*PMT::envthickness, bl2_16, bl2_16, 0);
      auto   logE_16 = new G4LogicalVolume(solid16, Construction::Material::Air, env16Name);
      G4double zpos16 = -0.5*toth_16;
      G4double ypos16 = 0;
      for (unsigned int k=0; k<nc_16; ++k) {
        if (k == 0) {
          ypos16 = -13*cm;
        } else if (k == 1) {
          ypos16 = -8*cm;
        }
        zpos16 += 0.5*heights_16[k];
        bl2_16   = bl1_16 + heights_16[k]*cfac;

        auto  outersolid16a = new G4Trap("outerchildNames_16[k]", 0.5*heights_16[k], 0, 0, h1_16, bl1_16, bl1_16, 0, h1_16, bl2_16, bl2_16, 0);
        auto  innersolid16a = new G4Trap("innerchildNames_16[k]", 0.5*heights_16[k] - 0.1*cm, 0, 0, h1_16 - 0.1*cm, bl1_16 - 0.1*cm, bl1_16 - 0.1*cm, 0, h1_16 - 0.1*cm, bl2_16 - 0.1*cm, bl2_16 - 0.1*cm, 0);
        G4SubtractionSolid *hollow16 = new G4SubtractionSolid("Hollow 16",outersolid16a,innersolid16a);
        auto   child_16 = new G4LogicalVolume(hollow16, PMT::Material, "Hollow 16");

        new G4PVPlacement(0, G4ThreeVector(0, ypos16, zpos16), child_16, childNames_16[k],
              logE_16, false, 0);
        zpos16 += 0.5*heights_16[k];
        bl1_16  = bl2_16;
      }

      //Now the modules in mother
      G4double xpos_16 = xpos_15 - 1.05*bl_15;
      G4double ypos_16 = 0.99*ypos_15;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_16, ypos_16, 250.0*cm), logE_16, env16Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos16p = ypos_B11_4p + 1.5*36.9714743409067*cm;
      G4double zpos16p = 0;
      G4double xpos16p = xpos15p - 2.15*bl_15 - PMT::trx;

      for (unsigned int k=0; k<nc_16; ++k) {
        if (k == 0) {
          zpos16p = -13*cm;
        } else if (k == 1) {
          zpos16p = -8*cm;
        }
        ypos16p += -heights_16[k];
        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos16p, ypos16p, zpos16p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_16x(0);
      const unsigned int nc_16x(2);
      std::string childNames_16x[nc_16x] = {"C3", "C4"};
      G4double    heights_16x[nc_16x]    = {36.9714743409067*cm - 0.3*cm,
            42.6670798474789*cm - 0.3*cm};
      std::string env16xName("Envelope16x");

      toth_16x = toth_16 - 0.3*cm;
      G4double bl1_16x  = 0.5*edge_15 - 0.15*cm;
      G4double bl2_16x  = bl1_16x + toth_16x*cfac;
      G4double h1_16x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid16x = new G4Trap(env16xName, 0.5*toth_16x, 0, 0, 0.5*PMT::envthickness, bl1_16x, bl1_16x, 0, 0.5*PMT::envthickness, bl2_16x, bl2_16x, 0);
      auto   logE_16x = new G4LogicalVolume(solid16x, Construction::Material::Air, env16xName);
      G4double zpos16x = -0.5*toth_16x;
      G4double ypos16x = 0;

      for (unsigned int k=0; k<nc_16x; ++k) {
        if (k == 0) {
          ypos16x = -13*cm;
        } else if (k == 1) {
          ypos16x = -8*cm;
        }

        bl2_16x   = bl1_16x + heights_16x[k]*cfac;
        zpos16x += 0.5*heights_16x[k] + 0.15*cm;

        auto  solid16xa = new G4Trap(childNames_16x[k], 0.5*heights_16x[k], 0, 0, h1_16x, bl1_16x, bl1_16x, 0, h1_16x, bl2_16x, bl2_16x, 0);
        auto   child_16x = new G4LogicalVolume(solid16xa, Material::Scintillator, childNames_16x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos16x, zpos16x), child_16x, childNames_16x[k],
              logE_16x, false, 0);
        zpos16x += 0.5*heights_16x[k] + 0.15*cm;
        bl1_16x   = bl2_16x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_16x, env16xName,
              logE_16, false, 0);

    // env 17 //

      const unsigned int nc_17(2);
      G4double edge_17(25.8*cm);
      std::string childNames_17[nc_17] = {"A9", "A10"};
      G4double    heights_17[nc_17]    = {50.0613747156602*cm,
            34.2735559430568*cm};
      std::string env17Name("Envelope17");

      G4double toth_17(0);
      for (unsigned int k=0; k<nc_17; ++k) toth_17 += heights_17[k];
      G4double bl1_17  = 0.5*edge_17;
      G4double bl2_17  = bl1_17 + toth_17*cfac;
      G4double h1_17  = 0.5*PMT::thickness;

      auto  solid17 = new G4Trap(env17Name, 0.5*toth_17, 0, 0, 0.5*PMT::envthickness, bl1_17, bl1_17, 0, 0.5*PMT::envthickness, bl2_17, bl2_17, 0);
      auto   logE_17 = new G4LogicalVolume(solid17, Construction::Material::Air, env17Name);
      G4double zpos17 = -0.5*toth_17;
      G4double ypos17 = 0;
      for (unsigned int k=0; k<nc_17; ++k) {
        if (k == 0) {
          ypos17 = -3*cm;
        } else if (k == 1) {
          ypos17 = -18*cm;
        }
        zpos17 += 0.5*heights_17[k];
        bl2_17   = bl1_17 + heights_17[k]*cfac;

        auto  outersolid17a = new G4Trap("outerchildNames_17[k]", 0.5*heights_17[k], 0, 0, h1_17, bl1_17, bl1_17, 0, h1_17, bl2_17, bl2_17, 0);
        auto  innersolid17a = new G4Trap("innerchildNames_17[k]", 0.5*heights_17[k] - 0.1*cm, 0, 0, h1_17 - 0.1*cm, bl1_17 - 0.1*cm, bl1_17 - 0.1*cm, 0, h1_17 - 0.1*cm, bl2_17 - 0.1*cm, bl2_17 - 0.1*cm, 0);
        G4SubtractionSolid *hollow17 = new G4SubtractionSolid("Hollow 17",outersolid17a,innersolid17a);
        auto   child_17 = new G4LogicalVolume(hollow17, PMT::Material, "Hollow 17");

        new G4PVPlacement(0, G4ThreeVector(0, ypos17, zpos17), child_17, childNames_17[k],
              logE_17, false, 0);
        zpos17 += 0.5*heights_17[k];
        bl1_17  = bl2_17;
      }

      //Now the modules in mother
      G4double bl_17 = 0.5*tan(0.5*PMT::angle)*toth_17 + edge_17;
      G4double xpos_17 = xpos_16 - 0.55*bl_15 - 0.5*bl_17;
      G4double ypos_17 = 0.99*ypos_16 + 0.5*(toth_16 - toth_17);
      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_17, ypos_17, 250.0*cm), logE_17, env17Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos17p = ypos_B11_4p + 4*cm;
      G4double zpos17p = 0;
      G4double xpos17p = xpos16p + PMT::trx;

      for (unsigned int k=0; k<nc_17; ++k) {
        if (k == 0) {
          zpos17p = -3*cm;
        } else if (k == 1) {
          zpos17p = -18*cm;
        }
        ypos17p += -heights_17[k];
        new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos17p, ypos17p, zpos17p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_17x(0);
      const unsigned int nc_17x(2);
      std::string childNames_17x[nc_17x] = {"A9", "A10"};
      G4double    heights_17x[nc_17x]    = {50.0613747156602*cm - 0.3*cm,
            34.2735559430568*cm - 0.3*cm};
      std::string env17xName("Envelope17x");

      toth_17x = toth_17 - 0.3*cm;
      G4double bl1_17x  = 0.5*edge_17 - 0.15*cm;
      G4double bl2_17x  = bl1_17x + toth_17x*cfac;
      G4double h1_17x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid17x = new G4Trap(env17xName, 0.5*toth_17x, 0, 0, 0.5*PMT::envthickness, bl1_17x, bl1_17x, 0, 0.5*PMT::envthickness, bl2_17x, bl2_17x, 0);
      auto   logE_17x = new G4LogicalVolume(solid17x, Construction::Material::Air, env17xName);
      G4double zpos17x = -0.5*toth_17x;
      G4double ypos17x = 0;

      for (unsigned int k=0; k<nc_17x; ++k) {
        if (k == 0) {
          ypos17x = -3*cm;
        } else if (k == 1) {
          ypos17x = -18*cm;
        }

        bl2_17x   = bl1_17x + heights_17x[k]*cfac;
        zpos17x += 0.5*heights_17x[k] + 0.15*cm;

        auto  solid17xa = new G4Trap(childNames_17x[k], 0.5*heights_17x[k], 0, 0, h1_17x, bl1_17x, bl1_17x, 0, h1_17x, bl2_17x, bl2_17x, 0);
        auto   child_17x = new G4LogicalVolume(solid17xa, Material::Scintillator, childNames_17x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos17x, zpos17x), child_17x, childNames_17x[k],
              logE_17x, false, 0);
        zpos17x += 0.5*heights_17x[k] + 0.15*cm;
        bl1_17x   = bl2_17x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_17x, env17xName,
              logE_17, false, 0);

    // env 18 //

      const unsigned int nc_18(2);
      std::string childNames_18[nc_18] = {"A9", "A10"};
      G4double    heights_18[nc_18]    = {50.0613747156602*cm,
            34.2735559430568*cm};
      std::string env18Name("Envelope18");

      G4double toth_18(0);
      for (unsigned int k=0; k<nc_18; ++k) toth_18 += heights_18[k];
      G4double bl1_18  = 0.5*edge_17;
      G4double bl2_18  = bl1_18 + toth_18*cfac;
      G4double h1_18  = 0.5*PMT::thickness;

      auto  solid18 = new G4Trap(env18Name, 0.5*toth_18, 0, 0, 0.5*PMT::envthickness, bl1_18, bl1_18, 0, 0.5*PMT::envthickness, bl2_18, bl2_18, 0);
      auto   logE_18 = new G4LogicalVolume(solid18, Construction::Material::Air, env18Name);
      G4double zpos18 = -0.5*toth_18;
      G4double ypos18 = 0;
      for (unsigned int k=0; k<nc_18; ++k) {
        if (k == 0) {
          ypos18 = -13*cm;
        } else if (k == 1) {
          ypos18 = -18*cm;
        }
        zpos18 += 0.5*heights_18[k];
        bl2_18   = bl1_18 + heights_18[k]*cfac;

        auto  outersolid18a = new G4Trap("outerchildNames_18[k]", 0.5*heights_18[k], 0, 0, h1_18, bl1_18, bl1_18, 0, h1_18, bl2_18, bl2_18, 0);
        auto  innersolid18a = new G4Trap("innerchildNames_18[k]", 0.5*heights_18[k] - 0.1*cm, 0, 0, h1_18 - 0.1*cm, bl1_18 - 0.1*cm, bl1_18 - 0.1*cm, 0, h1_18 - 0.1*cm, bl2_18 - 0.1*cm, bl2_18 - 0.1*cm, 0);
        G4SubtractionSolid *hollow18 = new G4SubtractionSolid("Hollow 18",outersolid18a,innersolid18a);
        auto   child_18 = new G4LogicalVolume(hollow18, PMT::Material, "Hollow 18");

        new G4PVPlacement(0, G4ThreeVector(0, ypos18, zpos18), child_18, childNames_18[k],
              logE_18, false, 0);
        zpos18 += 0.5*heights_18[k];
        bl1_18  = bl2_18;
      }

      //Now the modules in mother
      G4double xpos_18 = xpos_17 - 1.05*bl_17;
      G4double ypos_18 = 0.99*ypos_17;
      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_18, ypos_18, 250.0*cm), logE_18, env18Name,
              Calorimeter, false, 0);

      // pmt //

      G4double ypos18p = ypos_B11_4p + PMT::trx + 58.0613747156602*cm;
      G4double zpos18p = 0;
      G4double xpos18p = xpos17p - 2.1*bl_17 - PMT::trx;

      for (unsigned int k=0; k<nc_18; ++k) {
        if (k == 0) {
          zpos18p = -13*cm;
        } else if (k == 1) {
          zpos18p = -18*cm;
        }
        ypos18p += -heights_18[k];
        new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos18p, ypos18p, zpos18p), Cylinder, "pmt",
              Calorimeter, false, 0);
      }

      // Inside //

      G4double toth_18x(0);
      const unsigned int nc_18x(2);
      std::string childNames_18x[nc_18x] = {"A9", "A10"};
      G4double    heights_18x[nc_18x]    = {50.0613747156602*cm - 0.3*cm,
            34.2735559430568*cm - 0.3*cm};
      std::string env18xName("Envelope18x");

      toth_18x = toth_18 - 0.3*cm;
      G4double bl1_18x  = 0.5*edge_17 - 0.15*cm;
      G4double bl2_18x  = bl1_18x + toth_18x*cfac;
      G4double h1_18x  = 0.5*PMT::thickness - 0.15*cm;

      auto  solid18x = new G4Trap(env18xName, 0.5*toth_18x, 0, 0, 0.5*PMT::envthickness, bl1_18x, bl1_18x, 0, 0.5*PMT::envthickness, bl2_18x, bl2_18x, 0);
      auto   logE_18x = new G4LogicalVolume(solid18x, Construction::Material::Air, env18xName);
      G4double zpos18x = -0.5*toth_18x;
      G4double ypos18x = 0;

      for (unsigned int k=0; k<nc_18x; ++k) {
        if (k == 0) {
          ypos18x = -13*cm;
        } else if (k == 1) {
          ypos18x = -18*cm;
        }

        bl2_18x   = bl1_18x + heights_18x[k]*cfac;
        zpos18x += 0.5*heights_18x[k] + 0.15*cm;

        auto  solid18xa = new G4Trap(childNames_18x[k], 0.5*heights_18x[k], 0, 0, h1_18x, bl1_18x, bl1_18x, 0, h1_18x, bl2_18x, bl2_18x, 0);
        auto   child_18x = new G4LogicalVolume(solid18xa, Material::Scintillator, childNames_18x[k]);

        new G4PVPlacement(0, G4ThreeVector(0, ypos18x, zpos18x), child_18x, childNames_18x[k],
              logE_18x, false, 0);
        zpos18x += 0.5*heights_18x[k] + 0.15*cm;
        bl1_18x   = bl2_18x + 0.3*cm*cfac;
      }

      //Now the modules in mother
      new G4PVPlacement(0, G4ThreeVector(), logE_18x, env18xName,
              logE_18, false, 0);

    // C9_1 //

      G4double edge_C9_1(49.3*cm);
      G4double bl1_C9_1 = 0.5*edge_C9_1;
      G4double bl2_C9_1 = 0.5*53*cm;
      G4double heightC9_1 = 46.8638417996899*cm;
      G4double h1_C9_1 = 0.5*PMT::thickness;

      auto  outersolidC9_1 = new G4Trap("outerC9_1", 0.5*heightC9_1, 0, 0, h1_C9_1, bl1_C9_1, bl1_C9_1, 0, h1_C9_1, bl2_C9_1, bl2_C9_1, 0);
      auto  innersolidC9_1 = new G4Trap("innerC9_1", 0.5*heightC9_1 - 0.1*cm, 0, 0, h1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, 0, h1_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC9_1 = new G4SubtractionSolid("Hollow C9_1",outersolidC9_1,innersolidC9_1);
      auto   solidCalorimeter9_1 = new G4LogicalVolume(hollowC9_1, PMT::Material, "C9_1");

      G4double bl_C9_1 = 0.5*tan(0.5*PMT::angle)*heightC9_1 + edge_C9_1;
      G4double xpos_C9_1 = xpos_15 - 0.5*(bl_C9_1 - bl_15);
      G4double ypos_C9_1 = ypos_15 + 0.505*(heightC9_1 + toth_15);

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C9_1, ypos_C9_1, 232.0*cm), solidCalorimeter9_1, "C9_1",
              Calorimeter, false, 0);

      G4double edge_C9_1x(49.3*cm);
      G4double bl1_C9_1x = 0.5*edge_C9_1x - 0.15*cm;
      G4double bl2_C9_1x = 0.5*53*cm - 0.15*cm;
      G4double heightC9_1x = 46.8638417996899*cm - 0.3*cm;
      G4double h1_C9_1x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidC9_1x = new G4Trap("C9_1x", 0.5*heightC9_1x, 0, 0, h1_C9_1x, bl1_C9_1x, bl1_C9_1x, 0, h1_C9_1x, bl2_C9_1x, bl2_C9_1x, 0);
      auto   solidCalorimeter9_1x = new G4LogicalVolume(solidC9_1x, Material::Scintillator, "C9_1x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C9_1, ypos_C9_1, 232.0*cm), solidCalorimeter9_1x, "C9_1x",
              Calorimeter, false, 0);

      G4double xpos_C9_1p = xpos15p;
      G4double ypos_C9_1p = ypos15p + toth_15;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_C9_1p, ypos_C9_1p, 232.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C9_2 //

      auto  outersolidC9_2 = new G4Trap("outerC9_2", 0.5*heightC9_1, 0, 0, h1_C9_1, bl1_C9_1, bl1_C9_1, 0, h1_C9_1, bl2_C9_1, bl2_C9_1, 0);
      auto  innersolidC9_2 = new G4Trap("innerC9_2", 0.5*heightC9_1 - 0.1*cm, 0, 0, h1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, 0, h1_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC9_2 = new G4SubtractionSolid("Hollow C9_2",outersolidC9_2,innersolidC9_2);
      auto   solidCalorimeter9_2 = new G4LogicalVolume(hollowC9_2, PMT::Material, "C9_2");

      G4double xpos_C9_2 = xpos_C9_1;
      G4double ypos_C9_2 = ypos_C9_1 + heightC9_1;

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C9_2, ypos_C9_2, 247.0*cm), solidCalorimeter9_2, "C9_2",
              Calorimeter, false, 0);

      auto  solidC9_2x = new G4Trap("C9_2x", 0.5*heightC9_1x, 0, 0, h1_C9_1x, bl1_C9_1x, bl1_C9_1x, 0, h1_C9_1x, bl2_C9_1x, bl2_C9_1x, 0);
      auto   solidCalorimeter9_2x = new G4LogicalVolume(solidC9_2x, Material::Scintillator, "C9_2x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C9_2, ypos_C9_2, 247.0*cm), solidCalorimeter9_2x, "C9_2x",
              Calorimeter, false, 0);

      G4double xpos_C9_2p = xpos15p;
      G4double ypos_C9_2p = ypos_C9_1p + heightC9_1;

      new G4PVPlacement(PMT::rot_4, G4ThreeVector(xpos_C9_2p, ypos_C9_2p, 247.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C9_3 //

      auto  outersolidC9_3 = new G4Trap("outerC9_3", 0.5*heightC9_1, 0, 0, h1_C9_1, bl1_C9_1, bl1_C9_1, 0, h1_C9_1, bl2_C9_1, bl2_C9_1, 0);
      auto  innersolidC9_3 = new G4Trap("innerC9_3", 0.5*heightC9_1 - 0.1*cm, 0, 0, h1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, 0, h1_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC9_3 = new G4SubtractionSolid("Hollow C9_3",outersolidC9_3,innersolidC9_3);
      auto   solidCalorimeter9_3 = new G4LogicalVolume(hollowC9_3, PMT::Material, "C9_3");

      G4double xpos_C9_3 = xpos_C9_1 - 1.01*bl_C9_1;
      G4double ypos_C9_3 = 0.95*ypos_C9_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C9_3, ypos_C9_3, 237.0*cm), solidCalorimeter9_3, "C9_3",
              Calorimeter, false, 0);

      auto  solidC9_3x = new G4Trap("C9_3x", 0.5*heightC9_1x, 0, 0, h1_C9_1x, bl1_C9_1x, bl1_C9_1x, 0, h1_C9_1x, bl2_C9_1x, bl2_C9_1x, 0);
      auto   solidCalorimeter9_3x = new G4LogicalVolume(solidC9_3x, Material::Scintillator, "C9_3x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C9_3, ypos_C9_3, 237.0*cm), solidCalorimeter9_3x, "C9_3x",
              Calorimeter, false, 0);

      G4double xpos_C9_3p = xpos_C9_1p - 2.05*bl_C9_1 - PMT::trx;
      G4double ypos_C9_3p = ypos_C9_2p + 1.4*PMT::trx;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_C9_3p, ypos_C9_3p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C9_4 //

      auto  outersolidC9_4 = new G4Trap("outerC9_4", 0.5*heightC9_1, 0, 0, h1_C9_1, bl1_C9_1, bl1_C9_1, 0, h1_C9_1, bl2_C9_1, bl2_C9_1, 0);
      auto  innersolidC9_4 = new G4Trap("innerC9_4", 0.5*heightC9_1 - 0.1*cm, 0, 0, h1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, bl1_C9_1 - 0.1*cm, 0, h1_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, bl2_C9_1 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC9_4 = new G4SubtractionSolid("Hollow C9_4",outersolidC9_4,innersolidC9_4);
      auto   solidCalorimeter9_4 = new G4LogicalVolume(hollowC9_4, PMT::Material, "C9_4");

      G4double xpos_C9_4 = xpos_C9_1 - 1.01*bl_C9_1;
      G4double ypos_C9_4 = ypos_C9_3 + heightC9_1;

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C9_4, ypos_C9_4, 242.0*cm), solidCalorimeter9_4, "C9_4",
              Calorimeter, false, 0);

      auto  solidC9_4x = new G4Trap("C9_4x", 0.5*heightC9_1x, 0, 0, h1_C9_1x, bl1_C9_1x, bl1_C9_1x, 0, h1_C9_1x, bl2_C9_1x, bl2_C9_1x, 0);
      auto   solidCalorimeter9_4x = new G4LogicalVolume(solidC9_4x, Material::Scintillator, "C9_4x");

      new G4PVPlacement(PMT::rot_1, G4ThreeVector(xpos_C9_4, ypos_C9_4, 242.0*cm), solidCalorimeter9_4x, "C9_4x",
              Calorimeter, false, 0);

      G4double xpos_C9_4p = xpos_C9_3p;
      G4double ypos_C9_4p = ypos_C9_3p + heightC9_1;

      new G4PVPlacement(PMT::rot_3, G4ThreeVector(xpos_C9_4p, ypos_C9_4p, 242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // C5 //  FLIP

      G4double edge_C5(29.1*cm);
      G4double bl1_C5 = 0.5*edge_C5;
      G4double bl2_C5 = 0.5*32.5*cm;
      G4double heightC5 = 49.5617601975399*cm;
      G4double h1_C5 = 0.5*PMT::thickness;

      auto  outersolidC5 = new G4Trap("outerC5", 0.5*heightC5, 0, 0, h1_C5, bl1_C5, bl1_C5, 0, h1_C5, bl2_C5, bl2_C5, 0);
      auto  innersolidC5 = new G4Trap("innerC5", 0.5*heightC5 - 0.1*cm, 0, 0, h1_C5 - 0.1*cm, bl1_C5 - 0.1*cm, bl1_C5 - 0.1*cm, 0, h1_C5 - 0.1*cm, bl2_C5 - 0.1*cm, bl2_C5 - 0.1*cm, 0);
      G4SubtractionSolid *hollowC5 = new G4SubtractionSolid("Hollow C5",outersolidC5,innersolidC5);
      auto   solidCalorimeter5 = new G4LogicalVolume(hollowC5, PMT::Material, "C5");

      G4double bl_C5 = 0.5*tan(0.5*PMT::angle)*heightC5 + edge_C5;
      G4double xpos_C5 = xpos_14 - 0.55*bl_C5 - 0.5*bl_13;
      G4double ypos_C5 = ypos_C9_4 + 0.505*(heightC9_1 + heightC5);

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C5, ypos_C5, 237.0*cm), solidCalorimeter5, "C5",
              Calorimeter, false, 0);

      G4double edge_C5x(29.1*cm);
      G4double bl1_C5x = 0.5*edge_C5x - 0.15*cm;
      G4double bl2_C5x = 0.5*32.5*cm - 0.15*cm;
      G4double heightC5x = 49.5617601975399*cm - 0.3*cm;
      G4double h1_C5x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidC5x = new G4Trap("C5x", 0.5*heightC5x, 0, 0, h1_C5x, bl1_C5x, bl1_C5x, 0, h1_C5x, bl2_C5x, bl2_C5x, 0);
      auto   solidCalorimeter5x = new G4LogicalVolume(solidC5x, Material::Scintillator, "C5x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_C5, ypos_C5, 237.0*cm), solidCalorimeter5x, "C5x",
              Calorimeter, false, 0);

      G4double xpos_C5p = xpos_C9_4p - 8*cm - PMT::trx;
      G4double ypos_C5p = ypos_C9_4p - PMT::trx;

      new G4PVPlacement(PMT::rot_5, G4ThreeVector(xpos_C5p, ypos_C5p, 237.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    // A7 //

      G4double edge_A7(19.8*cm);
      G4double bl1_A7 = 0.5*edge_A7;
      G4double bl2_A7 = 0.5*22.3*cm;
      G4double heightA7 = 34.1736330394327*cm;
      G4double h1_A7 = 0.5*PMT::thickness;

      auto  outersolidA7 = new G4Trap("outerA7", 0.5*heightA7, 0, 0, h1_A7, bl1_A7, bl1_A7, 0, h1_A7, bl2_A7, bl2_A7, 0);
      auto  innersolidA7 = new G4Trap("innerA7", 0.5*heightA7 - 0.1*cm, 0, 0, h1_A7 - 0.1*cm, bl1_A7 - 0.1*cm, bl1_A7 - 0.1*cm, 0, h1_A7 - 0.1*cm, bl2_A7 - 0.1*cm, bl2_A7 - 0.1*cm, 0);
      G4SubtractionSolid *hollowA7 = new G4SubtractionSolid("Hollow A7",outersolidA7,innersolidA7);
      auto   solidlogA7 = new G4LogicalVolume(hollowA7, PMT::Material, "A7");

      G4double xpos_A7 = 0.965*xpos_C5;
      G4double ypos_A7 = ypos_C5 + 0.5*(heightC5 + heightA7);

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_A7, ypos_A7, 242.0*cm), solidlogA7, "A7",
              Calorimeter, false, 0);

      G4double edge_A7x(19.8*cm);
      G4double bl1_A7x = 0.5*edge_A7x - 0.15*cm;
      G4double bl2_A7x = 0.5*22.3*cm - 0.15*cm;
      G4double heightA7x = 34.1736330394327*cm - 0.3*cm;
      G4double h1_A7x = 0.5*PMT::thickness - 0.15*cm;

      auto  solidA7x = new G4Trap("A7x", 0.5*heightA7x, 0, 0, h1_A7x, bl1_A7x, bl1_A7x, 0, h1_A7x, bl2_A7x, bl2_A7x, 0);
      auto   solidlogA7x = new G4LogicalVolume(solidA7x, Material::Scintillator, "A7x");

      new G4PVPlacement(PMT::rot_2, G4ThreeVector(xpos_A7, ypos_A7, 242.0*cm), solidlogA7x, "A7x",
              Calorimeter, false, 0);

      G4double xpos_A7p = xpos_C5p + 12*cm;
      G4double ypos_A7p = ypos_C5p + heightC5;

      new G4PVPlacement(PMT::rot_5, G4ThreeVector(xpos_A7p, ypos_A7p, 242.0*cm), Cylinder, "pmt",
              Calorimeter, false, 0);

    return Construction::PlaceVolume(Calorimeter, world);
}

} } /* namespace MATHUSLA::MU */