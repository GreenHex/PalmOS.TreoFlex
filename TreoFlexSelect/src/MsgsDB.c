/*
 * MsgsDB.c
 */

#include "MsgsDB.h"

// Prototypes
static Err 			OpenMsgsDB( DmOpenRef* dbPP );
static Boolean 		MsgsSeekRecord( DmOpenRef* dbPP, msgsDB_t* pMsg, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP );

/*
 * OpenMsgsDB()
 */
static Err OpenMsgsDB( DmOpenRef* dbPP )
{
	Err					error = errNone;
	UInt16				numRecords = 0;
	appPrefs_t			prefs;
	msgsDB_t			msg;
	
	error = DBOpen( dbPP, tfsDbName, tfsDbType, tfsDbCreator, dmModeReadOnly, tfsDbVersion, sizeof( msgsDB_t ) ); 

	if ( ( error == errNone ) && ( *dbPP ) )
	{
		numRecords = DmNumRecordsInCategory( *dbPP, dmAllCategories );
		
		if ( !numRecords )
		{
			DmCloseDatabase( *dbPP );
			
			ReadAppPrefs( &prefs );
			
			if ( StrLen( prefs.msg ) )
			{
				StrCopy( msg.msg, prefs.msg );
			}
			else
			{
				StrCopy( msg.msg, STR_INITIAL_MSG );
			}
			
			error = MsgsSaveNewRecord( &msg, false, NULL );
			
			if ( !error )
			{
				error = DBOpen( dbPP, tfsDbName, tfsDbType, tfsDbCreator, dmModeReadOnly, tfsDbVersion, sizeof( msgsDB_t ) ); 
			}
		}	
	}
	
	return ( error );
	
} // OpenMsgsDB()

/*
 * MsgsGetNumberOfRecords()
 */
UInt16 MsgsGetNumberOfRecords( void )
{
	Err 			error = errNone;
	UInt16			retVal = 0;
	DmOpenRef 		dbP = NULL;
	
	error = OpenMsgsDB( &dbP );
    
    if ( dbP )
    {
	    retVal = DmNumRecordsInCategory( dbP, dmAllCategories );
		
		DmCloseDatabase( dbP );
    }
    
	return ( retVal );
	
} // MsgsGetNumberOfRecords()

/*
 * MsgsSaveNewRecord()
 */
Err MsgsSaveNewRecord( msgsDB_t* pMsg, Boolean isSecret, UInt16* idxP )
{
	Err 			error = errNone;
	DmOpenRef 		dbP = NULL;
	
	error = DBOpen( &dbP, tfsDbName, tfsDbType, tfsDbCreator, dmModeReadWrite, tfsDbVersion, sizeof( msgsDB_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		msgsDB_t*		recordP = NULL;
		UInt16			idx = dmMaxRecordIndex;
		UInt16			recordAttributes = 0;		
		
		recordH = DmNewRecord( dbP, &idx, sizeof( msgsDB_t ) );
		if (recordH)
		{
			recordP = MemHandleLock( recordH );
			
			if ( recordP )
			{
				error = DmWrite( recordP, 0, pMsg, sizeof( msgsDB_t ) );
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
	
} // MsgsSaveNewRecord()

/*
 * MsgsReplaceRecordAtIdx()
 */
Err MsgsReplaceRecordAtIdx( UInt16* idxP, msgsDB_t* pMsg, Boolean isSecret )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	MemHandle		recordH;
	msgsDB_t*		recordP;
	
	error = DBOpen( &dbP, tfsDbName, tfsDbType, tfsDbCreator, dmModeReadWrite, tfsDbVersion, sizeof( msgsDB_t ) );
	
	if ( ( error == errNone) && ( dbP ) )
	{
		recordH = DmGetRecord( dbP, *idxP );
		
		if ( recordH )
		{
			recordP = (msgsDB_t *) MemHandleLock( recordH );
			if ( recordP )
			{	
				error = DmWrite( recordP, 0, pMsg, sizeof( msgsDB_t ) );
			}
				
			MemHandleUnlock( recordH );			
			recordP = NULL;
		}
		
		error = ReleaseRecord( &dbP, idxP );		
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );

} // MsgsReplaceRecordAtIdx()

/*
 * MsgsRemoveRecordAtIdx()
 */
Err MsgsRemoveRecordAtIdx( UInt16* idxP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;

	error = DBOpen( &dbP, tfsDbName, tfsDbType, tfsDbCreator, dmModeReadWrite, tfsDbVersion, sizeof( msgsDB_t ) );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		if ( DmNumRecords( dbP ) > 0 )
		{
			if ( ( error = DmRemoveRecord( dbP, *idxP ) ) == errNone )
			{	
				*idxP = ( ( *idxP ) > 0 ) ? --( *idxP ) : 0;				
			}
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // MsgsRemoveRecordAtIdx()

/*
 * MsgsGetRecordAtIdx
 */
Err MsgsGetRecordAtIdx( UInt16* idxP, msgsDB_t* pMsg )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( ( !idxP ) || ( !pMsg ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	MemSet( pMsg, sizeof( msgsDB_t ), 0 );

	error = OpenMsgsDB( &dbP );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		msgsDB_t*		recordP = NULL;
		UInt16			numRecords = GetNumRecords( &dbP, dmAllCategories );
		
		if ( *idxP < numRecords )
		{
			recordH = DmQueryRecord( dbP, *idxP );
			
			if ( recordH )
			{		
				recordP = (msgsDB_t *) MemHandleLock( recordH );

				if ( recordP )
				{
					MemMove( pMsg, recordP, sizeof( msgsDB_t ) );
				}
				
				MemHandleUnlock( recordH );
				recordP = NULL;
			}
			else
			{
				error =  DmGetLastErr();
			}
		}
		else
		{
			error = dmErrIndexOutOfRange;
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // MsgsGetRecordAtIdx()

/*
 * MsgsSeekRecord()
 */
static Boolean MsgsSeekRecord( DmOpenRef* dbPP, msgsDB_t* pMsg, UInt16* idxP, Boolean bLoadRecord, DmComparF* comparFP )
{
	Boolean 		retVal = false;
	UInt16			numRecords = GetNumRecords( dbPP, dmAllCategories );
	MemHandle		recordH = NULL;
	msgsDB_t*		recordP = NULL;
		
	for ( *idxP = 0 ; *idxP < numRecords ; (*idxP)++)
	{
		recordH = DmQueryRecord( *dbPP, *idxP );
		
		if ( recordH )
		{
			recordP = (msgsDB_t *) MemHandleLock( recordH );
			
			if ( recordP )
			{
				retVal = ( comparFP( recordP, pMsg, 0, NULL, NULL, NULL ) == 0 ); 
				
				if ( retVal )
				{
					if ( ( pMsg ) && ( bLoadRecord ) )
					{
						MemMove( pMsg, recordP, sizeof( msgsDB_t ) );	
					}

					// What a stupid mistake!!!
					MemHandleUnlock( recordH );
					recordP = NULL;
										
					break;	
				}	
			}
			
			MemHandleUnlock( recordH );
			recordP = NULL;
		}	
	}
	
	return ( retVal );
	
} // MsgsSeekRecord()

/*
 * MsgsFindRecord
 */
Err MsgsFindRecord( msgsDB_t* pMsg, UInt16* idxP, Boolean bLoadRecord, Boolean* bFound, DmComparF* comparFP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	
	if ( bFound )
	{
		*bFound = false;
	}
	
	if ( !( ( pMsg ) && ( idxP ) ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	error = OpenMsgsDB( &dbP );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		*bFound = MsgsSeekRecord( &dbP, pMsg, idxP, bLoadRecord, comparFP );
					
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // MsgsFindRecord()

/*
 * Msgs_SortNameF()
 */
Int16 Msgs_SortNameF( msgsDB_t* recP1, msgsDB_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16		retVal = 0;
	
	retVal = StrCompare( recP1->msg, recP2->msg );
	
	return ( retVal );
	
} // Msgs_SortNameF()

/*
 * LoadMsgsList
 */
Boolean LoadMsgsList( ListType* pList, Char* msgP, UInt16* idxP )
{
	Boolean			retVal = false;
	Err				error = errNone;
	UInt16			itemCount = 0;
	Char			nameStr[STRLEN_MSG];
	UInt16 			nameListSize = 0;
	UInt16			nameStrSize = 0;
	Char**          listOptions = NULL;

	if ( idxP ) *idxP = dmMaxRecordIndex;
	
	if ( gMsgsH != NULL ) // unlock... // don't want problems
	{
        MemHandleUnlock( gMsgsH );
        MemHandleFree( gMsgsH );
        gMsgsH = NULL;
    }
	    
	if ( pList )
	{
		DmOpenRef		dbP;

		error = OpenMsgsDB( &dbP );
		
		if ( dbP )	
		{
			MemHandle		recordH = NULL;
			msgsDB_t*		recordP = NULL;
			UInt16			numRecords = DmNumRecordsInCategory( dbP, dmAllCategories );
			UInt16 			i = 0;
			
			if ( numRecords )
			{
				for ( i = 0 ; i < numRecords ; i++ )
				{
					recordH = DmQueryRecord( dbP, i );
					if ( recordH )
					{	
						recordP = (msgsDB_t *) MemHandleLock( recordH );
						
						if ( recordP )
						{
							StrCopy( nameStr, recordP->msg );
				
							nameStrSize = StrLen( nameStr ) + 1;
							MemMove( gMsgsList + nameListSize, nameStr, nameStrSize );
							nameListSize = nameListSize + nameStrSize;
							
							++itemCount;
							
							if ( ( msgP ) && ( !StrCompare( msgP, recordP->msg ) ) )
							{
								if ( idxP ) *idxP = i;
							}
						}
	
						MemHandleUnlock( recordH );
					}
				}

				retVal = true; // indicates DB was not empty...
			}
					    
	    	if ( itemCount )
	    	{
		    	gMsgsH = SysFormPointerArrayToStrings( gMsgsList, itemCount);
			    listOptions = MemHandleLock( gMsgsH ); // to be unlocked somewhere...
	    	}
	    	
		    LstSetListChoices( pList, listOptions, itemCount );

			DmCloseDatabase( dbP );	
		}
	}
	
	return ( retVal );
	
}  // LoadMsgsList

/*
 * MsgsDB.c
 */