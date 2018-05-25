/* include/analysis.hh
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

#ifndef MU__ANALYSIS_HH
#define MU__ANALYSIS_HH
#pragma once

#include <vector>

#include "Geant4/g4root.hh"

namespace MATHUSLA { namespace MU {

namespace Analysis { ///////////////////////////////////////////////////////////////////////////

//__Setup Analysis Tool_________________________________________________________________________
void Setup();
//----------------------------------------------------------------------------------------------

//__Open Output File____________________________________________________________________________
bool Open(const std::string& file);
//----------------------------------------------------------------------------------------------

//__Save Output_________________________________________________________________________________
bool Save();
//----------------------------------------------------------------------------------------------

//__NTuple Initializer__________________________________________________________________________
bool CreateNTuple(const std::string& name,
                  const std::vector<std::string>& columns);
//----------------------------------------------------------------------------------------------

//__Add Data to NTuple__________________________________________________________________________
bool FillNTuple(const std::string& name,
                const std::vector<double>& values);
//----------------------------------------------------------------------------------------------

//__Add Data to NTuple__________________________________________________________________________
bool FillNTuple(const std::string& prefix,
                const size_t id,
                const std::vector<double>& values);
//----------------------------------------------------------------------------------------------

//__NTuple Collection Initializer_______________________________________________________________
bool GenerateNTupleCollection(const size_t count,
                              const std::string& prefix,
                              const std::vector<std::string>& columns);
//----------------------------------------------------------------------------------------------

} /* namespace Analysis */ /////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__ANALYSIS_HH */
