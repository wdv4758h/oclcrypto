cmake_minimum_required(VERSION 2.6)

project("libopenclcrypto")
set(LIBOPENCLCRYPTO_VERSION_MAJOR "0")
set(LIBOPENCLCRYPTO_VERSION_MINOR "8")
set(LIBOPENCLCRYPTO_VERSION_PATCH "9")
set(LIBOPENCLCRYPTO_VERSION "${LIBOPENCLCRYPTO_VERSION_MAJOR}.${LIBOPENCLCRYPTO_VERSION_MINOR}.${LIBOPENCLCRYPTO_VERSION_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(GNUInstallDirs)

# This is optional, only required if user wants to run cppcheck (unlikely)
find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)

file(GLOB LIBOPENCLCRYPTO_HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB LIBOPENCLCRYPTO_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

set(LIBOPENCLCRYPTO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_BINARY_DIR})

set(LIBOPENCLCRYPTO_LINK_LIBRARIES)

configure_file("include/Config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Config.h")

include_directories(${LIBOPENCLCRYPTO_INCLUDE_DIRS})

add_library("openclcrypto" SHARED
    ${LIBOPENCLCRYPTO_HEADERS}
    ${LIBOPENCLCRYPTO_SOURCES}
)

target_link_libraries("openclcrypto" ${LIBOPENCLCRYPTO_LINK_LIBRARIES})

install(TARGETS "openclcrypto"
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/libopenclcrypto"
    DESTINATION "${CMAKE_INSTALL_INCLUDE_DIR}")

if (CPPCHECK_EXECUTABLE)
    set(CPPCHECK_INCL_PARAMS -I /usr/include)
    foreach(DIR ${LIBOPENCLCRYPTO_INCLUDE_DIRS})
        set(CPPCHECK_INCL_PARAMS ${CPPCHECK_INCL_PARAMS} -I ${DIR})
    endforeach()

    add_custom_command(
        OUTPUT cppcheck.txt
        COMMAND ${CPPCHECK_EXECUTABLE} ${CPPCHECK_INCL_PARAMS} --enable=all --force ${LIBOPENCLCRYPTO_SOURCES} 2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.txt
        DEPENDS ${LIBOPENCLCRYPTO_SOURCES} ${LIBOPENCLCRYPTO_HEADERS}
        VERBATIM
    )
    add_custom_target(
        cppcheck
        DEPENDS cppcheck.txt
    )
endif()

# only CPack should follow
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "libopenclcrypto-${LIBOPENCLCRYPTO_VERSION}" CACHE INTERNAL "tarball basename")
set(CPACK_SOURCE_IGNORE_FILES
"\\\\.git.*"

"build/"
"~$"

"\\\\.settings/"
"\\\\.project"
"\\\\.cproject"
)
include(CPack)
