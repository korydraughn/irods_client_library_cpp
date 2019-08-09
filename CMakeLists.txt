cmake_minimum_required(VERSION 3.11.4 FATAL_ERROR)
project(irods_client_library_cpp LANGUAGES C CXX)

set(CMAKE_C_COMPILER /opt/irods-externals/clang6.0-0/bin/clang)
set(CMAKE_CXX_COMPILER /opt/irods-externals/clang6.0-0/bin/clang++)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

#find_package(Boost 1.67 REQUIRED)
find_package(Boost REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

set(IRODS_LIBRARY_NAME irods_client_cpp)

set(IRODS_LIBRARY_VERSION_MAJOR "0")
set(IRODS_LIBRARY_VERSION_MINOR "0")
set(IRODS_LIBRARY_VERSION_PATCH "1")
set(IRODS_LIBRARY_VERSION ${IRODS_LIBRARY_VERSION_MAJOR}.${IRODS_LIBRARY_VERSION_MINOR}.${IRODS_LIBRARY_VERSION_PATCH})

set(IRODS_FILESYSTEM_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/filesystem/path.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/filesystem.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/collection_iterator.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/recursive_collection_iterator.cpp)

set(IRODS_CORE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/core/src/connection_pool.cpp)

add_library(${IRODS_LIBRARY_NAME}
            SHARED
            ${IRODS_FILESYSTEM_SOURCE_FILES}
            ${IRODS_CORE_SOURCE_FILES})

set_target_properties(${IRODS_LIBRARY_NAME}
                      PROPERTIES
                      VERSION ${IRODS_LIBRARY_VERSION}
                      SOVERSION ${IRODS_LIBRARY_VERSION_MAJOR})

target_compile_definitions(${IRODS_LIBRARY_NAME}
                           PRIVATE
                           RODS_REL_VERSION="rods4.3.0"
                           RODS_API_VERSION="d"
                           IRODS_VERSION_MAJOR=4
                           IRODS_VERSION_MINOR=3
                           IRODS_VERSION_PATCHLEVEL=0)

target_compile_options(${IRODS_LIBRARY_NAME} PRIVATE -fPIC -Wall -nostdlib -stdlib=libc++ -pthread)

set(IRODS_CLIENT_C_INCLUDES_ROOT /home/kory/dev/prog/cpp/irods_client_library_c)
set(IRODS_CLIENT_C_LIBS_ROOT /home/kory/dev/prog/cpp/build_irods_client_library_c)

target_include_directories(${IRODS_LIBRARY_NAME}
                           PRIVATE
                           ${CMAKE_SOURCE_DIR}/include
                           ${CMAKE_SOURCE_DIR}/include/filesystem/include
                           ${IRODS_CLIENT_C_INCLUDES_ROOT}/include/api
                           ${IRODS_CLIENT_C_INCLUDES_ROOT}/include/core
                           ${IRODS_CLIENT_C_INCLUDES_ROOT}/include/hasher
                           ${CMAKE_SOURCE_DIR}/src/core/include  # For irods_at_scope_exit (should probably be moved into this repo)
                           ${OPENSSL_INCLUDE_DIR}
                           /opt/irods-externals/boost1.67.0-0/include
                           /opt/irods-externals/json3.1.2-0/include
                           /opt/irods-externals/spdlog0.17.0-0/include
                           /opt/irods-externals/clang6.0-0/include/c++/v1)

target_link_libraries(${IRODS_LIBRARY_NAME}
                      PRIVATE
                      ${IRODS_CLIENT_C_LIBS_ROOT}/libirods_client_c.so
                      /opt/irods-externals/boost1.67.0-0/lib/libboost_filesystem.so
                      /opt/irods-externals/boost1.67.0-0/lib/libboost_system.so
                      /opt/irods-externals/boost1.67.0-0/lib/libboost_thread.so
                      /opt/irods-externals/boost1.67.0-0/lib/libboost_chrono.so
                      /opt/irods-externals/boost1.67.0-0/lib/libboost_regex.so
                      ${OPENSSL_SSL_LIBRARY}
                      ${OPENSSL_CRYTO_LIBRARY}
                      ${CMAKE_THREAD_LIBS_INIT})

