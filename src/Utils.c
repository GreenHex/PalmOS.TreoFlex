/*
 * Utils.c
 */
 
#include "Utils.h"

/*
 * MakeTimeDateStr()
 */
void MakeTimeDateStr( UInt32 timeSecs, Char** strTimeDatePP, Char* strTimeP, Char* strDateP, Boolean bRemoveYear )
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
    // ReplaceChar( strTime, 'm', chrNull );
	// StripSpaces( strTime );
	
	if ( strTimeP )
	{
		StrCopy( strTimeP, strTime );	
	}

	// Date
	DateSecondsToDate( timeSecs, &d );
	DateToAscii( d.month, d.day, d.year + 1904, dfmt, strDate );

	if ( bRemoveYear )
	{
		if ( ( dfmt == dfYMDWithSlashes ) 
			|| ( dfmt == dfYMDWithDots ) 
			|| ( dfmt == dfYMDWithDashes ) )
		{
			MemMove( strDate, strDate + 3, StrLen( strDate ) );
		}
		else
		{
			strDate[StrLen(strDate) - 3] = chrNull;
		}		
	}
	
	if ( strDateP )
	{
		StrCopy( strDateP, strDate );	
	}
	
	if ( strTimeDatePP )
	{
		if ( !( *strTimeDatePP ) ) *strTimeDatePP = MemPtrNew( timeStringLength + longDateStrLength + 3 );
		
		if ( *strTimeDatePP )
		{
			StrCopy( *strTimeDatePP, strTime );
			StrCat( *strTimeDatePP, " " );
			StrCat( *strTimeDatePP, strDate );
		}
	}
		
	return;
		
} // MakeTimeDateStr()

/*
 * MakeNum
 */
Char* MakeNum( unsigned char* s )
{
	unsigned char c, *d = s;
	
	while ( ( c = *s++ ) )
	{
		if ( TxtCharIsDigit( c ) )
		{
			*d++ = c;
		}
	}
					
	*d = 0;
	
	return ( s );
	
} // MakeNum

/*
 * ReplaceChar
 */
Char* ReplaceChar( unsigned char *s, char orgChr, char newChr )
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
	
} // ReplaceChar

/*
 * StripSpace
 */
Char* StripSpaces( unsigned char *s )
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
	
} // StripSpaces

/* 
 * IsPhoneReady()
 */
Boolean IsPhoneReady( void )
{
	Boolean			retVal = false;
	UInt16 			phnLibRefNum = sysInvalidRefNum;
	
	if ( HsGetPhoneLibrary( &phnLibRefNum ) )
		return ( retVal );
					
	if ( PhnLibOpen( phnLibRefNum ) )
		return ( retVal );
		
	if ( PhnLibModulePowered( phnLibRefNum ) == phnPowerOn )
	{
		retVal = PhnLibRegistered( phnLibRefNum );
	}
		
	PhnLibClose( phnLibRefNum );
	
	return ( retVal );
	
} // IsPhoneReady()

/*
 * IsPhoneGSM()
 */
Boolean IsPhoneGSM( void )
{
  	UInt32 phnType = hsAttrPhoneTypeGSM; 
  
  	HsAttrGet( hsAttrPhoneType, 0, &phnType );
  	
  	return ( phnType == hsAttrPhoneTypeGSM );

} // IsPhoneGSM()

/*
 * RomVersionCompatible()
 */
Err RomVersionCompatible( UInt32 requiredVersion, UInt16 launchFlags )
{	
	UInt32 	rom = 0;
	UInt32 	hal = 0;
	UInt32	company = 0;
	UInt32	device = 0;
	UInt32	hsFtrVersion;

	if( FtrGet( hsFtrCreator, hsFtrIDVersion, &hsFtrVersion ) != 0 )
	{
		return ( 1 );
	}
	
    FtrGet( sysFtrCreator, sysFtrNumOEMHALID, &hal );
    FtrGet( sysFtrCreator, sysFtrNumOEMCompanyID, &company );
    FtrGet( sysFtrCreator, sysFtrNumOEMDeviceID, &device );
	FtrGet( sysFtrCreator, sysFtrNumROMVersion, &rom );
		
	if (rom >= requiredVersion
		&&	(company == kPalmCompanyIDPalm
			|| company == kPalmCompanyIDOldPalm
			|| kPalmCompanyIDHandspring)

		&&	(device == kPalmOneDeviceIDTreo650
			|| device == kPalmOneDeviceIDTreo650Sim

			|| device == kPalmOneDeviceIDTreo600
			|| device == kPalmOneDeviceIDTreo600Sim

			|| device == 'D053' // Treo 680
			|| device == 'H104'

			|| device == 'D052' // Treo 700p
			|| device == 'H103'

			|| device == 'D060' // Treo 755p
			|| device == 'H104'
			
			|| device == 'D061' // Centro ???
			|| device == 'D062' // Centro
			)
			
		&&	(hal == hsHALIDHandspringOs5Rev2 // Treo 650
			|| hal == hsHALIDHandspringOs5Rev2Sim

			|| hal == hsHALIDHandspringOs5Rev1 // Treo 600
			|| hal == hsHALIDHandspringOs5Rev1Sim

			|| hal == hsHALIDHandspringOs5Rev4 // Treo 680, 755p
			|| hal == hsHALIDHandspringOs5Rev4Sim

			|| hal == hsHALIDHandspringOs5Rev3 // Treo 700p
			|| hal == hsHALIDHandspringOs5Rev3Sim
			
			|| hal == 'P052' // Centro ???
			|| hal == 'P053' // Centro
		) )
	{
		return 0;
	}

	if ((launchFlags & LAUNCH_FLAGS) == LAUNCH_FLAGS)
	{
		// Pilot 1.0 will continuously relaunch this app unless we switch to 
		// another safe one.
		if ( rom < sysMakeROMVersion( 2, 0, 0, sysROMStageRelease, 0 ) )
			AppLaunchWithCommand( sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL );
	}

	return ( sysErrRomIncompatible );

} // RomVersionCompatible()

/*
 * AboutFormHandleEvent()
 */
Boolean AboutFormHandleEvent( EventType* pEvent )
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
						StrCat( strVer, " [Freeware]" );
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
	
} // AboutFormHandleEvent()

/*
 * Utils.c
 */
