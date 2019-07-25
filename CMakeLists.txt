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
    #${CMAKE_SOURCE_DIR}/src/api/src/rcAuthCheck.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcAuthPluginRequest.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcAuthRequest.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcAuthResponse.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcBulkDataObjPut.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcBulkDataObjReg.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcChkNVPathPerm.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcChkObjPermAndStat.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcClientHints.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcCloseCollection.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcCollCreate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcCollRepl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataCopy.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataGet.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjChksum.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjClose.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjCopy.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjCreateAndStat.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjCreate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjGet.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjLock.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjLseek.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjOpenAndStat.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjOpen.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjPhymv.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjPut.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRead.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRename.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRepl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjRsync.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjTrim.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjTruncate.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjUnlink.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcDataObjWrite.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcDataPut.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcEndTransaction.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcExecCmd.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcExecMyRule.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcExecRuleExpression.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileChksum.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileChmod.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileClose.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileClosedir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileCreate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileGet.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileGetFsFreeSpace.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileLseek.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileMkdir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileOpen.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileOpendir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFilePut.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileRead.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileReaddir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileRename.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileRmdir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileStageToCache.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileStat.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileSyncToArch.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileTruncate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileUnlink.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcFileWrite.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGeneralAdmin.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGeneralRowInsert.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGeneralRowPurge.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGeneralUpdate.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcGenQuery.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetHierarchyForResc.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetHierFromLeafId.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetHostForGet.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetHostForPut.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetLimitedPassword.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetMiscSvrInfo.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetRemoteZoneResc.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetRescQuota.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetTempPassword.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcGetTempPasswordForOther.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcIESClientHints.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcL3FileGetSingleBuf.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcL3FilePutSingleBuf.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModAccessControl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModAVUMetadata.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModColl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcModDataObjMeta.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcObjStat.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcOpenCollection.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcOprComplete.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcPamAuthRequest.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcPhyBundleColl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcPhyPathReg.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcProcStat.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcQuerySpecColl.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcReadCollection.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRegColl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRegDataObj.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRegReplica.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcRmColl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRuleExecDel.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRuleExecMod.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcRuleExecSubmit.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcServerReport.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSetRoundRobinContext.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSimpleQuery.cpp
    ${CMAKE_SOURCE_DIR}/src/api/src/rcSpecificQuery.cpp)
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSslEnd.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSslStart.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStreamClose.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStreamRead.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStructFileBundle.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStructFileExtAndReg.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStructFileExtract.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcStructFileSync.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileClose.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileClosedir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileCreate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileGet.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileLseek.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileMkdir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileOpen.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileOpendir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFilePut.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileRead.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileReaddir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileRename.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileRmdir.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileStat.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileTruncate.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileUnlink.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSubStructFileWrite.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcSyncMountedColl.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcTicketAdmin.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcUnbunAndRegPhyBunfile.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcUnregDataObj.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcUserAdmin.cpp
    #${CMAKE_SOURCE_DIR}/src/api/src/rcZoneReport.cpp)

set(IRODS_CORE_SOURCE_FILES
    #${CMAKE_SOURCE_DIR}/src/core/src/apiHandler.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/base64.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/bunUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/chksumUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/clientLogin.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/connection_pool.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/cpUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/fsckUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/getRodsEnv.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/getUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/hashtable.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_factory.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_manager.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_auth_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_buffer_encryption.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_c_api.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_children_parser.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_client_api_table.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_client_negotiation.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_configuration_parser.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_default_paths.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_environment_properties.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_error.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_exception.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_generic_auth_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_get_full_path_for_config_file.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_gsi_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_hierarchy_parser.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_hostname.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_krb_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_kvp_string_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_log.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_native_auth_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_network_factory.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_network_manager.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_network_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_osauth_auth_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_pack_table.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_pam_auth_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_parse_command_line_options.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_path_recursion.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_pluggable_auth_scheme.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_plugin_name_generator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_random.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_serialization.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_server_properties.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_socket_information.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_ssl_object.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/irods_stacktrace.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_string_tokenize.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_tcp_object.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_threads.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/irods_virtual_path.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/list.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/lsUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/mcollUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/miscUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/mkdirUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/msParam.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/mvUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/obf.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/osauth.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/packStruct.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/parseCommandLine.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/phybunUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/phymvUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/procApiRequest.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/putUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rcConnect.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rcGlobal.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rcMisc.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rcPortalOpr.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/region.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/regUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/replUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rmdirUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rmtrashUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rmUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/rodsLog.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rodsPath.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/rsyncUtil.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/scanUtil.cpp
    ${CMAKE_SOURCE_DIR}/src/core/src/sockComm.cpp)
    #${CMAKE_SOURCE_DIR}/src/core/src/sslSockComm.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/stringOpr.cpp
    #${CMAKE_SOURCE_DIR}/src/core/src/trimUtil.cpp)

set(IRODS_HASHER_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/hasher/src/checksum.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/Hasher.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/irods_hasher_factory.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/MD5Strategy.cpp
    ${CMAKE_SOURCE_DIR}/src/hasher/src/SHA256Strategy.cpp)

set(IRODS_RBUDP_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/rbudp/src/QUANTAnet_rbudpBase_c.cpp
    ${CMAKE_SOURCE_DIR}/src/rbudp/src/QUANTAnet_rbudpReceiver_c.cpp
    ${CMAKE_SOURCE_DIR}/src/rbudp/src/QUANTAnet_rbudpSender_c.cpp
    ${CMAKE_SOURCE_DIR}/src/rbudp/src/recvfile.cpp
    ${CMAKE_SOURCE_DIR}/src/rbudp/src/sendfile.cpp)

set(IRODS_SERVER_CORE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/server/core/src/rodsConnect.cpp)
    #${CMAKE_SOURCE_DIR}/src/server/core/src/fileOpr.cpp
    #${CMAKE_SOURCE_DIR}/src/server/drivers/src/fileDriver.cpp
    #${CMAKE_SOURCE_DIR}/src/server/re/src/irods_re_structs.cpp)

add_library(${IRODS_LIBRARY_NAME} SHARED ${IRODS_FILESYSTEM_SOURCE_FILES}
                                         ${IRODS_API_SOURCE_FILES}
                                         ${IRODS_CORE_SOURCE_FILES}
                                         ${IRODS_SERVER_CORE_SOURCE_FILES})
                                         #${IRODS_HASHER_SOURCE_FILES}
                                         #${IRODS_RBUDP_SOURCE_FILES})

#set_target_properties(${IRODS_LIBRARY_NAME} PROPERTIES CXX_STANDARD 17
#                                                       VERSION ${IRODS_LIBRARY_VERSION}
#                                                       SOVERSION ${IRODS_LIBRARY_VERSION_MAJOR})
set_target_properties(${IRODS_LIBRARY_NAME} PROPERTIES VERSION ${IRODS_LIBRARY_VERSION}
                                                       SOVERSION ${IRODS_LIBRARY_VERSION_MAJOR})

target_compile_definitions(${IRODS_LIBRARY_NAME} PRIVATE RODS_REL_VERSION="rods4.3.0" RODS_API_VERSION="d")
target_compile_options(${IRODS_LIBRARY_NAME} PRIVATE -fPIC -Wall -nostdlib -stdlib=libc++ -pthread) # -std=c++17 )

target_include_directories(${IRODS_LIBRARY_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/filesystem/include
                                                         ${CMAKE_SOURCE_DIR}/include
                                                         ${CMAKE_SOURCE_DIR}/src/api/include
                                                         ${CMAKE_SOURCE_DIR}/src/core/include
                                                         ${CMAKE_SOURCE_DIR}/src/server/core/include
                                                         ${CMAKE_SOURCE_DIR}/src/server/drivers/include
                                                         ${CMAKE_SOURCE_DIR}/src/server/icat/include
                                                         ${CMAKE_SOURCE_DIR}/src/server/re/include
                                                         /opt/irods-externals/boost1.67.0-0/include
                                                         /opt/irods-externals/json3.1.2-0/include
                                                         /opt/irods-externals/spdlog0.17.0-0/include
                                                         ${OPENSSL_INCLUDE_DIR})
                                                         #${CMAKE_SOURCE_DIR}/src/hasher/include
                                                         #${CMAKE_SOURCE_DIR}/src/rdudp/include
                                                         #${BOOST_ROOT}/include)

target_link_libraries(${IRODS_LIBRARY_NAME} PRIVATE /opt/irods-externals/boost1.67.0-0/lib/libboost_filesystem.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_system.so
                                                    /opt/irods-externals/boost1.67.0-0/lib/libboost_thread.so
                                                    ${OPENSSL_SSL_LIBRARY}
                                                    ${OPENSSL_CRYTO_LIBRARY}
                                                    ${CMAKE_THREAD_LIBS_INIT})

