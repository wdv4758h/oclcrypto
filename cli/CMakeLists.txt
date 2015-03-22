file(GLOB LIBOCLCRYPTO_CLI_HEADERS "${CMAKE_SOURCE_DIR}/cli/*.h")
file(GLOB LIBOCLCRYPTO_CLI_SOURCES "${CMAKE_SOURCE_DIR}/cli/*.cpp")

add_executable(oclcrypto-cli ${LIBOCLCRYPTO_CLI_HEADERS} ${LIBOCLCRYPTO_CLI_SOURCES})
add_definitions(-DBOOST_ALL_NO_LIB)

target_link_libraries(oclcrypto-cli
    oclcrypto
)
