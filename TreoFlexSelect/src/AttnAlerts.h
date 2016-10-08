/*
 * AttnAlerts.h
 */
 
#ifndef __TFS_ATTNALERTS_H__
#define __TFS_ATTNALERTS_H__

#include <Hs.h>

#include "Global.h"
#include "AppResources.h"
#include "../../TreoFlex2/src/Global.h"
#include "../../Centroid/src/Bitmaps.h"
#include "../../TreoFlex2/src/Utils.h"

#define STRLEN_ALERT_NOTE					32

typedef enum {
	onDisable			= 0,
	onEnable			= 1
} attnType_e;

// Prototypes
extern void 			DoAlert( attnType_e attnType, AttnLevelType level );


/* AttnLevelType
		kAttnLevelInsistent 
		kAttnLevelSubtle 
*/

/*
typedef enum SndSysBeepTag {
  sndInfo = 1,
  sndWarning,
  sndError,
  sndStartUp,
  sndAlarm,
  sndConfirmation,
  sndClick
}
*/

#endif /* __TFS_ATTNALERTS_H__ */

/*
 * AttnAlerts.h
 */