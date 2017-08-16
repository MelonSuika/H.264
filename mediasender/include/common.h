#ifndef _COMMON_H
#define _COMMON_H

#ifdef WIN32
#define g_debug         printf
#define g_warning       printf
#define g_malloc0       malloc
#define g_usleep        Sleep
#define gpointer        void*
#define g_free          free
#define g_snprintf      sprintf_s
#define LINUX_TIME      1
#else
#define LINUX_TIME      1000
#endif

#define SAFE_FCLOSE(p)      if (p){fclose(p); p = NULL;}
#define SAFE_DELETEA(p)     if (p){delete []p; p = NULL;}
#define SAFE_DELETE(p)      if (p){delete p; p = NULL;}



















#endif
