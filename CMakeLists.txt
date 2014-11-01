cmake_minimum_required(VERSION 2.6)

project("oclcrypto")
set(LIBOCLCRYPTO_VERSION_MAJOR "0")
set(LIBOCLCRYPTO_VERSION_MINOR "0")
set(LIBOCLCRYPTO_VERSION_PATCH "1")
set(LIBOCLCRYPTO_VERSION "${LIBOCLCRYPTO_VERSION_MAJOR}.${LIBOCLCRYPTO_VERSION_MINOR}.${LIBOCLCRYPTO_VERSION_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(GNUInstallDirs)

find_package(OpenCL REQUIRED)
find_package(Boost COMPONENTS unit_test_framework)

# This is optional, only required if user wants to run cppcheck (unlikely)
find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)

file(GLOB LIBOCLCRYPTO_OPENCL_SOURCES "${CMAKE_SOURCE_DIR}/opencl_src/*.c")

file(GLOB LIBOCLCRYPTO_HEADERS "${CMAKE_SOURCE_DIR}/include/oclcrypto/*.h")
file(GLOB LIBOCLCRYPTO_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

set(LIBOCLCRYPTO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}

    ${Boost_INCLUDE_DIR}
    ${OPENCL_INCLUDE_DIRS}
)

set(LIBOCLCRYPTO_LINK_LIBRARIES
    ${OPENCL_LIBRARIES}
)

configure_file("include/oclcrypto/Config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/oclcrypto/Config.h")

set(INBUILT_PROGRAM_SOURCES "")
foreach(CL_FILE ${LIBOCLCRYPTO_OPENCL_SOURCES})
    get_filename_component(CL_BASENAME ${CL_FILE} NAME_WE)
    set(INBUILT_PROGRAM_SOURCES "${INBUILT_PROGRAM_SOURCES}\nconst char ${CL_BASENAME}[] = ")
    file(STRINGS ${CL_FILE} CL_SOURCE)
    foreach(LINE ${CL_SOURCE})
        string(REPLACE "\\" "\\\\" LINE "${LINE}")
        string(REPLACE "\"" "\\\"" LINE "${LINE}")
        set(INBUILT_PROGRAM_SOURCES "${INBUILT_PROGRAM_SOURCES}\"${LINE}\\n\"\n")
    endforeach()
    set(INBUILT_PROGRAM_SOURCES "${INBUILT_PROGRAM_SOURCES};\n")
endforeach()

configure_file("src/InbuiltProgramSources.inc.in" "${CMAKE_CURRENT_BINARY_DIR}/InbuiltProgramSources.inc")

include_directories(${LIBOCLCRYPTO_INCLUDE_DIRS})

add_library("oclcrypto" SHARED
    ${LIBOCLCRYPTO_HEADERS}
    ${LIBOCLCRYPTO_SOURCES}
)

target_link_libraries("oclcrypto" ${LIBOCLCRYPTO_LINK_LIBRARIES})

#install(TARGETS "oclcrypto"
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/liboclcrypto"
#    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

enable_testing()
add_subdirectory(tests)

if (CPPCHECK_EXECUTABLE)
    set(CPPCHECK_INCL_PARAMS -I /usr/include)
    foreach(DIR ${LIBOCLCRYPTO_INCLUDE_DIRS})
        set(CPPCHECK_INCL_PARAMS ${CPPCHECK_INCL_PARAMS} -I ${DIR})
    endforeach()

    add_custom_command(
        OUTPUT cppcheck.txt
        COMMAND ${CPPCHECK_EXECUTABLE} ${CPPCHECK_INCL_PARAMS} --enable=all --force ${LIBOCLCRYPTO_SOURCES} 2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.txt
        DEPENDS ${LIBOCLCRYPTO_SOURCES} ${LIBOCLCRYPTO_HEADERS}
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
set(CPACK_SOURCE_PACKAGE_FILE_NAME "liboclcrypto-${LIBOCLCRYPTO_VERSION}" CACHE INTERNAL "tarball basename")
set(CPACK_SOURCE_IGNORE_FILES
"\\\\.git.*"

"build/"
"~$"

"\\\\.settings/"
"\\\\.project"
"\\\\.cproject"
)
include(CPack)
