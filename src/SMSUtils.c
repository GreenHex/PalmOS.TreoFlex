/*
 * SMSUtils.c
 */

#include "SMSUtils.h"

/*
 * SendMessage
 */
Err SendMessage( msg_t* msgP, ProgressPtr pProgress, Char* strErrorP )
{
	Err					error = phnErrUnknownError;
	Char				strError[STRLEN_ERROR] = ERR_STR_SMS_ERR;
	UInt32 				msgID = 0;
	PhnAddressList 		addList;
  	PhnAddressHandle 	hAddress;
  	DmOpenRef 			smsRefNum = 0;      // CDMA???
  	UInt16				PhoneLibRefNum = sysInvalidRefNum;
  	
	if ( HsGetPhoneLibrary( &PhoneLibRefNum ) )
		return false;

	if ( PhnLibOpen( PhoneLibRefNum ) )
		return false;
		
	if ( PhnLibModulePowered( PhoneLibRefNum ) != phnPowerOn )
	{
		StrCopy( strError, ERR_STR_PHN_OFF );
	  	goto CloseAndRelease;
	}
	
	if ( !PhnLibRegistered( PhoneLibRefNum ) )
	{
		StrCopy( strError, ERR_STR_NETWORK_ERR );
	  	goto CloseAndRelease;
	}
	
	smsRefNum = PhnLibGetDBRef( PhoneLibRefNum );    // CDMA???

	msgID = PhnLibNewMessage( PhoneLibRefNum, kMTOutgoing );
  	if ( !msgID )
  	{
  		StrCopy( strError, ERR_STR_SMS_CREATE_ERR );
		goto SendMessage_CloseAndRelease;
  	}
  	
  	error = PhnLibSetOwner( PhoneLibRefNum, msgID, appFileCreator );
	error |= PhnLibSetDate( PhoneLibRefNum, msgID, TimGetSeconds() );
  	error |= PhnLibSetText( PhoneLibRefNum, msgID, msgP->Message, (short) StrLen( msgP->Message ) );
  	
  	// fill in the address
  	addList = PhnLibNewAddressList( PhoneLibRefNum );
  	if ( !addList )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
		goto SendMessage_CloseAndRelease;
  	}
  		
  	hAddress = PhnLibNewAddress( PhoneLibRefNum, msgP->Address, phnLibUnknownID );
  	if ( !hAddress )
	{
		StrCopy( strError, ERR_STR_ADDRESS_ERR );
		goto AddressErr_FreeAndRelease;
	}
			
  	if ( ( error = PhnLibAddAddress( PhoneLibRefNum, addList, hAddress ) ) )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
  		goto AddressErr_FreeAndRelease;
  	}
  	
  	MemHandleFree( hAddress );
  	
  	if ( ( error = PhnLibSetAddresses( PhoneLibRefNum, msgID, addList ) ) )
  	{
  		StrCopy( strError, ERR_STR_ADDRESS_ERR );
  		goto AddressErr_FreeAndRelease;
  	}	
	
	if ( pProgress )
	{
		PrgUpdateDialog( pProgress, 0, 1, msgP->CallerID, true );
	}

  	if ( ( error = PhnLibSendMessage( PhoneLibRefNum, msgID, true ) ) )
  	{
		StrCopy( strError, ERR_STR_SMS_ERR );
	}
	else
	{
		StrCopy( strError, ERR_STR_MESSAGE_SENT );
	}
  
AddressErr_FreeAndRelease:
	PhnLibDisposeAddressList( PhoneLibRefNum, addList );
	
SendMessage_CloseAndRelease:
	PhnLibReleaseDBRef( PhoneLibRefNum, smsRefNum ); // CDMA???

CloseAndRelease:
	
	PhnLibClose( PhoneLibRefNum );
	
	if ( pProgress )
	{
		if ( error == errNone )
		{
			PrgUpdateDialog( pProgress, 0, 2, msgP->CallerID, true ); // it's done			
		}
		else
		{
			PrgUpdateDialog( pProgress, error, 3, strError, true ); // some error
		}
	}
	
	if ( strErrorP )
	{
		StrCopy( strErrorP, strError );	
	}
	
	return ( error );
	
} // SendMessage

/*
 * SMSUtils.c
 */
