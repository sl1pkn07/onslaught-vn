#if defined(_DEBUG)
#define ONSLAUGHT_BUILD_VERSION 99999999
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Debug"
#define ONSLAUGHT_BUILD_VERSION_STR "Debug"
#elif defined(NONS_SVN)
#define ONSLAUGHT_BUILD_VERSION 20110307
#define ONSLAUGHT_BUILD_VERSION_WSTR L"SVN 2011-03-07 20:36:57"
#define ONSLAUGHT_BUILD_VERSION_STR "SVN 2011-03-07 20:36:57"
#else
#define ONSLAUGHT_BUILD_VERSION 20110307
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Beta 2011-03-07 20:36:57"
#define ONSLAUGHT_BUILD_VERSION_STR "Beta 2011-03-07 20:36:57"
#endif
#define ONSLAUGHT_COPYRIGHT_YEAR_STR "2008-2011"
#define ONSLAUGHT_COPYRIGHT_YEAR_WSTR L"2008-2011"
