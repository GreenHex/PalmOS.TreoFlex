/*
 * LogDB.c
 */

#include "LogDB.h"

// Prototypes
static Err 			OpenLogDB( DmOpenRef* dbPP, UInt16* numRecordsP );
static Int16 		LogDB_SortTimeF( log_t* recP1, log_t* recP2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH );

/*
 * OpenLogDB()
 */
static Err OpenLogDB( DmOpenRef* dbPP, UInt16* numRecordsP )
{
	Err					error = errNone;
	appPrefs_t			prefs;
	
	error = DBOpen( dbPP, TreoFlexLogDbName, TreoFlexLogDbType, TreoFlexLogDbCreator, dmModeReadWrite, TreoFlexLogDbVersion, sizeof( log_t ) );

	if ( ( error == errNone ) && ( *dbPP ) && ( numRecordsP ) )
	{
		*numRecordsP = DmNumRecordsInCategory( *dbPP, dmAllCategories );
	}
	
	return ( error );
	
} // OpenLogDB()

/*
 * DrawLogsListItem()
 */
void DrawLogsListItem( Int16 itemNum, RectangleType* pRect, Char **itemsText )
{
	Err						error = errNone;
	DmOpenRef				dbP;
	MemHandle				recordH = NULL;
	log_t*					recordP = NULL;
	UInt16					numRecords = dmMaxRecordIndex;
	Char					strTime[timeStringLength] = "\0";
	Char					strDate[longDateStrLength] = "\0";
	Char					strError[maxStrIToALen] = "\0";
    UInt16					strWidth = 0;
    RGBColorType 			rgb;
    IndexedColorType		strColor;
		
	error = OpenLogDB( &dbP, &numRecords );

	if ( dbP )	
	{
		if ( numRecords > itemNum )
		{
			recordH = DmQueryRecord( dbP, itemNum );
			if ( recordH )
			{	
				recordP = (log_t *) MemHandleLock( recordH );
				
				if ( recordP )
				{
					WinPushDrawState();
				
					MakeTimeDateStr( recordP->timeSecs, NULL, strTime, strDate, true );
					ReplaceChar( strTime, 'm', chrNull );
					StripSpaces( strTime );
					StrPrintF( strError, "0x%x", recordP->error );

					if ( recordP->error )
					{
						rgb.r=250; rgb.g=0; rgb.b=51; // Red
					}
					else
					{
						rgb.r=0; rgb.g=0; rgb.b=0; // BLACK
					}
					
					strColor = WinRGBToIndex( &rgb );	
    				WinSetTextColor( strColor );
   
					strWidth = FntLineWidth( strError, StrLen( strError ) );
					WinDrawChars( strError, StrLen( strError ), pRect->topLeft.x + ( POS_X_RIGHT_ERR - strWidth ), 
											pRect->topLeft.y );
											
					strWidth = FntLineWidth( strTime, StrLen( strTime ) );
					WinDrawChars( strTime, StrLen( strTime ), pRect->topLeft.x + ( POS_X_RIGHT_TIME - strWidth ), 
											pRect->topLeft.y );
					
					strWidth = FntLineWidth( strDate, StrLen( strDate ) );
					WinDrawChars( strDate, StrLen( strDate ), pRect->topLeft.x + ( POS_X_RIGHT_DATE - strWidth ), 
											pRect->topLeft.y );
										
					WinDrawTruncChars( recordP->CallerID, StrLen( recordP->CallerID ), 
											pRect->topLeft.x + POS_X_LEFT_PHN_NUM,
											pRect->topLeft.y,
											pRect->extent.x - ( POS_X_LEFT_PHN_NUM + 1 ) );
											
					WinPopDrawState();		
				
				}

				MemHandleUnlock( recordH );
			}
		}
		
		DmCloseDatabase( dbP );	
		
	}
	return;
	
} // DrawLogsListItem()


/*
 * InitializeLogList()
 */
UInt16 InitializeLogList( ListType* pList )
{
	Err				error = errNone;
	UInt16			numRecords = 0;
	    
	if ( pList )
	{
		DmOpenRef		dbP;
		
		error = OpenLogDB( &dbP, &numRecords );
	
		if ( dbP )	
		{
			DmQuickSort( dbP, (DmComparF *) &LogDB_SortTimeF, 1 );
				
			DmCloseDatabase( dbP );	
			
		    LstSetListChoices( pList, NULL, numRecords );

		}
	}
	
	return ( numRecords );
	
}  // InitializeLogList()

/*
 * LogDB_SortTimeF()
 */
static Int16 LogDB_SortTimeF( log_t* recP1, log_t* recP2,
				Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH )
{	
	Int16		retVal = 0;
	
	if ( recP1->timeSecs < recP2->timeSecs )
	{
		retVal = ( other ) ? 1 : -1;
	}
	else if ( recP1->timeSecs > recP2->timeSecs )
	{
		retVal = ( other ) ? -1 : 1;
	}
	
	return ( retVal );
	
} // LogDB_SortTimeF()

/*
 * DeleteLogDB()
 */
Err DeleteLogDB( void )
{
	Err						error = errNone;
	DmSearchStateType 		stateInfo;
	UInt16					cardNum = 0;
	LocalID					dbID = 0;
			
	error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, TreoFlexLogDbType, TreoFlexLogDbCreator, true, &cardNum, &dbID );
	
	if ( error == errNone )
	{
		error = DmDeleteDatabase( cardNum, dbID );
	}
	
	return ( error );
	
} // DeleteLogDB()

/*
 * LogRemoveRecordAtIdx()
 */
Err LogRemoveRecordAtIdx( UInt16* idxP )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	UInt16			numRecords = 0;

	error = OpenLogDB( &dbP, &numRecords );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		if ( *idxP < numRecords )
		{
			if ( ( error = DmRemoveRecord( dbP, *idxP ) ) == errNone )
			{	
				*idxP = ( ( *idxP ) > 0 ) ? --( *idxP ) : 0;				
			}
		}
		
		DmCloseDatabase( dbP );
	}
	
	return ( error );
	
} // LogRemoveRecordAtIdx()

/*
 * LogGetRecordAtIdx
 */
Err LogGetRecordAtIdx( UInt16* idxP, log_t* pLog )
{
	Err				error = errNone;
	DmOpenRef 		dbP = NULL;
	UInt16			numRecords = 0;
	
	if ( ( !idxP ) || ( !pLog ) )
	{ 
		return ( error = dmErrInvalidParam  );
	}

	MemSet( pLog, sizeof( log_t ), 0 );

	error = OpenLogDB( &dbP, &numRecords );
	
	if ( ( error == errNone ) && ( dbP ) )
	{
		MemHandle		recordH = NULL;
		log_t*			recordP = NULL;
	
		if ( *idxP < numRecords )
		{
			recordH = DmQueryRecord( dbP, *idxP );
			
			if ( recordH )
			{		
				recordP = (log_t *) MemHandleLock( recordH );

				if ( recordP )
				{
					MemMove( pLog, recordP, sizeof( log_t ) );
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
	
} // LogGetRecordAtIdx()

/*
 * LogDB.c
 */