/* ---------------------------------------------------------------------------------------
 *
 * cm_msg.h
 *
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 * Portions Copyright (c) 2010-2012 Postgres-XC Development Group
 *
 * IDENTIFICATION
 *        src/include/cm/cm_msg.h
 *
 * ---------------------------------------------------------------------------------------
 */
#ifndef CM_MSG_H
#define CM_MSG_H

#include <pthread.h>
#include "cm_defs.h"
#include "replication/replicainternal.h"
#include "access/xlogdefs.h"
#include "access/redo_statistic_msg.h"
#include "common/config/cm_config.h"
#include <vector>
#include <string>

#define CM_MAX_SENDER_NUM 2
#define LOGIC_CLUSTER_NUMBER (32 + 1)  // max 32 logic + 1 elastic group
#define CM_LOGIC_CLUSTER_NAME_LEN 64
#define CM_MSG_ERR_INFORMATION_LENGTH 1024
#ifndef MAX_INT32
#define MAX_INT32 (2147483600)
#endif
#define CN_INFO_NUM 8
#define RESERVE_NUM 160
#define RESERVE_NUM_USED 4
#define MAX_SYNC_STANDBY_LIST 1024
#define REMAIN_LEN 20

typedef struct timespec cmTime_t;

using std::string;
using std::vector;

const uint32 g_barrier_slot_version = 92380;
const uint32 g_hadr_key_cn = 92381;

const int32 FAILED_SYNC_DATA = 0;
const int32 SUCCESS_SYNC_DATA = 1;

/*
 * Symbols in the following enum are usd in cluster_msg_map_string defined in cm_misc.cpp.
 * Modifictaion to the following enum should be reflected to cluster_msg_map_string as well.
 */
typedef enum CmMessageType {
    MSG_CTL_CM_SWITCHOVER = 0,
    MSG_CTL_CM_BUILD = 1,
    MSG_CTL_CM_SYNC = 2,
    MSG_CTL_CM_QUERY = 3,
    MSG_CTL_CM_NOTIFY = 4,
    MSG_CTL_CM_BUTT = 5,
    MSG_CM_CTL_DATA_BEGIN = 6,
    MSG_CM_CTL_DATA = 7,
    MSG_CM_CTL_NODE_END = 8,
    MSG_CM_CTL_DATA_END = 9,
    MSG_CM_CTL_COMMAND_ACK = 10,

    MSG_CM_AGENT_SWITCHOVER = 11,
    MSG_CM_AGENT_FAILOVER = 12,
    MSG_CM_AGENT_BUILD = 13,
    MSG_CM_AGENT_SYNC = 14,
    MSG_CM_AGENT_NOTIFY = 15,
    MSG_CM_AGENT_NOTIFY_CN = 16,
    MSG_AGENT_CM_NOTIFY_CN_FEEDBACK = 17,
    MSG_CM_AGENT_CANCEL_SESSION = 18,
    MSG_CM_AGENT_RESTART = 19,
    MSG_CM_AGENT_RESTART_BY_MODE = 20,
    MSG_CM_AGENT_REP_SYNC = 21,
    MSG_CM_AGENT_REP_ASYNC = 22,
    MSG_CM_AGENT_REP_MOST_AVAILABLE = 23,
    MSG_CM_AGENT_BUTT = 24,

    MSG_AGENT_CM_DATA_INSTANCE_REPORT_STATUS = 25,
    MSG_AGENT_CM_COORDINATE_INSTANCE_STATUS = 26,
    MSG_AGENT_CM_GTM_INSTANCE_STATUS = 27,
    MSG_AGENT_CM_BUTT = 28,

    /****************  =====CAUTION=====  ****************:
    If you want to add a new MessageType, you should add at the end ,
    It's forbidden to insert new MessageType at middle,  it will change the other MessageType value.
    The MessageType is transfered between cm_agent and cm_server on different host,
    You should ensure the type value be identical and compatible between old and new versions */

    MSG_CM_CM_VOTE = 29,
    MSG_CM_CM_BROADCAST = 30,
    MSG_CM_CM_NOTIFY = 31,
    MSG_CM_CM_SWITCHOVER = 32,
    MSG_CM_CM_FAILOVER = 33,
    MSG_CM_CM_SYNC = 34,
    MSG_CM_CM_SWITCHOVER_ACK = 35,
    MSG_CM_CM_FAILOVER_ACK = 36,
    MSG_CM_CM_ROLE_CHANGE_NOTIFY = 37,
    MSG_CM_CM_REPORT_SYNC = 38,

    MSG_AGENT_CM_HEARTBEAT = 39,
    MSG_CM_AGENT_HEARTBEAT = 40,
    MSG_CTL_CM_SET = 41,
    MSG_CTL_CM_SWITCHOVER_ALL = 42,
    MSG_CM_CTL_SWITCHOVER_ALL_ACK = 43,
    MSG_CTL_CM_BALANCE_CHECK = 44,
    MSG_CM_CTL_BALANCE_CHECK_ACK = 45,
    MSG_CTL_CM_BALANCE_RESULT = 46,
    MSG_CM_CTL_BALANCE_RESULT_ACK = 47,
    MSG_CTL_CM_QUERY_CMSERVER = 48,
    MSG_CM_CTL_CMSERVER = 49,

    MSG_TYPE_BUTT = 50,
    MSG_CM_AGENT_NOTIFY_CN_CENTRAL_NODE = 51,
    MSG_CM_AGENT_DROP_CN = 52,
    MSG_CM_AGENT_DROPPED_CN = 53,
    MSG_AGENT_CM_FENCED_UDF_INSTANCE_STATUS = 54,
    MSG_CTL_CM_SWITCHOVER_FULL = 55,             /* inform cm agent to do switchover -A */
    MSG_CM_CTL_SWITCHOVER_FULL_ACK = 56,         /* inform cm ctl that cm server is doing swtichover -A */
    MSG_CM_CTL_SWITCHOVER_FULL_DENIED = 57,      /* inform cm ctl that switchover -A is denied by cm server */
    MSG_CTL_CM_SWITCHOVER_FULL_CHECK = 58,       /* cm ctl inform cm server to check if swtichover -A is done */
    MSG_CM_CTL_SWITCHOVER_FULL_CHECK_ACK = 59,   /* inform cm ctl that swtichover -A is done */
    MSG_CTL_CM_SWITCHOVER_FULL_TIMEOUT = 60,     /* cm ctl inform cm server to swtichover -A timed out */
    MSG_CM_CTL_SWITCHOVER_FULL_TIMEOUT_ACK = 61, /* inform cm ctl that swtichover -A stopped */

    MSG_CTL_CM_SETMODE = 62, /* new mode */
    MSG_CM_CTL_SETMODE_ACK = 63,

    MSG_CTL_CM_SWITCHOVER_AZ = 64,             /* inform cm agent to do switchover -zazName */
    MSG_CM_CTL_SWITCHOVER_AZ_ACK = 65,         /* inform cm ctl that cm server is doing swtichover -zazName */
    MSG_CM_CTL_SWITCHOVER_AZ_DENIED = 66,      /* inform cm ctl that switchover -zazName is denied by cm server */
    MSG_CTL_CM_SWITCHOVER_AZ_CHECK = 67,       /* cm ctl inform cm server to check if swtichover -zazName is done */
    MSG_CM_CTL_SWITCHOVER_AZ_CHECK_ACK = 68,   /* inform cm ctl that swtichover -zazName is done */
    MSG_CTL_CM_SWITCHOVER_AZ_TIMEOUT = 69,     /* cm ctl inform cm server to swtichover -zazName timed out */
    MSG_CM_CTL_SWITCHOVER_AZ_TIMEOUT_ACK = 70, /* inform cm ctl that swtichover -zazName stopped */

    MSG_CM_CTL_SET_ACK = 71,
    MSG_CTL_CM_GET = 72,
    MSG_CM_CTL_GET_ACK = 73,
    MSG_CM_AGENT_GS_GUC = 74,
    MSG_AGENT_CM_GS_GUC_ACK = 75,
    MSG_CM_CTL_SWITCHOVER_INCOMPLETE_ACK = 76,
    MSG_CM_CM_TIMELINE = 77, /* when restart cluster , cmserver primary and standy timeline */
    MSG_CM_BUILD_DOING = 78,
    MSG_AGENT_CM_ETCD_CURRENT_TIME = 79, /* etcd clock monitoring message */
    MSG_CM_QUERY_INSTANCE_STATUS = 80,
    MSG_CM_SERVER_TO_AGENT_CONN_CHECK = 81,
    MSG_CTL_CM_GET_DATANODE_RELATION = 82, /* depracated for the removal of quick switchover */
    MSG_CM_BUILD_DOWN = 83,
    MSG_CTL_CM_HOTPATCH = 84,
    MSG_CM_SERVER_REPAIR_CN_ACK = 85,
    MSG_CTL_CM_DISABLE_CN = 86,
    MSG_CTL_CM_DISABLE_CN_ACK = 87,
    MSG_CM_AGENT_LOCK_NO_PRIMARY = 88,
    MSG_CM_AGENT_LOCK_CHOSEN_PRIMARY = 89,
    MSG_CM_AGENT_UNLOCK = 90,
    MSG_CTL_CM_STOP_ARBITRATION = 91,
    MSG_CTL_CM_FINISH_REDO = 92,
    MSG_CM_CTL_FINISH_REDO_ACK = 93,
    MSG_CM_AGENT_FINISH_REDO = 94,
    MSG_CTL_CM_FINISH_REDO_CHECK = 95,
    MSG_CM_CTL_FINISH_REDO_CHECK_ACK = 96,
    MSG_AGENT_CM_KERBEROS_STATUS = 97,
    MSG_CTL_CM_QUERY_KERBEROS = 98,
    MSG_CTL_CM_QUERY_KERBEROS_ACK = 99,
    MSG_AGENT_CM_DISKUSAGE_STATUS = 100,
    MSG_CM_AGENT_OBS_DELETE_XLOG = 101,
    MSG_CM_AGENT_DROP_CN_OBS_XLOG = 102,
    MSG_AGENT_CM_DATANODE_INSTANCE_BARRIER = 103,
    MSG_AGENT_CM_COORDINATE_INSTANCE_BARRIER = 104,
    MSG_CTL_CM_GLOBAL_BARRIER_QUERY = 105,
    MSG_CM_CTL_GLOBAL_BARRIER_DATA = 106,
    MSG_CM_CTL_GLOBAL_BARRIER_DATA_BEGIN = 107,
    MSG_CM_CTL_BARRIER_DATA_END = 108,
    MSG_CM_CTL_BACKUP_OPEN = 109,
    MSG_CM_AGENT_DN_SYNC_LIST = 110,
    MSG_AGENT_CM_DN_SYNC_LIST = 111,
    MSG_CTL_CM_SWITCHOVER_FAST = 112,
    MSG_CM_AGENT_SWITCHOVER_FAST = 113,
    MSG_CTL_CM_RELOAD = 114,
    MSG_CM_CTL_RELOAD_ACK = 115,
    MSG_CM_CTL_INVALID_COMMAND_ACK = 116,
    MSG_AGENT_CM_CN_OBS_STATUS = 117,
    MSG_CM_AGENT_NOTIFY_CN_RECOVER = 118,
    MSG_CM_AGENT_FULL_BACKUP_CN_OBS = 119,
    MSG_AGENT_CM_BACKUP_STATUS_ACK = 120,
    MSG_CM_AGENT_REFRESH_OBS_DEL_TEXT = 121,
    MSG_AGENT_CM_INSTANCE_BARRIER_NEW = 122,
    MSG_CTL_CM_GLOBAL_BARRIER_QUERY_NEW = 123,
    MSG_CM_CTL_GLOBAL_BARRIER_DATA_BEGIN_NEW = 124,
    MSG_CM_AGENT_DATANODE_INSTANCE_BARRIER = 125,
    MSG_CM_AGENT_COORDINATE_INSTANCE_BARRIER = 126,
} CmMessageType;

#define UNDEFINED_LOCKMODE 0
#define POLLING_CONNECTION 1
#define SPECIFY_CONNECTION 2
#define PROHIBIT_CONNECTION 3

#define INSTANCE_ROLE_INIT 0
#define INSTANCE_ROLE_PRIMARY 1
#define INSTANCE_ROLE_STANDBY 2
#define INSTANCE_ROLE_PENDING 3
#define INSTANCE_ROLE_NORMAL 4
#define INSTANCE_ROLE_UNKNOWN 5
#define INSTANCE_ROLE_DUMMY_STANDBY 6
#define INSTANCE_ROLE_DELETED 7
#define INSTANCE_ROLE_DELETING 8
#define INSTANCE_ROLE_READONLY 9
#define INSTANCE_ROLE_OFFLINE 10
#define INSTANCE_ROLE_MAIN_STANDBY 11
#define INSTANCE_ROLE_CASCADE_STANDBY 12

#define INSTANCE_ROLE_FIRST_INIT 1
#define INSTANCE_ROLE_HAVE_INIT 2

#define INSTANCE_DATA_REPLICATION_SYNC 1
#define INSTANCE_DATA_REPLICATION_ASYNC 2
#define INSTANCE_DATA_REPLICATION_MOST_AVAILABLE 3
#define INSTANCE_DATA_REPLICATION_POTENTIAL_SYNC 4
#define INSTANCE_DATA_REPLICATION_QUORUM 5
#define INSTANCE_DATA_REPLICATION_UNKONWN 6

#define INSTANCE_TYPE_GTM 1
#define INSTANCE_TYPE_DATANODE 2
#define INSTANCE_TYPE_COORDINATE 3
#define INSTANCE_TYPE_FENCED_UDF 4
#define INSTANCE_TYPE_UNKNOWN 5

#define INSTANCE_WALSNDSTATE_STARTUP 0
#define INSTANCE_WALSNDSTATE_BACKUP 1
#define INSTANCE_WALSNDSTATE_CATCHUP 2
#define INSTANCE_WALSNDSTATE_STREAMING 3
#define INSTANCE_WALSNDSTATE_DUMPLOG 4
const int INSTANCE_WALSNDSTATE_NORMAL = 5;
const int INSTANCE_WALSNDSTATE_UNKNOWN = 6;

#define CON_OK 0
#define CON_BAD 1
#define CON_STARTED 2
#define CON_MADE 3
#define CON_AWAITING_RESPONSE 4
#define CON_AUTH_OK 5
#define CON_SETEN 6
#define CON_SSL_STARTUP 7
#define CON_NEEDED 8
#define CON_UNKNOWN 9
#define CON_MANUAL_STOPPED 10
#define CON_DISK_DEMAGED 11
#define CON_PORT_USED 12
#define CON_NIC_DOWN 13
#define CON_GTM_STARTING 14

#define CM_SERVER_UNKNOWN 0
#define CM_SERVER_PRIMARY 1
#define CM_SERVER_STANDBY 2
#define CM_SERVER_INIT 3
#define CM_SERVER_DOWN 4

#define CM_ETCD_UNKNOWN 0
#define CM_ETCD_FOLLOWER 1
#define CM_ETCD_LEADER 2
#define CM_ETCD_DOWN 3

#define SWITCHOVER_UNKNOWN 0
#define SWITCHOVER_FAIL 1
#define SWITCHOVER_SUCCESS 2
#define SWITCHOVER_EXECING 3
#define SWITCHOVER_PARTLY_SUCCESS 4
#define SWITCHOVER_ABNORMAL 5
#define INVALID_COMMAND 6


#define UNKNOWN_BAD_REASON 0
#define PORT_BAD_REASON 1
#define NIC_BAD_REASON 2
#define DISC_BAD_REASON 3
#define STOPPED_REASON 4
#define CN_DELETED_REASON 5

#define KERBEROS_STATUS_UNKNOWN 0
#define KERBEROS_STATUS_NORMAL 1
#define KERBEROS_STATUS_ABNORMAL 2
#define KERBEROS_STATUS_DOWN 3

#define HOST_LENGTH 32
#define BARRIERLEN 40
#define MAX_SLOT_NAME_LEN 64
#define MAX_BARRIER_SLOT_COUNT 5
// the length of cm_serer sync msg  is 9744, msg type is MSG_CM_CM_REPORT_SYNC
#define CM_MSG_MAX_LENGTH (12288 * 2)

#define CMAGENT_NO_CCN "NoCentralNode"

#define OBS_DEL_VERSION_V1 (1)
#define DEL_TEXT_HEADER_LEN_V1 (10)  // version(4->V%3d) + delCount(4->C%3d) + '\n' + '\0'
#define CN_BUILD_TASK_ID_MAX_LEN   (21)      // cnId(4) + cmsId(4) + time(12->yyMMddHH24mmss) + 1
#define MAX_OBS_CN_COUNT   (64)
#define MAX_OBS_DEL_TEXT_LEN  (CN_BUILD_TASK_ID_MAX_LEN * MAX_OBS_CN_COUNT + DEL_TEXT_HEADER_LEN_V1)

extern int g_gtm_phony_dead_times;
extern int g_dn_phony_dead_times[CM_MAX_DATANODE_PER_NODE];
extern int g_cn_phony_dead_times;

typedef enum {DN, CN} GetinstanceType;

typedef struct DatanodeSyncList {
    int count;
    uint32 dnSyncList[CM_PRIMARY_STANDBY_NUM];
    int syncStandbyNum;
    // remain
    int remain;
    char remainStr[DN_SYNC_LEN];
} DatanodeSyncList;

typedef struct CmMsgType {
    int msg_type;
} CmMsgType;

typedef struct CmSwitchoverIncompleteMsg {
    int msg_type;
    char errMsg[CM_MSG_ERR_INFORMATION_LENGTH];
} CmSwitchoverIncompleteMsg;

typedef struct CmRedoStats {
    int is_by_query;
    uint64 redo_replayed_speed;
    XLogRecPtr standby_last_replayed_read_Ptr;
} CmRedoStats;

typedef struct CtlToCmStopArbitration {
    int msg_type;
} CtlToCmStopArbitration;

typedef struct CtlToCmSwitchover {
    int msg_type;
    char azName[CM_AZ_NAME];
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CtlToCmSwitchover;

typedef struct CtlToCmFailover {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CtlToCmFailover;

typedef struct CtlToCmFinishRedoCheckAck {
    int msg_type;
    int finish_redo_count;
} CtlToCmFinishRedoCheckAck;

typedef struct CtlToCmFinishRedo {
    int msg_type;
} CtlToCmFinishRedo;

#define CM_CTL_UNFORCE_BUILD 0
#define CM_CTL_FORCE_BUILD 1

typedef struct CtlToCmBuild {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
    int force_build;
    int full_build;
} CtlToCmBuild;
typedef struct CtlToCmGlobalBarrierQuery {
    int msg_type;
}CtlToCmGlobalBarrierQuery;

typedef struct CtlToCmQuery {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
    int detail;
    int relation;
} CtlToCmQuery;

#define NOTIFY_MSG_RESERVED (32)

typedef struct Cm2AgentNotifyCnRecoverByObs {
    int msg_type;
    uint32 instanceId;
    bool changeKeyCn;
    uint32 syncCnId;
    char slotName[MAX_SLOT_NAME_LEN];
} Cm2AgentNotifyCnRecoverByObs;

typedef struct Cm2AgentBackupCn2Obs {
    int msg_type;
    uint32 instanceId;
    char slotName[MAX_SLOT_NAME_LEN];
    char taskIdStr[CN_BUILD_TASK_ID_MAX_LEN];
} Cm2AgentBackupCn2Obs;

typedef struct Agent2CmBackupStatusAck {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    char slotName[MAX_SLOT_NAME_LEN];
    char taskIdStr[CN_BUILD_TASK_ID_MAX_LEN];
    int32 status;
} Agent2CmBackupStatusAck;

typedef struct Cm2AgentRefreshObsDelText {
    int msg_type;
    uint32 instanceId;
    char slotName[MAX_SLOT_NAME_LEN];
    char obsDelCnText[MAX_OBS_DEL_TEXT_LEN];
} Cm2AgentRefreshObsDelText;

typedef struct CtlToCmNotify {
    CmMessageType msg_type;
    ctlToCmNotifyDetail detail;
} CtlToCmNotify;

typedef struct CtlToCmDisableCn {
    int msg_type;
    uint32 instanceId;
    int wait_seconds;
} CtlToCmDisableCn;

typedef struct CtlToCmDisableCnAck {
    int msg_type;
    bool disable_ok;
    char errMsg[CM_MSG_ERR_INFORMATION_LENGTH];
} CtlToCmDisableCnAck;

typedef enum ArbitrationMode {
    UNKNOWN_ARBITRATION = 0,
    MAJORITY_ARBITRATION = 1,
    MINORITY_ARBITRATION = 2
} ArbitrationMode;

typedef enum CmStartMode {
    UNKNOWN_START = 0,
    MAJORITY_START = 1,
    MINORITY_START = 2,
    OTHER_MINORITY_START = 3
} CmStartMode;

typedef enum SwitchoverAzMode {
    UNKNOWN_SWITCHOVER_AZ = 0,
    NON_AUTOSWITCHOVER_AZ = 1,
    AUTOSWITCHOVER_AZ = 2
} SwitchoverAzMode;

typedef enum LogicClusterRestartMode {
    UNKNOWN_LOGIC_CLUSTER_RESTART = 0,
    INITIAL_LOGIC_CLUSTER_RESTART = 1,
    MODIFY_LOGIC_CLUSTER_RESTART = 2
} LogicClusterRestartMode;

typedef enum ClusterMode {
    INVALID_CLUSTER_MODE = 0,
    ONE_MASTER_1_SLAVE,
    ONE_MASTER_2_SLAVE,
    ONE_MASTER_3_SLAVE,
    ONE_MASTER_4_SLAVE,
    ONE_MASTER_5_SLAVE
} ClusterMode;

typedef enum SynchronousStandbyMode {
    AnyFirstNo = 0, /* don't have */
    AnyAz1,         /* ANY 1(az1) */
    FirstAz1,       /* FIRST 1(az1) */
    AnyAz2,         /* ANY 1(az2) */
    FirstAz2,       /* FIRST 1(az2) */
    Any2Az1Az2,     /* ANY 2(az1,az2) */
    First2Az1Az2,   /* FIRST 2(az1,az2) */
    Any3Az1Az2,     /* ANY 3(az1, az2) */
    First3Az1Az2    /* FIRST 3(az1, az2) */
} SynchronousStandbyMode;

typedef enum {
    CLUSTER_PRIMARY = 0,
    CLUSTER_OBS_STANDBY = 1,
    CLUSTER_STREAMING_STANDBY = 2
} ClusterRole;

typedef enum {
    DISASTER_RECOVERY_NULL = 0,
    DISASTER_RECOVERY_OBS = 1,
    DISASTER_RECOVERY_STREAMING = 2
} DisasterRecoveryType;

typedef enum {
    INSTALL_TYPE_DEFAULT = 0,
    INSTALL_TYPE_SHARE_STORAGE = 1,
    INSTALL_TYPE_STREAMING = 2
} ClusterInstallType;

typedef struct CtlToCmSet {
    int msg_type;
    int log_level;
    uint32 logic_cluster_delay;
    ArbitrationMode cm_arbitration_mode;
    SwitchoverAzMode cm_switchover_az_mode;
    LogicClusterRestartMode cm_logic_cluster_restart_mode;
} CtlToCmSet, CtlToCmGet;

typedef struct CmToAgentSwitchover {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
    int role;
    uint32 term;
} CmToAgentSwitchover;

typedef struct CmToAgentFailover {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
    uint32 term;
} CmToAgentFailover;

typedef struct CmToAgentBuild {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
    int role;
    int full_build;
    uint32 term;
} CmToAgentBuild;

typedef struct CmToAgentLock1 {
    int msg_type;
    uint32 node;
    uint32 instanceId;
} CmToAgentLock1;

typedef struct CmToAgentObsDeleteXlog {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    uint64 lsn;
} CmToAgentObsDeleteXlog;

typedef struct CmToAgentLock2 {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    char disconn_host[CM_IP_LENGTH];
    uint32 disconn_port;
} CmToAgentLock2;

typedef struct CmToAgentUnlock {
    int msg_type;
    uint32 node;
    uint32 instanceId;
} CmToAgentUnlock;

typedef struct CmToAgentFinishRedo {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    bool is_finish_redo_cmd_sent;
} CmToAgentFinishRedo;

typedef struct CmToAgentGsGuc {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    SynchronousStandbyMode type;
} CmToAgentGsGuc;

typedef struct AgentToCmGsGucFeedback {
    int msg_type;
    uint32 node;
    uint32 instanceId; /* node of this agent */
    SynchronousStandbyMode type;
    bool status; /* gs guc command exec status */
} AgentToCmGsGucFeedback;

typedef struct CmToAgentGsGucSyncList {
    int msgType;
    uint32 node;
    uint32 instanceId;
    uint32 groupIndex;
    DatanodeSyncList dnSyncList;
    int instanceNum;
    // remain
    int remain[REMAIN_LEN];
} CmToAgentGsGucSyncList;

typedef struct CmToAgentNotify {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int role;
    uint32 term;
} CmToAgentNotify;

/*
 * msg struct using for cmserver to cmagent
 * including primaryed datanode count and datanode instanceId list.
 */
typedef struct CmToAgentNotifyCn {
    int msg_type;
    uint32 node;       /* node of this coordinator */
    uint32 instanceId; /* coordinator instance id */
    int datanodeCount; /* current count of datanode got primaryed */
    uint32 coordinatorId;
    int notifyCount;
    /* datanode instance id array */
    uint32 datanodeId[FLEXIBLE_ARRAY_MEMBER]; /* VARIABLE LENGTH ARRAY */
} CmToAgentNotifyCn;

/*
 * msg struct using for cmserver to cmagent
 * including primaryed datanode count and datanode instanceId list.
 */
typedef struct CmToAgentDropCn {
    int msg_type;
    uint32 node;       /* node of this coordinator */
    uint32 instanceId; /* coordinator instance id */
    uint32 coordinatorId;
    int role;
    bool delay_repair;
} CmToAgentDropCn;

typedef struct CmToAgentNotifyCnCentralNode {
    int msg_type;
    uint32 node;                 /* node of this coordinator */
    uint32 instanceId;           /* coordinator instance id */
    char cnodename[NAMEDATALEN]; /* central node id */
    char nodename[NAMEDATALEN];
} CmToAgentNotifyCnCentralNode;

/*
 * msg struct using for cmserver to cmagent
 * including primaryed datanode count and datanode instanceId list.
 */
typedef struct CmToAgentCancelSession {
    int msg_type;
    uint32 node;       /* node of this coordinator */
    uint32 instanceId; /* coordinator instance id */
} CmToAgentCancelSession;

/*
 * msg struct using for cmagent to cmserver
 * feedback msg for notify cn.
 */
typedef struct AgentToCmNotifyCnFeedback {
    int msg_type;
    uint32 node;       /* node of this coordinator */
    uint32 instanceId; /* coordinator instance id */
    bool status;       /* notify command exec status */
    int notifyCount;
} AgentToCmNotifyCnFeedback;

typedef struct BackupInfo {
    uint32 localKeyCnId;
    uint32 obsKeyCnId;
    char slotName[MAX_SLOT_NAME_LEN];
    char obsDelCnText[MAX_OBS_DEL_TEXT_LEN];
} BackupInfo;

typedef struct Agent2CmBackupInfoRep {
    int msg_type;
    uint32 instanceId; /* coordinator instance id */
    uint32 slotCount;
    BackupInfo backupInfos[MAX_BARRIER_SLOT_COUNT];
} Agent2CmBackupInfoRep;

typedef struct CmToAgentRestart {
    int msg_type;
    uint32 node;
    uint32 instanceId;
} CmToAgentRestart;

typedef struct CmToAgentRestartByMode {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int role_old;
    int role_new;
} CmToAgentRestartByMode;

typedef struct CmToAgentRepSync {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int sync_mode;
} CmToAgentRepSync;

typedef struct CmToAgentRepAsync {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int sync_mode;
} CmToAgentRepAsync;

typedef struct CmToAgentRepMostAvailable {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int sync_mode;
} CmToAgentRepMostAvailable;

typedef struct CmInstanceCentralNode {
    pthread_rwlock_t rw_lock;
    pthread_mutex_t mt_lock;
    uint32 instanceId;
    uint32 node;
    uint32 recover;
    uint32 isCentral;
    uint32 nodecount;
    char nodename[NAMEDATALEN];
    char cnodename[NAMEDATALEN];
    char* failnodes;
    CmToAgentNotifyCnCentralNode notify;
} CmInstanceCentralNode;

typedef struct CmInstanceCentralNodeMsg {
    pthread_rwlock_t rw_lock;
    CmToAgentNotifyCnCentralNode notify;
} CmInstanceCentralNodeMsg;

#define MAX_LENGTH_HP_CMD (9)
#define MAX_LENGTH_HP_PATH (256)
#define MAX_LENGTH_HP_RETURN_MSG (1024)

typedef struct CmHotpatchMsg {
    int msg_type;
    char command[MAX_LENGTH_HP_CMD];
    char path[MAX_LENGTH_HP_PATH];
} CmHotpatchMsg;

typedef struct CmHotpatchRetMsg {
    char msg[MAX_LENGTH_HP_RETURN_MSG];
} CmHotpatchRetMsg;

typedef struct CmToAgentBarrierInfo {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    char queryBarrier[BARRIERLEN];
    char targetBarrier[BARRIERLEN];
} CmToAgentBarrierInfo;

#define IP_LEN 64
#define MAX_REPL_CONNINFO_LEN 256
#define MAX_REBUILD_REASON_LEN 256

const int cn_active_unknown = 0;
const int cn_active = 1;
const int cn_inactive = 2;

#define INSTANCE_HA_STATE_UNKONWN 0
#define INSTANCE_HA_STATE_NORMAL 1
#define INSTANCE_HA_STATE_NEED_REPAIR 2
#define INSTANCE_HA_STATE_STARTING 3
#define INSTANCE_HA_STATE_WAITING 4
#define INSTANCE_HA_STATE_DEMOTING 5
#define INSTANCE_HA_STATE_PROMOTING 6
#define INSTANCE_HA_STATE_BUILDING 7
#define INSTANCE_HA_STATE_CATCH_UP 8
#define INSTANCE_HA_STATE_COREDUMP 9
#define INSTANCE_HA_STATE_MANUAL_STOPPED 10
#define INSTANCE_HA_STATE_DISK_DAMAGED 11
#define INSTANCE_HA_STATE_PORT_USED 12
#define INSTANCE_HA_STATE_BUILD_FAILED 13
#define INSTANCE_HA_STATE_HEARTBEAT_TIMEOUT 14
#define INSTANCE_HA_STATE_NIC_DOWN 15
#define INSTANCE_HA_STATE_READ_ONLY 16

#define INSTANCE_HA_DATANODE_BUILD_REASON_NORMAL 0
#define INSTANCE_HA_DATANODE_BUILD_REASON_WALSEGMENT_REMOVED 1
#define INSTANCE_HA_DATANODE_BUILD_REASON_DISCONNECT 2
#define INSTANCE_HA_DATANODE_BUILD_REASON_VERSION_NOT_MATCHED 3
#define INSTANCE_HA_DATANODE_BUILD_REASON_MODE_NOT_MATCHED 4
#define INSTANCE_HA_DATANODE_BUILD_REASON_SYSTEMID_NOT_MATCHED 5
#define INSTANCE_HA_DATANODE_BUILD_REASON_TIMELINE_NOT_MATCHED 6
#define INSTANCE_HA_DATANODE_BUILD_REASON_UNKNOWN 7
#define INSTANCE_HA_DATANODE_BUILD_REASON_USER_PASSWD_INVALID 8
#define INSTANCE_HA_DATANODE_BUILD_REASON_CONNECTING 9
#define INSTANCE_HA_DATANODE_BUILD_REASON_DCF_LOG_LOSS 10

#define UNKNOWN_LEVEL 0

typedef uint64 XLogRecPtr;

typedef enum CmDcfRole {
    DCF_ROLE_UNKNOWN = 0,
    DCF_ROLE_LEADER,
    DCF_ROLE_FOLLOWER,
    DCF_ROLE_PASSIVE,
    DCF_ROLE_LOGGER,
    DCF_ROLE_PRE_CANDIDATE,
    DCF_ROLE_CANDIDATE,
    DCF_ROLE_CEIL,
} DcfRole;

typedef enum {
    DCF_MAJORITY = 0,
    DCF_MINORITY = 1,
    DCF_CEIL
} DcfWorkMode;

typedef enum {
    CANDIDATE_STATUS_INIT     = 0,
    CANDIDATE_STATUS_NORMAL   = 1,
    CANDIDATE_STATUS_ISOLATED = 2
} CandidateState;

typedef struct AgentToCmCoordinateBarrierStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    uint64 ckpt_redo_point;
    char global_barrierId[BARRIERLEN];
    char global_achive_barrierId[BARRIERLEN];
    char barrierID [BARRIERLEN];
    char query_barrierId[BARRIERLEN];
    uint64 barrierLSN;
    uint64 archive_LSN;
    uint64 flush_LSN;
    bool is_barrier_exist;
} AgentToCmCoordinateBarrierStatusReport;

typedef struct GlobalBarrierItem {
    char slotname[MAX_SLOT_NAME_LEN];
    char globalBarrierId[BARRIERLEN];
    char globalAchiveBarrierId[BARRIERLEN];
} GlobalBarrierItem;

typedef struct GlobalBarrierStatus {
    int slotCount;
    GlobalBarrierItem globalBarriers[MAX_BARRIER_SLOT_COUNT];
} GlobalBarrierStatus;

typedef struct LocalBarrierStatus {
    uint64 ckptRedoPoint;
    uint64 barrierLSN;
    uint64 archiveLSN;
    uint64 flushLSN;
    char barrierID[BARRIERLEN];
} LocalBarrierStatus;

typedef struct Agent2CmBarrierStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    LocalBarrierStatus localStatus;
    GlobalBarrierStatus globalStatus;
} Agent2CmBarrierStatusReport;

typedef struct AgentToCmDatanodeBarrierStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    uint64 ckpt_redo_point;
    char barrierID [BARRIERLEN];
    uint64 barrierLSN;
    uint64 archive_LSN;
    uint64 flush_LSN;
} AgentToCmDatanodeBarrierStatusReport;

typedef struct CmLocalReplconninfo {
    int local_role;
    int static_connections;
    int db_state;
    XLogRecPtr last_flush_lsn;
    int buildReason;
    uint32 term;
    uint32 disconn_mode;
    char disconn_host[CM_IP_LENGTH];
    uint32 disconn_port;
    char local_host[CM_IP_LENGTH];
    uint32 local_port;
    bool redo_finished;
} CmLocalReplconninfo;

typedef struct CmSenderReplconninfo {
    pid_t sender_pid;
    int local_role;
    int peer_role;
    int peer_state;
    int state;
    XLogRecPtr sender_sent_location;
    XLogRecPtr sender_write_location;
    XLogRecPtr sender_flush_location;
    XLogRecPtr sender_replay_location;
    XLogRecPtr receiver_received_location;
    XLogRecPtr receiver_write_location;
    XLogRecPtr receiver_flush_location;
    XLogRecPtr receiver_replay_location;
    int sync_percent;
    int sync_state;
    int sync_priority;
} CmSenderReplconninfo;

typedef struct CmReceiverReplconninfo {
    pid_t receiver_pid;
    int local_role;
    int peer_role;
    int peer_state;
    int state;
    XLogRecPtr sender_sent_location;
    XLogRecPtr sender_write_location;
    XLogRecPtr sender_flush_location;
    XLogRecPtr sender_replay_location;
    XLogRecPtr receiver_received_location;
    XLogRecPtr receiver_write_location;
    XLogRecPtr receiver_flush_location;
    XLogRecPtr receiver_replay_location;
    int sync_percent;
} CmReceiverReplconninfo;

typedef struct {
    DcfRole dcfRole;
    uint32 lastTerm;
    uint64 lastIndex;
    uint32 dcfTerm;
    uint32 reserved1;
    bool hasLeader;
    bool isInMajor;
    DcfWorkMode dcfWorkMode;
    uint32 localNodeId;
    int32 pushNum;
    uint32 dnPStatus;
    bool isArbitrating;
    char reserved[43];  // keep sizeof(CmDcfInfo) is 88
} CmDcfInfo;

typedef struct CmGtmReplconninfo {
    int local_role;
    int connect_status;
    TransactionId xid;
    uint64 send_msg_count;
    uint64 receive_msg_count;
    int sync_mode;
} CmGtmReplconninfo;

typedef struct CmCoordinateReplconninfo {
    int status;
    int db_state;
} CmCoordinateReplconninfo;

typedef enum CmCoordinateGroupMode {
    GROUP_MODE_INIT,
    GROUP_MODE_NORMAL,
    GROUP_MODE_PENDING,
    GROUP_MODE_BUTT
} CmCoordinateGroupMode;

#define AGENT_TO_INSTANCE_CONNECTION_BAD 0
#define AGENT_TO_INSTANCE_CONNECTION_OK 1

#define INSTANCE_PROCESS_DIED 0
#define INSTANCE_PROCESS_RUNNING 1

const int max_cn_node_num_for_old_version = 16;

typedef struct ClusterCnInfo {
    uint32 cn_Id;
    uint32 cn_active;
    bool cn_connect;
    bool drop_success;
} ClusterCnInfo;

typedef struct AgentToCmCoordinateStatusReportOld {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int connectStatus;
    int processStatus;
    int isCentral;
    char nodename[NAMEDATALEN];
    char logicClusterName[CM_LOGIC_CLUSTER_NAME_LEN];
    char cnodename[NAMEDATALEN];
    CmCoordinateReplconninfo status;
    CmCoordinateGroupMode group_mode;
    bool cleanDropCnFlag;
    bool isCnDnDisconnected;
    ClusterCnInfo cn_active_info[max_cn_node_num_for_old_version];
    int cn_restart_counts;
    int phony_dead_times;
} AgentToCmCoordinateStatusReportOld;

typedef struct AgentToCmCoordinateStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int connectStatus;
    int processStatus;
    int isCentral;
    char nodename[NAMEDATALEN];
    char logicClusterName[CM_LOGIC_CLUSTER_NAME_LEN];
    char cnodename[NAMEDATALEN];
    CmCoordinateReplconninfo status;
    CmCoordinateGroupMode group_mode;
    bool cleanDropCnFlag;
    bool isCnDnDisconnected;
    uint32 cn_active_info[CN_INFO_NUM];
    int buildReason;
    char resevered[RESERVE_NUM - RESERVE_NUM_USED];
    int cn_restart_counts;
    int phony_dead_times;
} AgentToCmCoordinateStatusReport;

typedef struct AgentToCmCoordinateStatusReportV1 {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int connectStatus;
    int processStatus;
    int isCentral;
    char nodename[NAMEDATALEN];
    char logicClusterName[CM_LOGIC_CLUSTER_NAME_LEN];
    char cnodename[NAMEDATALEN];
    CmCoordinateReplconninfo status;
    CmCoordinateGroupMode group_mode;
    bool cleanDropCnFlag;
    bool isCnDnDisconnected;
    uint32 cn_active_info[CN_INFO_NUM];
    int buildReason;
    int cn_dn_disconnect_times;
    char resevered[RESERVE_NUM - (2 * RESERVE_NUM_USED)];
    int cn_restart_counts;
    int phony_dead_times;
} AgentToCmCoordinateStatusReportV1;

typedef struct AgentToCmFencedUdfStatusReport {
    int msg_type;
    uint32 nodeid;
    int status;
} AgentToCmFencedUdfStatusReport;

typedef struct AgentToCmDatanodeStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int connectStatus;
    int processStatus;
    CmLocalReplconninfo local_status;
    BuildState build_info;
    CmSenderReplconninfo sender_status[CM_MAX_SENDER_NUM];
    CmReceiverReplconninfo receive_status;
    RedoStatsData parallel_redo_status;
    CmRedoStats local_redo_stats;
    int dn_restart_counts;
    int phony_dead_times;
    int dn_restart_counts_in_hour;
} AgentToCmDatanodeStatusReport;

typedef struct AgentToCmserverDnSyncList {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    char dnSynLists[DN_SYNC_LEN];
    // remain
    int remain[REMAIN_LEN];
    char remainStr[DN_SYNC_LEN];
} AgentToCmserverDnSyncList;

typedef struct AgentToCmGtmStatusReport {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int connectStatus;
    int processStatus;
    CmGtmReplconninfo status;
    int phony_dead_times;
} AgentToCmGtmStatusReport;

typedef struct AgentToCmCurrentTimeReport {
    int msg_type;
    uint32 nodeid;
    long int etcd_time;
} AgentToCmCurrentTimeReport;

typedef struct AgentToCmsDiskUsageStatusReport {
    int msgType;
    uint32 instanceId;
    uint32 dataPathUsage;
    uint32 logPathUsage;
} AgentToCmsDiskUsageStatusReport;

typedef struct AgentToCmHeartbeat {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int cluster_status_request;
} AgentToCmHeartbeat;

typedef struct DnLocalPeer {
    char localIp[CM_IP_LENGTH];
    char peerIp[CM_IP_LENGTH];
    uint32 localPort;
    uint32 peerPort;
    char reserver[DN_SYNC_LEN];
} DnLocalPeer;

typedef struct AgentCmDnLocalPeer {
    int32 msgType;
    uint32 node;
    uint32 instanceId;
    int32 instanceType;
    DnLocalPeer dnLpInfo;
} AgentCmDnLocalPeer;

typedef enum { READ_ONLY_INIT, READ_ONLY_ON, READ_ONLY_OFF } ReadOnlyState;

typedef struct {
    int msgType;
    uint32 instanceId;
    int instanceType;
    uint32 dataPathUsage;
    uint32 logPathUsage;
    ReadOnlyState readOnly;
    uint32 readonlyThreshold;
    char reserved[12];
} AgentToCmDiskUsageStatusReport;

const uint32 BASE_INST_RES = 16;

typedef struct BaseInstInfoT {
    int msgType;
    uint32 node;
    uint32 instId;  // instanceId
    int instType;   // instanceType
    char remain[BASE_INST_RES];
} BaseInstInfo;

const uint32 MAX_FLOAT_IP_COUNT = 6;
const uint32 FLOAT_IP_MSG_RES = 512;

typedef struct DnFloatIpInfoT {
    uint32 count;
    int32 dnNetState[MAX_FLOAT_IP_COUNT];
    int32 nicNetState[MAX_FLOAT_IP_COUNT];
} DnFloatIpInfo;

typedef struct CmaDnFloatIpInfoT {
    BaseInstInfo baseInfo;
    DnFloatIpInfo info;
    char remain[FLOAT_IP_MSG_RES];
} CmaDnFloatIpInfo;

typedef struct AgentToCmserverDnCandiListSt {
    BaseInstInfo baseInfo;
    char dnCandiList[DN_SYNC_LEN];
    // remain
    bool isValid;
    int remain[REMAIN_LEN - 1];
    char remainStr[DN_SYNC_LEN];
} AgentToCmserverDnCandiList;  // total 608

typedef struct DnStatus {
    CmMessageType barrierMsgType;
    AgentToCmDatanodeStatusReport reportMsg;
    union {
        AgentToCmCoordinateBarrierStatusReport barrierMsg;
        Agent2CmBarrierStatusReport barrierMsgNew;
    };
    AgentCmDnLocalPeer lpInfo;
    AgentToCmDiskUsageStatusReport diskUsageMsg;
    CmaDnFloatIpInfo floatIpInfo;
    AgentToCmserverDnCandiList candiListInfo;
} DnStatus;

typedef struct DnSyncListInfo {
    pthread_rwlock_t lk_lock;
    AgentToCmserverDnSyncList dnSyncListMsg;
} DnSyncListInfo;

typedef struct CnStatus {
    CmMessageType barrierMsgType;
    AgentToCmCoordinateStatusReport reportMsg;
    Agent2CmBackupInfoRep backupMsg;
    union {
        AgentToCmCoordinateBarrierStatusReport barrierMsg;
        Agent2CmBarrierStatusReport barrierMsgNew;
    };
} CnStatus;

typedef struct CoordinateStatusInfo {
    pthread_rwlock_t lk_lock;
    CnStatus cnStatus;
} CoordinateStatusInfo;

typedef struct DatanodeStatusInfo {
    pthread_rwlock_t lk_lock;
    DnStatus dnStatus;
} DatanodeStatusInfo;

typedef struct GtmStatusInfo {
    pthread_rwlock_t lk_lock;
    AgentToCmGtmStatusReport report_msg;
} GtmStatusInfo;

typedef struct CmToAgentHeartbeat {
    int msg_type;
    uint32 node;
    int type;
    int cluster_status;
    uint32 healthCoorId;
} CmToAgentHeartbeat;

typedef struct CmToCmVote {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int role;
} CmToCmVote;

typedef struct CmToCmTimeline {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    long timeline;
} CmToCmTimeline;

typedef struct CmToCmBroadcast {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int role;
} CmToCmBroadcast;

typedef struct CmToCmNotify {
    int msg_type;
    int role;
} CmToCmNotify;

typedef struct CmToCmSwitchover {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CmToCmSwitchover;

typedef struct CmToCmSwitchoverAck {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CmToCmSwitchoverAck;

typedef struct CmToCmFailover {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CmToCmFailover;

typedef struct CmToCmFailoverAck {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CmToCmFailoverAck;

typedef struct CmToCmSync {
    int msg_type;
    int role;
} CmToCmSync;

typedef enum NotifyCnE { NOT_NEED_TO_NOTITY_CN = 0, WAIT_TO_NOTFY_CN, NEED_TO_NOTITY_CN } NotifyCn_t;

typedef struct CmsErCommandT {
    uint32 action;
    int64 erScore;  // er score: the value is larger and it is more serious
} CmsErCommand;

typedef struct CmInstanceCommandStatus {
    int command_status;
    int command_send_status;
    int command_send_times;
    int command_send_num;
    int pengding_command;
    int cmdPur;      // purpose
    int cmdSour;     // source
    int cmdRealPur;  // real purpose
    uint32 peerInstId;
    int time_out;
    int delaySwitchoverTime;
    int role_changed;
    volatile int heat_beat;
    int arbitrate_delay_time_out;
    int arbitrate_delay_set;
    int local_arbitrate_delay_role;
    int peerl_arbitrate_delay_role;
    int full_build;
    int notifyCnCount;
    volatile int keep_heartbeat_timeout;
    int sync_mode;
    int maxSendTimes;
    int parallel;
    int32 buildFailedTimeout;
    cmTime_t cmTime;  // use to record time
    NotifyCn_t notifyCn;
    CmsErCommand erCmd;
} CmInstanceCommandStatus;

typedef struct DatanodelocalPeer {
    uint32 ipCount;
    char localIp[CM_IP_NUM][CM_IP_LENGTH];
    uint32 localPort;
    uint32 peerInst;
} DatanodelocalPeer;

typedef enum {
    EXT_IP_INIT,
    EXT_IP_NORMAL,
    EXT_IP_NIC_DOWN,
    EXT_IP_FILE_UNREADABLE,
    EXT_IP_CONN_BAD,
    EXT_IP_UNKNOWN,
} ExtIpState;

typedef struct {
    bool enableExtIp;
    char ip[CM_IP_LENGTH];
    uint32 port;
    ExtIpState state;
    uint32 abnormalTimes;
} ExtIpStatus;

typedef enum {
    READ_ONLY_DDB_INIT = 0,
    READ_ONLY_EXPECT = 1,
    READ_ONLY_ALREADY = 2,
    READ_ONLY_NOT_EXPECT = 3,
    READ_ONLY_DDB_MAX
} ReadOnlyDdbValue;

typedef struct DataNodeReadOnlyInfo {
    uint32 node;
    uint32 instanceId;
    uint32 groupIndex;
    int memberIndex;
    int dataDiskUsage;
    int instanceType;
    ReadOnlyState readOnly;
    ReadOnlyDdbValue ddbValue;
    bool finalState;
    bool isNeedCancel;
    bool haveCanceled;
    char dataNodePath[CM_PATH_LENGTH];
    char instanceName[CM_NODE_NAME];
    char nodeName[CM_NODE_NAME];
} DataNodeReadOnlyInfo;

typedef struct DnSyncListArbT {
    uint32 syncDoneCnt;
    uint32 lastSyncDoneCnt;
    uint32 dnSyncMode;
    bool32 isSyncValid;
    uint32 peerDnNum;
    uint32 assignPrimaryId;
} DnSyncListArb;

typedef struct DnProcessStatusRetT {
    uint64 pingFaultTime;
} DnProcessStatusRet;

typedef struct {
    CandidateState candiState;
    uint32 isolatedReason;
    uint32 isolatedTimeout;
} DnCandiStatus;

// need to keep consist with CmToCtlInstanceDatanodeStatus
typedef struct CmInstanceDatanodeReportStatus {
    CmLocalReplconninfo local_status;
    int sender_count;
    BuildState build_info;
    CmSenderReplconninfo sender_status[CM_MAX_SENDER_NUM];
    CmReceiverReplconninfo receive_status;
    RedoStatsData parallel_redo_status;
    CmRedoStats local_redo_stats;
    SynchronousStandbyMode sync_standby_mode;
    int send_gs_guc_time;
    int dn_restart_counts;
    bool arbitrateFlag;
    int failoverStep;
    int failoverTimeout;
    int phony_dead_times;
    int phony_dead_interval;
    int dn_restart_counts_in_hour;
    bool is_finish_redo_cmd_sent;
    uint64 ckpt_redo_point;
    char barrierID[BARRIERLEN];
    char query_barrierId[BARRIERLEN];
    uint32 barrierStoppedTime;
    uint64 barrierLSN;
    uint64 archive_LSN;
    uint64 flush_LSN;
    DatanodeSyncList dnSyncList;
    int32 syncDone;
    uint32 arbiTime;
    uint32 sendFailoverTimes;
    bool is_barrier_exist;
    cmTime_t printBegin;  // print synclist time
    DatanodelocalPeer dnLp;
    CmDcfInfo dcfInfo;
    pthread_rwlock_t syncListLock;
    ExtIpStatus extIpStatus;
    DnFloatIpInfo floatIp;
    DataNodeReadOnlyInfo *readOnly;
    uint64 lock1Time;
    XLogRecPtr localReplayLocation;
    DnSyncListArb syncListArb;
    DnProcessStatusRet dnPStRet;
    uint32 leaderTime;
    pthread_rwlock_t candiStatusLock;
    DnCandiStatus dnCandiStatus;
    bool have_canceled;
} CmInstanceDatanodeReportStatus;

typedef struct CmInstanceGtmReportStatus {
    CmGtmReplconninfo local_status;
    int phony_dead_times;
    int phony_dead_interval;
} CmInstanceGtmReportStatus;

/*
 * each coordinator manage a list of datanode notify status
 */
typedef struct CmNotifyMsgStatus {
    uint32 *datanode_instance;
    uint32 *datanode_index;
    bool *notify_status;
    bool have_canceled;
    uint32 gtmIdBroadCast;
} CmNotifyMsgStatus;

#define OBS_BACKUP_INIT         (0)    // not start
#define OBS_BACKUP_PROCESSING   (1)
#define OBS_BACKUP_COMPLETED    (2)
#define OBS_BACKUP_FAILED       (3)
#define OBS_BACKUP_UNKNOWN      (4)    // conn failed, can't get status, will do nothing until it change to other

typedef struct CmInstanceCoordinateReportStatus {
    CmCoordinateReplconninfo status;
    int isdown;
    int clean;
    uint32 exec_drop_instanceId;
    CmCoordinateGroupMode group_mode;
    CmNotifyMsgStatus notify_msg;
    char reserve[64];
    uint32 cn_restart_counts;
    int phony_dead_times;
    int phony_dead_interval;
    bool delay_repair;
    bool isCnDnDisconnected;
    int auto_delete_delay_time;
    int disable_time_out;
    int cma_fault_timeout_to_killcn;
    char barrierID[BARRIERLEN];
    char query_barrierId[BARRIERLEN];
    uint32 barrierStoppedTime;
    uint64 barrierLSN;
    uint64 archive_LSN;
    uint64 flush_LSN;
    uint64 ckpt_redo_point;
    bool is_barrier_exist;
    int buildReason;
    uint32 readonlyTimeout;
    ExtIpStatus extIpStatus;
    DataNodeReadOnlyInfo *readOnly;
} CmInstanceCoordinateReportStatus;

typedef struct CmInstanceArbitrateStatus {
    int sync_mode;
    bool restarting;
    int promoting_timeout;
} CmInstanceArbitrateStatus;

#define MAX_CM_TO_CM_REPORT_SYNC_COUNT_PER_CYCLE 5
typedef struct CmToCmReportSync {
    int msg_type;
    uint32 node[CM_PRIMARY_STANDBY_NUM];
    uint32 instanceId[CM_PRIMARY_STANDBY_NUM];
    int instance_type[CM_PRIMARY_STANDBY_NUM];
    CmInstanceCommandStatus command_member[CM_PRIMARY_STANDBY_NUM];
    CmInstanceDatanodeReportStatus data_node_member[CM_PRIMARY_STANDBY_NUM];
    CmInstanceGtmReportStatus gtm_member[CM_PRIMARY_STANDBY_NUM];
    CmInstanceCoordinateReportStatus coordinatemember;
    CmInstanceArbitrateStatus arbitrate_status_member[CM_PRIMARY_STANDBY_NUM];
} CmToCmReportSync;

typedef struct CmInstanceRoleStatus {
    // available zone information
    char azName[CM_AZ_NAME];
    uint32 azPriority;

    uint32 node;
    uint32 instanceId;
    int instanceType;
    int role;
    int dataReplicationMode;
    int instanceRoleInit;
} CmInstanceRoleStatus;

typedef struct CmInstanceRoleStatus0 {
    uint32 node;
    uint32 instanceId;
    int instanceType;
    int role;
    int dataReplicationMode;
    int instanceRoleInit;
} CmInstanceRoleStatus0;

#define CM_PRIMARY_STANDBY_MAX_NUM 8    // supprot 1 primary and [1, 7] standby
#define CM_PRIMARY_STANDBY_MAX_NUM_0 3  // support master standby dummy

typedef struct CmInstanceRoleGroup0 {
    int count;
    CmInstanceRoleStatus0 instanceMember[CM_PRIMARY_STANDBY_MAX_NUM_0];
} CmInstanceRoleGroup0;

typedef struct CmInstanceRoleGroup {
    int count;
    CmInstanceRoleStatus instanceMember[CM_PRIMARY_STANDBY_MAX_NUM];
} CmInstanceRoleGroup;

typedef struct CmToCmRoleChangeNotify {
    int msg_type;
    CmInstanceRoleGroup role_change;
} CmToCmRoleChangeNotify;

typedef struct CtlToCmDatanodeRelationInfo {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int wait_seconds;
} CtlToCmDatanodeRelationInfo;

typedef struct CmToCtlGetDatanodeRelationAck {
    int command_result;
    int member_index;
    CmInstanceRoleStatus instanceMember[CM_PRIMARY_STANDBY_MAX_NUM];
    CmInstanceGtmReportStatus gtm_member[CM_PRIMARY_STANDBY_NUM];
    CmInstanceDatanodeReportStatus data_node_member[CM_PRIMARY_STANDBY_MAX_NUM];
} CmToCtlGetDatanodeRelationAck;

// need to keep consist with the struct CmInstanceDatanodeReportStatus
typedef struct CmToCtlInstanceDatanodeStatus {
    CmLocalReplconninfo local_status;
    int sender_count;
    BuildState build_info;
    CmSenderReplconninfo sender_status[CM_MAX_SENDER_NUM];
    CmReceiverReplconninfo receive_status;
    RedoStatsData parallel_redo_status;
    CmRedoStats local_redo_stats;
    SynchronousStandbyMode sync_standby_mode;
    int send_gs_guc_time;
} CmToCtlInstanceDatanodeStatus;

typedef struct CmToCtlInstanceGtmStatus {
    CmGtmReplconninfo local_status;
} CmToCtlInstanceGtmStatus;

typedef struct CmToCtlInstanceCoordinateStatus {
    int status;
    CmCoordinateGroupMode group_mode;
    /* no notify map in ctl */
} CmToCtlInstanceCoordinateStatus;

typedef struct CmToCtlInstanceStatus {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int member_index;
    int is_central;
    int fenced_UDF_status;
    CmToCtlInstanceDatanodeStatus data_node_member;
    CmToCtlInstanceGtmStatus gtm_member;
    CmToCtlInstanceCoordinateStatus coordinatemember;
    CmDcfInfo dcfInfo;
} CmToCtlInstanceStatus;

typedef struct CmToCtlInstanceBarrierInfo {
    int msg_type;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int member_index;
    uint64 ckpt_redo_point;
    char barrierID [BARRIERLEN];
    uint64 barrierLSN;
    uint64 archive_LSN;
    uint64 flush_LSN;
} CmToCtlInstanceBarrierInfo;

typedef struct CmToCtlCentralNodeStatus {
    uint32 instanceId;
    int node_index;
    int status;
} CmToCtlCentralNodeStatus;

#define CM_CAN_PRCESS_COMMAND 0
#define CM_ANOTHER_COMMAND_RUNNING 1
#define CM_INVALID_COMMAND 2
#define CM_DN_NORMAL_STATE 3

typedef struct CmToCtlCommandAck {
    int msg_type;
    int command_result;
    uint32 node;
    uint32 instanceId;
    int instance_type;
    int command_status;
    int pengding_command;
    int time_out;
} CmToCtlCommandAck;

typedef struct CmToCtlBalanceCheckAck {
    int msg_type;
    int switchoverDone;
} CmToCtlBalanceCheckAck;

typedef struct CmToCtlSwitchoverFullCheckAck {
    int msg_type;
    int switchoverDone;
} CmToCtlSwitchoverFullCheckAck, CmToCtlSwitchoverAzCheckAck;

#define MAX_INSTANCES_LEN 512
typedef struct CmToCtlBalanceResult {
    int msg_type;
    int imbalanceCount;
    uint32 instances[MAX_INSTANCES_LEN];
} CmToCtlBalanceResult;

#define CM_STATUS_STARTING 0
#define CM_STATUS_PENDING 1
#define CM_STATUS_NORMAL 2
#define CM_STATUS_NEED_REPAIR 3
#define CM_STATUS_DEGRADE 4
#define CM_STATUS_UNKNOWN 5
#define CM_STATUS_NORMAL_WITH_CN_DELETED 6
#define CM_STATUS_UNKNOWN_WITH_BINARY_DAMAGED (7)

typedef struct CmToCtlClusterStatus {
    int msg_type;
    int cluster_status;
    bool is_all_group_mode_pending;
    int switchedCount;
    int node_id;
    bool inReloading;
} CmToCtlClusterStatus;

typedef struct CmToCtlClusterGlobalBarrierInfo {
    int msg_type;
    char global_barrierId[BARRIERLEN];
    char global_achive_barrierId[BARRIERLEN];
    char globalRecoveryBarrierId[BARRIERLEN];
} CmToCtlClusterGlobalBarrierInfo;

typedef struct Cm2CtlGlobalBarrierNew {
    int msg_type;
    char globalRecoveryBarrierId[BARRIERLEN];
    GlobalBarrierStatus globalStatus;
} Cm2CtlGlobalBarrierNew;

typedef struct CmToCtlLogicClusterStatus {
    int msg_type;
    int cluster_status;
    bool is_all_group_mode_pending;
    int switchedCount;
    bool inReloading;

    int logic_cluster_status[LOGIC_CLUSTER_NUMBER];
    bool logic_is_all_group_mode_pending[LOGIC_CLUSTER_NUMBER];
    int logic_switchedCount[LOGIC_CLUSTER_NUMBER];
} CmToCtlLogicClusterStatus;

typedef struct CmToCtlCmserverStatus {
    int msg_type;
    int local_role;
    bool is_pending;
} CmToCtlCmserverStatus;

typedef struct CmQueryInstanceStatus {
    int msg_type;
    uint32 nodeId;
    uint32 instanceType;  // only for etcd and cmserver
    uint32 msg_step;
    uint32 status;
    bool pending;
} CmQueryInstanceStatus;

typedef struct EtcdStatusInfo {
    pthread_rwlock_t lk_lock;
    CmQueryInstanceStatus report_msg;
} EtcdStatusInfo;

/* kerberos information */
#define ENV_MAX 100
#define ENVLUE_NUM 3
#define MAX_BUFF 1024
#define MAXLEN 20
#define KERBEROS_NUM 2

typedef struct AgentToCmKerberosStatusReport {
    int msg_type;
    uint32 node;
    char kerberos_ip[CM_IP_LENGTH];
    uint32 port;
    uint32 status;
    char role[MAXLEN];
    char nodeName[CM_NODE_NAME];
} AgentToCmKerberosStatusReport;

typedef struct KerberosStatusInfo {
    pthread_rwlock_t lk_lock;
    AgentToCmKerberosStatusReport report_msg;
} KerberosStatusInfo;

typedef struct CmToCtlKerberosStatusQuery {
    int msg_type;
    uint32 heartbeat[KERBEROS_NUM];
    uint32 node[KERBEROS_NUM];
    char kerberos_ip[KERBEROS_NUM][CM_IP_LENGTH];
    uint32 port[KERBEROS_NUM];
    uint32 status[KERBEROS_NUM];
    char role[KERBEROS_NUM][MAXLEN];
    char nodeName[KERBEROS_NUM][CM_NODE_NAME];
} CmToCtlKerberosStatusQuery;

typedef struct KerberosGroupReportStatus {
    pthread_rwlock_t lk_lock;
    CmToCtlKerberosStatusQuery kerberos_status;
} KerberosGroupReportStatus;


typedef uint32 ShortTransactionId;

/* ----------------
 *        Special transaction ID values
 *
 * BOOTSTRAP_XACT_ID is the XID for "bootstrap" operations, and
 * FROZEN_XACT_ID is used for very old tuples.  Both should
 * always be considered valid.
 *
 * FIRST_NORMAL_XACT_ID is the first "normal" transaction id.
 * Note: if you need to change it, you must change pg_class.h as well.
 * ----------------
 */
#define INVALID_XACT_ID ((TransactionId)0)
#define BOOTSTRAP_XACT_ID ((TransactionId)1)
#define FROZEN_XACT_ID ((TransactionId)2)
#define FIRST_NORMAL_XACT_ID ((TransactionId)3)
#define MAX_XACT_ID ((TransactionId)0xFFFFFFFF)

/* ----------------
 *        transaction ID manipulation macros
 * ----------------
 */
#define XACT_ID_IS_VALID(xid) ((xid) != INVALID_XACT_ID)
#define XACT_ID_IS_NORMAL(xid) ((xid) >= FIRST_NORMAL_XACT_ID)
#define XACT_ID_EQUALS(id1, id2) ((id1) == (id2))
#define XACT_ID_STORE(xid, dest) (*(dest) = (xid))
#define STORE_INVALID_XACT_ID(dest) (*(dest) = INVALID_XACT_ID)

/*
 * Macros for comparing XLogRecPtrs
 *
 * Beware of passing expressions with side-effects to these macros,
 * since the arguments may be evaluated multiple times.
 */
#define WAL_BYTE_LT(a, b) \
    (((a) & PAGE_EXPANSION_BIT_64) == ((b) & PAGE_EXPANSION_BIT_64) ? (a) < (b) : ((a) & PAGE_EXPANSION_BIT_64))
#define WAL_BYTE_LE(a, b) \
    (((a) & PAGE_EXPANSION_BIT_64) == ((b) & PAGE_EXPANSION_BIT_64) ? (a) <= (b) : ((a) & PAGE_EXPANSION_BIT_64))
#define WAL_BYTE_EQ(a, b) ((a) == (b))

#define InvalidTerm (0)
#define FirstTerm (1)
#define TermIsInvalid(term) ((term) == InvalidTerm)

#define XLByteLT_W_TERM(a_term, a_logptr, b_term, b_logptr) \
    (((a_term) < (b_term)) || (((a_term) == (b_term)) && ((a_logptr) < (b_logptr))))
#define XLByteLE_W_TERM(a_term, a_logptr, b_term, b_logptr) \
    (((a_term) < (b_term)) || (((a_term) == (b_term)) && ((a_logptr) <= (b_logptr))))
#define XLByteEQ_W_TERM(a_term, a_logptr, b_term, b_logptr) (((a_term) == (b_term)) && ((a_logptr) == (b_logptr)))
#define XLByteWE_W_TERM(a_term, a_logptr, b_term, b_logptr) \
    (((a_term) > (b_term)) || (((a_term) == (b_term)) && ((a_logptr) > (b_logptr))))

#define CM_RESULT_COMM_ERROR (-2) /* Communication error */
#define CM_RESULT_ERROR (-1)
#define CM_RESULT_OK (0)
/*
 * This error is used ion the case where allocated buffer is not large
 * enough to store the errors. It may happen of an allocation failed
 * so it's status is considered as unknown.
 */
#define CM_RESULT_UNKNOWN (1)

typedef struct ResultDataPacked {
    char pad[CM_MSG_MAX_LENGTH];
} ResultDataPacked;

typedef union CmResultData {
    ResultDataPacked packed;
} CmResultData;

typedef struct CmResult {
    int gr_msglen;
    int gr_status;
    int gr_type;
    CmResultData gr_resdata;
} CmResult;

extern int query_gtm_status_wrapper(const char pid_path[MAXPGPATH], AgentToCmGtmStatusReport& agent_to_cm_gtm);
extern int query_gtm_status_for_phony_dead(const char pid_path[MAXPGPATH]);

typedef struct CtlToCmReload {
    int msgType;
} CtlToCmReload;
typedef struct CmToCtlReloadAck {
    int msgType;
    bool reloadOk;
} CmToCtlReloadAck;

#endif
