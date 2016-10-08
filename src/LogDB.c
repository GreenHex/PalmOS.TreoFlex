/*
 * LogDB.c
 */
 
#include "LogDB.h"

// Prototypes
static Err 				LogDBSaveNewRecord( log_t* pLog, Boolean isSecret, UInt16* idxP );

Err	SaveLog( Err error, Char* strError, callDisconectType_e disconnectType, msg_t* pMsg )
{
	log_t		log;
	
	log.error = error;
	StrCopy( log.strError, strError );
	log.timeSecs = TimGetSeconds();
	log.disconnectType = disconnectType;
	StrCopy( log.Address, pMsg->Address );
	StrCopy( log.CallerID, pMsg->CallerID );
	StrNCopy( log.Message, pMsg->Message, STRLEN_TRUNCMSG_LEN - 1 );
	log.Message[STRLEN_TRUNCMSG_LEN - 1] = chrNull;
		
	return ( LogDBSaveNewRecord( &log, false, NULL ) );
}

/*
 * ProfilesSaveNewRecord()
 */
static Err LogDBSaveNewRecord( log_t* pLog, Boolean isSecret, UInt16* idxP )
{
	Err 			error = errNone;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, TreoFlexLogDbName, TreoFlexLogDbType, TreoFlexLogDbCreator, dmModeReadWrite, TreoFlexLogDbVersion, sizeof( log_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		log_t*			recordP = NULL;
		UInt16			idx = dmMaxRecordIndex;
		UInt16			recordAttributes = 0;		
		
		recordH = DmNewRecord( dbP, &idx, sizeof( log_t ) );
		if (recordH)
		{
			recordP = MemHandleLock( recordH );
			
			if ( recordP )
			{
				error = DmWrite( recordP, 0, pLog, sizeof( log_t ) );
			}
			
			MemHandleUnlock( recordH );
			recordP = NULL;
		
			DmRecordInfo( dbP, idx, &recordAttributes, NULL, NULL );
			if (isSecret)
			{
				recordAttributes |= dmRecAttrSecret;
			}
			else
			{
				recordAttributes &= ~dmRecAttrSecret;
			}
			error = DmSetRecordInfo( dbP, idx, &recordAttributes, NULL );
			
			error = ReleaseRecord( &dbP, &idx );
			
			if ( idxP )
			{
				*idxP = idx;
			}
		}		
    	 		
		DmCloseDatabase( dbP );
	}
		
	return ( error );
	
} // LogDBSaveNewRecord()

/*
 * LogDB.c
 */