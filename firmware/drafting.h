//
//  drafting.h
//
//
//

#ifndef _drafting_h
#define _drafting_h

#include "AT91SAM7S256.h"
#include <stdconst.h>

typedef enum {
    DRAFT_CREATE,
    DRAFT_RESTORE,
    DRAFT_ERASE
} draft_t;

#define INVALID_LENGTH  120
#define INVALID_ANGLE   361


UBYTE isDraftReady(void);
UBYTE CheckDraftSetInactive(void);
UBYTE draft_polyline(draft_t);
ULONG computeLength(ULONG, ULONG, ULONG, ULONG);
ULONG computeAngle(ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);
void updateLineAngle(ULONG, ULONG);


#endif
