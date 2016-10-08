/*
 * NotifyReg.c
 */
 
#include "NotifyReg.h"

/*
 * RegisterForNotifications()
 */
Err RegisterForNotifications( Boolean bRegister )
{
	Err					error = errNone;
	UInt16 				cardNo = 0; 
	LocalID  			dbID = 0;	
	UInt16 				PhoneLibRefNum = sysInvalidRefNum;
	DmSearchStateType 	stateInfo;

	if ( ( error = DmGetNextDatabaseByTypeCreator( true, &stateInfo, sysFileTPanel, appFileCreator, true, &cardNo, &dbID ) ) )
		return ( error );
	
	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) )
		return ( error );
		
	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) )
		return ( error );
		
	if ( bRegister )
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, phnServiceVoice );
		SysNotifyRegister( cardNo, dbID, chDeferredNotifyEvent, NULL, sysNotifyNormalPriority, NULL );
	}
	else
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, 0 );
		SysNotifyUnregister( cardNo, dbID, chDeferredNotifyEvent, sysNotifyNormalPriority );
	}
			
	PhnLibClose( PhoneLibRefNum );

	return ( error );
		
} // RegisterForNotifications()

/*
 * NotifyReg.c
 */