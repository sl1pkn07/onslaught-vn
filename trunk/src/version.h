#if defined(_DEBUG)
#define ONSLAUGHT_BUILD_VERSION 99999999
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Debug"
#define ONSLAUGHT_BUILD_VERSION_STR "Debug"
#elif defined(NONS_SVN)
#define ONSLAUGHT_BUILD_VERSION 20100712
#define ONSLAUGHT_BUILD_VERSION_WSTR L"SVN 2010-07-12 00:05:36"
#define ONSLAUGHT_BUILD_VERSION_STR "SVN 2010-07-12 00:05:36"
#else
#define ONSLAUGHT_BUILD_VERSION 20100712
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Beta 2010-07-12 00:05:36"
#define ONSLAUGHT_BUILD_VERSION_STR "Beta 2010-07-12 00:05:36"
#endif
#define ONSLAUGHT_COPYRIGHT_YEAR_STR "2008-2010"
#define ONSLAUGHT_COPYRIGHT_YEAR_WSTR L"2008-2010"
