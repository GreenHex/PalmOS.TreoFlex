/*
 * ConfirmFrm.c
 */

#include "ConfirmFrm.h"

// Prototypes
static Boolean 		ConfirmFormHandleEvent( EventType* pEvent );

/*
 * ConfirmSendMsg
 */
Boolean ConfirmSendMsg( Boolean bDefaultCancel, UInt32 timeOutSecs, Char* CallerID )
{
	Boolean 					retVal = false;
	FormType*					pOldForm = FrmGetActiveForm();
	FormType*					pForm = FrmInitForm( CONFIRM_FORM );						
	ControlType*				pCtl = NULL;
	FormActiveStateType			frmState;
	EventType					event;
	UInt32						endTime = TimGetSeconds() + timeOutSecs + 1;
	Boolean						bAttnIndicatorEnabled = AttnIndicatorEnabled();
	
	AttnIndicatorEnable( false );				
	FrmSaveActiveState( &frmState );
	
	if ( !pForm )
	{
		return retVal;
	}
		
	FrmSetActiveForm( pForm );
	FrmSetEventHandler( pForm, ConfirmFormHandleEvent );	

	FrmNavObjectTakeFocus( pForm, ( bDefaultCancel ) ? CONFIRM_CANCEL_BTN : CONFIRM_SEND_BTN );
	
	pCtl = FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, CONFIRM_NUM_LBL ) );
	if ( pCtl )
	{
		CtlSetLabel( pCtl, CallerID );
	}
	
	SndPlaySystemSound( sndClick );
					
	FrmDrawForm( pForm );
	
	do // to prevent PalmOS from getting control...
	{
		EvtGetEvent( &event, SysTicksPerSecond() );

		FrmDispatchEvent( &event );	

		if ( event.eType == ctlSelectEvent ) 
			break;
			
		if ( event.eType == keyDownEvent )
		{
			if ( ( event.data.keyDown.chr == vchrHardPower )
						|| ( event.data.keyDown.chr == vchrHard4 ) )
			{
				break;
			}
			else if ( event.data.keyDown.keyCode == vchrRockerCenter ) 
			{
				EvtGetEvent( &event, evtWaitForever );
				break;
			}
		}
			
		// if (SysHandleEvent( &event ))
		// 	continue;			
		
	} while ( TimGetSeconds() < endTime );
				
	retVal = ( FrmGetFocus ( pForm ) == FrmGetObjectIndex( pForm, CONFIRM_SEND_BTN ) );
	
	FrmEraseForm( pForm );
	FrmDeleteForm( pForm );

	FrmRestoreActiveState( &frmState );	
	AttnIndicatorEnable( bAttnIndicatorEnabled );
	
	if ( pOldForm ) FrmSetActiveForm( pOldForm );
	
	return ( retVal );
	
} // ConfirmSendMsg

/*
 * ConfirmFormHandleEvent()
 */
static Boolean ConfirmFormHandleEvent( EventType* pEvent )
{
	Boolean								handled = false;
	FormType*							pForm = FrmGetActiveForm();
//	EventType							newEvent;
	
	switch ( pEvent->eType )
	{
		case frmOpenEvent:
		
			FrmDrawForm( pForm );
	
			handled = true;
			
			break;
			
		case ctlSelectEvent:
			
			FrmNavObjectTakeFocus( pForm, pEvent->data.ctlSelect.controlID );
					
			break;
			
		case ctlEnterEvent:
		
			FrmNavObjectTakeFocus( pForm, pEvent->data.ctlSelect.controlID );
		
			break;
						
		case nilEvent:
		
			break;
					
		default:
		
			break;
	}

	return ( handled );
		
} // ConfirmFormHandleEvent
 
/*
 * ConfirmFrm.c
 */