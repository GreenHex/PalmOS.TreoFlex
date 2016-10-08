/*
 * AttnAlerts.c
 */
 
#include "AttnAlerts.h"

// Prototypes
static void 			DrawDetail( attnType_e attnType, RectangleType rect );
static void 			DrawList( attnType_e attnType, RectangleType rect );

/*
 * DoAlert()
 */
void DoAlert( attnType_e attnType, AttnLevelType level )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		AttnForgetIt( cardNo, dbID, (UInt32) ( 1 - attnType ) );
		
		if ( !AttnUpdate( cardNo, dbID, (UInt32) attnType, NULL, NULL, NULL, NULL ) )
		{
			AttnGetAttention( cardNo, dbID, (UInt32) attnType, NULL, 
									level, kAttnFlagsUseUserSettings, 180, 3 );
		}
	}
		
	return;
	
} // DoAlert()

/*
 * ProcessAttention
 */
void ProcessAttention( AttnLaunchCodeArgsType* paramP )
{
	AttnCommandArgsType* 		argsP = (AttnCommandArgsType *)paramP->commandArgsP;
	GoToParamsType*				cmdPBP = NULL;	
	UInt16 						cardNo = 0;
	LocalID						dbID = 0;
			
	switch ( paramP->command ) 
	{
		case kAttnCommandDrawDetail:
			
			DrawDetail( (attnType_e) paramP->userData, (RectangleType) argsP->drawDetail.bounds );

			break;
			
		case kAttnCommandDrawList:
			
			DrawList( (attnType_e) paramP->userData, (RectangleType) argsP->drawDetail.bounds );
			
			break;
		
		case kAttnCommandPlaySound:
			
			SndPlaySystemSound( sndClick /* sndInfo */ /* sndAlarm */ );
			
			break;  
	
		case kAttnCommandCustomEffect:
		
			// Perform a custom attention-getting effect.
			
			break;
		
		case kAttnCommandGoThere:

			if ( !SysCurAppDatabase( &cardNo, &dbID ) )
			{
				AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );
			}
			
			cmdPBP = MemPtrNew( sizeof( GoToParamsType ) );

			if ( cmdPBP )
			{
				MemSet( cmdPBP, sizeof( GoToParamsType ), 0 );
				
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					cmdPBP->matchCustom = (UInt32)( paramP->userData );
				
					MemPtrSetOwner( cmdPBP, 0 ); // set owner to system, or it will be freed on exit...
					
					SysUIAppSwitch( cardNo, dbID, sysAppLaunchCmdNormalLaunch /* sysAppLaunchCmdGoTo */, cmdPBP );
				}
			}

			break; 
		
		case kAttnCommandGotIt:
			
			if ( argsP->gotIt.dismissedByUser )
			{
				if ( !SysCurAppDatabase( &cardNo, &dbID ) )
				{
					AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );		
				}
			}
			
			break;   
		
		case kAttnCommandIterate:

			if ( !SysCurAppDatabase( &cardNo, &dbID ) )
			{
				AttnForgetIt( cardNo, dbID, (UInt32) paramP->userData );		
			}

			break;
		
		default:
		
			break;
	}

} // ProcessAttention

/*
 * DrawDetail() // Large alert
 */
static void DrawDetail( attnType_e attnType, RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	Char					note[STRLEN_ALERT_NOTE] = "\0";
	Char					strNextAlarm[timeStringLength + longDateStrLength + 3] = "\0";
	Char					str[STRLEN_ALERT_NOTE] = "\0";
	UInt32					nextAlarm = 0;
	UInt32					ref = -1;
	WinPushDrawState();
		
	BmpP = GetBitmap( BMP_ICON_LARGE, &BmpSizeX, &BmpSizeY, &BmpH );
	WinDrawBitmap( BmpP, rect.topLeft.x + 29 - BmpSizeX, rect.topLeft.y + 8 );
	ReleaseBitmap( &BmpH );	
	BmpH = NULL;
	BmpP = NULL;
	
	FntSetFont( largeBoldFont );
	
	strHeight = FntLineHeight();
	strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );
	
	rgb.r=0; rgb.g=51; rgb.b=200; // Blue
	StrColor = WinRGBToIndex( &rgb );	
    WinSetTextColor( StrColor );
    
	WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + 34, rect.topLeft.y + 12 );

	if ( attnType == onEnable )
	{
		StrCopy( note, "ENABLED" );
	}
	else
	{
		StrCopy( note, "DISABLED" );
	}
	
	rgb.r=250; rgb.g=0; rgb.b=51; // Red
	StrColor = WinRGBToIndex( &rgb );	
    WinSetTextColor( StrColor );
	WinDrawChars( note, StrLen( note ), rect.topLeft.x + 34 + strWidth + 5, rect.topLeft.y + 12 );
	
	if ( !GetNextAlarm( &nextAlarm, &ref ) )
	{
		rgb.r=0; rgb.g=0; rgb.b=0; // Black
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );

		strHeight = FntLineHeight();
	
		MakeTimeDateStr2( nextAlarm, strNextAlarm );	    
		StrCopy( str, ( ref == 1 ) ? "Next: Enable" : "Next: Disable" );
		WinDrawChars( str, StrLen( str ), rect.topLeft.x + 34, rect.topLeft.y + 45 );
		StrCopy( str, "@ " );
		StrCat( str, strNextAlarm );
		WinDrawChars( str, StrLen( str ), rect.topLeft.x + 34, rect.topLeft.y + 45 + strHeight);
	}
			
	WinPopDrawState();
		
	return;
	
} // DrawDetail()

/*
 * DrawList() // Small alert.
 */
static void DrawList( attnType_e attnType, RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	Char					note[STRLEN_ALERT_NOTE] = "\0";
	Char					strNextAlarm[timeStringLength + longDateStrLength + 3] = "\0";
	Char					str[STRLEN_ALERT_NOTE] = "\0";
	UInt32					nextAlarm = 0;
	UInt32					ref = -1;

	WinPushDrawState();
		
	BmpP = GetBitmap( BMP_ICON_SMALL, &BmpSizeX, &BmpSizeY, &BmpH );
	WinDrawBitmap( BmpP, rect.topLeft.x + 15 - BmpSizeX, rect.topLeft.y + 1 );
	ReleaseBitmap( &BmpH );	
	BmpH = NULL;
	BmpP = NULL;
	
	FntSetFont( stdFont );
	strHeight = FntLineHeight();
	
	rgb.r=0; rgb.g=51; rgb.b=200; // Blue
	StrColor = WinRGBToIndex( &rgb );	
    WinSetTextColor( StrColor );
	
	strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );	
	WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + 19, rect.topLeft.y );

	if ( attnType == onEnable )
	{
		StrCopy( note, "ENABLED" );
	}
	else
	{
		StrCopy( note, "DISABLED" );
	}

	rgb.r=250; rgb.g=0; rgb.b=51; // Red
	StrColor = WinRGBToIndex( &rgb );	
    WinSetTextColor( StrColor );
	
	WinDrawChars( note, StrLen( note ), rect.topLeft.x + 19 + strWidth + 3, rect.topLeft.y );
	
	if ( !GetNextAlarm( &nextAlarm, &ref ) )
	{
		rgb.r=0; rgb.g=0; rgb.b=0; // Black
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
	    
	    MakeTimeDateStr2( nextAlarm, strNextAlarm );
		
		StrCopy( str, ( ref == 1 ) ? "Enable @ " : "Disable @ " );
		StrCat( str, strNextAlarm ); 
		
		WinDrawChars( str, StrLen( str ), rect.topLeft.x + 20, rect.topLeft.y + 11 );	
	}	
	
	WinPopDrawState();

	return;
	
} // DrawList()

/*
 * AttnAlerts.c
 */