file(GLOB OCLCRYPTO_BENCHMARKS_HEADERS "${CMAKE_SOURCE_DIR}/benchmarks/*.h")
file(GLOB OCLCRYPTO_BENCHMARKS_SOURCES "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp")

add_executable(oclcrypto-benchmarks ${OCLCRYPTO_BENCHMARKS_HEADERS} ${OCLCRYPTO_BENCHMARKS_SOURCES})
add_definitions(-DBOOST_ALL_NO_LIB)

target_link_libraries(oclcrypto-benchmarks
    oclcrypto
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_TIMER_LIBRARY}
)

install(TARGETS "oclcrypto-benchmarks"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
