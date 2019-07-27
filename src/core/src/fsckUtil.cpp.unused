/*** Copyright (c) 2010 Data Intensive Cyberinfrastructure Foundation. All rights reserved.    ***
 *** For full copyright notice please refer to files in the COPYRIGHT directory                ***/
/* Written by Jean-Yves Nief of CCIN2P3 and copyright assigned to Data Intensive Cyberinfrastructure Foundation */

#include "rodsPath.h"
#include "rodsErrorTable.h"
#include "rodsLog.h"
#include "fsckUtil.h"
#include "miscUtil.h"
#include "scanUtil.h"
#include "checksum.hpp"
#include "rcGlobalExtern.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <iostream>
#include <string>

using namespace boost::filesystem;

int
fsckObjDir( rcComm_t *conn, rodsArguments_t *myRodsArgs, char *inpPath, SetGenQueryInpFromPhysicalPath strategy, const char* argument_for_SetGenQueryInpFromPhysicalPath) {
    int status = 0;
    char fullPath[MAX_PATH_ALLOWED] = "\0";

    path srcDirPath( inpPath );
    if ( is_symlink( srcDirPath ) ) {
        return 0;
    } else if ( is_directory( srcDirPath ) ) {
    } else {
        status = chkObjConsistency( conn, myRodsArgs, inpPath, strategy, argument_for_SetGenQueryInpFromPhysicalPath );
        return status;
    }
    directory_iterator end_itr; // default construction yields past-the-end
    for ( directory_iterator itr( srcDirPath ); itr != end_itr; ++itr ) {
        path cp = itr->path();
        snprintf( fullPath, MAX_PATH_ALLOWED, "%s",
                  cp.c_str() );
        if ( is_symlink( cp ) ) {
            /* don't do anything if it is symlink */
            continue;
        }
        else if ( is_directory( cp ) ) {
            if ( myRodsArgs->recursive == True ) {
                const int tmp_status = fsckObjDir( conn, myRodsArgs, fullPath, strategy, argument_for_SetGenQueryInpFromPhysicalPath );
                if (status == 0) {
                    status = tmp_status;
                }
            }
        }
        else {
            const int tmp_status = chkObjConsistency( conn, myRodsArgs, fullPath, strategy, argument_for_SetGenQueryInpFromPhysicalPath );
            if (status == 0) {
                status = tmp_status;
            }
        }
    }
    return status;

}

int
chkObjConsistency( rcComm_t *conn, rodsArguments_t *myRodsArgs, char *inpPath, SetGenQueryInpFromPhysicalPath strategy, const char* argument_for_SetGenQueryInpFromPhysicalPath ) {
    const path p(inpPath);
    if ( is_symlink( p ) ) {
        return 0;
    }
    const intmax_t srcSize = file_size(p);
    genQueryInp_t genQueryInp;
    strategy(&genQueryInp, inpPath, argument_for_SetGenQueryInpFromPhysicalPath);

    genQueryOut_t *genQueryOut = NULL;
    int status = rcGenQuery( conn, &genQueryInp, &genQueryOut );

    if ( status == 0 && NULL != genQueryOut ) {
        const char *objName = genQueryOut->sqlResult[0].value;
        const char *objPath = genQueryOut->sqlResult[1].value;

        intmax_t objSize = 0;

        try {
            objSize = std::stoll(genQueryOut->sqlResult[2].value);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "ERROR: could not parse object size into integer [exception => " << e.what() << ", path => " << inpPath << "].\n";
            return SYS_INTERNAL_ERR;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "ERROR: could not parse object size into integer [exception => " << e.what() << ", path => " << inpPath << "].\n";
            return SYS_INTERNAL_ERR;
        }

        const char *objChksum = genQueryOut->sqlResult[3].value;
        if ( srcSize == objSize ) {
            if ( myRodsArgs->verifyChecksum == True ) {
                if ( strcmp( objChksum, "" ) != 0 ) {
                    status = verifyChksumLocFile( inpPath, objChksum, NULL );
                    if ( status == USER_CHKSUM_MISMATCH ) {
                        printf( "CORRUPTION: local file [%s] checksum not consistent with iRODS object [%s/%s] checksum.\n", inpPath, objPath, objName );
                    } else if (status != 0) {
                        printf("ERROR chkObjConsistency: verifyChksumLocFile failed: status [%d] file [%s] objPath [%s] objName [%s] objChksum [%s]\n", status, inpPath, objPath, objName, objChksum);
                    }
                } else {
                    printf( "WARNING: checksum not available for iRODS object [%s/%s], no checksum comparison possible with local file [%s] .\n", objPath, objName, inpPath );
                }
            }
        } else {
            printf( "CORRUPTION: local file [%s] size [%ji] not consistent with iRODS object [%s/%s] size [%ji].\n", inpPath, srcSize, objPath, objName, objSize );
            status = SYS_INTERNAL_ERR;
        }
    } else {
        printf("ERROR chkObjConsistency: rcGenQuery failed: status [%d] genQueryOut [%p] file [%s]\n", status, genQueryOut, inpPath);
    }

    clearGenQueryInp( &genQueryInp );
    freeGenQueryOut( &genQueryOut );

    return status;

}
