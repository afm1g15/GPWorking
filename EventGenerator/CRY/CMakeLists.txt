
include_directories ( $ENV{CRYHOME}/src )

art_make(MODULE_LIBRARIES Geometry
                          Geometry_Geometry_service
                          SummaryDataProducts
                          nutools_EventGeneratorBase_CRY
                          ${CRY}
                          nusimdata_SimulationBase
                          ${ART_FRAMEWORK_CORE}
                          ${ART_FRAMEWORK_PRINCIPAL}
                          ${ART_FRAMEWORK_SERVICES_REGISTRY}
                          ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                          ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                          ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
                          art_Persistency_Common
                          art_Persistency_Provenance
                          art_Utilities
                          canvas
                          ${MF_MESSAGELOGGER}
                          ${MF_UTILITIES}
                          ${FHICLCPP}
                          ${CETLIB}
                          ${CETLIB_EXCEPT}
                          ${CLHEP}
                          ${ROOT_BASIC_LIB_LIST}
        )

install_headers()
install_fhicl()
install_source()

