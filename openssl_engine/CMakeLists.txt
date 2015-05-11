file(GLOB OCLCRYPTO_OPENSSL_ENGINE_HEADERS "${CMAKE_SOURCE_DIR}/openssl_engine/*.h")
file(GLOB OCLCRYPTO_OPENSSL_ENGINE_SOURCES "${CMAKE_SOURCE_DIR}/openssl_engine/*.cpp")

add_library(oclcrypto_openssl_engine SHARED
    ${OCLCRYPTO_OPENSSL_ENGINE_HEADERS}
    ${OCLCRYPTO_OPENSSL_ENGINE_SOURCES}
)

target_link_libraries(oclcrypto_openssl_engine
    oclcrypto
)
