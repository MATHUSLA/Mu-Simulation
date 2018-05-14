#include "tracking.hh"

#include <iomanip>

#include "physics/Units.hh"
#include "ui.hh"

namespace MATHUSLA { namespace MU {

namespace Tracking { ///////////////////////////////////////////////////////////////////////////

//__Hit Allocator_______________________________________________________________________________
G4ThreadLocal G4Allocator<Hit>* HitAllocator = nullptr;
//----------------------------------------------------------------------------------------------

//__Hit Constructor_____________________________________________________________________________
Hit::Hit(const std::string& particle,
         const int track,
         const int parent,
         const std::string& chamber,
         const double deposit,
         const G4LorentzVector position,
         const G4LorentzVector momentum)
    : G4VHit(), _particle(particle), _trackID(track), _parentID(parent),
      _chamberID(chamber), _deposit(deposit), _position(position),
      _momentum(momentum) {}
//----------------------------------------------------------------------------------------------

//__Draw Hit in World___________________________________________________________________________
void Hit::Draw() {
  Vis::Draw(Vis::Circle(_position.vect(), 4, G4Color::White()));
}
//----------------------------------------------------------------------------------------------

//__Print Hit Data______________________________________________________________________________
void Hit::Print() {
  static constexpr auto PRECISION = 10;
  std::cout << " "            << _particle
            << " | "          << _trackID
            << " | "          << _parentID
            << " | "          << _chamberID
            << " | Deposit: " << std::setw(PRECISION) << G4BestUnit(_deposit, "Energy")
            << " | ["
              << std::setw(PRECISION) << G4BestUnit(_position.t(), "Time") << " "
              << std::setw(PRECISION) << G4BestUnit(_position.x(), "Length")
              << std::setw(PRECISION) << G4BestUnit(_position.y(), "Length")
              << std::setw(PRECISION) << G4BestUnit(_position.z(), "Length")
            << "] | ["
              << std::setw(PRECISION) << G4BestUnit(_momentum.e(),  "Energy")
              << std::setw(PRECISION) << G4BestUnit(_momentum.px(), "Momentum")
              << std::setw(PRECISION) << G4BestUnit(_momentum.py(), "Momentum")
              << std::setw(PRECISION) << G4BestUnit(_momentum.pz(), "Momentum")
            << " ]"
            << "\n";
}
//----------------------------------------------------------------------------------------------

} /* namespace Tracking */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
