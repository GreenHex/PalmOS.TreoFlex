/*
 * Utils.c
 */
 
#include "Utils.h"

/*
 * GetPopStr()
 */
Char* GetPopStr( Char* s )
{
	Char 			c;
	Char*			m = s;
	UInt16			strLen = StrLen( s );
	Int16			strWidth = 0;
	
	WinPushDrawState();
	FntSetFont( largeBoldFont );
	
	while ( ( c = *s++ ) )
	{
		strWidth += FntCharWidth( c );
		
		if ( strWidth > MAX_POP_PIXEL_WIDTH )
		{
			*s = chrNull;

			if ( strLen > StrLen( m ) )
			{
				*s = 0x0085;
				*(++s) = chrNull;
			}

			break;
		}
	}
	
	WinPopDrawState();
						
	return ( s );
	 
} // GetPopStr()

/*
 * ReplaceChar()
 */
Char* ReplaceChar2( unsigned char *s, char orgChr, char newChr )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( c == orgChr )
			*d++ = newChr;
		else
			*d++ = c;
	}
					
	*d = 0;
	
	return ( s );
	
} // ReplaceChar()

/*
 * StripSpace()
 */
Char* StripSpaces2( unsigned char *s )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( c != chrSpace )
		{
			*d++ = c;
		}
	}
					
	*d = 0;
	
	return ( s );
	
} // StripSpaces()

/*
 * SetFormState()
 */
void SetFormState( FormType* pForm )
{
	MemHandle 			fldH = FldGetTextHandle( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
	Char*				fldP = NULL;
	Boolean				bNewState = false; // TRUE if the field is NOT EMPTY
	
	if ( fldH ) 
	{
		fldP = MemHandleLock( fldH );
		bNewState = ( ( fldP ) && ( StrLen( fldP ) ) );
	}
	else
	{
		bNewState = false;
	}
	
	if ( bNewState )
	{
		FrmHideObject( pForm, FrmGetObjectIndex( pForm, BMP_ENTER_TXT ) );
		FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_CLEAR_BTN ) ), true );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_NEW_BTN ) ), true );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_SAVE_BTN ) ), true );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_DELETE_BTN ) ), true );
	}
	else
	{
		FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
		FrmShowObject( pForm, FrmGetObjectIndex( pForm, BMP_ENTER_TXT ) );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_CLEAR_BTN ) ), false );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_NEW_BTN ) ), false );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_SAVE_BTN ) ), false );
		CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_DELETE_BTN ) ), false );
	}
			
	if ( fldP ) MemHandleUnlock( fldH );

} // SetFormState()

/*
 * SetPopLabelAndFieldFromList2()
 */
void SetPopLabelAndFieldFromList2( FormType* pForm, UInt16 popID, UInt16 lstID, UInt16 fldID, UInt16 idx )
{
	ControlType*	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, popID ) );
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, lstID ) );
	
	if ( ( pCtl ) && ( pList ) )
	{
		LstSetSelection( pList, ( LstGetNumberOfItems( pList ) > idx ) ? idx : 0 );
		
		StrCopy( gStrPopup, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
		GetPopStr( gStrPopup );
		
		CtlSetLabel( pCtl, gStrPopup );
		
		SetFieldStr( pForm, fldID, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
		
		FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, fldID ) ) );
	}

	return;
	
} // SetPopLabelAndFieldFromList2()

/*
 * IsTreoFlexInstalled()
 */
Boolean IsTreoFlexInstalled( void )
{
	DmSearchStateType 		stateInfo;
	UInt16					cardNum = 0;
	LocalID					dbID = 0;
			
	return ( DmGetNextDatabaseByTypeCreator( true, &stateInfo, sysFileTPanel, appFileCreator, true, &cardNum, &dbID ) == errNone );
						
} // IsTreoFlexInstalled()

/*
 * AboutFormHandleEvent2()
 */
Boolean AboutFormHandleEvent2( EventType* pEvent )
{
	Boolean 			handled = false;
	FormType*			pForm = FrmGetActiveForm();
	MemHandle			strVerH = NULL;
	Char*				strVerP = NULL;
	Char				strVer[20];
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		
			strVerH = DmGet1Resource('tver', 1000);
	
			if ( strVerH )
			{
				strVerP = MemHandleLock( strVerH );
		
				if ( strVerP )
				{
						StrCopy( strVer, "v" );
						StrCat( strVer, strVerP );

						strVer[StrLen( (Char *)FrmGetLabel( pForm, ABOUT_VER_LBL ) )] = chrNull;
						
						FrmCopyLabel( pForm, ABOUT_VER_LBL, strVer );
			
						MemHandleUnlock( strVerH );
				}
	   	
			   	DmReleaseResource( strVerH );
			   	strVerH = NULL;
			}
		
			FrmDrawForm( pForm );
			
			handled = true;
						
			break;
		
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case ABOUT_OK_BTN:
				
					FrmReturnToForm( 0 );
					
					handled = true;

					break;
					
				default:
					break;
			}
		
			break;
			
		default:
							
			break;
	}
	
	return ( handled );
	
} // AboutFormHandleEvent2()

/*
 * MakeTimeInterval()
 */
void MakeTimeInterval( TimeType startTime, TimeType endTime, Char* strTimeInterval )
{
	Char					strTime[timeStringLength] = "\0";
	
	if ( ( *( (UInt16 *) &startTime ) == noTime ) || ( *( (UInt16 *) &endTime ) == noTime ) )
	{
		StrCopy( strTimeInterval, "No Time" );
	}
	else
	{
		TimeToAscii( startTime.hours, startTime.minutes, PrefGetPreference( prefTimeFormat ), strTime );
		StrCopy( strTimeInterval, strTime );
		
		StrCat( strTimeInterval, " - " );
	
		TimeToAscii( endTime.hours, endTime.minutes, PrefGetPreference( prefTimeFormat ), strTime );
		StrCat( strTimeInterval, strTime );
	}
	
	return;
	
} // MakeTimeInterval()

/*
 * MakeTimeDateStr2()
 */
void MakeTimeDateStr2( UInt32 timeSecs, Char* strTimeDateP )
{
	DateTimeType 			dt;
	DateType				d;
	TimeFormatType			tfmt = PrefGetPreference( prefTimeFormat );
	DateFormatType 			dfmt = PrefGetPreference( prefDateFormat );
	Char					strTime[timeStringLength] = "\0";
	Char					strDate[longDateStrLength] = "\0";
	
	// Time
	TimSecondsToDateTime( timeSecs, &dt );
	TimeToAscii( dt.hour, dt.minute, tfmt, strTime );
    ReplaceChar2( strTime, 'm', chrNull );
	StripSpaces2( strTime );

	// Date
	DateSecondsToDate( timeSecs, &d );
	DateToAscii( d.month, d.day, d.year + 1904, dfmt, strDate );
	
	if ( strTimeDateP )
	{	 
		StrCopy( strTimeDateP, strTime );
		StrCat( strTimeDateP, " " );
		StrCat( strTimeDateP, strDate );
	}
		
	return;
		
} // MakeTimeDateStr2()

/*
 * GetNextAlarm()
 */
Err GetNextAlarm( UInt32* nextAlmSecP, UInt32* refP )
{
	Err					error = errNone;
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	
	if ( ( !nextAlmSecP ) || ( !refP ) ) return ( 1 );
	
	error = SysCurAppDatabase( &cardNo, &dbID );
	
	if ( error == errNone )
	{
		*nextAlmSecP = AlmGetAlarm( cardNo, dbID, refP );	
	}

	return ( error );
	
} // GetNextAlarm()

/*
 * DialPhoneLaunch()
 */
Err DialPhoneLaunch( Char* number, Char* name )
{
	Err								error = errNone;
	PhoneAppLaunchCmdDialType* 		dialP;
	Char*							phNumber;
	Char*							phName = MemPtrNew( StrLen( name ) + 1 );
	DmSearchStateType 				stateInfo;
   	UInt16 							cardNo = 0;
   	LocalID							dbID = 0;
	
	
	dialP = MemPtrNew( sizeof( PhoneAppLaunchCmdDialType ) );
	
	if ( dialP )
	{
		MemSet( dialP, sizeof( PhoneAppLaunchCmdDialType ), 0 );
	
		dialP->version = 1;
	    dialP->confirm = false;
	    dialP->extraDigits = NULL;
	    dialP->failLaunchCreator = appFileCreator;
	    dialP->failLaunchCode = sysAppLaunchCmdNormalLaunch;
	    dialP->failLaunchParams = 0;
	    dialP->dialMethod = PhoneAppDialMethodNormal;
	    
		if ( number )
		{
			dialP->number = MemPtrNew( StrLen( number ) + 1 );
			StrCopy( dialP->number, number );
			MemPtrSetOwner( dialP->number, 0 );
		}
		
		if ( name )
		{
			dialP->name = MemPtrNew( StrLen( name ) + 1 );
			StrCopy( dialP->name, name );
			MemPtrSetOwner( dialP->name, 0 );
		}	
	
	   MemPtrSetOwner ( dialP, 0 );
	    
	    // LaunchWithCommand( sysFileTApplication, hsFileCPhone, phoneAppLaunchCmdDial, &dial);
	   DmGetNextDatabaseByTypeCreator ( true, &stateInfo, sysFileTApplication, hsFileCPhone, true, 
											&cardNo, &dbID );
										   
		error = SysUIAppSwitch( cardNo, dbID, phoneAppLaunchCmdDial, dialP );
	}
	
	return ( error );
		
} // DialPhoneLaunch()

/*
 * Utils.c
 */