/*
 * PhnEventHandler.h
 */
 
#ifndef __PHNEVENTHANDLER_H__
#define __PHNEVENTHANDLER_H__

#include <Hs.h>
#include <HsPhone.h>

#include "Global.h"
#include "Prefs.h"
#include "CallerLookup.h"
#include "SMSUtils.h"
#include "ConfirmFrm.h"
#include "DispProgress.h"
#include "LogDB.h"

// Prototypes
extern Err 				HandlePhnEvent( PhnEventType* phnEventP );
extern Err 				HandleChEvent( SysNotifyParamType* notifyParamP, appPrefs_t* pPrefs );

#endif /* __PHNEVENTHANDLER_H__ */

/*
 * PhnEventHandler.h
 */