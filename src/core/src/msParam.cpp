/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* msParam.cpp - function for handling msParam_t.
 */

#include "msParam.h"
#include "apiHeaderAll.h"
#include "modDataObjMeta.h"
#include "rcGlobalExtern.h"

/* addMsParam - This is for backward compatibility only.
 *  addMsParamToArray should be used for all new functions
 */

int
addMsParam( msParamArray_t *msParamArray, const char *label,
            const char *type, void *inOutStruct, bytesBuf_t *inpOutBuf ) {
    return addMsParamToArray( msParamArray, label, type,
                                    inOutStruct, inpOutBuf, 0 );
}

/* addMsParamToArray - Add a msParam_t to the msParamArray.
 * Input char *label - an element of the msParam_t. This input must be
 *            non null.
 *       const char *type - can be NULL
 *       void *inOutStruct - can be NULL;
 *       bytesBuf_t *inpOutBuf - can be NULL
 *	 int replFlag - label and type will be automatically replicated
 *         (strdup). If replFlag == 0, only the pointers of inOutStruct
 *         and inpOutBuf will be passed. If replFlag == 1, the inOutStruct
 *         and inpOutBuf will be replicated.
 */

int
addMsParamToArray( msParamArray_t *msParamArray, const char *label,
                   const char *type, void *inOutStruct, bytesBuf_t *inpOutBuf, int replFlag ) {
    msParam_t **newParam;
    int len, newLen;

    if ( msParamArray == NULL || label == NULL ) {
        rodsLog( LOG_ERROR,
                 "addMsParam: NULL msParamArray or label input" );
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    len = msParamArray->len;

    for ( int i = 0; i < len; i++ ) {
        if ( msParamArray->msParam[i]->label == NULL ) {
            continue;
        }
        if ( strcmp( msParamArray->msParam[i]->label, label ) == 0 ) {
            if (!type || !msParamArray->msParam[i]->type) {
                rodsLog(LOG_ERROR, "[%s] - type is null for [%s]", __FUNCTION__, label);
                continue;
            }
            /***  Jan 28 2010 to make it not given an error ***/
            if ( !strcmp( msParamArray->msParam[i]->type, STR_MS_T ) &&
                    !strcmp( type, STR_MS_T ) &&
                    !strcmp( ( char * ) inOutStruct, ( char * ) msParamArray->msParam[i]->inOutStruct ) ) {
                return 0;
            }
            /***  Jan 28 2010 to make it not given an error ***/
            rodsLog( LOG_ERROR,
                     "addMsParam: Two params have the same label %s", label );
            if ( !strcmp( msParamArray->msParam[i]->type, STR_MS_T ) )
                rodsLog( LOG_ERROR,
                         "addMsParam: old string value = %s\n", ( char * ) msParamArray->msParam[i]->inOutStruct );
            else
                rodsLog( LOG_ERROR,
                         "addMsParam: old param is of type: %s\n", msParamArray->msParam[i]->type );
            if ( !strcmp( type, STR_MS_T ) )
                rodsLog( LOG_ERROR,
                         "addMsParam: new string value = %s\n", ( char * ) inOutStruct );
            else
                rodsLog( LOG_ERROR,
                         "addMsParam: new param is of type: %s\n", type );
            return USER_PARAM_LABEL_ERR;
        }
    }

    if ( ( msParamArray->len % PTR_ARRAY_MALLOC_LEN ) == 0 ) {
        newLen = msParamArray->len + PTR_ARRAY_MALLOC_LEN;
        newParam = ( msParam_t ** ) malloc( newLen * sizeof( *newParam ) );
        memset( newParam, 0, newLen * sizeof( *newParam ) );
        for ( int i = 0; i < len; i++ ) {
            newParam[i] = msParamArray->msParam[i];
        }
        if ( msParamArray->msParam != NULL ) {
            free( msParamArray->msParam );
        }
        msParamArray->msParam = newParam;
    }

    msParamArray->msParam[len] = ( msParam_t * ) malloc( sizeof( msParam_t ) );
    memset( msParamArray->msParam[len], 0, sizeof( msParam_t ) );
    if ( replFlag == 0 ) {
        fillMsParam( msParamArray->msParam[len], label, type, inOutStruct,
                     inpOutBuf );
    }
    else {
        int status = replInOutStruct( inOutStruct, &msParamArray->msParam[len]->inOutStruct, type );
        if ( status < 0 ) {
            rodsLogError( LOG_ERROR, status, "Error when calling replInOutStruct in %s", __PRETTY_FUNCTION__ );
            return status;
        }
        msParamArray->msParam[len]->label = label ? strdup(label) : NULL;
        msParamArray->msParam[len]->type = type ? strdup( type ) : NULL;
        msParamArray->msParam[len]->inpOutBuf = replBytesBuf(inpOutBuf);
    }
    msParamArray->len++;

    return 0;
}

int
replMsParam( msParam_t *in, msParam_t *out ) {
    if ( in == NULL  || out == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    int status = replInOutStruct(in->inOutStruct, &out->inOutStruct, in->type);
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status, "Error when calling replInOutStruct in %s", __PRETTY_FUNCTION__ );
        return status;
    }
    out->label = in->label ? strdup(in->label) : NULL;
    out->type = in->type ? strdup(in->type) : NULL;
    out->inpOutBuf = replBytesBuf(in->inpOutBuf);
    return 0;
}

int
replInOutStruct( void *inStruct, void **outStruct, const char *type ) {
    if (outStruct == NULL) {
        rodsLogError( LOG_ERROR, SYS_INTERNAL_NULL_INPUT_ERR, "replInOutStruct was called with a null pointer in outStruct");
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    if (inStruct == NULL) {
        *outStruct = NULL;
        return 0;
    }
    if (type == NULL) {
        rodsLogError( LOG_ERROR, SYS_INTERNAL_NULL_INPUT_ERR, "replInOutStruct was called with a null pointer in type");
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    if ( strcmp( type, STR_MS_T ) == 0 ) {
        *outStruct = strdup( ( char * )inStruct );
        return 0;
    }
    bytesBuf_t *packedResult;
    int status = packStruct( inStruct, &packedResult, type,
                            NULL, 0, NATIVE_PROT );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status, "replInOutStruct: packStruct error for type %s", type );
        return status;
    }
    status = unpackStruct( packedResult->buf,
                            outStruct, type, NULL, NATIVE_PROT );
    freeBBuf( packedResult );
    if ( status < 0 ) {
        rodsLogError( LOG_ERROR, status, "replInOutStruct: unpackStruct error for type %s", type );
        return status;
    }
    return 0;
}

bytesBuf_t*
replBytesBuf( const bytesBuf_t* in) {
    if (in == NULL || in->len == 0) {
        return NULL;
    }
    bytesBuf_t* out = (bytesBuf_t*)malloc(sizeof(bytesBuf_t));
    out->len = in->len;
    //TODO: this is horrible. check if it is necessary
    out->buf = malloc( out->len + 100 );
    memcpy( out->buf, in->buf, out->len );
    return out;
}


int
fillMsParam( msParam_t *msParam, const char *label,
             const char *type, void *inOutStruct, bytesBuf_t *inpOutBuf ) {
    if ( label != NULL ) {
        msParam->label = strdup( label );
    }

    msParam->type = type ? strdup( type ) : NULL;
    if ( inOutStruct != NULL && msParam->type != NULL &&
            strcmp( msParam->type, STR_MS_T ) == 0 ) {
        msParam->inOutStruct = ( void * ) strdup( ( char * )inOutStruct );
    }
    else {
        msParam->inOutStruct = inOutStruct;
    }
    msParam->inpOutBuf = inpOutBuf;

    return 0;
}

void
fillStrInMsParam( msParam_t *msParam, const char *str ) {
    if ( msParam != NULL ) {
        msParam->inOutStruct = str ? strdup( str ) : NULL;
        msParam->type = strdup( STR_MS_T );
    }
}

int
writeMsParam( char *buf, int len, msParam_t *msParam ) {
    int j;
    keyValPair_t *kVPairs;
    tagStruct_t *tagValues;

    buf[0] = '\0';


    if ( msParam->label != NULL &&
            msParam->type != NULL &&
            msParam->inOutStruct != NULL ) {
        if ( strcmp( msParam->type, STR_MS_T ) == 0 ) {
            snprintf( &buf[strlen( buf )], len - strlen( buf ), "%s: %s\n", msParam->label, ( char * ) msParam->inOutStruct );
        }
        else  if ( strcmp( msParam->type, INT_MS_T ) == 0 ) {
            snprintf( &buf[strlen( buf )], len - strlen( buf ), "%s: %i\n", msParam->label, *( int * ) msParam->inOutStruct );
        }
        else if ( strcmp( msParam->type, KeyValPair_MS_T ) == 0 ) {
            kVPairs = ( keyValPair_t * )msParam->inOutStruct;
            snprintf( &buf[strlen( buf )], len - strlen( buf ), "KVpairs %s: %i\n", msParam->label, kVPairs->len );
            for ( j = 0; j < kVPairs->len; j++ ) {
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "       %s = %s\n", kVPairs->keyWord[j],
                          kVPairs->value[j] );
            }
        }
        else if ( strcmp( msParam->type, TagStruct_MS_T ) == 0 ) {
            tagValues = ( tagStruct_t * ) msParam->inOutStruct;
            snprintf( &buf[strlen( buf )], len - strlen( buf ), "Tags %s: %i\n", msParam->label, tagValues->len );
            for ( j = 0; j < tagValues->len; j++ ) {
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "       AttName = %s\n", tagValues->keyWord[j] );
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "       PreTag  = %s\n", tagValues->preTag[j] );
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "       PostTag = %s\n", tagValues->postTag[j] );
            }
        }
        else if ( strcmp( msParam->type, ExecCmdOut_MS_T ) == 0 ) {
            execCmdOut_t *execCmdOut;
            execCmdOut = ( execCmdOut_t * ) msParam->inOutStruct;
            if ( execCmdOut->stdoutBuf.buf != NULL ) {
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "STDOUT = %s", ( char * ) execCmdOut->stdoutBuf.buf );
            }
            if ( execCmdOut->stderrBuf.buf != NULL ) {
                snprintf( &buf[strlen( buf )], len - strlen( buf ), "STRERR = %s", ( char * ) execCmdOut->stderrBuf.buf );
            }
        }
    }

    if ( msParam->inpOutBuf != NULL ) {
        snprintf( &buf[strlen( buf )], len - strlen( buf ), "    outBuf: buf length = %d\n", msParam->inpOutBuf->len );
    }

    return 0;
}


msParam_t *
getMsParamByLabel( msParamArray_t *msParamArray, const char *label ) {
    int i;

    if ( msParamArray == NULL || msParamArray->msParam == NULL || label == NULL ) {
        return NULL;
    }

    for ( i = 0; i < msParamArray->len; i++ ) {
        if ( strcmp( msParamArray->msParam[i]->label, label ) == 0 ) {
            return msParamArray->msParam[i];
        }
    }
    return NULL;
}

int
clearMsParamArray( msParamArray_t *msParamArray, int freeStruct ) {
    int i;

    if ( msParamArray == NULL ) {
        return 0;
    }

    for ( i = 0; i < msParamArray->len; i++ ) {
        clearMsParam( msParamArray->msParam[i], freeStruct );
        free( msParamArray->msParam[i] );
    }

    if ( msParamArray->len > 0 && msParamArray->msParam != NULL ) {
        free( msParamArray->msParam );
        memset( msParamArray, 0, sizeof( msParamArray_t ) );
    }

    return 0;
}

int
clearMsParam( msParam_t *msParam, int freeStruct ) {
    if ( msParam == NULL ) {
        return 0;
    }

    if ( msParam->label != NULL ) {
        free( msParam->label );
    }
    if ( msParam->inOutStruct != NULL && ( freeStruct > 0 ||
                                           ( msParam->type != NULL && strcmp( msParam->type, STR_MS_T ) == 0 ) ) ) {
        free( msParam->inOutStruct );
    }
    if ( msParam->type != NULL ) {
        free( msParam->type );
    }

    memset( msParam, 0, sizeof( msParam_t ) );
    return 0;
}

char *
parseMspForStr( msParam_t * inpParam ) {
    if ( inpParam == NULL || inpParam->inOutStruct == NULL ) {
        return NULL;
    }

    if ( strcmp( inpParam->type, STR_MS_T ) != 0 ) {
        rodsLog( LOG_ERROR,
                 "parseMspForStr: inpParam type %s is not STR_MS_T",
                 inpParam->type );
        return NULL;
    }

    if ( strcmp( ( char * ) inpParam->inOutStruct, "null" ) == 0 ) {
        return NULL;
    }

    return ( char * )( inpParam->inOutStruct );
}

int
initParsedMsKeyValStr( char * inpStr, parsedMsKeyValStr_t * parsedMsKeyValStr ) {
    if ( inpStr == NULL || parsedMsKeyValStr == NULL ) {
        rodsLog( LOG_ERROR,
                 "initParsedMsKeyValStr: input inpStr or parsedMsKeyValStr is NULL" );
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }

    bzero( parsedMsKeyValStr, sizeof( parsedMsKeyValStr_t ) );
    parsedMsKeyValStr->inpStr = parsedMsKeyValStr->curPtr = strdup( inpStr );
    parsedMsKeyValStr->endPtr = parsedMsKeyValStr->inpStr +
                                strlen( parsedMsKeyValStr->inpStr );

    return 0;
}

int
clearParsedMsKeyValStr( parsedMsKeyValStr_t * parsedMsKeyValStr ) {
    if ( parsedMsKeyValStr == NULL ) {
        rodsLog( LOG_ERROR,
                 "clearParsedMsKeyValStr: input parsedMsKeyValStr is NULL" );
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    if ( parsedMsKeyValStr->inpStr != NULL ) {
        free( parsedMsKeyValStr->inpStr );
    }

    bzero( parsedMsKeyValStr, sizeof( parsedMsKeyValStr_t ) );

    return 0;
}

/* getNextKeyValFromMsKeyValStr - parse the inpStr for keyWd value pair.
  * The str is expected to have the format keyWd=value separated by
  * 4 "+" char. e.g. keyWd1=value1++++keyWd2=value2++++keyWd3=value3...
  * If the char "=" is not present, the entire string is assumed to
  * be value with a NULL value for kwPtr.
  */
int
getNextKeyValFromMsKeyValStr( parsedMsKeyValStr_t * parsedMsKeyValStr ) {
    char *tmpEndPtr;
    char *equalPtr;

    if ( parsedMsKeyValStr == NULL ) {
        rodsLog( LOG_ERROR,
                 "getNextKeyValFromMsKeyValStr: input parsedMsKeyValStr is NULL" );
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    if ( parsedMsKeyValStr->curPtr >= parsedMsKeyValStr->endPtr ) {
        return NO_MORE_RESULT;
    }

    if ( ( tmpEndPtr = strstr( parsedMsKeyValStr->curPtr, MS_INP_SEP_STR ) ) !=
            NULL ) {
        /* NULL terminate the str we are trying to parse */
        *tmpEndPtr = '\0';
    }
    else {
        tmpEndPtr = parsedMsKeyValStr->endPtr;
    }

    if ( strcmp( parsedMsKeyValStr->curPtr, MS_NULL_STR ) == 0 ) {
        return NO_MORE_RESULT;
    }

    if ( ( equalPtr = strstr( parsedMsKeyValStr->curPtr, "=" ) ) != NULL ) {
        *equalPtr = '\0';
        parsedMsKeyValStr->kwPtr = parsedMsKeyValStr->curPtr;
        if ( equalPtr + 1 == tmpEndPtr ) {
            /* pair has no value */
            parsedMsKeyValStr->valPtr = equalPtr;
        }
        else {
            parsedMsKeyValStr->valPtr = equalPtr + 1;
        }
    }
    else {
        parsedMsKeyValStr->kwPtr = NULL;
        parsedMsKeyValStr->valPtr = parsedMsKeyValStr->curPtr;
    }

    /* advance curPtr */
    parsedMsKeyValStr->curPtr = tmpEndPtr + strlen( MS_INP_SEP_STR );

    return 0;
}

int
chkDataObjInpKw( char * keyWd, int validKwFlags ) {
    int i;

    if ( keyWd == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    for ( i = 0; i < NumDataObjInpKeyWd; i++ ) {
        if ( strcmp( DataObjInpKeyWd[i].keyWd, keyWd ) == 0 ) {
            if ( ( DataObjInpKeyWd[i].flag & validKwFlags ) == 0 ) {
                /* not valid */
                break;
            }
            else {
                /* OK */
                return DataObjInpKeyWd[i].flag;
            }
        }
    }
    return USER_BAD_KEYWORD_ERR;
}

int
chkCollInpKw( char * keyWd, int validKwFlags ) {
    int i;

    if ( keyWd == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    for ( i = 0; i < NumCollInpKeyWd; i++ ) {
        if ( strcmp( CollInpKeyWd[i].keyWd, keyWd ) == 0 ) {
            if ( ( CollInpKeyWd[i].flag & validKwFlags ) == 0 ) {
                /* not valid */
                break;
            }
            else {
                /* OK */
                return CollInpKeyWd[i].flag;
            }
        }
    }
    return USER_BAD_KEYWORD_ERR;
}

int
chkStructFileExtAndRegInpKw( char * keyWd, int validKwFlags ) {
    int i;

    if ( keyWd == NULL ) {
        return SYS_INTERNAL_NULL_INPUT_ERR;
    }
    for ( i = 0; i < NumStructFileExtAndRegInpKeyWd; i++ ) {
        if ( strcmp( StructFileExtAndRegInpKeyWd[i].keyWd, keyWd ) == 0 ) {
            if ( ( StructFileExtAndRegInpKeyWd[i].flag & validKwFlags )
                    == 0 ) {
                /* not valid */
                break;
            }
            else {
                /* OK */
                return StructFileExtAndRegInpKeyWd[i].flag;
            }
        }
    }
    return USER_BAD_KEYWORD_ERR;
}

