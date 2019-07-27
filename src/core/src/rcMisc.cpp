/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* rcMisc.c - misc client routines
 */
#ifndef windows_platform
#include <sys/time.h>
#include <sys/wait.h>
#else
#include "Unix2Nt.hpp"
#endif
#include "rcMisc.h"
#include "apiHeaderAll.h"
#include "modDataObjMeta.h"
#include "rcGlobalExtern.h"
#include "rodsGenQueryNames.h"
#include "rodsType.h"
#include "dataObjPut.h"

#include "bulkDataObjPut.h"
#include "putUtil.h"
#include "sockComm.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <map>
#include <random>
#include <openssl/md5.h>

// =-=-=-=-=-=-=-
#include "irods_virtual_path.hpp"
#include "irods_hierarchy_parser.hpp"
#include "irods_stacktrace.hpp"
#include "irods_exception.hpp"
#include "irods_log.hpp"
#include "irods_random.hpp"
#include "irods_path_recursion.hpp"

// =-=-=-=-=-=-=-
// boost includes
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/format.hpp>
#include <boost/generator_iterator.hpp>

rodsLong_t
getFileSize( char *myPath ) {
    namespace fs = boost::filesystem;

    fs::path p( myPath );

    if ( exists( p ) && is_regular_file( p ) ) {
        return file_size( p );
    }
    else {
        return -1;
    }
}


int freeBBuf( bytesBuf_t *myBBuf ) {
    if ( myBBuf == NULL ) {
        return 0;
    }

    if ( myBBuf->buf != NULL ) {
        free( myBBuf->buf );
    }
    free( myBBuf );
    return 0;
}

int
clearBBuf( bytesBuf_t *myBBuf ) {
    if ( myBBuf == NULL ) {
        return 0;
    }

    if ( myBBuf->buf != NULL ) {
        free( myBBuf->buf );
    }

    memset( myBBuf, 0, sizeof( bytesBuf_t ) );
    return 0;
}

/* addRErrorMsg - Add an error msg to the rError_t struct.
 *    rError_t *myError - the rError_t struct  for the error msg.
 *    int status - the input error status.
 *    char *msg - the error msg string. This string will be copied to myError.
 */

int
addRErrorMsg( rError_t *myError, int status, const char *msg ) {
    rErrMsg_t **newErrMsg;
    int newLen;
    int i;

    if ( myError == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( ( myError->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        newLen = myError->len + PTR_ARRAY_MALLOC_LEN;
        newErrMsg = ( rErrMsg_t ** ) malloc( newLen * sizeof( *newErrMsg ) );
        memset( newErrMsg, 0, newLen * sizeof( *newErrMsg ) );
        for ( i = 0; i < myError->len; i++ ) {
            newErrMsg[i] = myError->errMsg[i];
        }
        if ( myError->errMsg != NULL ) {
            free( myError->errMsg );
        }
        myError->errMsg = newErrMsg;
    }

    myError->errMsg[myError->len] = ( rErrMsg_t* )malloc( sizeof( rErrMsg_t ) );
    strncpy( myError->errMsg[myError->len]->msg, msg, ERR_MSG_LEN - 1 );
    myError->errMsg[myError->len]->status = status;
    myError->len++;

    return 0;
}

int
freeRError( rError_t *myError ) {

    if ( myError == NULL ) {
        return 0;
    }

    freeRErrorContent( myError );
    free( myError );
    return 0;
}

int
freeRErrorContent( rError_t *myError ) {
    int i;

    if ( myError == NULL ) {
        return 0;
    }

    if ( myError->len > 0 ) {
        for ( i = 0; i < myError->len; i++ ) {
            free( myError->errMsg[i] );
        }
        free( myError->errMsg );
    }

    memset( myError, 0, sizeof( rError_t ) );

    return 0;
}

int
myHtonll( rodsLong_t inlonglong, rodsLong_t *outlonglong ) {
    char *inPtr, *outPtr;

    if ( outlonglong == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( ntohl( 1 ) == 1 ) {
        *outlonglong = inlonglong;
        return 0;
    }

    inPtr = ( char * )( static_cast< void * >( &inlonglong ) );
    outPtr = ( char * )( static_cast<void *>( outlonglong ) );

    int i;
    int byte_length = sizeof( rodsLong_t );
    for ( i = 0; i < byte_length; i++ ) {
        outPtr[i] = inPtr[byte_length - 1 - i];
    }
    return 0;
}

int
myNtohll( rodsLong_t inlonglong,  rodsLong_t *outlonglong ) {
    char *inPtr, *outPtr;

    if ( outlonglong == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( ntohl( 1 ) == 1 ) {
        *outlonglong = inlonglong;
        return 0;
    }

    inPtr = ( char * )( static_cast< void * >( &inlonglong ) );
    outPtr = ( char * )( static_cast<void *>( outlonglong ) );

    int i;
    int byte_length = sizeof( rodsLong_t );
    for ( i = 0; i < byte_length; i++ ) {
        outPtr[i] = inPtr[byte_length - 1 - i];
    }
    return 0;
}

int
freeDataObjInfo( dataObjInfo_t *dataObjInfo ) {
    if ( dataObjInfo == NULL ) {
        return 0;
    }

    clearKeyVal( &dataObjInfo->condInput );
    /* separate specColl */
    if ( dataObjInfo->specColl != NULL ) {
        free( dataObjInfo->specColl );
    }

    free( dataObjInfo );
    dataObjInfo = 0;
    return 0;
}

char *
getValByKey( const keyValPair_t *condInput, const char *keyWord ) {
    int i;

    if ( condInput == NULL ) {
        return NULL;
    }

    for ( i = 0; i < condInput->len; i++ ) {
        if ( strcmp( condInput->keyWord[i], keyWord ) == 0 ) {
            return condInput->value[i];
        }
    }

    return NULL;
}

int
rmKeyVal( keyValPair_t *condInput, char *keyWord ) {
    int i, j;

    if ( condInput == NULL ) {
        return 0;
    }

    for ( i = 0; i < condInput->len; i++ ) {
        if ( condInput->keyWord[i] != NULL &&
                strcmp( condInput->keyWord[i], keyWord ) == 0 ) {
            free( condInput->keyWord[i] );
            free( condInput->value[i] );
            condInput->len--;
            for ( j = i; j < condInput->len; j++ ) {
                condInput->keyWord[j] = condInput->keyWord[j + 1];
                condInput->value[j] = condInput->value[j + 1];
            }
            if ( condInput->len <= 0 ) {
                free( condInput->keyWord );
                free( condInput->value );
                condInput->value = condInput->keyWord = NULL;
            }
            break;
        }
    }
    return 0;
}

int
replKeyVal( const keyValPair_t *srcCondInput, keyValPair_t *destCondInput ) {
    int i;

    memset( destCondInput, 0, sizeof( keyValPair_t ) );

    for ( i = 0; i < srcCondInput->len; i++ ) {
        addKeyVal( destCondInput, srcCondInput->keyWord[i],
                   srcCondInput->value[i] );
    }
    return 0;
}

int
replSpecColl( specColl_t *inSpecColl, specColl_t **outSpecColl ) {
    if ( inSpecColl == NULL || outSpecColl == NULL ) {
        return USER__NULL_INPUT_ERR;
    }
    *outSpecColl = ( specColl_t * )malloc( sizeof( specColl_t ) );
    *( *outSpecColl ) = *inSpecColl;

    return 0;
}

int
addKeyVal( keyValPair_t *condInput, const char *keyWord, const char *value ) {
    if ( condInput == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    if ( condInput->keyWord == NULL || condInput->value == NULL ) {
        condInput->len = 0;
    }

    /* check if the keyword exists */
    for ( int i = 0; i < condInput->len; i++ ) {
        if ( condInput->keyWord[i] == NULL || strlen( condInput->keyWord[i] ) == 0 ) {
            free( condInput->keyWord[i] );
            free( condInput->value[i] );
            condInput->keyWord[i] = strdup( keyWord );
            condInput->value[i] = value ? strdup( value ) : NULL;
            return 0;
        }
        else if ( strcmp( keyWord, condInput->keyWord[i] ) == 0 ) {
            free( condInput->value[i] );
            condInput->value[i] = value ? strdup( value ) : NULL;
            return 0;
        }
    }

    if ( ( condInput->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        condInput->keyWord = ( char ** )realloc( condInput->keyWord,
                             ( condInput->len + PTR_ARRAY_MALLOC_LEN ) * sizeof( *condInput->keyWord ) );
        condInput->value = ( char ** )realloc( condInput->value,
                                               ( condInput->len + PTR_ARRAY_MALLOC_LEN ) * sizeof( *condInput->value ) );
        memset( condInput->keyWord + condInput->len, 0, PTR_ARRAY_MALLOC_LEN * sizeof( *condInput->keyWord ) );
        memset( condInput->value + condInput->len, 0, PTR_ARRAY_MALLOC_LEN * sizeof( *condInput->value ) );
    }

    condInput->keyWord[condInput->len] = strdup( keyWord );
    condInput->value[condInput->len] = value ? strdup( value ) : NULL;
    condInput->len++;

    return 0;
}

int
addInxIval( inxIvalPair_t *inxIvalPair, int inx, int value ) {
    int *newInx;
    int *newValue;
    int newLen;
    int i;

    if ( inxIvalPair == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( ( inxIvalPair->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        newLen = inxIvalPair->len + PTR_ARRAY_MALLOC_LEN;
        newInx = ( int * ) malloc( newLen * sizeof( int ) );
        newValue = ( int * ) malloc( newLen * sizeof( int ) );
        memset( newInx, 0, newLen * sizeof( int ) );
        memset( newValue, 0, newLen * sizeof( int ) );
        for ( i = 0; i < inxIvalPair->len; i++ ) {
            newInx[i] = inxIvalPair->inx[i];
            newValue[i] = inxIvalPair->value[i];
        }
        if ( inxIvalPair->inx != NULL ) {
            free( inxIvalPair->inx );
        }
        if ( inxIvalPair->value != NULL ) {
            free( inxIvalPair->value );
        }
        inxIvalPair->inx = newInx;
        inxIvalPair->value = newValue;
    }

    inxIvalPair->inx[inxIvalPair->len] = inx;
    inxIvalPair->value[inxIvalPair->len] = value;
    inxIvalPair->len++;

    return 0;
}

int
addInxVal( inxValPair_t *inxValPair, int inx, const char *value ) {
    int *newInx;
    char **newValue;
    int newLen;
    int i;

    if ( inxValPair == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( ( inxValPair->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        newLen = inxValPair->len + PTR_ARRAY_MALLOC_LEN;
        newInx = ( int * ) malloc( newLen * sizeof( *newInx ) );
        newValue = ( char ** ) malloc( newLen * sizeof( *newValue ) );
        memset( newInx, 0, newLen * sizeof( *newInx ) );
        memset( newValue, 0, newLen * sizeof( *newValue ) );
        for ( i = 0; i < inxValPair->len; i++ ) {
            newInx[i] = inxValPair->inx[i];
            newValue[i] = inxValPair->value[i];
        }
        if ( inxValPair->inx != NULL ) {
            free( inxValPair->inx );
        }
        if ( inxValPair->value != NULL ) {
            free( inxValPair->value );
        }
        inxValPair->inx = newInx;
        inxValPair->value = newValue;
    }

    inxValPair->inx[inxValPair->len] = inx;
    inxValPair->value[inxValPair->len] = strdup( value );
    inxValPair->len++;

    return 0;
}

int
addStrArray( strArray_t *strArray, char *value ) {
    char *newValue;
    int newLen;
    int i;
    int size;
    int myLen;

    if ( strArray == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( strArray->size <= 0 ) {
        if ( strArray->len == 0 ) {
            /* default to NAME_LEN */

            strArray->size = NAME_LEN;
        }
        else {
            rodsLog( LOG_ERROR,
                     "addStrArray: invalid size %d, len %d",
                     strArray->size, strArray->len );
            return SYS_INTERNAL_NULL_INPUT_ERR;
        }
    }

    myLen = strlen( value );

    size = strArray->size;
    while ( size < myLen + 1 ) {
        size = size * 2;
    }


    /* XXXXXXX should be replaced by resizeStrArray after 2.3 release */
    if ( size != strArray->size ||
            ( strArray->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        int oldSize = strArray->size;
        /* have to redo it */
        strArray->size = size;
        newLen = strArray->len + PTR_ARRAY_MALLOC_LEN;
        newValue = ( char * ) malloc( newLen * size );
        memset( newValue, 0, newLen * size );
        for ( i = 0; i < strArray->len; i++ ) {
            rstrcpy( &newValue[i * size], &strArray->value[i * oldSize], size );
        }
        if ( strArray->value != NULL ) {
            free( strArray->value );
        }
        strArray->value = newValue;
    }

    rstrcpy( &strArray->value[strArray->len * size], value, size );
    strArray->len++;

    return 0;
}

int
clearKeyVal( keyValPair_t *condInput ) {

    if ( condInput == NULL || condInput->len < 1 ) {
        return 0;
    }

    for ( int i = 0; i < condInput->len; i++ ) {
        if ( condInput->keyWord != NULL ) {
            free( condInput->keyWord[i] );
        }
        if ( condInput->value != NULL ) {
            free( condInput->value[i] );
        }
    }

    free( condInput->keyWord );
    free( condInput->value );
    memset( condInput, 0, sizeof( keyValPair_t ) );
    return 0;
}

int
clearInxIval( inxIvalPair_t *inxIvalPair ) {
    if ( inxIvalPair == NULL || inxIvalPair->len <= 0 ) {
        return 0;
    }

    free( inxIvalPair->inx );
    free( inxIvalPair->value );
    memset( inxIvalPair, 0, sizeof( inxIvalPair_t ) );

    return 0;
}

int
clearInxVal( inxValPair_t *inxValPair ) {
    int i;

    if ( inxValPair == NULL || inxValPair->len <= 0 ) {
        return 0;
    }

    for ( i = 0; i < inxValPair->len; i++ ) {
        free( inxValPair->value[i] );
    }

    free( inxValPair->inx );
    free( inxValPair->value );
    memset( inxValPair, 0, sizeof( inxValPair_t ) );

    return 0;
}

void
clearGenQueryInp( void* voidInp ) {

    if ( voidInp == NULL ) {
        return;
    }

    genQueryInp_t *genQueryInp = ( genQueryInp_t* ) voidInp;
    clearInxIval( &genQueryInp->selectInp );
    clearInxVal( &genQueryInp->sqlCondInp );
    clearKeyVal( &genQueryInp->condInput );

    return;
}

int
freeGenQueryOut( genQueryOut_t **genQueryOut ) {
    if ( genQueryOut == NULL ) {
        return 0;
    }

    if ( *genQueryOut == NULL ) {
        return 0;
    }

    clearGenQueryOut( *genQueryOut );
    free( *genQueryOut );
    *genQueryOut = NULL;

    return 0;
}

void
clearGenQueryOut( void* voidInp ) {
    genQueryOut_t *genQueryOut = ( genQueryOut_t* ) voidInp;
    int i;

    if ( genQueryOut == NULL ) {
        return;
    }

    for ( i = 0; i < genQueryOut->attriCnt; i++ ) {
        if ( genQueryOut->sqlResult[i].value != NULL ) {
            free( genQueryOut->sqlResult[i].value );
        }
    }
    return;
}

void
clearBulkOprInp( void* voidInp ) {
    bulkOprInp_t *bulkOprInp = ( bulkOprInp_t* ) voidInp;
    if ( bulkOprInp == NULL ) {
        return;
    }
    clearGenQueryOut( &bulkOprInp->attriArray );
    clearKeyVal( &bulkOprInp->condInput );
    return;
}

sqlResult_t *
getSqlResultByInx( genQueryOut_t *genQueryOut, int attriInx ) {
    int i;

    if ( genQueryOut == NULL ) {
        return NULL;
    }

    for ( i = 0; i < genQueryOut->attriCnt; i++ ) {
        if ( genQueryOut->sqlResult[i].attriInx == attriInx ) {
            return &genQueryOut->sqlResult[i];
        }
    }
    return NULL;
}

void
clearModDataObjMetaInp( void* voidInp ) {
    modDataObjMeta_t *modDataObjMetaInp = ( modDataObjMeta_t* ) voidInp;
    if ( modDataObjMetaInp == NULL ) {
        return;
    }

    if ( modDataObjMetaInp->regParam != NULL ) {
        clearKeyVal( modDataObjMetaInp->regParam );
        free( modDataObjMetaInp->regParam );
    }

    if ( modDataObjMetaInp->dataObjInfo != NULL ) {
        freeDataObjInfo( modDataObjMetaInp->dataObjInfo );
    }

    return;
}

void
clearUnregDataObj( void* voidInp ) {
    unregDataObj_t *unregDataObjInp = ( unregDataObj_t* ) voidInp;
    if ( unregDataObjInp == NULL ) {
        return;
    }

    if ( unregDataObjInp->condInput != NULL ) {
        clearKeyVal( unregDataObjInp->condInput );
        free( unregDataObjInp->condInput );
    }

    if ( unregDataObjInp->dataObjInfo != NULL ) {
        freeDataObjInfo( unregDataObjInp->dataObjInfo );
    }

    return ;
}

void
clearRegReplicaInp( void* voidInp ) {
    regReplica_t *regReplicaInp = ( regReplica_t* ) voidInp;
    if ( regReplicaInp == NULL ) {
        return;
    }

    clearKeyVal( &regReplicaInp->condInput );

    if ( regReplicaInp->srcDataObjInfo != NULL ) {
        freeDataObjInfo( regReplicaInp->srcDataObjInfo );
    }

    if ( regReplicaInp->destDataObjInfo != NULL ) {
        freeDataObjInfo( regReplicaInp->destDataObjInfo );
    }

    memset( regReplicaInp, 0, sizeof( regReplica_t ) );

    return;
}

void
clearFileOpenInp( void* voidInp ) {
    fileOpenInp_t *fileOpenInp = ( fileOpenInp_t* ) voidInp;
    if ( fileOpenInp == NULL ) {
        return;
    }
    clearKeyVal( &fileOpenInp->condInput );
    memset( fileOpenInp, 0, sizeof( fileOpenInp_t ) );

    return;
}

void
clearDataObjInp( void* voidInp ) {
    dataObjInp_t *dataObjInp = ( dataObjInp_t* ) voidInp;
    if ( dataObjInp == NULL ) {
        return;
    }

    clearKeyVal( &dataObjInp->condInput );
    if ( dataObjInp->specColl != NULL ) {
        free( dataObjInp->specColl );
    }

    memset( dataObjInp, 0, sizeof( dataObjInp_t ) );

    return;
}

void
clearCollInp( void* voidInp ) {

    collInp_t *collInp = ( collInp_t* ) voidInp;
    if ( collInp == NULL ) {
        return;
    }

    clearKeyVal( &collInp->condInput );

    memset( collInp, 0, sizeof( collInp_t ) );

    return;
}

void
clearDataObjCopyInp( void* voidInp ) {
    dataObjCopyInp_t *dataObjCopyInp = ( dataObjCopyInp_t* ) voidInp;
    if ( dataObjCopyInp == NULL ) {
        return;
    }

    clearKeyVal( &dataObjCopyInp->destDataObjInp.condInput );
    clearKeyVal( &dataObjCopyInp->srcDataObjInp.condInput );

    if ( dataObjCopyInp->srcDataObjInp.specColl != NULL ) {
        free( dataObjCopyInp->srcDataObjInp.specColl );
    }

    memset( dataObjCopyInp, 0, sizeof( dataObjCopyInp_t ) );

    return;
}

int
parseMultiStr( char *strInput, strArray_t *strArray ) {
    char *startPtr, *endPtr;
    int endReached = 0;

    if ( strInput == NULL || strArray == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    startPtr = endPtr = strInput;

    while ( 1 ) {
        // two %% will be taken as an input instead of as a delimiter
        while ( *endPtr != '%' && *endPtr != '\0' ) {
            endPtr ++;
        }
        if ( *endPtr == '%' ) {
            if ( *( endPtr + 1 ) == '%' ) {
                endPtr ++;
                endPtr ++;
                continue;
            }
            *endPtr = '\0';
        }
        else {
            endReached = 1;
        }

        addStrArray( strArray, startPtr );

        if ( endReached == 1 ) {
            break;
        }

        endPtr++;
        startPtr = endPtr;
    }

    return strArray->len;
}

/* Get the current time, in the  form: 2006-10-25-10.52.43 */
/*                                     0123456789012345678 */
extern char *tzname[2];

/*
   Convert a Unix time value (as from getNowStr) to a local
   time format.
 */
int
getLocalTimeFromRodsTime( const char *timeStrIn, char *timeStr ) {
    time_t myTime;
    struct tm *mytm;

    // This is 1 because they actually capture a leading space
    if ( strlen( timeStrIn ) <= 1 ) {
        strcpy( timeStr, "Never" );
    }
    else {
        if ( sizeof( time_t ) == 4 ) {
            myTime = atol( timeStrIn );
        }
        else {
#ifdef _WIN32
            myTime = _atoi64( timeStrIn );
#else
            myTime = atoll( timeStrIn );
#endif
        }

        mytm = localtime( &myTime );

        getLocalTimeStr( mytm, timeStr );
    }
    return 0;
}

int
getLocalTimeStr( struct tm *mytm, char *timeStr ) {
    snprintf( timeStr, TIME_LEN, "%4d-%2d-%2d.%2d:%2d:%2d",
              mytm->tm_year + 1900, mytm->tm_mon + 1, mytm->tm_mday,
              mytm->tm_hour, mytm->tm_min, mytm->tm_sec );

    if ( timeStr[5] == ' ' ) {
        timeStr[5] = '0';
    }
    if ( timeStr[8] == ' ' ) {
        timeStr[8] = '0';
    }
    if ( timeStr[11] == ' ' ) {
        timeStr[11] = '0';
    }
    if ( timeStr[14] == ' ' ) {
        timeStr[14] = '0';
    }
    if ( timeStr[17] == ' ' ) {
        timeStr[17] = '0';
    }

    return 0;
}

int
localToUnixTime( char * localTime, char * unixTime ) {
    time_t myTime;
    struct tm *mytm;
    time_t newTime;
    char s[TIME_LEN];

    myTime = time( NULL );
    mytm = localtime( &myTime );

    rstrcpy( s, localTime, TIME_LEN );

    s[19] = '\0';
    mytm->tm_sec = atoi( &s[17] );
    s[16] = '\0';
    mytm->tm_min = atoi( &s[14] );
    s[13] = '\0';
    mytm->tm_hour = atoi( &s[11] );
    s[10] = '\0';
    mytm->tm_mday = atoi( &s[8] );
    s[7] = '\0';
    mytm->tm_mon = atoi( &s[5] ) - 1;
    s[4] = '\0';
    mytm->tm_year = atoi( &s[0] ) - 1900;

    newTime = mktime( mytm );
    if ( sizeof( newTime ) == 64 ) {
        snprintf( unixTime, TIME_LEN, "%lld", ( rodsLong_t ) newTime );
    }
    else {
        snprintf( unixTime, TIME_LEN, "%d", ( uint ) newTime );
    }
    return 0;
}

int
isInteger( const char * inStr ) {
    int i;
    int len;

    len = strlen( inStr );
    /* see if it is all digit */
    for ( i = 0; i < len; i++ ) {
        if ( !isdigit( inStr[i] ) ) {
            return 0;
        }
    }
    return 1;
}


/* convertDateFormat  - uses the checkDateFormat to convert string 's'
 * into sec of unix time. But if the time is in the YYYY-*** format
 * adds the current date (in unix seconds format) to forma  full date
 */

int
convertDateFormat( char * s, char * currTime ) {
    rodsLong_t  it;
    char tstr[200];
    int i;
    rstrcpy( tstr, s, 199 );
    i = checkDateFormat( tstr );
    if ( i != 0 ) {
        return i;
    }
    if ( !isInteger( s ) && strchr( s, '-' ) == NULL && strchr( s, ':' ) == NULL ) {
        it = atol( tstr ) + atol( currTime );
        sprintf( s, "%lld", it );
    }
    else {
        strcpy( s, tstr );
    }
    return 0;
}

/* checkDateFormat - convert the string given in s and output the time
 * in sec of unix time in the same string s
 * The input can be incremental time given in :
 *     nnnn - an integer. assumed to be in sec
 *     nnnns - an integer followed by 's' ==> in sec
 *     nnnnm - an integer followed by 'm' ==> in min
 *     nnnnh - an integer followed by 'h' ==> in hours
 *     nnnnd - an integer followed by 'd' ==> in days
 *     nnnny - an integer followed by 'y' ==> in years
 *     dd.hh:mm:ss - where dd, hh, mm and ss are 2 digits integers representing
 *       days, hours minutes and seconds, repectively. Truncation from the
 *       end is allowed. e.g. 20:40 means mm:ss
 * The input can also be full calendar time in the form:
 *    YYYY-MM-DD.hh:mm:ss  - Truncation from the beginning is allowed.
 *       e.g., 2007-07-29.12 means noon of July 29, 2007.
 *
 */

int
checkDateFormat( char * s ) {
    /* Note. The input *s is assumed to be TIME_LEN long */
    int len;
    char t[] = "0000-00-00.00:00:00";
    char outUnixTime[TIME_LEN];
    int status;
    int offset = 0;

    if ( isInteger( s ) ) {
        return 0;
    }

    len = strlen( s );

    if ( s[len - 1] == 's' ) {
        /* in sec */
        s[len - 1] = '\0';
        offset = atoi( s );
        snprintf( s, 19, "%d", offset );
        return 0;
    }
    else if ( s[len - 1] == 'm' ) {
        /* in min */
        s[len - 1] = '\0';
        offset = atoi( s ) * 60;
        snprintf( s, 19, "%d", offset );
        return 0;
    }
    else if ( s[len - 1] == 'h' ) {
        /* in hours */
        s[len - 1] = '\0';
        offset = atoi( s ) * 3600;
        snprintf( s, 19, "%d", offset );
        return 0;
    }
    else if ( s[len - 1] == 'd' ) {
        /* in days */
        s[len - 1] = '\0';
        offset = atoi( s ) * 3600 * 24;
        snprintf( s, 19, "%d", offset );
        return 0;
    }
    else if ( s[len - 1] == 'y' ) {
        /* in days */
        s[len - 1] = '\0';
        offset = atoi( s ) * 3600 * 24 * 365;
        snprintf( s, 19, "%d", offset );
        return 0;
    }
    else if ( len < 19 ) {
        /* not a full date. */
        if ( isdigit( s[0] ) && isdigit( s[1] ) && isdigit( s[2] ) && isdigit( s[3] ) ) {
            /* start with year, fill in the rest */
            strcat( s, ( char * )&t[len] );
        }
        else {
            /* must be offset */
            int mypos;

            /* sec */
            mypos = len - 1;
            while ( mypos >= 0 ) {
                if ( isdigit( s[mypos] ) ) {
                    offset += s[mypos] - 48;
                }
                else {
                    return DATE_FORMAT_ERR;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 10 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( s[mypos] != ':' ) {
                        return DATE_FORMAT_ERR;
                    }

                /* min */
                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 60 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 10 * 60 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( s[mypos] != ':' ) {
                        return DATE_FORMAT_ERR;
                    }

                /* hour */
                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 3600 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 10 * 3600 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( s[mypos] != '.' ) {
                        return DATE_FORMAT_ERR;
                    }

                /* day */

                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 24 * 3600 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }

                mypos--;
                if ( mypos >= 0 )
                    if ( isdigit( s[mypos] ) ) {
                        offset += 10 * 24 * 3600 * ( s[mypos] - 48 );
                    }
                    else {
                        return DATE_FORMAT_ERR;
                    }
                else {
                    break;
                }
            }
            snprintf( s, 19, "%d", offset );
            return 0;
        }
    }

    if ( isdigit( s[0] ) && isdigit( s[1] ) && isdigit( s[2] ) && isdigit( s[3] ) &&
            isdigit( s[5] ) && isdigit( s[6] ) && isdigit( s[8] ) && isdigit( s[9] ) &&
            isdigit( s[11] ) && isdigit( s[12] ) && isdigit( s[14] ) && isdigit( s[15] ) &&
            isdigit( s[17] ) && isdigit( s[18] ) &&
            s[4] == '-' && s[7] == '-' && s[10] == '.' &&
            s[13] == ':' && s[16] == ':' ) {
        status = localToUnixTime( s, outUnixTime );
        if ( status >= 0 ) {
            rstrcpy( s, outUnixTime, TIME_LEN );
        }
        return status;
    }
    else {
        return DATE_FORMAT_ERR;
    }
}

int
printErrorStack( rError_t * rError ) {
    int i, len;
    rErrMsg_t *errMsg;

    if ( rError == NULL ) {
        return 0;
    }

    len = rError->len;

    for ( i = 0; i < len; i++ ) {
        errMsg = rError->errMsg[i];
        if ( errMsg->status != STDOUT_STATUS ) {
            printf( "Level %d: ", i );
        }
        printf( "%s\n", errMsg->msg );
    }
    return 0;
}

int
openRestartFile( char * restartFile, rodsRestart_t * rodsRestart ) {
    namespace fs = boost::filesystem;

    fs::path p( restartFile );
    char buf[MAX_NAME_LEN * 3];
    char *inptr;
    char tmpStr[MAX_NAME_LEN];
    int status;

    if ( !exists( p ) || file_size( p ) == 0 ) {
#ifndef windows_platform
        rodsRestart->fd = open( restartFile, O_RDWR | O_CREAT, 0644 );
#else
        rodsRestart->fd = iRODSNt_bopen( restartFile, O_RDWR | O_CREAT, 0644 );
#endif
        if ( rodsRestart->fd < 0 ) {
            status = UNIX_FILE_OPEN_ERR - errno;
            rodsLogError( LOG_ERROR, status,
                          "openRestartFile: open error for %s", restartFile );
            return status;
        }
        rodsRestart->restartState = 0;
        printf( "New restartFile %s opened\n", restartFile );
    }
    else if ( !is_regular_file( p ) ) {
        close( rodsRestart->fd );
        rodsRestart->fd = -1;
        status = UNIX_FILE_OPEN_ERR;
        rodsLogError( LOG_ERROR, status,
                      "openRestartFile: %s is not a file", restartFile );
        return UNIX_FILE_OPEN_ERR;
    }
    else {
#ifndef windows_platform
        rodsRestart->fd = open( restartFile, O_RDWR, 0644 );
#else
        rodsRestart->fd = iRODSNt_bopen( restartFile, O_RDWR, 0644 );
#endif
        if ( rodsRestart->fd < 0 ) {
            status = UNIX_FILE_OPEN_ERR - errno;
            rodsLogError( LOG_ERROR, status,
                          "openRestartFile: open error for %s", restartFile );
            return status;
        }
        status = read( rodsRestart->fd, ( void * ) buf, MAX_NAME_LEN * 3 );
        if ( status <= 0 ) {
            close( rodsRestart->fd );
            status = UNIX_FILE_READ_ERR - errno;
            rodsLogError( LOG_ERROR, status,
                          "openRestartFile: read error for %s", restartFile );
            return status;
        }

        inptr = buf;
        if ( getLineInBuf( &inptr, rodsRestart->collection, MAX_NAME_LEN ) < 0 ) {
            rodsLog( LOG_ERROR,
                     "openRestartFile: restartFile %s is empty", restartFile );
            return USER_RESTART_FILE_INPUT_ERR;
        }
        if ( getLineInBuf( &inptr, tmpStr, MAX_NAME_LEN ) < 0 ) {
            rodsLog( LOG_ERROR,
                     "openRestartFile: restartFile %s has 1 only line", restartFile );
            return USER_RESTART_FILE_INPUT_ERR;
        }
        rodsRestart->doneCnt = atoi( tmpStr );

        if ( getLineInBuf( &inptr, rodsRestart->lastDonePath,
                           MAX_NAME_LEN ) < 0 ) {
            rodsLog( LOG_ERROR,
                     "openRestartFile: restartFile %s has only 2 lines", restartFile );
            return USER_RESTART_FILE_INPUT_ERR;
        }

        if ( getLineInBuf( &inptr, rodsRestart->oprType,
                           NAME_LEN ) < 0 ) {
            rodsLog( LOG_ERROR,
                     "openRestartFile: restartFile %s has only 3 lines", restartFile );
            return USER_RESTART_FILE_INPUT_ERR;
        }

        rodsRestart->restartState = PATH_MATCHING;
        printf( "RestartFile %s opened\n", restartFile );
        printf( "Restarting collection/directory = %s     File count %d\n",
                rodsRestart->collection, rodsRestart->doneCnt );
        printf( "File last completed = %s\n", rodsRestart->lastDonePath );
    }
    return 0;
}

int
getLineInBuf( char **inbuf, char * outbuf, int bufLen ) {
    char *inPtr, *outPtr;
    int bytesCopied  = 0;
    int c;

    inPtr = *inbuf;
    outPtr = outbuf;

    while ( ( c = *inPtr ) != '\n' && c != EOF && bytesCopied < bufLen ) {
        c = *inPtr;
        if ( c == '\n' || c == EOF ) {
            break;
        }
        *outPtr = c;
        inPtr++;
        outPtr++;
        bytesCopied++;
    }
    *outPtr = '\0';
    *inbuf = inPtr + 1;
    return bytesCopied;
}





/* writeRestartFile - the restart file contain 4 lines:
 *   line 1 - collection.
 *   line 2 - doneCnt.
 *   line 3 - lastDonePath
 *   line 4 - oprType (BULK_OPR_KW or NON_BULK_OPR_KW);
 */

int
writeRestartFile( rodsRestart_t * rodsRestart, char * lastDonePath ) {
    char buf[MAX_NAME_LEN * 3];
    int status;

    rodsRestart->doneCnt = rodsRestart->curCnt;
    rstrcpy( rodsRestart->lastDonePath, lastDonePath, MAX_NAME_LEN );
    memset( buf, 0, MAX_NAME_LEN * 3 );
    snprintf( buf, MAX_NAME_LEN * 3, "%s\n%d\n%s\n%s\n",
              rodsRestart->collection, rodsRestart->doneCnt,
              rodsRestart->lastDonePath, rodsRestart->oprType );

    lseek( rodsRestart->fd, 0, SEEK_SET );
    status = write( rodsRestart->fd, buf, MAX_NAME_LEN * 3 );
    if ( status != MAX_NAME_LEN * 3 ) {
        rodsLog( LOG_ERROR,
                 "writeRestartFile: write error, errno = %d",
                 errno );
        return SYS_COPY_LEN_ERR - errno;
    }
    return 0;
}

int
procAndWriteRestartFile( rodsRestart_t * rodsRestart, char * donePath ) {
    int status;

    if ( rodsRestart->fd <= 0 ) {
        return 0;
    }

    rodsRestart->curCnt ++;
    status = writeRestartFile( rodsRestart, donePath );

    return status;
}

int
setStateForRestart( rodsRestart_t * rodsRestart, rodsPath_t * targPath,
                    rodsArguments_t * rodsArgs ) {
    if ( rodsRestart->restartState & PATH_MATCHING ) {
        /* check the restart collection */
        if ( strstr( targPath->outPath, rodsRestart->collection ) != NULL ) {
            /* just use the rodsRestart->collection because the
             * targPath may be resolved into a different path */
            rstrcpy( targPath->outPath, rodsRestart->collection, MAX_NAME_LEN );
            rodsRestart->restartState |= MATCHED_RESTART_COLL;
            rodsRestart->curCnt = 0;
            if ( rodsArgs->verbose == True ) {
                printf( "**** Scanning to Restart Operation in %s ****\n",
                        targPath->outPath );
            }
        }
        else {
            /* take out MATCHED_RESTART_COLL */
            if ( rodsArgs->verbose == True ) {
                printf( "**** Skip Coll/dir %s ****\n",
                        targPath->outPath );
            }
            rodsRestart->restartState = rodsRestart->restartState &
                                        ( ~MATCHED_RESTART_COLL );
        }
    }
    else if ( rodsRestart->fd > 0 ) {
        /* just writing restart file */
        rstrcpy( rodsRestart->collection, targPath->outPath,
                 MAX_NAME_LEN );
        rodsRestart->doneCnt = rodsRestart->curCnt = 0;
    }
    return 0;
}



int
getAttrIdFromAttrName( char * cname ) {

    int i;
    for ( i = 0; i < NumOfColumnNames ; i++ ) {
        if ( !strcmp( columnNames[i].columnName, cname ) ) {
            return columnNames[i].columnId;
        }
    }
    return NO_COLUMN_NAME_FOUND;
}

int
separateSelFuncFromAttr( char * t, char **aggOp, char **colNm ) {
    char *s;

    if ( ( s = strchr( t, '(' ) ) == NULL ) {
        *colNm = t;
        *aggOp = NULL;
        return 0;
    }
    *aggOp = t;
    *s = '\0';
    s++;
    *colNm = s;
    if ( ( s = strchr( *colNm, ')' ) ) == NULL ) {
        return NO_COLUMN_NAME_FOUND;
    }
    *s = '\0';
    return 0;
}

int
getSelVal( char * c ) {
    if ( c == NULL ) {
        return 1;
    }
    if ( !strcmp( c, "sum" ) || !strcmp( c, "SUM" ) ) {
        return SELECT_SUM;
    }
    if ( !strcmp( c, "min" ) || !strcmp( c, "MIN" ) ) {
        return SELECT_MIN;
    }
    if ( !strcmp( c, "max" ) || !strcmp( c, "MAX" ) ) {
        return SELECT_MAX;
    }
    if ( !strcmp( c, "avg" ) || !strcmp( c, "AVG" ) ) {
        return SELECT_AVG;
    }
    if ( !strcmp( c, "count" ) || !strcmp( c, "COUNT" ) ) {
        return SELECT_COUNT;
    }
    // =-=-=-=-=-=-=-
    // JMC - backport 4795
    if ( !strcmp( c, "order" ) || !strcmp( c, "ORDER" ) ) {
        return ORDER_BY;
    }
    if ( !strcmp( c, "order_desc" ) || !strcmp( c, "ORDER_DESC" ) ) {
        return ORDER_BY_DESC;
    }
    // =-=-=-=-=-=-=-

    return 1;
}


char *
getAttrNameFromAttrId( int cid ) {

    int i;
    for ( i = 0; i < NumOfColumnNames ; i++ ) {
        if ( columnNames[i].columnId == cid ) {
            return columnNames[i].columnName;
        }
    }
    return NULL;
}

int
goodStrExpr( char * expr ) {
    int qcnt = 0;
    int qqcnt = 0;
    int bcnt = 0;
    int i = 0;
    int inq =  0;
    int inqq =  0;
    while ( expr[i] != '\0' ) {
        if ( inq ) {
            if ( expr[i] == '\'' ) {
                inq--;
                qcnt++;
            }
        }
        else if ( inqq ) {
            if ( expr[i] == '"' ) {
                inqq--;
                qqcnt++;
            }
        }
        else if ( expr[i] == '\'' ) {
            inq++;
            qcnt++;
        }
        else if ( expr[i] == '"' ) {
            inqq++;
            qqcnt++;
        }
        else if ( expr[i] == '(' ) {
            bcnt++;
        }
        else if ( expr[i] == ')' )
            if ( bcnt > 0 ) {
                bcnt--;
            }
        i++;
    }
    if ( bcnt != 0 || qcnt % 2 != 0 || qqcnt % 2 != 0 ) {
        return -1;
    }
    return 0;

}


char *getCondFromString( char * t ) {
    char *u;
    char *u1, *u2;
    char *s;

    s = t;
    for ( ;; ) {
        /* Search for an 'and' string, either case, and use the one
           that appears first. */
        u1 = strstr( s, " and " );
        u2 = strstr( s, " AND " );
        u = u1;
        if ( u1 == NULL ) {
            u = u2;
        }
        if ( u1 != NULL && u2 != NULL ) {
            if ( strlen( u2 ) > strlen( u1 ) ) {
                u = u2;    /* both are present, use the first */
            }
        }

        if ( u != NULL ) {
            *u = '\0';
            if ( goodStrExpr( t ) == 0 ) {
                *u = ' ';
                return u;
            }
            *u = ' ';
            s = u + 1;
        }
        else {
            break;
        }
    }
    return NULL;
}

int
fillGenQueryInpFromStrCond( char * str, genQueryInp_t * genQueryInp ) {

    int  n, m;
    char *p, *t, *f, *u, *a, *c;
    char *s;
    s = strdup( str );
    if ( ( t = strstr( s, "select" ) ) != NULL ||
            ( t = strstr( s, "SELECT" ) ) != NULL ) {

        if ( ( f = strstr( t, "where" ) ) != NULL ||
                ( f = strstr( t, "WHERE" ) ) != NULL ) {
            /* Where Condition Found*/
            *f = '\0';
        }
        t = t +  7;
        while ( ( u = strchr( t, ',' ) ) != NULL ) {
            *u = '\0';
            trimWS( t );
            separateSelFuncFromAttr( t, &a, &c );
            m = getSelVal( a );
            n = getAttrIdFromAttrName( c );
            if ( n < 0 ) {
                free( s );
                return n;
            }
            addInxIval( &genQueryInp->selectInp, n, m );
            t  = u + 1;
        }
        trimWS( t );
        separateSelFuncFromAttr( t, &a, &c );
        m = getSelVal( a );
        n = getAttrIdFromAttrName( c );
        if ( n < 0 ) {
            free( s );
            return n;
        }
        addInxIval( &genQueryInp->selectInp, n, m );
        if ( f == NULL ) {
            free( s );
            return 0;
        }
    }
    else {
        free( s );
        return INPUT_ARG_NOT_WELL_FORMED_ERR;
    }
    t = f + 6;
    while ( ( u = getCondFromString( t ) ) != NULL ) {
        *u = '\0';
        trimWS( t );
        if ( ( p = strchr( t, ' ' ) ) == NULL ) {
            free( s );
            return INPUT_ARG_NOT_WELL_FORMED_ERR;
        }
        *p = '\0';
        n = getAttrIdFromAttrName( t );
        if ( n < 0 ) {
            free( s );
            return n;
        }
        addInxVal( &genQueryInp->sqlCondInp, n, p + 1 );
        t = u + 5;
    }
    trimWS( t );
    if ( ( p = strchr( t, ' ' ) ) == NULL ) {
        free( s );
        return INPUT_ARG_NOT_WELL_FORMED_ERR;
    }
    *p = '\0';
    n = getAttrIdFromAttrName( t );
    if ( n < 0 ) {
        free( s );
        return n;
    }
    addInxVal( &genQueryInp->sqlCondInp, n, p + 1 );
    free( s );
    return 0;
}

int
getSpecCollTypeStr( specColl_t * specColl, char * outStr ) {
    int i;

    if ( specColl->collClass == NO_SPEC_COLL ) {
        return SYS_UNMATCHED_SPEC_COLL_TYPE;
    }
    else if ( specColl->collClass == MOUNTED_COLL ) {
        rstrcpy( outStr, MOUNT_POINT_STR, NAME_LEN );
        return 0;
    }
    else if ( specColl->collClass == LINKED_COLL ) {
        rstrcpy( outStr, LINK_POINT_STR, NAME_LEN );
        return 0;
    }
    else {
        for ( i = 0; i < NumStructFileType; i++ ) {
            if ( specColl->type == StructFileTypeDef[i].type ) {
                rstrcpy( outStr, StructFileTypeDef[i].typeName, NAME_LEN );
                return 0;
            }
        }
        rodsLog( LOG_ERROR,
                 "getSpecCollTypeStr: unmatch specColl type %d", specColl->type );
        return SYS_UNMATCHED_SPEC_COLL_TYPE;
    }
}

int
resolveSpecCollType( char * type, char * collection, char * collInfo1,
                     char * collInfo2, specColl_t * specColl ) {
    int i;

    if ( specColl == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    if ( *type == '\0' ) {
        specColl->collClass = NO_SPEC_COLL;
        return SYS_UNMATCHED_SPEC_COLL_TYPE;
    }

    rstrcpy( specColl->collection, collection,
             MAX_NAME_LEN );

    if ( strcmp( type, MOUNT_POINT_STR ) == 0 ) {
        specColl->collClass = MOUNTED_COLL;
        rstrcpy( specColl->phyPath, collInfo1, MAX_NAME_LEN );

        irods::hierarchy_parser parse;
        parse.set_string( collInfo2 );

        std::string first_resc;
        parse.first_resc( first_resc );

        rstrcpy( specColl->resource, first_resc.c_str(), NAME_LEN );
        rstrcpy( specColl->rescHier, collInfo2, NAME_LEN );

        return 0;
    }
    else if ( strcmp( type, LINK_POINT_STR ) == 0 ) {
        specColl->collClass = LINKED_COLL;
        rstrcpy( specColl->phyPath, collInfo1, MAX_NAME_LEN );

        return 0;
    }
    else {
        for ( i = 0; i < NumStructFileType; i++ ) {
            if ( strcmp( type, StructFileTypeDef[i].typeName ) == 0 ) {
                specColl->collClass = STRUCT_FILE_COLL;
                specColl->type = StructFileTypeDef[i].type;
                rstrcpy( specColl->objPath, collInfo1,
                         MAX_NAME_LEN );
                parseCachedStructFileStr( collInfo2, specColl );
                return 0;
            }
        }

        specColl->collClass = NO_SPEC_COLL;
        rodsLog( LOG_ERROR,
                 "resolveSpecCollType: unmatch specColl type %s", type );
        return SYS_UNMATCHED_SPEC_COLL_TYPE;
    }
}

int
parseCachedStructFileStr( char * collInfo2, specColl_t * specColl ) {
    char *tmpPtr1, *tmpPtr2;
    int len;

    if ( collInfo2 == NULL || specColl == NULL ) {
        rodsLog( LOG_ERROR,
                 "parseCachedStructFileStr: NULL input" );
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    if ( strlen( collInfo2 ) == 0 ) {
        /* empty */
        specColl->cacheDir[0] = specColl->resource[0] = '\0';
        return 0;
    }

    tmpPtr1 = strstr( collInfo2, ";;;" );

    if ( tmpPtr1 == NULL ) {
        rodsLog( LOG_NOTICE,
                 "parseCachedStructFileStr: collInfo2 %s format error 1", collInfo2 );
        return SYS_COLLINFO_2_FORMAT_ERR;
    }

    len = ( int )( tmpPtr1 - collInfo2 );
    strncpy( specColl->cacheDir, collInfo2, len );

    tmpPtr1 += 3;

    tmpPtr2 = strstr( tmpPtr1, ";;;" );

    if ( tmpPtr2 == NULL ) {
        rodsLog( LOG_NOTICE,
                 "parseCachedStructFileStr: collInfo2 %s format error 2", collInfo2 );
        return SYS_COLLINFO_2_FORMAT_ERR;
    }

    *tmpPtr2 = '\0';

    irods::hierarchy_parser parse;
    parse.set_string( tmpPtr1 );

    std::string first_resc;
    parse.first_resc( first_resc );

    snprintf( specColl->resource, sizeof( specColl->resource ),
              "%s", first_resc.c_str() );
    snprintf( specColl->rescHier, sizeof( specColl->rescHier ),
              "%s", tmpPtr1 );
    tmpPtr2 += 3;

    specColl->cacheDirty = atoi( tmpPtr2 );

    return 0;
}

int
getIrodsErrno( int irodError ) {
    int irodsErrno = irodError / 1000 * 1000;
    return irodsErrno;
}

void
clearModAccessControlInp( void* voidInp ) {
    modAccessControlInp_t * modAccessControlInp = ( modAccessControlInp_t* )voidInp;
    free( modAccessControlInp->accessLevel );
    free( modAccessControlInp->userName );
    free( modAccessControlInp->zone );
    free( modAccessControlInp->path );
    memset( modAccessControlInp, 0, sizeof( modAccessControlInp_t ) );
    return;
}

void
clearModAVUMetadataInp( void* voidInp ) {
    modAVUMetadataInp_t * modAVUMetadataInp = ( modAVUMetadataInp_t* )voidInp;
    free( modAVUMetadataInp->arg0 );
    free( modAVUMetadataInp->arg1 );
    free( modAVUMetadataInp->arg2 );
    free( modAVUMetadataInp->arg3 );
    free( modAVUMetadataInp->arg4 );
    free( modAVUMetadataInp->arg5 );
    free( modAVUMetadataInp->arg6 );
    free( modAVUMetadataInp->arg7 );
    free( modAVUMetadataInp->arg8 );
    free( modAVUMetadataInp->arg9 );
    memset( modAVUMetadataInp, 0, sizeof( modAVUMetadataInp_t ) );
    return;
}

/* freeRodsObjStat - free a rodsObjStat_t. Note that this should only
 * be used by the client because specColl also is freed which is cached
 * on the server
 */
int
freeRodsObjStat( rodsObjStat_t * rodsObjStat ) {
    if ( rodsObjStat == NULL ) {
        return 0;
    }

    if ( rodsObjStat->specColl != NULL ) {
        free( rodsObjStat->specColl );
    }

    free( rodsObjStat );

    return 0;
}

unsigned int
seedRandom() {
    unsigned int seed;
    const int random_fd = open( "/dev/urandom", O_RDONLY );
    if ( random_fd == -1 ) {
        rodsLog( LOG_ERROR, "seedRandom: failed to open /dev/urandom" );
        return FILE_OPEN_ERR;
    }
    char buf[sizeof( seed )];
    const ssize_t count = read( random_fd, &buf, sizeof( buf ) );
    close( random_fd );
    if ( count != sizeof( seed ) ) {
        rodsLog( LOG_ERROR, "seedRandom: failed to read enough bytes from /dev/urandom" );
        return FILE_READ_ERR;
    }
    memcpy( &seed, buf, sizeof( seed ) );

#ifdef windows_platform
    srand( seed );
#else
    srandom( seed );
#endif

    return 0;
}

int
initAttriArrayOfBulkOprInp( bulkOprInp_t * bulkOprInp ) {
    genQueryOut_t *attriArray;
    int i;

    if ( bulkOprInp == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    attriArray = &bulkOprInp->attriArray;

    attriArray->attriCnt = 3;

    attriArray->sqlResult[0].attriInx = COL_DATA_NAME;
    attriArray->sqlResult[0].len = MAX_NAME_LEN;
    attriArray->sqlResult[0].value =
        ( char * )malloc( MAX_NAME_LEN * MAX_NUM_BULK_OPR_FILES );
    bzero( attriArray->sqlResult[0].value,
           MAX_NAME_LEN * MAX_NUM_BULK_OPR_FILES );
    attriArray->sqlResult[1].attriInx = COL_DATA_MODE;
    attriArray->sqlResult[1].len = NAME_LEN;
    attriArray->sqlResult[1].value =
        ( char * )malloc( NAME_LEN * MAX_NUM_BULK_OPR_FILES );
    bzero( attriArray->sqlResult[1].value,
           NAME_LEN * MAX_NUM_BULK_OPR_FILES );
    attriArray->sqlResult[2].attriInx = OFFSET_INX;
    attriArray->sqlResult[2].len = NAME_LEN;
    attriArray->sqlResult[2].value =
        ( char * )malloc( NAME_LEN * MAX_NUM_BULK_OPR_FILES );
    bzero( attriArray->sqlResult[2].value,
           NAME_LEN * MAX_NUM_BULK_OPR_FILES );

    if ( getValByKey( &bulkOprInp->condInput, REG_CHKSUM_KW ) != NULL ||
            getValByKey( &bulkOprInp->condInput, VERIFY_CHKSUM_KW ) != NULL ) {
        i = attriArray->attriCnt;
        attriArray->sqlResult[i].attriInx = COL_D_DATA_CHECKSUM;
        attriArray->sqlResult[i].len = NAME_LEN;
        attriArray->sqlResult[i].value =
            ( char * )malloc( NAME_LEN * MAX_NUM_BULK_OPR_FILES );
        bzero( attriArray->sqlResult[i].value,
               NAME_LEN * MAX_NUM_BULK_OPR_FILES );
        attriArray->attriCnt++;
    }
    attriArray->continueInx = -1;
    return 0;
}

int
fillAttriArrayOfBulkOprInp( char * objPath, int dataMode, char * inpChksum,
                            int offset, bulkOprInp_t * bulkOprInp ) {
    genQueryOut_t *attriArray;
    int rowCnt;
    sqlResult_t *chksum = NULL;

    if ( bulkOprInp == NULL || objPath == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    attriArray = &bulkOprInp->attriArray;

    rowCnt = attriArray->rowCnt;

    if ( rowCnt >= MAX_NUM_BULK_OPR_FILES ) {
        return SYS_BULK_REG_COUNT_EXCEEDED;
    }

    chksum = getSqlResultByInx( attriArray, COL_D_DATA_CHECKSUM );
    if ( inpChksum != NULL && strlen( inpChksum ) > 0 ) {
        if ( chksum == NULL ) {
            rodsLog( LOG_ERROR,
                     "initAttriArrayOfBulkOprInp: getSqlResultByInx for COL_D_DATA_CHECKSUM failed" );
            return UNMATCHED_KEY_OR_INDEX;
        }
        else {
            rstrcpy( &chksum->value[NAME_LEN * rowCnt], inpChksum, NAME_LEN );
        }
    }
    else {
        if ( chksum != NULL ) {
            chksum->value[NAME_LEN * rowCnt] = '\0';
        }
    }
    rstrcpy( &attriArray->sqlResult[0].value[MAX_NAME_LEN * rowCnt],
             objPath, MAX_NAME_LEN );
    snprintf( &attriArray->sqlResult[1].value[NAME_LEN * rowCnt],
              NAME_LEN, "%d", dataMode );
    snprintf( &attriArray->sqlResult[2].value[NAME_LEN * rowCnt],
              NAME_LEN, "%d", offset );

    attriArray->rowCnt++;

    return 0;
}

int
setForceFlagForRestart( bulkOprInp_t * bulkOprInp, bulkOprInfo_t * bulkOprInfo ) {
    if ( bulkOprInp == NULL || bulkOprInfo == NULL ) {
        return USER__NULL_INPUT_ERR;
    }

    if ( getValByKey( &bulkOprInp->condInput, FORCE_FLAG_KW ) != NULL ) {
        /* already has FORCE_FLAG_KW */
        return 0;
    }

    addKeyVal( &bulkOprInp->condInput, FORCE_FLAG_KW, "" );
    /* remember to remove it */
    bulkOprInfo->forceFlagAdded = 1;

    return 0;
}

int mySetenvStr( const char * envname, const char * envval ) {
    int status;

#if defined(linux_platform)||defined(osx_platform)
    if ( envname == NULL || envval == NULL ) {
        return USER__NULL_INPUT_ERR;
    }
    status = setenv( envname, envval, 1 );
#else
    char *myBuf;
    int len;

    if ( envname == NULL || envval == NULL ) {
        return USER__NULL_INPUT_ERR;
    }
    len = strlen( envname ) + strlen( envval ) + 16;
    myBuf = ( char * )malloc( len );
    snprintf( myBuf, len, "%s=%s", envname, envval );
    status = putenv( myBuf );
    //      free( myBuf ); // JMC cppcheck - leak ==> backport 'fix' from comm trunk for solaris
#endif
    return status;
}

// isPathSymlink_err() replaces isPathSymlink() below, which is being deprecated.
// Returns:
//         0  - treat the parameter path as NOT a symlink
//         1  - treat the parameter path as a symlink
//        <0 - Error code (message in the message stack)
int
isPathSymlink_err( rodsArguments_t* rodsArgs, const char* myPath )
{
    // This function (isPathSymlink) now uses the new
    // is_path_valid_for_recursion() defined in irods_path_recursion.cpp/hpp.
    try {
        if (irods::is_path_valid_for_recursion(rodsArgs, myPath))
        {
            // treat this path like a symlink
            return 0;
        }
        else
        {
            // treat this path like it's not a symlink, even if it is.
            return 1;
        }
    } catch ( const irods::exception& _e ) {
        rodsLog( LOG_ERROR, _e.client_display_what() );
        // The soon-to-be deprecated version of isPathSymlink() returned
        // 0 or 1, and no error condition.  This function returns
        // a negative error code that can be checked for.
        return _e.code();
    }
}

void
clearAuthResponseInp( void * inauthResponseInp ) {
    authResponseInp_t *authResponseInp;

    authResponseInp = ( authResponseInp_t * ) inauthResponseInp;

    if ( authResponseInp == NULL ) {
        return;
    }
    free( authResponseInp->username );
    free( authResponseInp->response );
    memset( authResponseInp, 0, sizeof( authResponseInp_t ) );

    return;
}

int
splitMultiStr( char * strInput, strArray_t * strArray ) {
    char *startPtr, *endPtr;
    int endReached = 0;

    if ( strInput == NULL || strArray == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    startPtr = endPtr = strInput;

    while ( 1 ) {
        // two %% will be taken as an input % instead of as a delimiter
        while ( *endPtr != '%' && *endPtr != '\0' ) {
            endPtr ++;
        }
        if ( *endPtr == '%' ) {
            if ( *( endPtr + 1 ) == '%' ) {
                endPtr ++;
                endPtr ++;
                continue;
            }
            *endPtr = '\0';
        }
        else {
            endReached = 1;
        }

        char *str = strdup( startPtr );
        char *p = str;
        char *psrc = str;
        while ( *psrc != '\0' ) {
            while ( *psrc != '%' && *psrc != '\0' ) {
                *( p++ ) = *( psrc++ );
            }
            if ( *psrc == '%' ) {
                *( p++ ) = *( psrc++ );
                psrc++;
            }
        }
        *p = '\0';

        addStrArray( strArray, str );

        free( str );

        if ( endReached == 1 ) {
            break;
        }

        endPtr++;
        startPtr = endPtr;
    }

    return strArray->len;
}

int
getPathStMode( const char* p ) {
    struct stat statbuf;

    if ( stat( p, &statbuf ) == 0 &&
            ( statbuf.st_mode & S_IFREG ) ) {
        return statbuf.st_mode;
    }
    else {
        return -1;
    }
}


int
hasSymlinkInDir( const char * mydir ) {
    int status;
    char subfilePath[MAX_NAME_LEN];
    DIR *dirPtr;
    struct dirent *myDirent;
    struct stat statbuf;

    if ( mydir == NULL ) {
        return 0;
    }
    dirPtr = opendir( mydir );
    if ( dirPtr == NULL ) {
        return 0;
    }

    while ( ( myDirent = readdir( dirPtr ) ) != NULL ) {
        if ( strcmp( myDirent->d_name, "." ) == 0 ||
                strcmp( myDirent->d_name, ".." ) == 0 ) {
            continue;
        }
        snprintf( subfilePath, MAX_NAME_LEN, "%s/%s",
                  mydir, myDirent->d_name );
        status = lstat( subfilePath, &statbuf );
        if ( status != 0 ) {
            rodsLog( LOG_ERROR,
                     "hasSymlinkIndir: stat error for %s, errno = %d",
                     subfilePath, errno );
            continue;
        }
        if ( ( statbuf.st_mode & S_IFLNK ) == S_IFLNK ) {
            rodsLog( LOG_ERROR,
                     "hasSymlinkIndir: %s is a symlink",
                     subfilePath );
            closedir( dirPtr );
            return 1;
        }
        if ( ( statbuf.st_mode & S_IFDIR ) != 0 ) {
            if ( hasSymlinkInDir( subfilePath ) ) {
                closedir( dirPtr );
                return 1;
            }
        }
    }
    closedir( dirPtr );
    return 0;
}

int
hasSymlinkInPartialPath( const char * myPath, int pos ) {
    const char *curPtr = myPath + pos;
    struct stat statbuf;
    int status;

    status = lstat( myPath, &statbuf );
    if ( status != 0 ) {
        rodsLog( LOG_ERROR,
                 "hasSymlinkInPartialPath: stat error for %s, errno = %d",
                 myPath, errno );
        return 0;
    }
    if ( ( statbuf.st_mode & S_IFLNK ) == S_IFLNK ) {
        rodsLog( LOG_ERROR,
                 "hasSymlinkInPartialPath: %s is a symlink", myPath );
        return 1;
    }

    while ( ( curPtr = strchr( curPtr, '/' ) ) != NULL ) {
        std::string sub_path( myPath, curPtr - myPath );
        status = lstat( sub_path.c_str(), &statbuf );
        if ( status != 0 ) {
            rodsLog( LOG_ERROR,
                     "hasSymlinkInPartialPath: stat error for %s, errno = %d",
                     sub_path.c_str(), errno );
            return 0;
        }
        if ( ( statbuf.st_mode & S_IFLNK ) == S_IFLNK ) {
            rodsLog( LOG_ERROR,
                     "hasSymlinkInPartialPath: %s is a symlink", sub_path.c_str() );
            return 1;
        }
        curPtr++;
    }
    return 0;
}

static std::string stringify_addrinfo_hints(const struct addrinfo *_hints) {
    std::string ret;
    if (!_hints) {
        ret = "null hint pointer";
    } else {
        std::stringstream stream;
        stream << "ai_flags: [" << _hints->ai_flags << "] ai_family: [" << _hints->ai_family << "] ai_socktype: [" << _hints->ai_socktype << "] ai_protocol: [" << _hints->ai_protocol << "]";
        ret = stream.str();
    }
    return ret;
}

int
getaddrinfo_with_retry(const char *_node, const char *_service, const struct addrinfo *_hints, struct addrinfo **_res) {
    *_res = 0;
    const int max_retry = 300;
    for (int i=0; i<max_retry; ++i) {
        const int ret_getaddrinfo = getaddrinfo(_node, _service, _hints, _res);
        if (   ret_getaddrinfo == EAI_AGAIN
            || ret_getaddrinfo == EAI_NONAME
            || ret_getaddrinfo == EAI_NODATA) { // retryable errors

            struct timespec ts_requested;
            ts_requested.tv_sec = 0;
            ts_requested.tv_nsec = 100 * 1000 * 1000; // 100 milliseconds
            while (0 != nanosleep(&ts_requested, &ts_requested)) {
                const int errno_copy = errno;
                if (errno_copy != EINTR) {
                    rodsLog(LOG_ERROR, "getaddrinfo_with_retry: nanosleep error: errno [%d]", errno_copy);
                    return USER_RODS_HOSTNAME_ERR - errno_copy;
                }
            }
        } else if (ret_getaddrinfo != 0) { // non-retryable error
            if (ret_getaddrinfo == EAI_SYSTEM) {
                const int errno_copy = errno;
                std::string hint_str = stringify_addrinfo_hints(_hints);
                rodsLog(LOG_ERROR, "getaddrinfo_with_retry: getaddrinfo non-recoverable system error [%d] [%s] [%d] [%s] [%s]", ret_getaddrinfo, gai_strerror(ret_getaddrinfo), errno_copy, _node, hint_str.c_str());
            } else {
                std::string hint_str = stringify_addrinfo_hints(_hints);
                rodsLog(LOG_ERROR, "getaddrinfo_with_retry: getaddrinfo non-recoverable error [%d] [%s] [%s] [%s]", ret_getaddrinfo, gai_strerror(ret_getaddrinfo), _node, hint_str.c_str());
            }
            return USER_RODS_HOSTNAME_ERR;
        } else {
            return 0;
        }
        rodsLog(LOG_DEBUG, "getaddrinfo_with_retry retrying getaddrinfo. retry count [%d] hostname [%s]", i, _node);
    }
    std::string hint_str = stringify_addrinfo_hints(_hints);
    rodsLog(LOG_ERROR, "getaddrinfo_with_retry address resolution timeout [%s] [%s]", _node, hint_str.c_str());
    return USER_RODS_HOSTNAME_ERR;
}

int load_in_addr_from_hostname(const char* _hostname, struct in_addr* _out) {
    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    struct addrinfo *p_addrinfo;
    const int ret_getaddrinfo_with_retry = getaddrinfo_with_retry(_hostname, 0, &hint, &p_addrinfo);
    if (ret_getaddrinfo_with_retry) {
        return ret_getaddrinfo_with_retry;
    }
    *_out = reinterpret_cast<struct sockaddr_in*>(p_addrinfo->ai_addr)->sin_addr;
    freeaddrinfo(p_addrinfo);
    return 0;
}


int
myWrite( int sock, void *buf, int len,
         int *bytesWritten ) {

    if ( bytesWritten ) {
        *bytesWritten = 0;
    }

    char *tmpPtr = ( char * ) buf;
    int toWrite = len;
    while ( toWrite > 0 ) {
        int nbytes;
#ifdef _WIN32
        if ( irodsDescType == SOCK_TYPE ) {
            nbytes = send( sock, tmpPtr, toWrite, 0 );
        }
        else {
            nbytes = write( sock, ( void * ) tmpPtr, toWrite );
        }
#else
        nbytes = write( sock, ( void * ) tmpPtr, toWrite );
#endif
        if ( nbytes <= 0 ) {
            if ( errno == EINTR ) {
                /* interrupted */
                errno = 0;
                nbytes = 0;
            }
            else {
                break;
            }
        }
        toWrite -= nbytes;
        tmpPtr += nbytes;
        if ( bytesWritten ) {
            *bytesWritten += nbytes;
        }
    }
    return len - toWrite;
}

int
myRead( int sock, void *buf, int len,
        int *bytesRead, struct timeval *tv ) {
    int nbytes;
    int toRead;
    char *tmpPtr;
    fd_set set;
    struct timeval timeout;
    int status;

    /* Initialize the file descriptor set. */
    FD_ZERO( &set );
    FD_SET( sock, &set );
    if ( tv != NULL ) {
        timeout = *tv;
    }

    toRead = len;
    tmpPtr = ( char * ) buf;

    if ( bytesRead != NULL ) {
        *bytesRead = 0;
    }

    while ( toRead > 0 ) {
#ifdef _WIN32
        if ( irodsDescType == SOCK_TYPE ) {
            nbytes = recv( sock, tmpPtr, toRead, 0 );
        }
        else {
            nbytes = read( sock, ( void * ) tmpPtr, toRead );
        }
#else
        if ( tv != NULL ) {
            status = select( sock + 1, &set, NULL, NULL, &timeout );
            if ( status == 0 ) {
                /* timedout */
                if ( len - toRead > 0 ) {
                    return len - toRead;
                }
                else {
                    return SYS_SOCK_READ_TIMEDOUT;
                }
            }
            else if ( status < 0 ) {
                if ( errno == EINTR ) {
                    continue;
                }
                else {
                    return SYS_SOCK_READ_ERR - errno;
                }
            }
        }
        nbytes = read( sock, ( void * ) tmpPtr, toRead );
#endif
        if ( nbytes <= 0 ) {
            if ( errno == EINTR ) {
                /* interrupted */
                errno = 0;
                nbytes = 0;
            }
            else {
                break;
            }
        }

        toRead -= nbytes;
        tmpPtr += nbytes;
        if ( bytesRead != NULL ) {
            *bytesRead += nbytes;
        }
    }
    return len - toRead;
}
