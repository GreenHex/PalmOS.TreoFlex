/*
 * LogDB.h
 */
 
#ifndef __LOGDB_H__
#define __LOGDB_H__

#include <Hs.h>
#include <DataMgr.h>

#include "Global.h"
#include "Prefs.h"
#include "SMSUtils.h"
#include "../../TreoFlex2/src/Global.h"
#include "../../Centroid/Src/DBUtils.h"

#define TreoFlexLogDbCreator			appFileCreator
#define TreoFlexLogDbName				"TreoFlexLogDB"
#define TreoFlexLogDbType				'DTFL'
#define TreoFlexLogDbVersion			0x01

#define STRLEN_TRUNCMSG_LEN				20

typedef struct {
	Err						error;
	Char					strError[STRLEN_ERROR];
	UInt32					timeSecs;
	callDisconectType_e		disconnectType;
	Char					Address[STRLEN_PHN_NUMBER];
	Char					CallerID[STRLEN_CLIR];
	Char					Message[STRLEN_TRUNCMSG_LEN];
} log_t;

// Prototypes
extern Err					SaveLog( Err error, Char* strError, callDisconectType_e disconnectType, msg_t* pMsg );

#endif /* __LOGDB_H__ */

/*
 * LogDB.h
 */