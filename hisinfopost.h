// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HISINFOPOST_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HISINFOPOST_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HISINFOPOST_EXPORTS
#define HISINFOPOST_API __declspec(dllexport)
#else
#define HISINFOPOST_API __declspec(dllimport)
#endif


HISINFOPOST_API int postPres(const char *url, const char *data, int post_type, int encoding);

HISINFOPOST_API BOOL confirmPres(const char *url, const char *pres_no, int post_type);