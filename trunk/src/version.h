#if defined(_DEBUG)
#define ONSLAUGHT_BUILD_VERSION 99999999
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Debug"
#define ONSLAUGHT_BUILD_VERSION_STR "Debug"
#elif defined(NONS_SVN)
#define ONSLAUGHT_BUILD_VERSION 20090830
#define ONSLAUGHT_BUILD_VERSION_WSTR L"SVN 2009-08-30 22:55:43"
#define ONSLAUGHT_BUILD_VERSION_STR "SVN 2009-08-30 22:55:43"
#else
#define ONSLAUGHT_BUILD_VERSION 20090830
#define ONSLAUGHT_BUILD_VERSION_WSTR L"Beta 2009-08-30 22:55:43"
#define ONSLAUGHT_BUILD_VERSION_STR "Beta 2009-08-30 22:55:43"
#endif
#define ONSLAUGHT_COPYRIGHT_YEAR_STR "2008, 2009"
#define ONSLAUGHT_COPYRIGHT_YEAR_WSTR L"2008, 2009"
