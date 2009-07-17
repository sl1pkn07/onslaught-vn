#ifndef NONS_COMMONTYPES_H
#define NONS_COMMONTYPES_H

#ifndef TOOLS_NSAIO
#include <SDL/SDL_stdinc.h>
#else
#include <climits>
#endif

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;
#ifdef TOOLS_NSAIO
typedef char Sint8;
typedef uchar Uint8;
typedef short Sint16;
typedef ushort Uint16;
typedef long Sint32;
typedef ulong Uint32;
#endif
#endif
