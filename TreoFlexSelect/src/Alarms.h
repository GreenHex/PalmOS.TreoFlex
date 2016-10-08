/*
 * Alarms.h
 */
 
#ifndef __TFS_ALARMS_H__
#define __TFS_ALARMS_H__

#include <Hs.h>

#include "Global.h"
#include "Prefs.h"
#include "AttnAlerts.h"
#include "../../TreoFlex2/src/Prefs.h"

#define SECONDS_IN_A_DAY		86400 // 24 * 60 * 60

typedef enum {
	invalid						= -1,
	disableTF					= 0,
	enableTF					= 1
} nextAlarmAction_e;

// Prototypes
extern void 			SetNextAlarm( Boolean bAlert, AttnLevelType level );

#endif /* __TFS_ALARMS_H__ */

/*
 * Alarms.h
 */