/*
 * Prefs.c
 */

#include "Prefs.h"

// Prototypes
static void 			InitializeTfsPrefs( tfsPrefs_t* pTfsPrefs );

/*
 * WriteTfsPrefs()
 */
void WriteTfsPrefs( tfsPrefs_t* pTfsPrefs )
{
	if ( !pTfsPrefs ) return;
	
	PrefSetAppPreferences( tfsPrefsCreator, tfsPrefID, tfsPrefVersionNum, pTfsPrefs, sizeof( tfsPrefs_t ), true );
	
	return;
	
} // WriteTfsPrefs()

/*
 * ReadTfsPrefs()
 */
void ReadTfsPrefs( tfsPrefs_t* pTfsPrefs )
{
	UInt16 			prefSize = sizeof( tfsPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pTfsPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( tfsPrefsCreator, tfsPrefID, pTfsPrefs, &prefSize, true );
	
	if ( prefsVersion != tfsPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( tfsPrefsCreator, tfsPrefID, prefsVersion, NULL, 0, true );				
	
		InitializeTfsPrefs( pTfsPrefs );
		
		WriteTfsPrefs( pTfsPrefs );
	}
	
	return;
	
} // ReadTfsPrefs()

/*
 * InitializeTfsPrefs()
 */
static void InitializeTfsPrefs( tfsPrefs_t* pTfsPrefs )
{
	if ( !pTfsPrefs ) return;
	
	*( (UInt16 *) &(pTfsPrefs->startTime) ) = -1;
	*( (UInt16 *) &(pTfsPrefs->endTime) ) = -1;
	
	return;
	
} // InitializeTfsPrefs
/*
 * Prefs.c
 */