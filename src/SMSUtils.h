/*
 * SMSUtils.h
 */
#ifndef __SMSUTILS_H__
#define __SMSUTILS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <Progress.h>

#include "Global.h"
#include "AppResources.h"
#include "DispProgress.h"

// Defines
#define STRLEN_ERROR						32
#define ERR_STR_PHN_OFF						"Phone Off"
#define ERR_STR_NETWORK_ERR					"Network Error"
#define ERR_STR_SMS_CREATE_ERR				"Message Create Error"
#define ERR_STR_ADDRESS_ERR					"Address Error"
#define ERR_STR_SMS_ERR						"SMS Error"
#define ERR_STR_MESSAGE_SENT				"Message Sent"

typedef struct {
	char 			Address[STRLEN_PHN_NUMBER]; 		// Phone number
	char 			Message[STRLEN_MSG]; 				// default text
	char			CallerID[STRLEN_CLIR]; 				// the Caller NAME or NUMBER
} msg_t;

// Prototypes	
extern Err 			SendMessage( msg_t* msgP, ProgressPtr pProgress, Char* strError );

#endif /* __SMSUTILS_H__ */

/*
 * SMSUtils.h
 */
 