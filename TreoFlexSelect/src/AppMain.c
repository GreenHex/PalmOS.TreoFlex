/*
 * AppMain.c
 */

#include <Hs.h>
#include <HsPhone.h>
#include <Form.h>
#include <List.h>

#include "Global.h"
#include "AppResources.h"
#include "MsgsDB.h"
#include "Prefs.h"
#include "Utils.h"
#include "Alarms.h"
#include "AttnAlerts.h"
#include "LogForm.h" 
#include "../../TreoFlex2/src/Global.h"
#include "../../TreoFlex2/src/Prefs.h"
#include "../../TreoFlex2/src/NotifyReg.h"
#include "../../TreoFlex2/src/Utils.h"
#include "../../TreoFlex2/src/FormUtils.h"

// Globals
Char						gStrPopup[STRLEN_MSG];

// Prototypes...
static Err 					AppStart( void );
static void 				AppStop( void );
static void 				InitializeMainForm( FormType* pForm, appPrefs_t* pPrefs, tfsPrefs_t* pTfsPrefs );
static void					CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs );
static Boolean 				MainFormHandleMenu( UInt16 menuID );
static Boolean 				MainFormHandleEvent( EventType* pEvent );
static void 				AppEventLoop( void );
UInt32 						PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags );

/*
 * AppStart()
 */
static Err AppStart( void )
{
	gMsgsH = NULL;
	gLogsH = NULL;
			
	FrmGotoForm( MAIN_FORM );
	
	return ( errNone );
	
} // AppStart()

/*
 * AppStop()
 */
static void AppStop( void )
{
	FrmCloseAllForms();

	if ( gMsgsH )
	{
        MemHandleUnlock( gMsgsH );
        MemHandleFree( gMsgsH );
        gMsgsH = NULL;
    }
    
    if ( gLogsH )
	{
        MemHandleUnlock( gLogsH );
        MemHandleFree( gLogsH );
        gLogsH = NULL;
    }
    
    return;
    	
} // AppStop()

/*
 * InitializeMainForm()
 */
static void InitializeMainForm( FormType* pForm, appPrefs_t* pPrefs, tfsPrefs_t* pTfsPrefs )
{	
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_LST ) );
	msgsDB_t		msg;
	UInt16			idx = 0;
	Char			strTimeInterval[STRLEN_TIME_INTERVAL] = "\0";
	
	MakeTimeInterval( pTfsPrefs->startTime, pTfsPrefs->endTime, strTimeInterval );
	SetCtlLabelStr( pForm, MAIN_TIME_INTERVAL_SEL, strTimeInterval );
		
	SetCtlBoolValue( pForm, MAIN_ENABLE_CBX, pPrefs->bEnabled );
	SetCtlBoolValue( pForm, MAIN_MOBILES_ONLY_CBX, pPrefs->bMsgMobilesOnly );
	SetCtlBoolValue( pForm, MAIN_ENABLE_NOTIFY_CBX, pPrefs->bMsgNotify );
	SetCtlBoolValue( pForm, MAIN_USE_CONTACTS_NOTES_CBX, pPrefs->bUseContactsNote );
	// SetCtlBoolValue( pForm, MAIN_ENABLE_LOGGING_PBT, pPrefs->bEnableLogging );
	
	LoadMsgsList( pList, pPrefs->msg, &idx );
	
	if ( ( idx == dmMaxRecordIndex ) && ( StrLen( pPrefs->msg ) ) )
	{
		StrCopy( msg.msg, pPrefs->msg );
		MsgsSaveNewRecord( &msg, false, &idx );
		LoadMsgsList( pList, pPrefs->msg, &idx );
	}
	
	FrmDrawForm( pForm );
	
	SetPopLabelAndFieldFromList2( pForm, MAIN_MSG_POP, MAIN_MSG_LST, MAIN_MSG_FLD, idx );
	
	SetFieldStr( pForm, MAIN_MSG_FLD, LstGetSelectionText( pList, LstGetSelection( pList ) ) );
	
	return;
	
} // InitializeMainForm()

/* 
 * CopyPrefsFromForm()
 */
static void	CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs )
{
	ListType*		pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_LST ) );
	Char*			strMsg = LstGetSelectionText( pList, LstGetSelection( pList ) );
	
	pPrefs->bEnabled = GetCtlBoolValue( pForm, MAIN_ENABLE_CBX );
	pPrefs->bMsgMobilesOnly = GetCtlBoolValue( pForm, MAIN_MOBILES_ONLY_CBX );
	pPrefs->bMsgNotify = GetCtlBoolValue( pForm, MAIN_ENABLE_NOTIFY_CBX );
	pPrefs->bUseContactsNote = GetCtlBoolValue( pForm, MAIN_USE_CONTACTS_NOTES_CBX );
	// pPrefs->bEnableLogging = GetCtlBoolValue( pForm, MAIN_ENABLE_LOGGING_PBT );
	
	if ( ( strMsg ) && ( StrLen( strMsg ) ) )
	{
		StrCopy( pPrefs->msg, strMsg );
	}
	
	return;	
	
} // CopyPrefsFromForm()

/*
 * MainFormHandleMenu()
 */
static Boolean MainFormHandleMenu( UInt16 menuID )
{
	Boolean 			handled = false;
	EventType			newEvent;

	switch ( menuID )
	{
		case MAIN_MENU_EXIT:
		
			newEvent.eType = appStopEvent;
									
			EvtAddEventToQueue( &newEvent );	
					
			handled = true;
		
			break;
			
		case MAIN_MENU_VIEW_LOG:
		
			FrmPopupForm( LOG_FORM );
			
			handled = true;
			
			break;
			
		case MAIN_MENU_HELP:
		
			FrmHelp( MAIN_HELP_STRING );
			
			handled = true;
			
			break;
			
		case MAIN_MENU_ABOUT:
		
			FrmPopupForm( ABOUT_FORM );
			
			handled = true;
			
			break;
						
		default:
			break;
	}

	return ( handled );

} // MainFormHandleMenu()

/*
 * MainFormHandleEvent()
 */
static Boolean MainFormHandleEvent( EventType* pEvent )
{
	Boolean 				handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	ListType*				pList = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_LST ) );
	static TimeType 		startTime = { -1, -1 };
	static TimeType 		endTime = { -1, -1 };
	appPrefs_t				prefs;
	tfsPrefs_t				tfsPrefs;
	msgsDB_t				msg;
	Char*					strMsgP = NULL;
	UInt16					idx = 0;
	EventType				newEvent;
	static Boolean			bOldEnableStatus = false;
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		 
			ReadTfsPrefs( &tfsPrefs );
			startTime = tfsPrefs.startTime;
			endTime = tfsPrefs.endTime;
			
			ReadAppPrefs( &prefs );
			bOldEnableStatus = prefs.bEnabled;
			
			InitializeMainForm( pForm, &prefs, &tfsPrefs );
			
			FrmDrawForm( pForm );
			 
			handled = true;
						
			break;
			
		case frmCloseEvent:
			
			ReadAppPrefs( &prefs );
			CopyPrefsFromForm( pForm, &prefs );
			WriteAppPrefs( &prefs );
			
			ReadTfsPrefs( &tfsPrefs );
			tfsPrefs.startTime = startTime;
			tfsPrefs.endTime = endTime;
			WriteTfsPrefs( &tfsPrefs );
			
			RegisterForNotifications( prefs.bEnabled );
			
			// Finally, do OR undo what's done above...
			SetNextAlarm( ( bOldEnableStatus != prefs.bEnabled ), kAttnLevelSubtle );
			
			handled = false;
			
			break;
			
		case popSelectEvent:
			
			if ( pEvent->data.popSelect.controlID == MAIN_MSG_POP )
			{
				SetFieldStr( pForm, MAIN_MSG_FLD, LstGetSelectionText( pList, LstGetSelection( pList ) ) );	
				
				if ( pEvent->data.popSelect.selection != noListSelection )
				{
					ListType*	pList = pEvent->data.popSelect.listP;
					UInt16		idx = LstGetSelection( pList );
					Char*		strMsg = LstGetSelectionText( pList, idx );
					
					SetPopLabelAndFieldFromList2( pForm, MAIN_MSG_POP, MAIN_MSG_LST, MAIN_MSG_FLD, idx );
					
					handled = true;
				}
			}
			
			break;
		
		case keyUpEvent:
		case penUpEvent:
			
			SetFormState( pForm );	
			
			handled = false;
					
			break;
						
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case MAIN_VIEW_LOG_BTN:
				
					FrmPopupForm( LOG_FORM );
					
					handled = true;
					
					break;
					
				case MAIN_MSG_CLEAR_BTN:
				
					ClearFieldStr( pForm, MAIN_MSG_FLD );
					
					SetFormState( pForm );	
					
					FrmNavObjectTakeFocus( pForm, MAIN_MSG_FLD );
					
					handled = true;
					
					break;
					
				case MAIN_TIME_INTERVAL_SEL:
					{
						Boolean			bUntimed = ( ( *( (UInt16 *) &startTime ) == noTime ) || ( *( (UInt16 *) &endTime ) == noTime ) );
						
						if ( bUntimed ) 
						{
							*( (UInt16 *) &startTime ) = 0;
							*( (UInt16 *) &endTime ) = 0;
						}
						
						if ( SelectTime( (TimeType *) &startTime, (TimeType *) &endTime, bUntimed, "TreoFlex: Enable Between", 8, 18, 9 ) )
						{
							Char			strTimeInterval[STRLEN_TIME_INTERVAL] = "\0";
									
							MakeTimeInterval( startTime, endTime, strTimeInterval );
							SetCtlLabelStr( pForm, pEvent->data.ctlSelect.controlID, strTimeInterval );
							
							FrmShowObject( pForm, FrmGetObjectIndex( pForm, pEvent->data.ctlSelect.controlID ) );
							// CtlDrawControl( pEvent->data.ctlSelect.pControl );
						}
						else if ( bUntimed ) 
						{
							startTime.hours = startTime.minutes = -1;
							endTime.hours = endTime.minutes = -1;
						}
					}

					handled = true;	
					
					break;
					
				case MAIN_MSG_NEW_BTN:
					
					strMsgP = FldGetTextPtr( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
					
					if ( ( strMsgP ) && ( StrLen( strMsgP ) ) )
					{
						StrCopy( msg.msg, strMsgP );
						MsgsSaveNewRecord( &msg, false, &idx );
						
						LoadMsgsList( pList, strMsgP, &idx );
						SetPopLabelAndFieldFromList2( pForm, MAIN_MSG_POP, MAIN_MSG_LST, MAIN_MSG_FLD, idx );
					}
					
					handled = true;
					
					break;
					
				case MAIN_MSG_SAVE_BTN:
					
					strMsgP = FldGetTextPtr( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
					
					if ( ( strMsgP ) && ( StrLen( strMsgP ) ) )
					{
						if ( !FrmCustomAlert( ConfirmationOKCancelAlert, "Replace current message?", "", "" ) )
						{
							StrCopy( msg.msg, strMsgP );
							
							idx = LstGetSelection( pList );
							MsgsReplaceRecordAtIdx( &idx, &msg, false );
							
							LoadMsgsList( pList, strMsgP, &idx );
							SetPopLabelAndFieldFromList2( pForm, MAIN_MSG_POP, MAIN_MSG_LST, MAIN_MSG_FLD, idx );
						}
					}
					
					handled = true;
					
					break;
				
				case MAIN_MSG_DELETE_BTN:
				
					strMsgP = FldGetTextPtr( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_MSG_FLD ) ) );
					
					if ( ( LstGetNumberOfItems( pList ) > 1 ) && ( strMsgP ) && ( StrLen( strMsgP ) ) )
					{
						if ( StrCompare( LstGetSelectionText( pList, LstGetSelection( pList ) ), strMsgP ) )
						{
							break;
						}
							
						if ( !FrmCustomAlert( ConfirmationOKCancelAlert, "Delete current message?", "", "" ) )
						{	
							idx = LstGetSelection( pList );
							MsgsRemoveRecordAtIdx( &idx );
							
							LoadMsgsList( pList, NULL, NULL );
							SetPopLabelAndFieldFromList2( pForm, MAIN_MSG_POP, MAIN_MSG_LST, MAIN_MSG_FLD, idx );
						}
					}
					
					handled = true;
						
					break;
							
				default:
					break;
			}
		
			break;
			
		case menuEvent:
										
			handled = MainFormHandleMenu( pEvent->data.menu.itemID );
			
		default:
								
			break;
	}
	
	return ( handled );
	
} // MainFormHandleEvent()

/*
 * AppHandleEvent()
 */
static Boolean AppHandleEvent( EventType* pEvent )
{
	UInt16 		formID;
	FormType* 	pForm;
	Boolean		handled = false;
	
	if ( pEvent->eType == frmLoadEvent )
	{
		formID = pEvent->data.frmLoad.formID;
		
		pForm = FrmInitForm( formID );
		FrmSetActiveForm( pForm );
		
		switch ( formID )
		{
			case MAIN_FORM:
			
				FrmSetEventHandler( pForm, MainFormHandleEvent );
				
				handled = true;
				
				break;
				
			case LOG_FORM:
				
				FrmSetEventHandler( pForm, LogFormHandleEvent );
				
				handled = true;
				
				break;
			
			case ABOUT_FORM:
				
				FrmSetEventHandler( pForm, AboutFormHandleEvent2 );
				
				handled = true;
				
				break;
				
			default:
				break;
		}
	}
	
	return ( handled );
	
} // AppHandleEvent()

/*
 * AppEventLoop()
 */
static void AppEventLoop( void )
{
	Err			error = errNone;
	EventType	event;

	do {
		EvtGetEvent( &event, evtWaitForever );

		if ( SysHandleEvent( &event ) )
			continue;
			
		if ( MenuHandleEvent( 0, &event, &error ) )
			continue;
			
		if ( AppHandleEvent( &event ) )
			continue;

		FrmDispatchEvent( &event );

	} while ( event.eType != appStopEvent );

} // AppEventLoop()

/*
 * PilotMain()
 */
UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags )
{
	Err 						error = errNone;
	
	switch ( cmd )
	{
		case sysAppLaunchCmdNormalLaunch:
				
			if ( ( error = RomVersionCompatible( MIN_VERSION, launchFlags ) ) )
			{
				FrmCustomAlert( ErrOKAlert, "Incompatible Device", "", "" );
				return ( error );
			} 
			
			if ( !IsTreoFlexInstalled() )
			{
				FrmCustomAlert( ErrOKAlert, "TreoFlex.PRC not installed. Exiting...", "", "" );
				return ( dmErrCantFind );
			}
			
			if ( ( error = AppStart() ) == 0 )
			{
				AppEventLoop();
				AppStop();
			}

			break;
			
		case sysAppLaunchCmdSystemReset:

			if ( ( error = RomVersionCompatible( MIN_VERSION, launchFlags) ) == errNone )
			{
				SetNextAlarm( false, kAttnLevelSubtle );
			}		
			break;
			
		case sysAppLaunchCmdAlarmTriggered:
			
			((SysAlarmTriggeredParamType *) cmdPBP)->purgeAlarm 
						= ( ((SysAlarmTriggeredParamType *) cmdPBP)->ref == invalid );
			
			break;
			
		case sysAppLaunchCmdDisplayAlarm:
			
			if ( ( (SysDisplayAlarmParamType *) cmdPBP )->ref == invalid ) break; // exit we have handled this chime
			
			SetNextAlarm( true, kAttnLevelInsistent );
			
			((SysDisplayAlarmParamType*)cmdPBP)->ref = invalid;
		
			break;
			
		case sysAppLaunchCmdAttention:
		
			ProcessAttention( ( AttnLaunchCodeArgsType *) cmdPBP );
			
			break;
			
		case sysAppLaunchCmdGoTo:
						
			break;
			
		default:
			break;
	}
		
	return ( error );

} // PilotMain()

/*
 * AppMain.c
 */