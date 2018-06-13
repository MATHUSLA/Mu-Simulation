/*
 * src/physics/HepMCGenerator.cc
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

#include "physics/HepMCGenerator.hh"

#include <HepMC/HepMC.h>
#include <HepMC/ReaderAscii.h>

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__HepMC Generator Constructor_________________________________________________________________
HepMCGenerator::HepMCGenerator(const int id,
                               const double pT_min,
                               const double pT_max,
                               const double eta_min,
                               const double eta_max,
                               const double phi_min,
                               const double phi_max)
    : RangeGenerator("hepmc", "HepMC Generator.", id,
                     pT_min, pT_max, eta_min, eta_max, phi_min, phi_max) {

}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
