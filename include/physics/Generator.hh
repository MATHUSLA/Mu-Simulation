#ifndef PHYSICS_GENERATOR_HH
#define PHYSICS_GENERATOR_HH
#pragma once

#include "Geant4/G4Event.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4PrimaryParticle.hh"

#include "ui/Command.hh"

namespace MATHUSLA { namespace MU {

class Generator : public G4UImessenger {
public:
  Generator(const G4String& name,
            const G4String& description,
            const G4int id,
            const G4double pT,
            const G4double eta,
            const G4double phi);

  virtual ~Generator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  inline const G4String& GetName() const { return _name; }
  inline G4int           id()      const { return _id;   }
  inline G4double        pT()      const { return _pT;   }
  inline G4double        eta()     const { return _eta;  }
  inline G4double        phi()     const { return _phi;  }

  virtual const std::string InfoString() const;

  static G4PrimaryVertex* DefaultVertex();
  static G4PrimaryParticle* CreateParticle(const G4int id,
                                           const G4ThreeVector& momentum);

  static G4PrimaryParticle* CreateParticle(const G4int id,
                                           const G4double pT,
                                           const G4double eta,
                                           const G4double phi);

  static const G4String MessengerDirectory;

protected:
  G4String _name;
  G4String _description;

  G4int    _id;
  G4double _pT;
  G4double _eta;
  G4double _phi;

  G4CMD_Integer*    _ui_id;
  G4CMD_DoubleUnit* _ui_pT;
  G4CMD_Double*     _ui_eta;
  G4CMD_DoubleUnit* _ui_phi;
};

class RangeGenerator : public Generator {
public:
  RangeGenerator(const G4String& name,
                 const G4String& description,
                 const G4int id,
                 const G4double pT,
                 const G4double eta,
                 const G4double phi);

  RangeGenerator(const G4String& name,
                 const G4String& description,
                 const G4int id,
                 const G4double pT_min,
                 const G4double pT_max,
                 const G4double eta_min,
                 const G4double eta_max,
                 const G4double phi_min,
                 const G4double phi_max);

  virtual ~RangeGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  inline G4double pT_min()  const { return _pT_min;  }
  inline G4double pT_max()  const { return _pT_max;  }
  inline G4double eta_min() const { return _eta_min; }
  inline G4double eta_max() const { return _eta_max; }
  inline G4double phi_min() const { return _phi_min; }
  inline G4double phi_max() const { return _phi_max; }

  virtual const std::string InfoString() const;

protected:
  G4double _pT_min;
  G4double _pT_max;
  G4double _eta_min;
  G4double _eta_max;
  G4double _phi_min;
  G4double _phi_max;

  G4CMD_DoubleUnit* _ui_pT_min;
  G4CMD_DoubleUnit* _ui_pT_max;
  G4CMD_Double*     _ui_eta_min;
  G4CMD_Double*     _ui_eta_max;
  G4CMD_DoubleUnit* _ui_phi_min;
  G4CMD_DoubleUnit* _ui_phi_max;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_GENERATOR_HH */
