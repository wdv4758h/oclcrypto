file(GLOB LIBOCLCRYPTO_BENCHMARKS_HEADERS "${CMAKE_SOURCE_DIR}/benchmarks/*.h")
file(GLOB LIBOCLCRYPTO_BENCHMARKS_SOURCES "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp")

add_executable(oclcrypto-benchmarks ${LIBOCLCRYPTO_BENCHMARKS_HEADERS} ${LIBOCLCRYPTO_BENCHMARKS_SOURCES})
add_definitions(-DBOOST_ALL_NO_LIB)

target_link_libraries(oclcrypto-benchmarks
    oclcrypto
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_TIMER_LIBRARY}
)
