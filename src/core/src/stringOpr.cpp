/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* stringOpr - a number of string operations designed for secure string
 * copying.
 */

#include "stringOpr.h"
#include "rodsErrorTable.h"
#include "rodsLog.h"
#include <string>
#include "boost/regex.hpp"

#include "irods_stacktrace.hpp"

char *rstrcpy( char *dest, const char *src, int maxLen ) {
    // snprintf with logging on fail

    if ( dest == NULL || src == NULL ) {
        return NULL;
    }
    int status = snprintf( dest, maxLen, "%s", src );

    if ( status >= 0 && status < maxLen ) {
        return dest;
    }
    else if ( status >= 0 ) {
        rodsLog( LOG_ERROR,
                 "rstrcpy not enough space in dest, slen:%d, maxLen:%d, stacktrace:\n%s",
                 status, maxLen, irods::stacktrace().dump().c_str());
        rodsLog( LOG_DEBUG, "rstrcpy arguments dest [%s] src [%s]", dest, src );
        return NULL;
    }
    else {
        rodsLog( LOG_ERROR, "rstrcpy encountered an encoding error." );
        return NULL;
    }
}

int
isAllDigit( const char * myStr ) {
    char c;

    while ( ( c = *myStr ) != '\0' ) {
        if ( isdigit( c ) == 0 ) {
            return 0;
        }
        myStr++;
    }
    return 1;
}

int
splitPathByKey( const char * srcPath, char * dir, size_t maxDirLen,
                char * file, size_t maxFileLen, char key ) {
    if ( maxDirLen == 0 || maxFileLen == 0 ) {
        rodsLog( LOG_ERROR, "splitPathByKey called with buffers of size 0" );
        return SYS_INVALID_INPUT_PARAM;
    }

    const std::string srcPathString( srcPath );
    if ( srcPathString.size() == 0 ) {
        *dir = '\0';
        *file = '\0';
        return 0;
    }

    const size_t index_of_last_key = srcPathString.rfind( key );
    if ( std::string::npos == index_of_last_key ) {
        *dir = '\0';
        rstrcpy( file, srcPathString.c_str(), maxFileLen );
        return SYS_INVALID_FILE_PATH;
    }

    // If dir is the root directory, we want to return the single-character
    // string consisting of the key, NOT the empty string.
    const std::string dirPathString = srcPathString.substr( 0, std::max< size_t >( index_of_last_key, 1 ) );
    const std::string filePathString = srcPathString.substr( index_of_last_key + 1 ) ;

    if ( dirPathString.size() >= maxDirLen || filePathString.size() >= maxFileLen ) {
        rodsLog( LOG_ERROR, "splitPathByKey called with buffers of insufficient size" );
        return USER_STRLEN_TOOLONG;
    }

    rstrcpy( dir, dirPathString.c_str(), maxDirLen );
    rstrcpy( file, filePathString.c_str(), maxFileLen );

    return 0;

}
int
trimWS( char * s ) {
    char *t;

    t = s;
    while ( isspace( *t ) ) {
        t++;
    }
    if ( s != t ) {
        memmove( s, t, strlen( t ) + 1 );
    }
    t = s + strlen( s ) - 1;
    while ( isspace( *t ) ) {
        t--;
    }
    *( t + 1 ) = '\0';

    /*TODO Please return appropriate value*/
    return 0;
}

