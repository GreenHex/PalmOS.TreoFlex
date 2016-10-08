/*
 * Utils.h
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>	
#include <Form.h>
#include <StringMgr.h>
#include <PmSysGadgetLib.h>
#include <PmSysGadgetLibCommon.h>

#include "Global.h"
#include "AppResources.h"

#define PROFILE_STR_LOC_X				4
#define PROFILE_STR_LOC_Y				136
#define PROFILE_STR_WIDTH				152
#define PROFILE_STR_HEIGHT				12

#define GID_STR_LOC_X					4
#define GID_STR_LOC_Y					148
#define GID_STR_WIDTH					152
#define GID_STR_HEIGHT					12

#define MIN_VERSION  					sysMakeROMVersion( 5, 0, 0, sysROMStageRelease, 0 )
#define LAUNCH_FLAGS 					( sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp )

/*
enum fontID {
  stdFont = 0x00, 
  boldFont, 
  largeFont, 
  symbolFont, 
  symbol11Font, 
  symbol7Font, 
  ledFont, 
  largeBoldFont, 
  fntAppCustomBase = 0x80
};
*/

// Prototypes
extern void 					MakeTimeDateStr( UInt32 timeSecs, Char** strTimeDatePP, Char* strTimeP, Char* strDateP, Boolean bRemoveYear );
extern Char* 					MakeNum( unsigned char* s );
extern Char* 					ReplaceChar(unsigned char *s, char orgChr, char newChr);
extern Char* 					StripSpaces(unsigned char *s);
extern Boolean 					IsPhoneReady( void );
extern Boolean 					IsRoaming( void );
extern Err 						GetPhnOperator( Char* operatorName, UInt16 operatorNameStrLen );
extern Boolean 					IsPhoneGSM( void );
extern Err 						RomVersionCompatible( UInt32 requiredVersion, UInt16 launchFlags );
extern Boolean 					AboutFormHandleEvent( EventType* pEvent );

#endif /* __UTILS_H__ */

/*
 * Utils.h
 */ 