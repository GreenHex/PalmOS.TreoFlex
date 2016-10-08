/*
 * Prefs.h
 */
 
#ifndef __TF_PREFS_H__
#define __TF_PREFS_H__

#include <Hs.h>
#include <Preferences.h>
#include <StringMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "NotifyReg.h"
#include "FormUtils.h"
#include "Utils.h"

#define STR_NOTIFY_NUM_NOTE 			"Fwd to..."
#define STR_INITIAL_MSG					"Sorry I missed your call. I'll get back ASAP."

typedef enum {
	prefUnsaved							= false,
	prefSaved							= true
} prefType_e;

#define appPrefCreator					appFileCreator
#define appPrefID						0x00
#define appPrefVersionNum				0x01
#define appPrefType						prefSaved						
//
#define chPrefCreator					appFileCreator
#define chPrefID						appPrefID
#define chPrefVersionNum				0x01
#define chPrefType						prefUnsaved

typedef struct {
	Boolean					bEnabled; // 1
	Boolean 				bMsgCallIgnored; // 1
	Boolean 				bMsgCallMissed; // 1
	Boolean					bMsgMobilesOnly; // 1
	UInt8					confirmTimeoutIdx; // 2
	Boolean					bDefaultCancel; // 1
	Boolean 				bUseContactsNote; // 1
	Boolean					bMsgNotify; // 1
	Char					notifyNumber[STRLEN_PHN_NUMBER]; // 20
	Char 					msg[STRLEN_MSG]; // 160
	Boolean					bEnableLogging;
} appPrefs_t;

/*
 * Miscellaneous stuff
 */
typedef enum {
	disconnectUnknown		= 0, // also indicates "Notify"
	disconnectNear 			= 1,
	disconnectFar 			= 2
} callDisconectType_e;

typedef struct {
	Boolean					bIncomingCallQueued;
	callDisconectType_e		disconnectType;
	Char					callerNumber[STRLEN_PHN_NUMBER];
} chPrefs_t;

// Prototypes
extern Boolean 				PrefsFormHandleEvent( EventType* pEvent );
extern void 				WriteAppPrefs( appPrefs_t* pPrefs );
extern void 				ReadAppPrefs( appPrefs_t* pPrefs );
extern void					WriteChPrefs( chPrefs_t* pChPrefs );
extern void					ReadChPrefs( chPrefs_t* pChPrefs );
extern void 				InitializeChPrefs( chPrefs_t* pChPrefs );

#endif /* __TF_PREFS_H__ */
 
/*
 * Prefs.h
 */