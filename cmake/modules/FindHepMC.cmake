cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_path(HEPMC_INCLUDE_DIR HepMC/HepMC.h
    HINTS
        $ENV{HEPMCLOCATION}/include
        ${HEPMCLOCATION}/include
        $ENV{HEPMC}/include
        ${HEPMC}/include
)

set(HEPMC_LIBRARY_HINTS
    HINTS
        $ENV{HEPMCLOCATION}/lib
        ${HEPMCLOCATION}/lib
        $ENV{HEPMC}/lib
        ${HEPMC}/lib
)

find_library(HEPMC_LIBRARY NAMES HepMC ${HEPMC_LIBRARY_HINTS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HepMC DEFAULT_MSG HEPMC_LIBRARY)

mark_as_advanced(HEPMC_FOUND
                 HEPMC_LIBRARY)
