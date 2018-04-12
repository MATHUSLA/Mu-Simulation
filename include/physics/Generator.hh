#ifndef MU__PHYSICS_GENERATOR_HH
#define MU__PHYSICS_GENERATOR_HH
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
            const int id,
            const double pT,
            const double eta,
            const double phi);

  virtual ~Generator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  inline const G4String& GetName() const { return _name; }
  inline int             id()      const { return _id;   }
  inline double          pT()      const { return _pT;   }
  inline double          eta()     const { return _eta;  }
  inline double          phi()     const { return _phi;  }

  virtual const std::string InfoString() const;

  static G4PrimaryVertex* DefaultVertex();
  static G4PrimaryParticle* CreateParticle(const int id,
                                           const G4ThreeVector& momentum);

  static G4PrimaryParticle* CreateParticle(const int id,
                                           const double pT,
                                           const double eta,
                                           const double phi);

  static const G4String MessengerDirectory;

protected:
  G4String _name;
  G4String _description;

  int    _id;
  double _pT;
  double _eta;
  double _phi;

  G4CMD_Integer*    _ui_id;
  G4CMD_DoubleUnit* _ui_pT;
  G4CMD_Double*     _ui_eta;
  G4CMD_DoubleUnit* _ui_phi;
};

class RangeGenerator : public Generator {
public:
  RangeGenerator(const G4String& name,
                 const G4String& description,
                 const int id,
                 const double pT,
                 const double eta,
                 const double phi);

  RangeGenerator(const G4String& name,
                 const G4String& description,
                 const int id,
                 const double pT_min,
                 const double pT_max,
                 const double eta_min,
                 const double eta_max,
                 const double phi_min,
                 const double phi_max);

  virtual ~RangeGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event* event);
  virtual void SetNewValue(G4UIcommand* command, G4String value);

  inline double pT_min()  const { return _pT_min;  }
  inline double pT_max()  const { return _pT_max;  }
  inline double eta_min() const { return _eta_min; }
  inline double eta_max() const { return _eta_max; }
  inline double phi_min() const { return _phi_min; }
  inline double phi_max() const { return _phi_max; }

  virtual const std::string InfoString() const;

protected:
  double _pT_min;
  double _pT_max;
  double _eta_min;
  double _eta_max;
  double _phi_min;
  double _phi_max;

  G4CMD_DoubleUnit* _ui_pT_min;
  G4CMD_DoubleUnit* _ui_pT_max;
  G4CMD_Double*     _ui_eta_min;
  G4CMD_Double*     _ui_eta_max;
  G4CMD_DoubleUnit* _ui_phi_min;
  G4CMD_DoubleUnit* _ui_phi_max;
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_GENERATOR_HH */
