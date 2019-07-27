/**
 * @file  rcGenQuery.cpp
 *
 */
/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* See genQuery.h for a description of this API call.*/

#include "genQuery.h"
#include "procApiRequest.h"
#include "apiNumber.h"

/**
 * \fn rcGenQuery (rcComm_t *conn, genQueryInp_t *genQueryInp, genQueryOut_t **genQueryOut)
 *
 * \brief Perform a general-query.
 *
 * \user client and server (internal queries to the ICAT as part of server ops)
 *
 * \ingroup metadata
 *
 * \since .5
 *
 *
 * \remark
 * Perform a general-query:
 * \n This is used extensively from within server code and from clients.
 * \n Provides a simplified interface to query the iRODS database (ICAT).
 * \n Although the inputs and controls are a bit complicated, it allows
 * \n SQL-like queries but without the caller needing to know the structure
 * \n of the database (schema).  SQL is generated for each call on the
 * \n server-side (in the ICAT code).
 *
 * \note none
 *
 * \usage
 *
 * \param[in] conn - A rcComm_t connection handle to the server
 * \param[in] genQueryInp - input general-query structure
 * \param[out] genQueryOut - output general-query structure
 * \return integer
 * \retval 0 on success
 *
 * \sideeffect none
 * \pre none
 * \post none
 * \sa none
**/

int
rcGenQuery( rcComm_t *conn, genQueryInp_t *genQueryInp,
            genQueryOut_t **genQueryOut ) {
    int status;
    status = procApiRequest( conn, GEN_QUERY_AN,  genQueryInp, NULL,
                             ( void ** )genQueryOut, NULL );
    return status;
}

