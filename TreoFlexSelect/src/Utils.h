/*
 * Utils.h
 */

#ifndef __TFS_UTILS_H__
#define __TFS_UTILS_H__

#include <Hs.h>

#include "Global.h"
#include "AppResources.h"
#include "../../TreoFlex2/src/Global.h"

#define MAX_POP_PIXEL_WIDTH		118
extern Char						gStrPopup[STRLEN_MSG];

// Prototypes
extern Char* 				GetPopStr( Char* s );
extern Char* 				ReplaceChar2( unsigned char *s, char orgChr, char newChr );
extern Char* 				StripSpaces2( unsigned char *s );
extern void 				SetFormState( FormType* pForm );
extern void 				SetPopLabelAndFieldFromList2( FormType* pForm, UInt16 popID, UInt16 lstID, UInt16 fldID, UInt16 idx );
extern Boolean 				IsTreoFlexInstalled( void );
extern Boolean 				AboutFormHandleEvent2( EventType* pEvent );
extern void 				MakeTimeInterval( TimeType startTime, TimeType endTime, Char* strTimeInterval );
extern void 				MakeTimeDateStr2( UInt32 timeSecs, Char* strTimeDateP );
extern Err 					GetNextAlarm( UInt32* nextAlmSecP, UInt32* refP );
extern Err 					DialPhoneLaunch( Char* number, Char* name );

#endif /* __TFS_UTILS_H__ */

/*
 * Utils.h
 */