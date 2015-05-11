file(GLOB OCLCRYPTO_TESTS_HEADERS "${CMAKE_SOURCE_DIR}/tests/*.h")
file(GLOB OCLCRYPTO_TESTS_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")

add_executable(oclcrypto-tests ${OCLCRYPTO_HEADERS} ${OCLCRYPTO_TESTS_SOURCES})
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_TEST_DYN_LINK)

target_link_libraries(oclcrypto-tests
    oclcrypto
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME oclcrypto-tests COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/oclcrypto-tests)

install(TARGETS "oclcrypto-tests"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
