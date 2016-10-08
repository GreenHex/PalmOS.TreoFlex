/*
 * AttnAlerts.c
 */
 
#include "Alarms.h"

// Prototypes
static void 			DoAlarmAction( nextAlarmAction_e alarmAction );

/*
 * SetNextAlarm()
 */
void SetNextAlarm( Boolean bAlert, AttnLevelType level )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	tfsPrefs_t			tfsPrefs;
	DateTimeType		dt;
	UInt32				minsNow = 0;
	UInt32				minsStart = 0;
	UInt32				minsEnd = 0;
	UInt32				nextAlarmSecs = 0;
	nextAlarmAction_e	nextAlarmAction = invalid;
	
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		ReadTfsPrefs( &tfsPrefs );
		
		
		if ( ( *( (UInt16 *) &(tfsPrefs.startTime) ) == noTime ) 
				|| ( *( (UInt16 *) &(tfsPrefs.endTime) ) == noTime ) )
		{
			AlmSetAlarm( cardNo, dbID, 0, 0, true );		
			return;
		}		
		
		TimSecondsToDateTime( TimGetSeconds(), &dt );
		
		minsNow = ( dt.hour * 60 ) + ( dt.minute );
		minsStart = ( tfsPrefs.startTime.hours * 60 ) + tfsPrefs.startTime.minutes;
		minsEnd = ( tfsPrefs.endTime.hours * 60 ) + tfsPrefs.endTime.minutes;
			
		dt.second = dt.minute = dt.hour = 0;
		nextAlarmSecs = TimDateTimeToSeconds( &dt );
		
		if ( minsNow < minsStart )
		{
			nextAlarmSecs += minsStart * 60;
			nextAlarmAction = enableTF;
		}
		else if ( minsNow >= minsEnd )
		{
			nextAlarmSecs += ( ( minsStart * 60 ) + SECONDS_IN_A_DAY );
			nextAlarmAction = enableTF;
		}
		else
		{
			nextAlarmSecs += ( minsEnd * 60 );
			nextAlarmAction = disableTF;
		} 
		
		AlmSetAlarm( cardNo, dbID, (UInt32) nextAlarmAction, nextAlarmSecs, true );
		
		DoAlarmAction( 1 - nextAlarmAction );
	
		if ( bAlert )
		{	
			DoAlert( ( ( 1 - nextAlarmAction ) == enableTF ) ? onEnable : onDisable, level );
		}
	}
	
	return;
	
} // SetNextAlarm()

/*
 * DoAlarmAction()
 */
static void DoAlarmAction( nextAlarmAction_e alarmAction )
{
	appPrefs_t 		prefs;
	
	ReadAppPrefs( &prefs );
	
	prefs.bEnabled = ( alarmAction == enableTF );
	
	WriteAppPrefs( &prefs );
	
	RegisterForNotifications( prefs.bEnabled );

	return;
	
} // DoAlarmAction()

/*
 * AttnAlerts.c
 */