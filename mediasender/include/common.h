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


#define MAX_FILENAME            (256)                         // 文件名最大长度

/* 统一unistd和io.h中的宏定义 */
#define F_OK        0
#define X_OK        1
#define W_OK        2
#define R_OK        4  
#define WR_OK       6














#endif
