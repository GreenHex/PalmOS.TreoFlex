/*
 * LogDB.h
 */

#ifndef __TFS_LOGDB_H__
#define __TFS_LOGDB_H__

#include <Hs.h>
#include <DataMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "../../TreoFlex2/src/Global.h"
#include "../../TreoFlex2/src/LogDB.h"
#include "../../Centroid/src/DBUtils.h"

#define POS_X_RIGHT_ERR			31
#define POS_X_RIGHT_TIME		59
#define POS_X_RIGHT_DATE		84
#define POS_X_LEFT_PHN_NUM		88

// Prototypes
extern void 				DrawLogsListItem( Int16 itemNum, RectangleType* pRect, Char **itemsText );
extern UInt16 				InitializeLogList( ListType* pList );
extern Err 					DeleteLogDB( void );
extern Err					LogRemoveRecordAtIdx( UInt16* idxP );
extern Err 					LogGetRecordAtIdx( UInt16* idxP, log_t* pLog );

#endif /* __TFS_LOGDB_H__ */

 
/*
 * LogDB.h
 */