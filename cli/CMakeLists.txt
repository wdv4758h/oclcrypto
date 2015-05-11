file(GLOB OCLCRYPTO_CLI_HEADERS "${CMAKE_SOURCE_DIR}/cli/*.h")
file(GLOB OCLCRYPTO_CLI_SOURCES "${CMAKE_SOURCE_DIR}/cli/*.cpp")

add_executable(oclcrypto-cli ${OCLCRYPTO_CLI_HEADERS} ${OCLCRYPTO_CLI_SOURCES})
add_definitions(-DBOOST_ALL_NO_LIB)

target_link_libraries(oclcrypto-cli
    oclcrypto
)

install(TARGETS "oclcrypto-cli"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
