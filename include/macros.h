#ifndef WIN98_MACROS_H
#define WIN98_MACROS_H

// Macro functions

#define VERBUILD(major, minor, year) (((major * 0x01000000) | minor * 0x010000) | year)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Macro definitions

#define VER_MAJOR_NUMBER 4
#define VER_MINOR_NUMBER 10
#define VER_YEAR_NUMBER 1998

#define VER_BUILD_NUMBER VERBUILD(VER_MAJOR_NUMBER, VER_MINOR_NUMBER, VER_YEAR_NUMBER)

#endif // WIN98_MACROS_H


