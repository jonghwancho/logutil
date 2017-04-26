#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <string>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <fstream>
#include <pthread.h>
#include <sys/time.h>
#include <dirent.h>
#include <bitset>
#include <sstream>

using namespace std;

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef LOG_LEVEL_ENUM
#define LOG_LEVEL_ENUM
typedef enum LOG_LEVEL {
    LOG_LEVEL_DLT       = 0,
    LOG_LEVEL_CONSOLE   = 1,
    LOG_LEVEL_FUNCTION  = 2,
    LOG_LEVEL_VERB      = 3,
    LOG_LEVEL_DEBUG     = 4,
    LOG_LEVEL_INFO      = 5,
    LOG_LEVEL_WARN      = 6,
    LOG_LEVEL_ERROR     = 7,
    LOG_LEVEL_MAX,
}LOG_LEVEL_E;
#endif

//#ifndef LOG_GLOBAL_EXTERN_VARIABLE
//#define LOG_GLOBAL_EXTERN_VARIABLE

struct _stlogdata {
    LOG_LEVEL_E level;
    string      logdata;
};

extern queue<struct _stlogdata> log_queue;
extern bitset<LOG_LEVEL_MAX>    log_level;
extern pthread_mutex_t          log_mutex_lock;
//#endif

static stringstream         log_ss;
static string log_getPreLog  ( string, int );

#ifndef LOG_STRINGSTREAM_BASE_CLASS
#define LOG_STRINGSTREAM_BASE_CLASS
class _clogssbase {
public :
    LOG_LEVEL_E level;
    _clogssbase() {}
    _clogssbase& s(LOG_LEVEL_E l){level=l;}
};
static class _clogssbase     log_ss_base;   // like the global std::cout
#endif

#ifndef LOG_STREAM_STRUCT
#define LOG_STREAM_STRUCT
struct _stlogstream {
    bool        alive;
    LOG_LEVEL_E level;
    _stlogstream(LOG_LEVEL_E l) : alive(true), level(l) {
//        cout << " construct level = " << level << endl;
        log_ss.str(""); }
    _stlogstream(_stlogstream && rhs) : alive(true) {
//        cout << " rhs.alive = " << rhs.alive << endl;
        rhs.alive = false;
        level = rhs.level;
//        cout << " level = " << level << endl;
    }
    _stlogstream(_stlogstream const &) = delete;
    ~_stlogstream() {
        if (alive) {
            struct _stlogdata data;
            data.level   = level;
            data.logdata = log_ss.str();
//            cout << " level = " << level << ", logdata = " << data.logdata << endl;
//            cout << " data.level = " << level << ", logdata = " << data.logdata << endl;
            log_queue.push(data);
            log_ss.str("");
            level = LOG_LEVEL_DLT;
            pthread_mutex_unlock(&log_mutex_lock);
        }
    }//cout << log_ss.str() << endl; log_ss.str(""); } }
};
#endif

#ifndef LOG_TEMPLATE
#define LOG_TEMPLATE
template <typename T>
_stlogstream operator<<(_stlogstream && o, T const & x)
{
    log_ss << x;
    return move(o);
}

template <typename T>
_stlogstream operator<<(_clogssbase & o, T const & x)
{
    pthread_mutex_lock(&log_mutex_lock);
//    cout << " _stlogssbase::level = " << o.level << endl;
    //cout << "x = " << x << endl;
    return move(_stlogstream(o.level) << x);
}
#endif


#ifndef LOG_GETPRELOG_FUNC
#define LOG_GETPRELOG_FUNC
static string log_getPreLog(string file, int line)
{
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char   tmbuf[256] = {0,};
    char   buf[256] = {0,};

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm   = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof buf, "%s.%06ld (%-30s, %5d)", tmbuf, tv.tv_usec, file.c_str(), line);

    string rtn(buf);
    return rtn;
}
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG      if( log_level[LOG_LEVEL_DEBUG] || log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_DEBUG) << log_getPreLog(__FILENAME__, __LINE__) << " : [D] "
#endif

#ifndef LOG_INFO
#define LOG_INFO       if( log_level[LOG_LEVEL_INFO] || log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_INFO) << log_getPreLog(__FILENAME__, __LINE__) << " : [I] "
#endif

#ifndef LOG_ERROR
#define LOG_ERROR      if( log_level[LOG_LEVEL_ERROR] || log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_ERROR) << log_getPreLog(__FILENAME__, __LINE__) << " : [E] "
#endif

#ifndef LOG_WARN
#define LOG_WARN       if( log_level[LOG_LEVEL_WARN] || log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_WARN) << log_getPreLog(__FILENAME__, __LINE__) << " : [W] "
#endif

#ifndef LOG_VERB
#define LOG_VERB       if( log_level[LOG_LEVEL_VERB] || log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_VERB) << log_getPreLog(__FILENAME__, __LINE__) << " : [V] "
#endif

#ifndef LOG_FUNC_START
#define LOG_FUNC_START if( log_level[LOG_LEVEL_FUNCTION] | log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_FUNCTION) << log_getPreLog(__FILENAME__,__LINE__) << " : [F] " << __FUNCTION__ << " FUNC START"
#endif

#ifndef LOG_FUNC_END
#define LOG_FUNC_END   if( log_level[LOG_LEVEL_FUNCTION] | log_level[LOG_LEVEL_DLT] ) \
                           log_ss_base.s(LOG_LEVEL_FUNCTION) << log_getPreLog(__FILENAME__,__LINE__) << " : [F] " << __FUNCTION__ << " FUNC END"
#endif

#endif // LOGUTIL_H
