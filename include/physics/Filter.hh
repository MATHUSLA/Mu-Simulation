#ifndef PHYSICS_FILTER_HH
#define PHYSICS_FILTER_HH
#pragma once

#include "Geant4/globals.hh"

namespace MATHUSLA { namespace MU {

namespace {

template<class T>
union _minmax {
  T value;
  struct { T min, max; } range;
};

} /* annonymous namespace */

struct Filter {
  G4int id;
  union _minmax<G4double> pT;
  union _minmax<G4double> eta;
  union _minmax<G4double> phi;
  union _minmax<G4double> E;
};

class AdvancedFilter {
public:
  enum class Ordering { Greater, GreaterEqual, Equal, LesserEqual, Lesser };
  enum class RangeOrdering { Inside, Outside };

  static AdvancedFilter id(G4int id);
  static AdvancedFilter id(std::initializer_list<G4int> ids);

  static AdvancedFilter pT(G4double pT,
                           Ordering ord=Ordering::GreaterEqual);

  static AdvancedFilter pT(G4double pT_min, G4double pT_max,
                           RangeOrdering ord=RangeOrdering::Inside);

  static AdvancedFilter eta(G4double eta,
                            Ordering ord=Ordering::LesserEqual);

  static AdvancedFilter eta(G4double eta_min, G4double eta_max,
                            RangeOrdering ord=RangeOrdering::Inside);

  static AdvancedFilter phi(G4double phi);

  static AdvancedFilter phi(G4double phi_min, G4double phi_max,
                            RangeOrdering ord=RangeOrdering::Inside);

  static AdvancedFilter E(G4double energy);

  inline AdvancedFilter operator&&(const AdvancedFilter& rhs) {

  }

  inline AdvancedFilter operator||(const AdvancedFilter& rhs) {

  }

  inline AdvancedFilter operator~() {

  }

  template <class Particle>
  bool eval(const Particle& particle);

  template <class List>
  bool find(const List& list);

private:
  AdvancedFilter();
  bool _evaluator;
  G4int _id;
  G4double _pT;
  G4double _eta;
  G4double _phi_min;
  G4double _phi_max;
};

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_FILTER_HH */
