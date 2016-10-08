/*
 * MsgsDB.h
 */

#ifndef __TFS_MSGSDB_H__
#define __TFS_MSGSDB_H__

#include <Hs.h>
#include <DataMgr.h>
#include <Form.h>

#include "Global.h"
#include "../../TreoFlex2/src/Global.h"
#include "../../TreoFlex2/src/Prefs.h"
#include "../../Centroid/src/DBUtils.h"

#define tfsDbCreator			tfsAppFileCreator
#define tfsDbName				"TreoFlexDB"
#define tfsDbType				'DATA'
#define tfsDbVersion			0x00

typedef struct {
	Char						msg[STRLEN_MSG];	
} msgsDB_t;

// Prototypes
extern UInt16 				MsgsGetNumberOfRecords( void );
extern Err 					MsgsSaveNewRecord( msgsDB_t* pMsg, Boolean isSecret, UInt16* idxP );
extern Err 					MsgsReplaceRecordAtIdx( UInt16* idxP, msgsDB_t* pMsg, Boolean isSecret );
extern Err 					MsgsRemoveRecordAtIdx( UInt16* idxP );
extern Err 					MsgsGetRecordAtIdx( UInt16* idxP, msgsDB_t* pMsg );
extern Err 					MsgsFindRecord( msgsDB_t* pMsg, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP );
extern Int16 				Msgs_SortNameF( msgsDB_t* recP1, msgsDB_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );
extern Boolean 				LoadMsgsList( ListType* pList, Char* msgP, UInt16* idxP );

#endif /* __TFS_MSGSDB_H__ */


/*
 * MsgsDB.h
 */ 
 
 