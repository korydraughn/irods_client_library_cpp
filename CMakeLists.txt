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

set(IRODS_LIBRARY_NAME irods++)

set(IRODS_LIBRARY_VERSION_MAJOR "0")
set(IRODS_LIBRARY_VERSION_MINOR "0")
set(IRODS_LIBRARY_VERSION_PATCH "1")
set(IRODS_LIBRARY_VERSION ${IRODS_LIBRARY_VERSION_MAJOR}.${IRODS_LIBRARY_VERSION_MINOR}.${IRODS_LIBRARY_VERSION_PATCH})

set(IRODS_FILESYSTEM_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/filesystem/path.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/filesystem.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/collection_iterator.cpp
    ${CMAKE_SOURCE_DIR}/src/filesystem/recursive_collection_iterator.cpp)

set(IRODS_API_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/api/src/rcAuthRequest.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcAuthResponse.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcCloseCollection.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcCollCreate.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjChksum.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjClose.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjCopy.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjLseek.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjOpen.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRead.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRename.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjUnlink.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjWrite.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGenQuery.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGetHierFromLeafId.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGetHostForGet.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGetHostForPut.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGetLimitedPassword.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGetRemoteZoneResc.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModAccessControl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModAVUMetadata.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModColl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModDataObjMeta.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcObjStat.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcOpenCollection.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcPamAuthRequest.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcQuerySpecColl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcReadCollection.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcRmColl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcSpecificQuery.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcSslEnd.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcSslStart.cpp)

set(IRODS_CORE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/core/src/apiHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/base64.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/chksumUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/clientLogin.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/connection_pool.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/getRodsEnv.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_factory.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_buffer_encryption.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_children_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_client_api_table.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_client_negotiation.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_configuration_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_default_paths.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_environment_properties.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_error.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_exception.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_generic_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_get_full_path_for_config_file.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_gsi_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_hierarchy_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_krb_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_kvp_string_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_log.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_native_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_network_factory.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_network_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_network_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_osauth_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_pack_table.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_pam_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_path_recursion.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_plugin_name_generator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_random.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_server_properties.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_socket_information.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_ssl_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_stacktrace.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_string_tokenize.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_tcp_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_virtual_path.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/miscUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/obf.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/packStruct.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/procApiRequest.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rcConnect.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rcGlobal.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rcMisc.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rodsLog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rodsPath.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/sockComm.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/sslSockComm.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/stringOpr.cpp)

set(IRODS_HASHER_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/hasher/src/checksum.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/Hasher.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/irods_hasher_factory.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/MD5Strategy.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/SHA256Strategy.cpp)

add_library(${IRODS_LIBRARY_NAME} SHARED ${IRODS_FILESYSTEM_SOURCE_FILES}
                                         ${IRODS_API_SOURCE_FILES}
                                         ${IRODS_CORE_SOURCE_FILES}
                                         ${IRODS_HASHER_SOURCE_FILES})

set_target_properties(${IRODS_LIBRARY_NAME} PROPERTIES VERSION ${IRODS_LIBRARY_VERSION}
                                                       SOVERSION ${IRODS_LIBRARY_VERSION_MAJOR})

target_compile_definitions(${IRODS_LIBRARY_NAME} PRIVATE RODS_REL_VERSION="rods4.3.0"
                                                         RODS_API_VERSION="d"
                                                         IRODS_VERSION_MAJOR=4
                                                         IRODS_VERSION_MINOR=3
                                                         IRODS_VERSION_PATCHLEVEL=0)

target_compile_options(${IRODS_LIBRARY_NAME} PRIVATE -fPIC -Wall -nostdlib -stdlib=libc++ -pthread)

target_include_directories(${IRODS_LIBRARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/filesystem/include
                                                         ${CMAKE_SOURCE_DIR}/include
                                                         ${CMAKE_SOURCE_DIR}/src/api/include
                                                         ${CMAKE_SOURCE_DIR}/src/core/include
                                                         ${CMAKE_SOURCE_DIR}/src/hasher/include
                                                         ${OPENSSL_INCLUDE_DIR}
                                                         /opt/irods-externals/boost1.67.0-0/include
                                                         /opt/irods-externals/json3.1.2-0/include
                                                         /opt/irods-externals/spdlog0.17.0-0/include)

target_link_libraries(${IRODS_LIBRARY_NAME} PRIVATE /opt/irods-externals/boost1.67.0-0/lib/libboost_filesystem.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_system.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_thread.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_chrono.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_regex.so
                                                    ${OPENSSL_SSL_LIBRARY}
                                                    ${OPENSSL_CRYTO_LIBRARY}
                                                    ${CMAKE_THREAD_LIBS_INIT})

