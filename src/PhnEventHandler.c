/*
 * PhnEventHandler.c
 */
 
#include "PhnEventHandler.h"
 
/*
 * HandlePhnEvent()
 */
Err HandlePhnEvent( PhnEventType* phnEventP )
{
	Err							error = errNone;
	chPrefs_t					chPrefs;
	SysNotifyParamType* 		chNotifyParamP = NULL;
	UInt16						phnLibRefNum = sysInvalidRefNum;
	
	switch ( phnEventP->eventType )
	{
		case phnEvtStartDial:
		case phnEvtConnectConf: // 0x0009 // other end accepted call // acknowledge = false
		case phnEvtConference: // 0x000F // Call goes into conference (is this required?)
		case phnEvtUpdate: // 0x000E // Accept an incoming call
		// case phnEvtDisconnectConf: // 0x000C // conference call disconnected (one of remote party disconnects) Amazing!
			
			InitializeChPrefs( &chPrefs );
			WriteChPrefs( &chPrefs );
			
			phnEventP->acknowledge = ( phnEventP->eventType != phnEvtConnectConf );
			
			break;
			
		case phnEvtSubscriber: // 0x000A, we use it now ONLY to capture the Caller ID.
			
			if ( !HsGetPhoneLibrary( &phnLibRefNum ) )
			{
				if ( !PhnLibOpen( phnLibRefNum ) )
				{	
					char* 		callerNum = NULL;
				
					if ( IsPhoneGSM() ) // GSM
					{
						callerNum = PhnLibGetField( phnLibRefNum,
														(PhnAddressHandle) phnEventP->data.info.caller, phnAddrFldPhone );
					}
					else // CDMA
					{
						callerNum = PhnLibAPGetField( phnLibRefNum,
														(PhnAddressHandle) phnEventP->data.info.caller, phnAddrFldPhone );
					}
					
					InitializeChPrefs( &chPrefs );
					
					if ( ( callerNum ) && StrLen( callerNum ) )
					{
						StrCopy( chPrefs.callerNumber, callerNum );
						MakeNum( (char *) &( chPrefs.callerNumber ) ); // remove unwanted characters
						chPrefs.bIncomingCallQueued = ( StrLen( chPrefs.callerNumber ) > 0 );										
					}
					
					WriteChPrefs( &chPrefs );
					
					PhnLibClose( phnLibRefNum );
				}	
			}

			phnEventP->acknowledge = true;
			
			break;
	
		case phnEvtDisconnectInd: // 0x000B // disconnected at the other end
		case phnEvtDisconnectConf: // 0x000C // conference call disconnected (one of remote party disconnects) Amazing!
		case phnEvtBusy: // 0x000D // phone disconnect (Ignore, red-button) sends this notification, don't know why!
			// 1. Normal, disconnect by caller w/o phone accept
			// 2. also on time-out
			// 3. normal disconnect on outgoing call.
			
			ReadChPrefs( &chPrefs );
			
			if ( chPrefs.bIncomingCallQueued )
			{	
				chPrefs.bIncomingCallQueued = false; // reset back to ignoring stuff... just to be safe
				chPrefs.disconnectType = ( phnEventP->eventType == phnEvtBusy) ? disconnectNear : disconnectFar;
				
				WriteChPrefs( &chPrefs );
				
				chNotifyParamP = MemPtrNew( sizeof( SysNotifyParamType ) );
				
				if ( chNotifyParamP )
				{
					MemSet( chNotifyParamP, sizeof( SysNotifyParamType ), 0 );
					
					chNotifyParamP->notifyType = chDeferredNotifyEvent;
					chNotifyParamP->broadcaster = appFileCreator;
					chNotifyParamP->notifyDetailsP = &chPrefs;
					chNotifyParamP->handled = false;
						
					SysNotifyBroadcastDeferred( chNotifyParamP, sizeof( chPrefs_t ) );
					
					MemPtrFree( chNotifyParamP );
				}
			}
			
			phnEventP->acknowledge = true; // NEW!!!
			
			break;
	
		default:
			break;
	}
	
	return ( error );
	
} // HandlePhnEvent()

/*
 * HandleChEvent()
 */
Err HandleChEvent( SysNotifyParamType* notifyParamP, appPrefs_t* pPrefs )
{
	Err							error = errNone;
	msg_t						msg;
	chPrefs_t*					chNotifyPrefsP = (chPrefs_t *) notifyParamP->notifyDetailsP;
	Char						name[STRLEN_CLIR]="\0";
	Char						note[STRLEN_MSG]="\0";
	ProgressPtr					pProgress = NULL;
	UInt32						timeout[4] = { 0, 5, 10, 15 };
	Boolean						bSendMsg = false; // Used TWICE
	Char*						strDateTimeP = NULL;
	Char						strError[STRLEN_ERROR] = "\0";
	
	if ( notifyParamP->notifyType != chDeferredNotifyEvent ) return ( error );
		
	if ( !( ( ( pPrefs->bMsgCallIgnored ) && ( chNotifyPrefsP->disconnectType == disconnectNear ) )
				|| ( ( pPrefs->bMsgCallMissed ) && ( chNotifyPrefsP->disconnectType == disconnectFar ) ) ) )
	{
		return ( error );
	}
	
	StrCopy( msg.Address, chNotifyPrefsP->callerNumber );
	StrCopy( msg.Message, pPrefs->msg );
	
	if ( pPrefs->bMsgMobilesOnly )
	{
		// Get only the lable
		if ( CallerLookup( chNotifyPrefsP->callerNumber, STR_LOOKUP_CALLER_LABEL_FMT, name, STRLEN_CLIR, NULL, NULL ) )
		{	
			bSendMsg = !( StrCompare( name, STR_MOBILE_LABEL ) );
		}
	}
	else
	{
		bSendMsg = true;
	}
	
	if ( bSendMsg )
	{
		if ( CallerLookup( chNotifyPrefsP->callerNumber, STR_LOOKUP_CALLER_NAME_FMT, name, STRLEN_CLIR, note, STRLEN_MSG ) )
		{
			StrCopy( msg.CallerID, name );
			
			if ( ( pPrefs->bUseContactsNote ) && ( note ) && ( StrLen(note) ) )
			{
				StrCopy( msg.Message, note );
			}							
		}
		else
		{
			StrCopy( msg.CallerID, chNotifyPrefsP->callerNumber );
		}
		
		if ( !( StrLen( msg.Message ) ) )
		{
			StrCopy( msg.Message, STR_INITIAL_MSG );
		}
		
		ReplaceDateTimeTokens( msg.Message, STRLEN_MSG );
		 
		if ( pPrefs->confirmTimeoutIdx )
		{
			bSendMsg = ConfirmSendMsg( pPrefs->bDefaultCancel, timeout[pPrefs->confirmTimeoutIdx], msg.CallerID );	
		}
		else
		{
			bSendMsg = true;
		}
		
		if ( bSendMsg )
		{
			pProgress = PrgStartDialog( APP_NAME, MsgProgressCallback, NULL );
			if ( pProgress )
			{
				PrgUpdateDialog( pProgress, 0, 0, msg.CallerID, true );
			}
			
			error = SendMessage( &msg, pProgress, strError );
			
			if ( pPrefs->bEnableLogging )
			{
				SaveLog( error, strError, chNotifyPrefsP->disconnectType, &msg );
			}
		}
	}
		
	// Notify msg
	if ( ( pPrefs->bMsgNotify ) && ( StrLen( pPrefs->notifyNumber ) ) ) // we already have name and number...
	{
		if ( CallerLookup( chNotifyPrefsP->callerNumber, STR_LOOKUP_CALLER_NAME_FMT, name, STRLEN_CLIR, note, STRLEN_MSG ) )
		{
			StrCopy( msg.CallerID, name );	
		}
		else
		{
			StrCopy( msg.CallerID, chNotifyPrefsP->callerNumber );
		}
			
		MakeTimeDateStr( TimGetSeconds(), &strDateTimeP, NULL, NULL, false );
		
		StrCopy( msg.Message, "TreoFlex: " );
		StrCat( msg.Message, msg.Address );
		if ( StrCompare( msg.Address, msg.CallerID ) )
		{
			StrCat( msg.Message, " [" );
			StrCat( msg.Message, msg.CallerID );
			StrCat( msg.Message, "]" );
		}
		StrCat( msg.Message, " at " );
		StrCat( msg.Message, strDateTimeP );
		
		StrCopy( msg.Address, pPrefs->notifyNumber );
		StrCopy( msg.CallerID, "Call Notification" );

		if ( !( pProgress ) )
		{
			pProgress = PrgStartDialog( APP_NAME, MsgProgressCallback, NULL ); 
		}
		
		if ( pProgress )
		{
			PrgUpdateDialog( pProgress, 0, 0, "Call Notification", true );
		}
		
		error = SendMessage( &msg, pProgress, strError );
		
		if ( pPrefs->bEnableLogging )
		{
			SaveLog( error, strError, 0, &msg );
		}
	}
	
	if ( pProgress ) 
	{
		PrgHandleEvent(pProgress, NULL);
		PrgStopDialog( pProgress, true );
		pProgress = NULL;										
	}
	
	if ( strDateTimeP )
	{
		MemPtrFree( strDateTimeP );	
	}

	notifyParamP->handled = true;
	
	return ( error );
	
} // HandleChEvent()

/*
 * PhnEventHandler.c
 */