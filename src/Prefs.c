/*
 * Prefs.c
 */

#include "Prefs.h"

// Prototypes
static Boolean 			PrefsFormHandleMenu( UInt16 menuID );
static void 			InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			InitializeAppPrefs( appPrefs_t* pPrefs );

/*
 * PrefsFormHandleMenu()
 */
Boolean PrefsFormHandleMenu( UInt16 menuID )
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

} // PrefsFormHandleMenu()

/*
 * PrefsFormHandleEvent()
 */
Boolean PrefsFormHandleEvent( EventType* pEvent )
{
	Boolean 				handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	appPrefs_t				prefs;
	EventType				newEvent;
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		
			ReadAppPrefs( &prefs );
		
			InitializePrefsForm( pForm, &prefs );
			
			FrmDrawForm( pForm );
			
			// FrmNavObjectTakeFocus( pForm, MAIN_OK_BTN );
			
			handled = true;
						
			break;
			
		case frmCloseEvent:
			
			CopyPrefsFromForm( pForm, &prefs );
								
			WriteAppPrefs( &prefs );
			
			RegisterForNotifications( prefs.bEnabled );
			
			handled = false;
			
			break;
			
		case popSelectEvent:
		
			if ( pEvent->data.popSelect.selection == 0 )
			{
				SetCtlBoolValue( pForm, MAIN_CONFIRM_DEFAULT_CANCEL_CBX, false );
			}
			
			CtlSetEnabled ( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_CONFIRM_DEFAULT_CANCEL_CBX ) ), ( pEvent->data.popSelect.selection > 0 ) );
					
			handled = false;
			
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case MAIN_OK_BTN:
					
					newEvent.eType = appStopEvent;
									
					EvtAddEventToQueue( &newEvent );
					
					handled = true;

					break;
					
				case MAIN_NOTIFY_CBX:
				
					FieldSetEditable( pForm, MAIN_NOTIFY_NUMBER_FLD, pEvent->data.ctlSelect.on );
					
					FldDrawField( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_NOTIFY_NUMBER_FLD ) ) );
					
					handled = true;
					
					break;
					
				case MAIN_HELP_BTN:
				
					FrmHelp( MAIN_HELP_STRING );
					
					handled = true;
					
					break;
						
				default:
					break;
			}
		
			break;
			
		case menuEvent:
										
			handled = PrefsFormHandleMenu( pEvent->data.menu.itemID );
			
		default:
								
			break;
	}
	
	return ( handled );
	
} // PrefsFormHandleEvent()

/*
 * InitializePrefsForm()
 */
static void InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs )
{		
	if ( ( !pForm ) || ( !pPrefs ) ) return;
	
	SetCtlBoolValue( pForm, MAIN_ENABLE_CBX, pPrefs->bEnabled );
	SetCtlBoolValue( pForm, MAIN_MSG_ON_IGNORE_CBX, pPrefs->bMsgCallIgnored );
	SetCtlBoolValue( pForm, MAIN_MSG_ON_MISSED_CBX,	pPrefs->bMsgCallMissed );
	SetCtlBoolValue( pForm, MAIN_USE_CONTACTS_NOTE_CBX,	pPrefs->bUseContactsNote );
	SetCtlBoolValue( pForm, MAIN_CONFIRM_DEFAULT_CANCEL_CBX, pPrefs->bDefaultCancel );
	SetCtlBoolValue( pForm, MAIN_MOBILES_ONLY_CBX, pPrefs->bMsgMobilesOnly );
	SetCtlBoolValue( pForm, MAIN_NOTIFY_CBX, pPrefs->bMsgNotify );
	SetCtlBoolValue( pForm, MAIN_ENABLE_LOGGING_PBT, pPrefs->bEnableLogging );

	SetPopLabelFromList( pForm, MAIN_CONFIRM_POP, MAIN_CONFIRM_LST, pPrefs->confirmTimeoutIdx );
	
	CtlSetEnabled( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_CONFIRM_DEFAULT_CANCEL_CBX ) ), ( pPrefs->confirmTimeoutIdx > 0 ) );
				
	SetFieldStr( pForm, MAIN_MSG_FLD, pPrefs->msg );

	SetFieldStr( pForm, MAIN_NOTIFY_NUMBER_FLD, pPrefs->notifyNumber );
	FieldSetEditable( pForm, MAIN_NOTIFY_NUMBER_FLD, pPrefs->bMsgNotify );	
			
	return;
	
} // InitializePrefsForm()

/*
 * CopyPrefsFromForm()
 */
static void CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs )
{
	
	if ( ( !pForm ) || ( !pPrefs ) ) return;

	pPrefs->bEnabled = GetCtlBoolValue( pForm, MAIN_ENABLE_CBX );
	pPrefs->bMsgCallIgnored = GetCtlBoolValue( pForm, MAIN_MSG_ON_IGNORE_CBX );
	pPrefs->bMsgCallMissed = GetCtlBoolValue( pForm, MAIN_MSG_ON_MISSED_CBX );
	pPrefs->bUseContactsNote = GetCtlBoolValue( pForm, MAIN_USE_CONTACTS_NOTE_CBX );
	pPrefs->bDefaultCancel = GetCtlBoolValue( pForm, MAIN_CONFIRM_DEFAULT_CANCEL_CBX );
	pPrefs->bMsgMobilesOnly = GetCtlBoolValue( pForm, MAIN_MOBILES_ONLY_CBX );
	pPrefs->bMsgNotify = GetCtlBoolValue( pForm, MAIN_NOTIFY_CBX );
	pPrefs->bEnableLogging = GetCtlBoolValue( pForm, MAIN_ENABLE_LOGGING_PBT );
	
	pPrefs->confirmTimeoutIdx = LstGetSelection( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_CONFIRM_LST ) ) );
	
	MakeNum( pPrefs->notifyNumber );
	GetFieldStr( pForm, MAIN_NOTIFY_NUMBER_FLD, pPrefs->notifyNumber, STRLEN_PHN_NUMBER );
	GetFieldStr( pForm, MAIN_MSG_FLD, pPrefs->msg, STRLEN_MSG );
	
	return;	

} // CopyPrefsFromForm()

/*
 * WriteAppPrefs()
 */
void WriteAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	PrefSetAppPreferences( appPrefCreator, appPrefID, appPrefVersionNum, pPrefs, sizeof( appPrefs_t ), appPrefType );
	
	return;
	
} // WriteAppPrefs()

/*
 * ReadAppPrefs()
 */
void ReadAppPrefs( appPrefs_t* pPrefs )
{
	UInt16 			prefSize = sizeof( appPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( appPrefCreator, appPrefID, pPrefs, &prefSize, appPrefType );
	
	if ( prefsVersion != appPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( appPrefCreator, appPrefID, prefsVersion, NULL, 0, appPrefType );				
	
		InitializeAppPrefs( pPrefs );
		
		WriteAppPrefs( pPrefs );
	}
	
	return;
	
} // ReadAppPrefs()

/*
 * InitializeAppPrefs()
 */
static void InitializeAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	pPrefs->bEnabled = true;
	pPrefs->bMsgCallIgnored = true;
	pPrefs->bMsgCallMissed = true;
	pPrefs->bUseContactsNote = false;
	pPrefs->bDefaultCancel = false;
	pPrefs->bMsgMobilesOnly = false;
	pPrefs->bMsgNotify = false;
	pPrefs->bEnableLogging = false;
	
	pPrefs->confirmTimeoutIdx = 1;

	StrCopy( pPrefs->notifyNumber, STR_NOTIFY_NUM_NOTE );
	StrCopy( pPrefs->msg, STR_INITIAL_MSG );
		
	return;
	
} // InitializeAppPrefs

/*
 * WriteChPrefs()
 */
void WriteChPrefs( chPrefs_t* pChPrefs )
{
	if ( !pChPrefs ) return;
	
	PrefSetAppPreferences( chPrefCreator, chPrefID, chPrefVersionNum, pChPrefs, sizeof( chPrefs_t ), chPrefType );
	
	return;
	
} // WriteChPrefs()

/*
 * ReadChPrefs()
 */
void ReadChPrefs( chPrefs_t* pChPrefs )
{
	UInt16 			prefSize = sizeof( chPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pChPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( chPrefCreator, chPrefID, pChPrefs, &prefSize, chPrefType );
	
	if ( prefsVersion != chPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( chPrefCreator, chPrefID, prefsVersion, NULL, 0, chPrefType );				
	
		InitializeChPrefs( pChPrefs );
		
		WriteChPrefs( chPrefType );
	}
	
	return;
	
} // ReadAppPrefs()

/*
 * InitializeChPrefs()
 */
void InitializeChPrefs( chPrefs_t* pChPrefs )
{
	if ( !pChPrefs ) return;
	
	pChPrefs->bIncomingCallQueued = false;
	pChPrefs->disconnectType = disconnectUnknown;
	pChPrefs->callerNumber[0] = chrNull;
	
	return;

} // InitializeChPrefs()

/*
 * Prefs.c
 */