/* ---------------------------------------------------------------------------------------
 *
 * libpq-fe.h
 *	  This file contains definitions for structures and
 *	  externs for functions used by frontend openGauss applications.
 *
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 * Portions Copyright (c) 2010-2012 Postgres-XC Development Group
 *
 *
 *
 * IDENTIFICATION
 *        src/include/cm/libpq-fe.h
 *
 * ---------------------------------------------------------------------------------------
 */

#ifndef CM_LIBPQ_FE_H
#define CM_LIBPQ_FE_H

#include <stdio.h>
#include "cm_c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define libpq_gettext(x) x

/*
 * Option flags for PQcopyResult
 */
#define PG_COPYRES_ATTRS 0x01
#define PG_COPYRES_TUPLES 0x02 /* Implies PG_COPYRES_ATTRS */
#define PG_COPYRES_EVENTS 0x04
#define PG_COPYRES_NOTICEHOOKS 0x08

/* Application-visible enum types */

/*
 * Although it is okay to add to these lists, values which become unused
 * should never be removed, nor should constants be redefined - that would
 * break compatibility with existing code.
 */

typedef enum {
    CONNECTION_OK,
    CONNECTION_BAD,
    /* Non-blocking mode only below here */

    /*
     * The existence of these should never be relied upon - they should only
     * be used for user feedback or similar purposes.
     */
    CONNECTION_STARTED,           /* Waiting for connection to be made.  */
    CONNECTION_MADE,              /* Connection OK; waiting to send.	   */
    CONNECTION_AWAITING_RESPONSE, /* Waiting for a response from the
                                   * postmaster.		  */
    CONNECTION_AUTH_OK,           /* Received authentication; waiting for
                                   * backend startup. */
    CONNECTION_SETENV,            /* Negotiating environment. */
    CONNECTION_SSL_STARTUP,       /* Negotiating SSL. */
    CONNECTION_NEEDED             /* Internal state: connect() needed */
} CmConnStatusType;

typedef enum {
    PGRES_POLLING_FAILED = 0,
    PGRES_POLLING_READING, /* These two indicate that one may	  */
    PGRES_POLLING_WRITING, /* use select before polling again.   */
    PGRES_POLLING_OK,
    PGRES_POLLING_ACTIVE /* unused; keep for awhile for backwards
                          * compatibility */
} CmPostgresPollingStatusType;

/* ----------------
 * Structure for the conninfo parameter definitions returned by PQconndefaults
 * or CMPQconninfoParse.
 *
 * All fields except "val" point at static strings which must not be altered.
 * "val" is either NULL or a malloc'd current-value string.  cmpq_conninfo_free()
 * will release both the val strings and the CmpqConninfoOption array itself.
 * ----------------
 */
typedef struct CmpqConninfoOption {
    char* keyword; /* The keyword of the option			*/
    char* val;     /* Option's current value, or NULL		 */
} CmpqConninfoOption;

typedef struct CmConn CmConn;

/* ----------------
 * Exported functions of libpq
 * ----------------
 */

/* ===	in fe-connect.c === */

/* make a new client connection to the backend */
/* Asynchronous (non-blocking) */
extern CmConn* pq_connect_cm_start(const char* conninfo);
extern CmPostgresPollingStatusType cmpq_connect_poll(CmConn* conn);

/* Synchronous (blocking) */
extern CmConn* pq_connect_cm(const char* conninfo);

/* close the current connection and free the CmConn data structure */
extern void cmpq_finish(CmConn* conn);

/* free the data structure returned by PQconndefaults() or CMPQconninfoParse() */
extern void cmpq_conninfo_free(CmpqConninfoOption* connOptions);

extern CmConnStatusType cmpq_status(const CmConn* conn);
extern char* cmpq_error_message(const CmConn* conn);

/* Force the write buffer to be written (or at least try) */
extern int cmpq_flush(CmConn* conn);

extern int cmpq_packet_send(CmConn* conn, char packet_type, const void* buf, size_t buf_len);
extern char* gs_getenv_with_check(const char* envKey);

#ifdef __cplusplus
}
#endif

#endif /* CM_LIBPQ_FE_H */
