# ======================================================================
#  garsoft main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/garsoft/ups/setup_for_development
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#        
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(garsoft)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{CANVAS_DIR}/Modules
                      $ENV{ART_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

set(IS_ART_V1 OFF)
if( "${QUAL_SUBDIR}" MATCHES ".*e9.*" )
  set(IS_ART_V1 ON)
  set(ARTDFLAG -DART_V1)
endif()
MESSAGE("## NDTF QUAL_SUBDIR=${QUAL_SUBDIR} IS_ART_V1 ${IS_ART_V1}  ${ARTDFLAG}##")

# -D_GLIBCXX_USE_NANOSLEEP is needed to make std::this_thread::sleep_for available.
cet_set_compiler_flags(DIAGS CAUTIOUS WERROR 
                       NO_UNDEFINED
                       EXTRA_FLAGS -pedantic 
                       EXTRA_CXX_FLAGS -D_GLIBCXX_USE_NANOSLEEP ${ARTDFLAG}
                                       -Wno-unused-local-typedefs -Wno-variadic-macros)
cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( genie           v2_8_0    )
find_ups_product( libwda          v2_21_0   )
find_ups_product( postgresql      v9_1_0    )
if (NOT IS_ART_V1) 
  find_ups_product( cetbuildtools   v4_09_00  )
  find_ups_product( nutools         v2_01_00  )
else()
  find_ups_product( cetbuildtools   v4_19_04  )
  find_ups_product( nutools         v1_24_05  )
endif()

message(STATUS "ROOTSYS is ${ROOTSYS}")

## find_library directives
# geant4 libraries
cet_find_library( XERCESC NAMES xerces-c PATHS ENV XERCES_C_LIB NO_DEFAULT_PATH  )
cet_find_library( G4FR NAMES G4FR PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GMOCREN NAMES G4GMocren PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4OPENGL NAMES G4OpenGL PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4RAYTRACER  NAMES G4RayTracer PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TREE  NAMES G4Tree PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VRML  NAMES G4VRML PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ANALYSIS  NAMES G4analysis PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4DIGITS_HITS  NAMES G4digits_hits PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ERROR_PROPAGATION  NAMES G4error_propagation PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4EVENT  NAMES G4event PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GEOMETRY  NAMES G4geometry PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GL2PS  NAMES G4gl2ps PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GLOBAL  NAMES G4global PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4GRAPHICS_REPS  NAMES G4graphics_reps PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4INTERCOMS  NAMES G4intercoms PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4INTERFACES  NAMES G4interfaces PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4MATERIALS NAMES G4materials PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4MODELING  NAMES G4modeling PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PARMODELS  NAMES G4parmodels PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PARTICLES  NAMES G4particles PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PERSISTENCY  NAMES G4persistency PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PHYSICSLISTS  NAMES G4physicslists PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4PROCESSES  NAMES G4processes PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4READOUT  NAMES G4readout PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4RUN  NAMES G4run PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TRACK  NAMES G4track PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4TRACKING  NAMES G4tracking PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VISHEPREP NAMES G4visHepRep PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VISXXX NAMES G4visXXX PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4VIS_MANAGEMENT NAMES G4vis_management PATHS ENV G4LIB NO_DEFAULT_PATH )
cet_find_library( G4ZLIB  NAMES G4zlib PATHS ENV G4LIB NO_DEFAULT_PATH )
# other dependencies
cet_find_library( PYTHIA6 NAMES Pythia6 PATHS ENV PYLIB NO_DEFAULT_PATH )
cet_find_library( LHAPDF NAMES LHAPDF PATHS ENV LHAPDF_LIB NO_DEFAULT_PATH )
cet_find_library( LOG4CPP NAMES log4cpp PATHS ENV LOG4CPP_LIB NO_DEFAULT_PATH )
cet_find_library( XML2 NAMES xml2 PATHS $ENV{LIBXML2_FQ_DIR}/lib NO_DEFAULT_PATH )
cet_find_library( CRY NAMES CRY PATHS ENV CRY_LIB NO_DEFAULT_PATH )
# genie
cet_find_library( GALGORITHM NAMES GAlgorithm PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GBARYONRESONANCE NAMES GBaryonResonance PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GBASE NAMES GBase PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GBODEKYANG NAMES GBodekYang PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GCHARM NAMES GCharm PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GCOH NAMES GCoh PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GDFRC NAMES GDfrc PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GDIS NAMES GDIS PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GCROSSSECTIONS NAMES GCrossSections PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GDECAY NAMES GDecay PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GELAS NAMES GElas PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GELFF NAMES GElFF PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GHEP NAMES GHEP PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GEVGCORE NAMES GEVGCore  PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GEVGMODULES NAMES GEVGModules PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GEVGDRIVERS NAMES GEVGDrivers PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GGIBUU NAMES GGiBUU PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GHADRONTRANSP NAMES GHadronTransp PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GFRAGMENTATION NAMES GFragmentation PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GINTERACTION NAMES GInteraction PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GLLEWELLYNSMITH NAMES GLlewellynSmith  PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GMEC NAMES GMEC PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GMESSENGER NAMES GMessenger PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNUGAMMA NAMES GNuGamma PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNUE NAMES GNuE PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNTUPLE NAMES GNtuple PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNUCLEAR NAMES GNuclear PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNUMERICAL NAMES GNumerical PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GQPM NAMES GQPM PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GPDG NAMES GPDG PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GPDF NAMES GPDF PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GQEL NAMES GQEL PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GRES NAMES GRES PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GREGISTRY NAMES GRegistry PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GREINSEHGAL NAMES GReinSehgal PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GUTILS NAMES GUtils PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GGEO NAMES GGeo PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GFLUXDRIVERS NAMES GFluxDrivers PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GMUELOSS NAMES GMuELoss PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GREWEIGHT NAMES GReWeight PATHS ENV GENIE_LIB NO_DEFAULT_PATH )
cet_find_library( GNUCLEONDECAY NAMES GNucleonDecay PATHS ENV GENIE_LIB NO_DEFAULT_PATH )

if(NOT IS_ART_V1)
  cet_find_library( SIMBASE NAMES nusimdata_SimulationBase PATHS ENV NUSIMDATA_LIB NO_DEFAULT_PATH  )
else()
  cet_find_library( SIMBASE NAMES SimulationBase PATHS ENV NUTOOLS_LIB NO_DEFAULT_PATH  )
endif()
 

set ( GENIE_LIB_LIST
        ${LOG4CPP}
        ${XML2}
        ${PYTHIA6}
        ${LHAPDF}
        ${GALGORITHM}
        ${GBARYONRESONANCE}
        ${GBASE}
        ${GBODEKYANG}
        ${GCHARM}
        ${GCOH}
        ${GDFRC}
        ${GDIS}
        ${GCROSSSECTIONS}
        ${GDECAY}
        ${GELAS}
        ${GELFF}
        ${GHEP}
        ${GEVGCORE}
        ${GEVGMODULES}
        ${GEVGDRIVERS}
        ${GGIBUU}
        ${GHADRONTRANSP}
        ${GFRAGMENTATION}
        ${GINTERACTION}
        ${GLLEWELLYNSMITH}
        ${GMEC}
        ${GMESSENGER}
        ${GNUGAMMA}
        ${GNUE}
        ${GNTUPLE}
        ${GNUCLEAR}
        ${GNUMERICAL}
        ${GQPM}
        ${GPDG}
        ${GPDF}
        ${GQEL}
        ${GRES}
        ${GREGISTRY}
        ${GREINSEHGAL}
        ${GUTILS}
        ${GGEO}
        ${GFLUXDRIVERS}
        ${GMUELOSS}
        ${GREWEIGHT}
        ${GNUCLEONDECAY}
	${ROOT_CORE}
	${ROOT_CINT}
        ${ROOT_RIO}
        ${ROOT_NET}
        ${ROOT_HIST}
        ${ROOT_GRAF}
        ${ROOT_GRAF3D}
        ${ROOT_GPAD}
        ${ROOT_TREE}
        ${ROOT_RINT}
        ${ROOT_POSTSCRIPT}
        ${ROOT_MATRIX}
        ${ROOT_PHYSICS}
        ${ROOT_MATHCORE}
        ${ROOT_THREAD}
        ${ROOT_GEOM}
        ${ROOT_EGPYTHIA6}
	)
      

# macros 
include(ArtMake)
include(ArtDictionary)
include(BuildPlugins)
include(CetRootCint)

# include search path
include_directories ( ${CMAKE_SOURCE_DIR} )
include_directories ( $ENV{GENIE_INC}/GENIE )
include_directories ( $ENV{LIBXML2_INC} )
include_directories ( $ENV{G4INCLUDE} )
include_directories ( $ENV{XERCES_C_INC} )
include_directories ( $ENV{CRYHOME}/src )
include_directories ( $ENV{LOG4CPP_INC} )
include_directories ( $ENV{DK2NU_INC} )

# included for Mac OSX using XQuartz for X11, ignored on other systems
include_directories ( /opt/X11/include )

# source
#add_subdirectory (EventDisplay)
add_subdirectory (EventGenerator)
#add_subdirectory (GArG4)
add_subdirectory (Geometry)
add_subdirectory (MCCheater)
#add_subdirectory (ReconstructionDataProducts)
#add_subdirectory (SimulationDataProducts)
add_subdirectory (Utilities)

# ups - table and config files
add_subdirectory(ups)
# packaging utility
include(UseCPack)