/* -------------------------------------------------------------------------
 *
 * postgres_ext.h
 *
 * 	   This file contains declarations of things that are visible everywhere
 * 	in openGauss *and* are visible to clients of frontend interface libraries.
 * 	For example, the Oid type is part of the API of libpq and other libraries.
 *
 * 	   Declarations which are specific to a particular interface should
 * 	go in the header file for that interface (such as libpq-fe.h).	This
 * 	file is only for fundamental openGauss declarations.
 *
 * 	   User-written C functions don't count as "external to openGauss."
 * 	Those function much as local modifications to the backend itself, and
 * 	use header files that are otherwise internal to openGauss to interface
 * 	with the backend.
 *
 * src/include/postgres_ext.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef POSTGRES_EXT_H
#define POSTGRES_EXT_H

#include "gs_thread.h"
/*
 * Object ID is a fundamental type in openGauss.
 */
typedef unsigned int Oid;

#ifdef __cplusplus
#define InvalidOid (Oid(0))
#else
#define InvalidOid ((Oid)0)
#endif

#define CDB_ID (Oid(0))

#define VirtualBktOid (Oid(1))
#define VirtualSegmentOid (Oid(2))
#define VirtualDnBktOid (Oid(3))
#define InvalidBktId (-1)
#define InvalidBktGroupId (-1)
#define ExrtoReadSpecialLsn (-7)

/*
bucketid's range values and its specific meaning.
------------------------------------------------------
           -1: regular
[0,     1024): hash bucket, bucketgroup 0
[1024,  2048):
         1024: segment regular
         1025: segment global temp
         1026: segment unlogged
         1027: segment local temp
          ...: reserved
[2048,  6144): slice bucket: range/list
 [2048, 3071]: bucketgroup 1
 [3072, 4095]: bucketgroup 2
 [4096, 5119]: bucketgroup 3
 [5120, 6143]: bucketgroup 4
------------------------------------------------------*/

#define SegmentBktId BUCKETMAPLEN
#define SegmentTempBktId (1025)
#define SegmentUnloggedBktId (1026)
#define SegmentTemp2BktId (1027)
#define SegmentMaxBktId SegmentTemp2BktId

#define HashBktGroupId (0)

#define SliceBktLen 4096
#define SliceBktStartId (2048)
#define SliceBktEndId (SliceBktStartId + SliceBktLen - 1)
#define MAX_BUCKET_LEN (SliceBktEndId + 1)
#define MAX_BUCKETNODE_NUM (BUCKETMAPLEN + SliceBktLen)

#define MAX_SLICE_BUCKET_GROUP 4
#define MAX_BUCKET_GROUP (1 + MAX_SLICE_BUCKET_GROUP)

#define SLICEMAPLEN 16384 // max slice

/*
exrto segpage standby read will shift the bucketnode from 0 to 6200 to the left(-6400 to -200)
to ensure that buf_tag will not conflict. see smgr.h:is_standby_read_seg_relnode. page table uses
the 8500 to represent the standby read file, You must ensure that the two values do not conflict.

bucketnode                               segpage
────────────────────────────────────┬───────────────┬───────
                                    │0              │ 6200
             ┌──────────────────────┘               │
             │               ┌──────────────────────┘
             ▼               ▼
────────┬────┬───────────────┬──────────────────────────────
      -8500 -6400           -200
        standby read
*/
#define MAX_SEGMENT_BUCKETID 6200
#define MIN_SEGMENT_BUCKETID 0
#define EXRTO_STANDBY_READ_BUCKET_OFFSET 6400
/* keep codecheck quiet, min exrto standby read bucketid is: MIN_SEGMENT_BUCKETID - EXRTO_STANDBY_READ_BUCKET_OFFSET */
#define MIN_EXRTO_STANDBY_READ_BUCKETID (-(EXRTO_STANDBY_READ_BUCKET_OFFSET))
#define MAX_EXRTO_STANDBY_READ_BUCKETID (MAX_SEGMENT_BUCKETID - EXRTO_STANDBY_READ_BUCKET_OFFSET)
#define EXRTO_BLOCK_INFO_BUCKET_OFFSET -8500

#define ExrtoReadStartLSNOpt (1)
#define ExrtoReadEndLSNOpt (2)
#define ExrtoReadInitialLSNOpt (3)

#define BUCKET_OID_IS_VALID(bucket_oid) ((bucket_oid) == VirtualDnBktOid)
#define BUCKET_OID_IS_CN_BUCKETOID(bucket_oid) ((bucket_oid) == VirtualBktOid)
#define BUCKET_OID_IS_VALID_FOR_CN_ALLOC(bucket_oid) (BUCKET_OID_IS_VALID(bucket_oid) || (bucket_oid) == VirtualBktOid)
#define BUCKET_NODE_IS_EXRTO_READ(bucket_node)                                                             \
    ((MIN_EXRTO_STANDBY_READ_BUCKETID <= bucket_node && bucket_node <= MAX_EXRTO_STANDBY_READ_BUCKETID) || \
     bucket_node == ExrtoReadSpecialLsn)

#define SLICE_BUCKET_GROUP(bucketnode) ((bucketnode - SliceBktStartId) / BUCKETMAPLEN)

// hash bucket node
#define HASH_BUCKET_NODE_IS_VALID(bucketnode) ((bucketnode) > InvalidBktId && (bucketnode) < SegmentBktId)

// range bucket node
#define SLICE_BUCKET_NODE_IS_VALID(bucketnode) ((bucketnode) >= SliceBktStartId \
    && (bucketnode) <= SliceBktEndId)

// hash and range bucket node
#define BUCKET_NODE_IS_VALID(bucketnode) \
    (HASH_BUCKET_NODE_IS_VALID(bucketnode) || SLICE_BUCKET_NODE_IS_VALID(bucketnode))

// segment bucket node
#define SEG_BUCKET_NODE_IS_VALID(bucketnode) ( \
    ((bucketnode) >= SegmentBktId && bucketnode <= SegmentMaxBktId))

#define BUCKET_GROUP_FROM_BUCKETID(bucketid) (((bucketid) < BUCKETMAPLEN) ? \
    0 : (((bucketid) - SliceBktStartId) / BUCKETMAPLEN + 1))

#define BUCKET_LOCATION_IN_BUCKET_GROUP(bucketid) (((bucketid) < BUCKETMAPLEN) ? \
    (bucketid) : (((bucketid) - SliceBktStartId) % BUCKETMAPLEN))

#define BUCKETID_TO_GLOBAL_IDX(bucketid) (SLICE_BUCKET_NODE_IS_VALID(bucketid) ? \
    bucketid - SliceBktStartId + BUCKETMAPLEN : bucketid)

#define OID_MAX UINT_MAX

/* you will need to include <limits.h> to use the above #define */

/*
 * Identifiers of error message fields.  Kept here to keep common
 * between frontend and backend, and also to export them to libpq
 * applications.
 */
#define PG_DIAG_SEVERITY 'S'
#define PG_DIAG_SQLSTATE 'C'
#define PG_DIAG_INTERNEL_ERRCODE 'c'
#define PG_DIAG_MESSAGE_PRIMARY 'M'
#define PG_DIAG_MESSAGE_DETAIL 'D'
#define PG_DIAG_MESSAGE_HINT 'H'
#define PG_DIAG_STATEMENT_POSITION 'P'
#define PG_DIAG_INTERNAL_POSITION 'p'
#define PG_DIAG_INTERNAL_QUERY 'q'
#define PG_DIAG_CONTEXT 'W'
#define PG_DIAG_SOURCE_FILE 'F'
#define PG_DIAG_SOURCE_LINE 'L'
#define PG_DIAG_SOURCE_FUNCTION 'R'
#define PG_DIAG_MESSAGE_ONLY 'm'
#define PG_DIAG_MODULE_ID 'd'

#endif
