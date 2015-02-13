#ifndef   STDCONST
#define   STDCONST

#ifndef NULL
#define NULL    ((void *)0)
#endif

#define   TRUE                          1
#define   FALSE                         0
typedef   unsigned char                 UCHAR;
typedef   unsigned short                USHORT;
typedef   unsigned char                 UBYTE;
typedef   signed char                   SBYTE;
typedef   unsigned short int            UWORD;
typedef   signed short int              SWORD;
typedef   unsigned long                 ULONG;
typedef   signed long                   SLONG;
typedef   ULONG*                        PULONG;
typedef   USHORT*                       PUSHORT;
typedef   UCHAR*                        PUCHAR;
typedef   char*                         PSZ;

#define   BASETYPES
#define __ramfunc __attribute__ ((long_call, section (".fastrun")))

#endif
