/*
 * Prefs.h
 */

#ifndef __TFS_PREFS_H__
#define __TFS_PREFS_H__

#include <Hs.h>
#include "Global.h"

// Defines
#define tfsPrefsCreator					tfsAppFileCreator	
#define tfsPrefID						0x00
#define tfsPrefVersionNum				0x00

typedef struct {
	TimeType					startTime;
	TimeType					endTime;
} tfsPrefs_t;

// Prototypes
extern void 					WriteTfsPrefs( tfsPrefs_t* pTfsPrefs );
extern void 					ReadTfsPrefs( tfsPrefs_t* pTfsPrefs );

#endif /* __TFS_PREFS_H__ */

/*
 * Prefs.h
 */