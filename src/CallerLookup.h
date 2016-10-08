/*
 * CallerLookup.h
 */
 
#ifndef __CALLERLOOKUP_H__
#define __CALLERLOOKUP_H__

#include <Hs.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <HsAppLaunchCmd.h>
#include <palmOneCreators.h>

#include "Global.h"
#include "AddrDB.h"
#include "Utils.h"

#define STR_MOBILE_LABEL					"M"
#define STR_LOOKUP_CALLER_LABEL_FMT			"^label" // returns phone field label char
#define STR_LOOKUP_CALLER_NAME_FMT			"^friendlyname (^label)" // "^first ^name (^label)"

#define STR_TOKEN_START						"<tf>"
#define STR_TOKEN_END						"</tf>"

// Prototypes
extern void 			ReplaceDateTimeTokens( Char* str, UInt16 strLen );
extern Boolean 			CallerLookup( char* number, char* formatStr, char* nameStr, UInt16 nameStrSize, char* contactsNote, UInt16 contactsNoteSize );

#endif /* __CALLERLOOKUP_H__ */
 
/*
 * CallerLookup.h
 */