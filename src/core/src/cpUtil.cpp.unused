/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
#ifndef windows_platform
#include <sys/time.h>
#endif
#include "rodsPath.h"
#include "rodsErrorTable.h"
#include "rodsLog.h"
#include "miscUtil.h"
#include "cpUtil.h"
#include "rcGlobalExtern.h"
#include "irods_virtual_path.hpp"

int
cpFileUtil( rcComm_t *conn, char *srcPath, char *targPath, rodsLong_t srcSize,
            rodsArguments_t *rodsArgs, dataObjCopyInp_t *dataObjCopyInp ) {
    int status;
    struct timeval startTime, endTime;

    if ( srcPath == NULL || targPath == NULL ) {
        rodsLog( LOG_ERROR,
                 "cpFileUtil: NULL srcPath or targPath in cp" );
        return USER__NULL_INPUT_ERR;
    }

    if ( rodsArgs->verbose == True ) {
        ( void ) gettimeofday( &startTime, ( struct timezone * )0 );
    }

    if ( gGuiProgressCB != NULL ) {
        rstrcpy( conn->operProgress.curFileName, srcPath, MAX_NAME_LEN );
        conn->operProgress.curFileSize = srcSize;
        conn->operProgress.curFileSizeDone = 0;
        conn->operProgress.flag = 0;
        gGuiProgressCB( &conn->operProgress );
    }

    rstrcpy( dataObjCopyInp->destDataObjInp.objPath, targPath, MAX_NAME_LEN );
    rstrcpy( dataObjCopyInp->srcDataObjInp.objPath, srcPath, MAX_NAME_LEN );
    dataObjCopyInp->srcDataObjInp.dataSize = -1;

    status = rcDataObjCopy( conn, dataObjCopyInp );

    if ( status >= 0 ) {
        if ( rodsArgs->verbose == True ) {
            ( void ) gettimeofday( &endTime, ( struct timezone * )0 );
            printTiming( conn, dataObjCopyInp->destDataObjInp.objPath,
                         conn->transStat.bytesWritten, NULL, &startTime, &endTime );
        }
        if ( gGuiProgressCB != NULL ) {
            conn->operProgress.totalNumFilesDone++;
            conn->operProgress.totalFileSizeDone += srcSize;
        }
    }

    return status;
}

int
initCondForCp( rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
               dataObjCopyInp_t *dataObjCopyInp, rodsRestart_t *rodsRestart ) {
    char *tmpStr;

    if ( dataObjCopyInp == NULL ) {
        rodsLog( LOG_ERROR,
                 "initCondForCp: NULL dataObjCopyInp in cp" );
        return USER__NULL_INPUT_ERR;
    }

    memset( dataObjCopyInp, 0, sizeof( dataObjCopyInp_t ) );

    if ( rodsArgs == NULL ) {
        return 0;
    }

    if ( rodsArgs->dataType == True ) {
        if ( rodsArgs->dataTypeString != NULL ) {
            addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, DATA_TYPE_KW,
                       rodsArgs->dataTypeString );
        }
    }

    if ( rodsArgs->force == True ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, FORCE_FLAG_KW, "" );
    }

    if ( rodsArgs->checksum == True ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, REG_CHKSUM_KW, "" );
    }

    if ( rodsArgs->verifyChecksum == True ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput,
                   VERIFY_CHKSUM_KW, "" );
    }

    if ( rodsArgs->number == True ) {
        if ( rodsArgs->numberValue == 0 ) {
            dataObjCopyInp->destDataObjInp.numThreads = NO_THREADING;
        }
        else {
            dataObjCopyInp->destDataObjInp.numThreads = rodsArgs->numberValue;
        }
    }

    if ( rodsArgs->physicalPath == True ) {
        if ( rodsArgs->physicalPathString == NULL ) {
            rodsLog( LOG_ERROR,
                     "initCondForCp: NULL physicalPathString error" );
            return USER__NULL_INPUT_ERR;
        }
        else {
            addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, FILE_PATH_KW,
                       rodsArgs->physicalPathString );
        }
    }

    if ( rodsArgs->resource == True ) {
        if ( rodsArgs->resourceString == NULL ) {
            rodsLog( LOG_ERROR,
                     "initCondForCp: NULL resourceString error" );
            return USER__NULL_INPUT_ERR;
        }
        else {
            addKeyVal( &dataObjCopyInp->destDataObjInp.condInput,
                       DEST_RESC_NAME_KW, rodsArgs->resourceString );
        }
    }
    else if ( myRodsEnv != NULL && strlen( myRodsEnv->rodsDefResource ) > 0 ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, DEST_RESC_NAME_KW,
                   myRodsEnv->rodsDefResource );
    }

    if ( rodsArgs->rbudp == True ) {
        /* use -Q for rbudp transfer */
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput,
                   RBUDP_TRANSFER_KW, "" );
        addKeyVal( &dataObjCopyInp->srcDataObjInp.condInput,
                   RBUDP_TRANSFER_KW, "" );
    }

    if ( rodsArgs->veryVerbose == True ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput, VERY_VERBOSE_KW, "" );
        addKeyVal( &dataObjCopyInp->srcDataObjInp.condInput, VERY_VERBOSE_KW, "" );
    }

    if ( ( tmpStr = getenv( RBUDP_SEND_RATE_KW ) ) != NULL ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput,
                   RBUDP_SEND_RATE_KW, tmpStr );
        addKeyVal( &dataObjCopyInp->srcDataObjInp.condInput,
                   RBUDP_SEND_RATE_KW, tmpStr );
    }

    if ( ( tmpStr = getenv( RBUDP_PACK_SIZE_KW ) ) != NULL ) {
        addKeyVal( &dataObjCopyInp->destDataObjInp.condInput,
                   RBUDP_PACK_SIZE_KW, tmpStr );
        addKeyVal( &dataObjCopyInp->srcDataObjInp.condInput,
                   RBUDP_PACK_SIZE_KW, tmpStr );
    }

    memset( rodsRestart, 0, sizeof( rodsRestart_t ) );
    if ( rodsArgs->restart == True ) {
        int status;
        status = openRestartFile( rodsArgs->restartFileString, rodsRestart );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status,
                          "initCondForCp: openRestartFile of %s errno",
                          rodsArgs->restartFileString );
            return status;
        }
    }

    //dataObjCopyInp->destDataObjInp.createMode = 0600;   // seems unused, and caused https://github.com/irods/irods/issues/2085
    dataObjCopyInp->destDataObjInp.openFlags = O_WRONLY;

    return 0;
}

int
cpCollUtil( rcComm_t *conn, char *srcColl, char *targColl,
            rodsEnv *myRodsEnv, rodsArguments_t *rodsArgs,
            dataObjCopyInp_t *dataObjCopyInp, rodsRestart_t *rodsRestart ) {
    int status = 0;
    int savedStatus = 0;
    char parPath[MAX_NAME_LEN], childPath[MAX_NAME_LEN];
    collHandle_t collHandle;
    collEnt_t collEnt;
    char srcChildPath[MAX_NAME_LEN], targChildPath[MAX_NAME_LEN];
    dataObjCopyInp_t childDataObjCopyInp;

    if ( srcColl == NULL || targColl == NULL ) {
        rodsLog( LOG_ERROR,
                 "cpCollUtil: NULL srcColl or targColl in cp" );
        return USER__NULL_INPUT_ERR;
    }

    if ( rodsArgs->recursive != True ) {
        rodsLog( LOG_ERROR,
                 "cpCollUtil: -r option must be used for copying %s directory",
                 srcColl );
        return USER_INPUT_OPTION_ERR;
    }

    // Save the current separator -- it's used in more than on place below
    std::string separator(irods::get_virtual_path_separator());

    // Making sure that the separator has a single char
    if (separator.size() > 1)
    {
        rodsLog( LOG_ERROR, "cpCollUtil: irods::get_virtual_path_separator() returned a string with more than one character.");
        return BAD_FUNCTION_CALL;
    }

    // Issue #3962 - find out if both the beginning of the target and source
    // (the /<zone>/... path) match, and that if the source logical path matches
    // completely, that the end of the matched string in the target is either the
    // end of the path string, or a "/" leading to another collection component.
    if (strstr(targColl, srcColl) == targColl)
    {
        // At this point, we know that the source path completely matches the
        // beginning of the target path.  Now look at the end of the target path:
        size_t srclen = strlen(srcColl);

        if (srclen == strlen(targColl) || targColl[srclen] == separator[0]) {
            // We're here because the source and target paths are identical (proved
            // by the fact that both string lengths are the same), or because the
            // character in the target path just beyond the end of the strstr() comparison,
            // is a separator ("/").
            rodsLog(LOG_ERROR, "cpCollUtil: cannot copy collection %s into itself", srcColl);
            return SAME_SRC_DEST_PATHS_ERR;
        }
    }

    status = rclOpenCollection( conn, srcColl, 0, &collHandle );
    if ( status < 0 ) {
        rodsLog( LOG_ERROR,
                 "cpCollUtil: rclOpenCollection of %s error. status = %d",
                 srcColl, status );
        return status;
    }
    while ( ( status = rclReadCollection( conn, &collHandle, &collEnt ) ) >= 0 ) {
        if ( collEnt.objType == DATA_OBJ_T ) {
            snprintf( srcChildPath, MAX_NAME_LEN, "%s/%s",
                      collEnt.collName, collEnt.dataName );
            snprintf( targChildPath, MAX_NAME_LEN, "%s/%s",
                      targColl, collEnt.dataName );

            int status = chkStateForResume( conn, rodsRestart, targChildPath,
                                        rodsArgs, DATA_OBJ_T, &dataObjCopyInp->destDataObjInp.condInput,
                                        1 );

            if ( status < 0 ) {
                /* restart failed */
                break;
            }
            else if ( status == 0 ) {
                continue;
            }

            status = cpFileUtil( conn, srcChildPath, targChildPath,
                                 collEnt.dataSize, rodsArgs, dataObjCopyInp );

            if ( status < 0 ) {
                rodsLogError( LOG_ERROR, status,
                              "getCollUtil: getDataObjUtil failed for %s. status = %d",
                              srcChildPath, status );
                savedStatus = status;
                if ( rodsRestart->fd > 0 ) {
                    break;
                }
            }
            else {
                status = procAndWriteRestartFile( rodsRestart, targChildPath );
                if ( status < 0 ) {
                    rodsLogError( LOG_ERROR, status,
                                "getCollUtil: procAndWriteRestartFile failed for %s. status = %d",
                                targChildPath, status );
                    savedStatus = status;
                }
            }
        }
        else if ( collEnt.objType == COLL_OBJ_T ) {
            if ( ( status = splitPathByKey(
                                collEnt.collName, parPath, MAX_NAME_LEN, childPath, MAX_NAME_LEN, separator[0] ) ) < 0 ) {
                rodsLogError( LOG_ERROR, status,
                              "cpCollUtil:: splitPathByKey for %s error, status = %d",
                              collEnt.collName, status );
                return status;
            }

            snprintf( targChildPath, MAX_NAME_LEN, "%s/%s",
                      targColl, childPath );
            mkCollR( conn, targColl, targChildPath );

            if ( rodsArgs->verbose == True ) {
                fprintf( stdout, "C- %s:\n", targChildPath );
            }

            /* the child is a spec coll. need to drill down */
            childDataObjCopyInp = *dataObjCopyInp;
            if ( collEnt.specColl.collClass != NO_SPEC_COLL )
                childDataObjCopyInp.srcDataObjInp.specColl =
                    &collEnt.specColl;
            int status = cpCollUtil( conn, collEnt.collName,
                                 targChildPath, myRodsEnv, rodsArgs, &childDataObjCopyInp,
                                 rodsRestart );

            if ( status < 0 && status != CAT_NO_ROWS_FOUND &&
                    status != SYS_SPEC_COLL_OBJ_NOT_EXIST ) {
                savedStatus = status;
            }
        }
    }
    rclCloseCollection( &collHandle );

    if ( savedStatus < 0 ) {
        return savedStatus;
    }
    else if ( status == CAT_NO_ROWS_FOUND ||
              status == SYS_SPEC_COLL_OBJ_NOT_EXIST ) {
        return 0;
    }
    else {
        return status;
    }
}

