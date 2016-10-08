/*
 * AppMain.c
 */
 
#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <Form.h>
#include <Window.h>

#include "Global.h"
#include "AppResources.h"
#include "Utils.h" 
#include "Prefs.h"
#include "NotifyReg.h"
#include "PhnEventHandler.h"

// Prototypes
static Err 					AppStart( void );
static void 				AppStop( void );
static Boolean 				AppHandleEvent( EventType* pEvent );
static void 				AppEventLoop( void );
UInt32 						PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags );

/*
 * AppStart()
 */
static Err AppStart( void )
{
	FrmGotoForm( MAIN_FORM );
	
	return ( errNone );

} // AppStart()

/*
 * AppStop()
 */
static void AppStop( void )
{
	FrmCloseAllForms();
	
	return;
		
} // AppStop()

/*
 * AppHandleEvent()
 */
static Boolean AppHandleEvent( EventType* pEvent )
{
	Boolean			handled = false;
	FormType* 		pForm = NULL;

	if ( pEvent->eType == frmLoadEvent )
	{		
		pForm = FrmInitForm( pEvent->data.frmLoad.formID );
		
		if ( !pForm ) return ( handled );
		
		FrmSetActiveForm( pForm );

		switch ( pEvent->data.frmLoad.formID )
		{
			case MAIN_FORM:
				
				FrmSetEventHandler( pForm, PrefsFormHandleEvent );
				
				break;
				
			case ABOUT_FORM:
			
				FrmSetEventHandler( pForm, AboutFormHandleEvent );
				
				break;
				
			default:
				break;
		}
			
		handled = true;
	}
	
	return handled;
	
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

	return;
	
} // AppEventLoop()

/*
 * PilotMain
 */
UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags )
{
	Err 						error = errNone;
	appPrefs_t 					prefs;
	
	switch ( cmd )
	{
		case phnLibLaunchCmdEvent:
		
			ReadAppPrefs( &prefs );
			
			if ( ( prefs.bEnabled ) 
					&& ( ( prefs.bMsgCallIgnored ) || ( prefs.bMsgCallMissed ) || ( prefs.bMsgNotify ) ) )
			{
				error = HandlePhnEvent( (PhnEventType *) cmdPBP );
			}
			
			break;
			
		case sysAppLaunchCmdNotify:
			
			ReadAppPrefs( &prefs );
			
			error = HandleChEvent( (SysNotifyParamType *) cmdPBP, &prefs );
			
			break;
			
		case sysAppLaunchCmdSystemReset:

			if ( ( error = RomVersionCompatible(MIN_VERSION, launchFlags) ) == errNone )
			{
				ReadAppPrefs( &prefs );
				RegisterForNotifications( prefs.bEnabled );
			}		
			break;
			
		case sysAppLaunchCmdNormalLaunch:
	    case sysAppLaunchCmdPanelCalledFromApp:
	    case sysAppLaunchCmdReturnFromPanel:

			if ( ( error = RomVersionCompatible(MIN_VERSION, launchFlags) ) )
			{
				FrmCustomAlert( ErrOKAlert, "Incompatible Device", "", "" );
				return ( error );
			} 

			if ( ( error = AppStart() ) == errNone )
			{
				AppEventLoop();
				AppStop();
			}

			break;

		default:
			break;
	}

	return error;
}

/*
 * AppMain.c
 */
