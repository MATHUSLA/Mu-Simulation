#ifndef PHYSICS_FILTER_HH
#define PHYSICS_FILTER_HH
#pragma once

#include "Geant4/globals.hh"

namespace MATHUSLA { namespace MU {

class Filter {
public:
  enum class Ordering { Greater, GreaterEqual, Equal, LesserEqual, Lesser };
  enum class RangeOrdering { Inside, Outside };

  static Filter id(G4int id);
  static Filter id(std::initializer_list<G4int> ids);

  static Filter pT(G4double pT,
                   Ordering ord=Ordering::GreaterEqual);

  static Filter eta(G4double eta,
                    Ordering ord=Ordering::LesserEqual);

  static Filter phi(G4double phi_min, G4double phi_max,
                    RangeOrdering ord=RangeOrdering::Inside);

  inline Filter operator&&(const Filter& rhs);
  inline Filter operator||(const Filter& rhs);
  inline Filter operator~();

  template <class Particle>
  bool eval(const Particle& particle);

  template <class List>
  bool find(const List& list);


private:
  Filter();
  bool _evaluator;
  G4int _id;
  G4double _pT;
  G4double _eta;
  G4double _phi_min;
  G4double _phi_max;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_FILTER_HH */
