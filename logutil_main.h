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

#ifdef DLT_LOG_ENABLE
#include <dlt/dlt.h>
DLT_DECLARE_CONTEXT(myDltCtx);
#endif

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

#ifndef LOG_GLOBAL_VARIABLE
#define LOG_GLOBAL_VARIABLE
struct _stlogdata {
    LOG_LEVEL_E level;
    string      logdata;
};

queue<struct _stlogdata>log_queue;
bitset<LOG_LEVEL_MAX>   log_level;
stringstream            log_ss;
static filebuf          log_filebuf;
static string           log_file_fullname;
static ofstream         log_fstream;
pthread_t               log_workingThread;
pthread_mutex_t         log_mutex_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

static bool   log_fileWrite  ( string );
static bool   log_stdoutWrite( string );
static bool   log_fileOpen   ( string );
static string log_getPreLog  ( string, int );
static void*  log_workerThreadFunc(void* );
static bool   log_initFunc   ( string, string, string, string, string, string );
//static pthread_cond_t   log_thread_cond  = PTHREAD_COND_INITIALIZER;

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


#ifndef LOG_FILEWRITE_FUNC
#define LOG_FILEWRITE_FUNC
static bool log_fileWrite ( string content )
{
    log_fstream << content << "file_size = " << log_fstream.tellp() << "\n";
    log_fstream.flush();
    return true;
}
#endif

#ifndef LOG_STDOUTWRITE_FUNC
#define LOG_STDOUTWRITE_FUNC
static bool log_stdoutWrite( string content )
{
    cout << content + "\n";
    return true;
}
#endif

#ifndef LOG_DLTWRITE_FUNC
#define LOG_DLTWRITE_FUNC
static bool log_dltWrite( LOG_LEVEL_E l, string content )
{
//    cout << "level = " << l << ", content = " << content << endl;
    ssize_t pos;
    switch(l) {
    case LOG_LEVEL_DEBUG :
        cout << "this log level is DEBUG..!!" << endl;
        pos = content.find("[D]", 0, 3);
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_INFO :
        cout << "this log level is INFO..!!" << endl;
        pos = content.find("[I]");
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_WARN :
        cout << "this log level is WARN..!!" << endl;
        pos = content.find("[W]");
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_VERB :
        cout << "this log level is VERB..!!" << endl;
        pos = content.find("[V]");
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_ERROR :
        cout << "this log level is ERROR..!!" << endl;
        pos = content.find("[E]");
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_FUNCTION :
        cout << "this log level is FUNCTION..!!" << endl;
        pos = content.find("[F]");
        content.erase( 0, pos + 4 );
        cout << " this is content : " << content << endl;
        break;
    case LOG_LEVEL_DLT :
    case LOG_LEVEL_CONSOLE :
    default :
        break;
    }
    return true;
}
#endif


#ifndef LOG_FILEOPEN_FUNC
#define LOG_FILEOPEN_FUNC
static bool log_fileOpen ( string filename )
{
    log_fstream.open(filename.c_str(), ios::out | ios::app);
    return log_fstream.is_open();
    //return log_filebuf.open(filename.c_str(), ios::out | ios::app);
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

#ifndef LOG_WORKERTHREAD_FUNC
#define LOG_WORKERTHREAD_FUNC
void* log_workerThreadFunc(void*)
{
    if( !log_fileOpen(log_file_fullname) )  {
        cout << " FAILED to open logfile..!!" << endl;
        return NULL;
    }

    while(true) {
        while( !log_queue.empty() ) {
            pthread_mutex_lock(&log_mutex_lock);
            struct _stlogdata ctx = log_queue.front();
            //string ctx(log_queue.front());
            log_queue.pop();
            pthread_mutex_unlock(&log_mutex_lock);
            log_stdoutWrite(ctx.logdata);
            log_fileWrite(ctx.logdata);
//#ifdef DLT_LOG_ENABLE
            log_dltWrite(ctx.level, ctx.logdata);
//#endif
        }
    }

    if( log_filebuf.is_open() == true )  log_filebuf.close();
    pthread_mutex_destroy(&log_mutex_lock);
}
#endif

#ifndef LOG_CONFIG_READ_FUNC
#define LOG_CONFIG_READ_FUNC
static bitset<LOG_LEVEL_MAX> log_configReadFunc(string configfilenamefull, string modulename )
{
    bitset<LOG_LEVEL_MAX>       bit_set_val_;
    ifstream        stream_val_;
    string          str_val_;
    ssize_t         found_open;
    ssize_t         found_close;
    ssize_t         found_0x;

    bit_set_val_.reset();
    stream_val_.open(configfilenamefull);

    if( stream_val_.is_open() == true ) {
        while( getline( stream_val_, str_val_ )) {
            string open_xml;
            open_xml.append("<");
            open_xml.append(modulename);
            open_xml.append(">");

            string close_xml;
            close_xml.append("</");
            close_xml.append(modulename);
            close_xml.append(">");

            found_open  = str_val_.find(open_xml);
            found_close = str_val_.find(close_xml);

            if( found_open != (ssize_t)string::npos && found_close != (ssize_t)string::npos ) {
                str_val_.erase(0, (found_open + open_xml.size()));
                str_val_.erase( (found_close - (found_open + open_xml.size() )), close_xml.size());
                cout << "str_val_ : " << str_val_ << ", found_open = " << found_open << ", found_close = " << found_close << endl;

                found_0x = str_val_.find("0x");

                if( found_0x != (ssize_t)string::npos ) {
                    str_val_.erase(found_0x, sizeof("0"));
                    cout << "str_val_ : " << str_val_ << ", found_0x = " << found_0x << ", found_close = " << found_close << endl;

                    for( int idx = 0 ; idx < (int)str_val_.size() ; idx++ ) {
                        if( str_val_[idx] == 'F' || str_val_[idx] == 'f' ) {
                            bit_set_val_.set( (LOG_LEVEL_MAX-1) - idx, true );
                            cout << "str_val_[idx] : " << idx << ", set true" << endl;
                        }
                    }
                }
            }
            str_val_="";
        }
    }
    else {
        cout << " WARNING..!! Cannot find config file ( " << configfilenamefull << ")" << endl;
        bit_set_val_.set();
    }
    cout << " ## Confile File Path & Name : " << configfilenamefull << endl;
    cout << " ## Found Module Name        : " << modulename << endl;
    cout << " ## [FATAL][ERROR][WARN][INFO][DEBUG][FUNCTION][CONSOLE][DLT]" << endl;
    cout << " ## Log Level                : " << bit_set_val_.to_string() << endl;

    return bit_set_val_;
}
#endif

#ifndef LOG_INIT_FUNC
#define LOG_INIF_FUNC
inline bool log_initFunc( string log_config_file_path, string log_file_path, string log_module_name, string/*appdesc*/, string/*ctxname*/, string/*ctxdesc*/ )
{
    /* config file reading */
    string  log_config_file(log_config_file_path + "/config_log.xml");
    log_level = log_configReadFunc(log_config_file, log_module_name);

    /* log file settting */
    log_file_fullname = log_file_path + "/" + log_module_name + ".log";

    /* log thread trigger */
    pthread_mutex_init(&log_mutex_lock, NULL);
    pthread_create(&log_workingThread, NULL, log_workerThreadFunc, NULL );

    return true;
}
#endif

#ifndef LOG_INIT
#define LOG_INIT( __configpath__, __logpath__, __appname__, __appdesc__, __ctxname__, __ctxdesc__ ) \
        log_initFunc( __configpath__, __logpath__, __appname__, __appdesc__, __ctxname__, __ctxdesc__ )
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
