#ifndef LOGUTIL_H
#define LOGUTIL_H

//#define LOG_INIT_MAIN

/*
 * This logutil header file was made by Jonghwan Cho in 2017
 *
 * Author : Jonghwan Cho ( South of Korea )
 * Email  : jonghwany.cho@gmail.com or jonghwan.cho@lge.com
 * Date   : May, 07, 2017, Sun, 14:10:13
 *
 * This is for LG Electorinics IVI systems ( based on Linux )
 * For other platform ( e.g. QNX, Windows, etc... )
 * Engineer need to modifty this file.
 *
 * If you want to use this header file, you should NOT delete this comment.
 * You should remain original Author.
 *
 * This logutil has below functions.
 * + Setting LogLevel ( Error/Warn/Info/Debug/Verb/Func ) by configuration file ( config_log.xml )
 * + Setting Console output by configuration file ( config_log.xml )
 * + Setting DLT(GENIVI) enableing by configuration file and #define DLT_LOG_ENABLE
 *   : For DLT Logging system, you should build your file when you compile your target file.
 * + Setting Log file Size
 *   : Logutil backup logfile as setted logfile size. ( logfile name was appended backup time )
 */

/*
 * ToDo
 * - DLT LOG Write...!!!
 */

#include <string>
#include <queue>
#include <fstream>
#include <bitset>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <string.h>
#include <unistd.h>
#include <sys/time.h>

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
/*
 * Structure
 */
struct _stlogdata {
    LOG_LEVEL_E level;
    string      logdata;
};

/*
 * Variables
 */
#ifndef LOG_INIT_MAIN
extern queue<struct _stlogdata> log_queue;
extern bitset<LOG_LEVEL_MAX>    log_level;
extern mutex                    log_mutex;
extern condition_variable       log_cond;
#else
queue<struct _stlogdata>        log_queue;
bitset<LOG_LEVEL_MAX>           log_level;
static mutex                    log_mutex;
static condition_variable       log_cond;
#endif

static stringstream             log_ss;

#endif // #ifndef LOG_GLOBAL_VARIABLE

/*
 * Functions
 */
static string log_getPreLog  ( string, int );

#ifndef LOG_STRINGSTREAM_BASE_CLASS
#define LOG_STRINGSTREAM_BASE_CLASS
class _clogssbase {
public :
    LOG_LEVEL_E level;
    _clogssbase() {}
    const _clogssbase& s(LOG_LEVEL_E l) {level=l; return *this;}
};
static class _clogssbase     log_ss_base;   // like the global std::cout
#endif

#ifndef LOG_STREAM_STRUCT
#define LOG_STREAM_STRUCT
struct _stlogstream {
    bool        alive;
    LOG_LEVEL_E level;
    _stlogstream(LOG_LEVEL_E l) : alive(true), level(l) {
        log_ss.str(""); }
    _stlogstream(_stlogstream && rhs) : alive(true) {
        rhs.alive = false;
        level = rhs.level;
    }
    _stlogstream(_stlogstream const &) = delete;
    ~_stlogstream() {
        if (alive) {
            struct _stlogdata data;
            data.level   = level;
            data.logdata = log_ss.str();
            log_queue.push(data);
            log_cond.notify_all();
            log_ss.str("");
            level = LOG_LEVEL_DLT;
            log_mutex.unlock();
        }
    }
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
_stlogstream operator<<(const _clogssbase & o, T const & x)
{
    log_mutex.lock();
    return move(_stlogstream(o.level) << x);
}
#endif

#ifdef LOG_INIT_MAIN
static filebuf                  log_file_buffer;
static string                   log_file_fullname;
static ofstream                 log_file_stream;
static long long                log_file_maxsize;
static string                   log_file_path;

static string                   log_config_path;
static string                   log_config_file;
static string                   log_module_name;

static thread                   log_workingThread;
static thread                   log_configMonitorThread;

static bool   log_fileWrite  ( string );
static bool   log_stdoutWrite( string );
static bool   log_fileOpen   ( string );

static void   log_workerThreadFunc(void* );
static void   log_configMonitorThreadFunc(void* );

static bool   log_initFunc   ( string, string, string, string, string, string );
//static pthread_cond_t   log_thread_cond  = PTHREAD_COND_INITIALIZER;

#ifndef LOG_FILEWRITE_CHANGE_FUNC
#define LOG_FILEWRITE_CHNAGE_FUNC
static bool log_fileChange()
{
    struct timeval tv;
    time_t nowtime;
    struct tm *nowtm;
    char   tmbuf[256] = {0,};

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec;
    nowtm   = localtime(&nowtime);
    strftime(tmbuf, sizeof tmbuf, "_%Y_%m_%d_%H_%M_%S", nowtm);
    string strBackupFileName;
    strBackupFileName.append(log_file_fullname);
    strBackupFileName.append(tmbuf);

    if( log_file_stream.is_open() == true ) {
        log_file_stream << "=========================================================================\n";
        log_file_stream << " This log file was truncated at this point" << tmbuf << "\n";
        log_file_stream << "=========================================================================\n";
        log_file_stream.flush();
    }

    log_file_stream.close();

    if( rename( log_file_fullname.c_str(), strBackupFileName.c_str() ) != 0 ) {
        cout << " Cannot change to old file : " << strBackupFileName << "\n";
    }

    return log_fileOpen(log_file_fullname);
}
#endif

#ifndef LOG_FILEOPEN_FUNC
#define LOG_FILEOPEN_FUNC
static bool log_fileOpen ( string filename )
{
    log_file_stream.open(filename.c_str(), ios::out | ios::app);
    return log_file_stream.is_open();
}
#endif

/*
 * Output Log File function
 */
#ifndef LOG_FILEWRITE_FUNC
#define LOG_FILEWRITE_FUNC
static bool log_fileWrite ( string content )
{
    log_file_stream << content + "\n";
    log_file_stream.flush();
    return true;
}
#endif

/*
 * Printing Console function
 */
#ifndef LOG_STDOUTWRITE_FUNC
#define LOG_STDOUTWRITE_FUNC
static bool log_stdoutWrite( string content )
{
    cout << content + "\n";
    return true;
}
#endif

/*
 * Output DLT Logging system function
 */
#ifdef DLT_LOG_ENABLE
#ifndef LOG_DLTWRITE_FUNC
#define LOG_DLTWRITE_FUNC
static bool log_dltWrite( LOG_LEVEL_E l, string content )
{
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
#endif // #ifdef DLT_LOG_ENABLE

#ifndef LOG_CONFIG_READ_FUNC
#define LOG_CONFIG_READ_FUNC
static bitset<LOG_LEVEL_MAX> log_configReadFunc(string configfilenamefull, string modulename )
{
    bitset<LOG_LEVEL_MAX>       bit_set_val_;
    ifstream                    stream_val_;
    string                      str_val_;

    ssize_t                     module_found_open;
    ssize_t                     module_found_close;

    ssize_t                     size_found_open;
    ssize_t                     size_found_close;

    ssize_t                     found_0x;

    string                      module_open_xml;
    string                      module_close_xml;

    string                      size_open_xml;
    string                      size_close_xml;

    log_file_maxsize = 10000000;

    bit_set_val_.reset();
    stream_val_.open(configfilenamefull);

    if( stream_val_.is_open() == true ) {
        module_open_xml.append("<");   module_open_xml.append(modulename);  module_open_xml.append(">");
        module_close_xml.append("</"); module_close_xml.append(modulename); module_close_xml.append(">");

        size_open_xml.append("<");   size_open_xml.append("filesize");  size_open_xml.append(">");
        size_close_xml.append("</"); size_close_xml.append("filesize");  size_close_xml.append(">");

        while( getline( stream_val_, str_val_ )) {

            module_found_open  = str_val_.find(module_open_xml);
            module_found_close = str_val_.find(module_close_xml);

            size_found_open    = str_val_.find(size_open_xml);
            size_found_close   = str_val_.find(size_close_xml);

            if( module_found_open  != (ssize_t)string::npos &&
                module_found_close != (ssize_t)string::npos ) {

                str_val_.erase(0, (module_found_open + module_open_xml.size()));
                str_val_.erase( (module_found_close - (module_found_open + module_open_xml.size() )), module_close_xml.size());

                found_0x = str_val_.find("0x");

                if( found_0x != (ssize_t)string::npos ) {
                    str_val_.erase(found_0x, sizeof("0"));

                    for( int idx = 0 ; idx < (int)str_val_.size() ; idx++ ) {
                        if( str_val_[idx] == 'F' || str_val_[idx] == 'f' ) {
                            bit_set_val_.set( (LOG_LEVEL_MAX-1) - idx, true );
                        }
                    }
                }
            }
            else if( size_found_open  != (ssize_t)string::npos &&
                     size_found_close != (ssize_t)string::npos ) {
                str_val_.erase(0, (size_found_open + size_open_xml.size()));
                str_val_.erase( (size_found_close - (size_found_open + size_open_xml.size() )), size_close_xml.size());

                log_file_maxsize = stoll(str_val_.c_str());
            }
            str_val_="";
        }  //while( getline( stream_val_, str_val_ )) {
    } // if( stream_val_.is_open() == true ) {
    else {
        cout << " WARNING..!! Cannot find config file ( " << configfilenamefull << ")" << endl;
        bit_set_val_.set();
    }
    cout << " ## Confile File Path & Name : " << configfilenamefull << endl;
    cout << " ## Found Module Name        : " << modulename << endl;
    cout << " ## [FATAL][ERROR][WARN][INFO][DEBUG][FUNCTION][CONSOLE][DLT]" << endl;
    cout << " ## Log Level                : " << bit_set_val_.to_string() << endl;
    cout << " ## Log FileSize(bytes)      : " << log_file_maxsize << endl;

    return bit_set_val_;
}
#endif

#ifndef LOG_CONFIG_MONITOR_THREAD_FUNC
#define LOG_CONFIG_MONITOR_THREAD_FUNC
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE ( sizeof ( struct inotify_event ) )
#define BUF_LEN	   ( 1024 * ( EVENT_SIZE + 16 ) )

void log_configMonitorThreadFunc(void*)
{
    int     length, i = 0;
    int     fd;
    int     wd;
    char    buffer[BUF_LEN];

    fd = inotify_init();

    if( fd < 0 ) {
        perror( "inotify_init" );
    }

    wd = inotify_add_watch( fd, log_config_path.c_str(), IN_MODIFY );

    cout << " Start configuration file monitoring..!! " << endl;
    cout << " Monitoring File = " << log_config_path << "/config_log.xml" << endl;

    while(true) {

        length = read( fd, buffer , BUF_LEN );

        if( length < 0 ) {
            perror( "read" );
        }

        while( i < length ) {
            struct inotify_event *event = ( struct inotify_event * ) &buffer[i];

            if( event->len ) {
                if( event->mask & IN_MODIFY ) {
                    //cout << " event name : " <<  event->name << endl;
                    if( event->mask & IN_ISDIR ) {
                        //printf(" The directory %s was modified.\n", event->name );
                    }
                    else {
                        string event_str_(event->name);

                        if( (ssize_t)event_str_.find(".swp") != (ssize_t)string::npos )
                            break;
                        else if( (ssize_t)event_str_.find("config_log.xml") != (ssize_t)string::npos ) {
                            log_level = log_configReadFunc(log_config_file, log_module_name);
                            cout << "The configuration file " << event->name << " was modified..!!" << endl;
                            cout << " The loglevel was changed : " << log_level.to_string() << endl;
                            if( log_level[LOG_LEVEL_DLT] )      cout << "log level DLT      is set" << endl;
                            if( log_level[LOG_LEVEL_CONSOLE] )  cout << "log level CONSOLE  is set" << endl;
                            if( log_level[LOG_LEVEL_FUNCTION] ) cout << "log level FUNCTION is set" << endl;
                            if( log_level[LOG_LEVEL_VERB] )     cout << "log level VERB     is set" << endl;
                            if( log_level[LOG_LEVEL_DEBUG] )    cout << "log level DEBUG    is set" << endl;
                            if( log_level[LOG_LEVEL_INFO] )     cout << "log level INFO     is set" << endl;
                            if( log_level[LOG_LEVEL_WARN] )     cout << "log level WARN     is set" << endl;
                            if( log_level[LOG_LEVEL_ERROR] )    cout << "log level ERROR    is set" << endl;
                        }
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
        i = 0;
    }

    (void) inotify_rm_watch(fd, wd);
    (void) close(fd);
}
#endif

#ifndef LOG_WORKERTHREAD_FUNC
#define LOG_WORKERTHREAD_FUNC
//void* log_workerThreadFunc(void*)
void log_workerThreadFunc(void*)
{
    if( !log_fileOpen(log_file_fullname) )  {
        cout << " FAILED to open logfile..!!" << endl;
        return;// NULL;
    }

    while(true) {
        std::unique_lock<std::mutex> lck(log_mutex);
        log_cond.wait(lck);
        while( !log_queue.empty() ) {
            struct _stlogdata ctx = log_queue.front();
            log_queue.pop();
            if( log_level[LOG_LEVEL_CONSOLE] ) log_stdoutWrite(ctx.logdata);
            if( log_file_stream.tellp() > log_file_maxsize ) log_fileChange();
            log_fileWrite(ctx.logdata);
#ifdef DLT_LOG_ENABLE
            log_dltWrite(ctx.level, ctx.logdata);
#endif
        }
    }

    if( log_file_buffer.is_open() == true )  log_file_buffer.close();
}
#endif

#ifndef LOG_INIT_FUNC
#define LOG_INIF_FUNC
inline bool log_initFunc( string config_file_path, string file_path, string module_name, string/*appdesc*/, string/*ctxname*/, string/*ctxdesc*/ )
{
    /* config file reading */
    log_config_path = config_file_path;
    log_config_file = config_file_path + "/config_log.xml";

    log_module_name = module_name;
    log_file_path   = file_path;
    log_level = log_configReadFunc(log_config_file, module_name);

    /* log file settting */
    log_file_fullname = file_path + "/" + module_name + ".log";

    /* log thread trigger */
    log_workingThread       = thread(log_workerThreadFunc, (void*)nullptr);
    log_configMonitorThread = thread(log_configMonitorThreadFunc, (void*)nullptr);

    log_workingThread.detach();
    log_configMonitorThread.detach();

    return true;
}
#endif

#ifndef LOG_INIT
#define LOG_INIT( __configpath__, __logpath__, __appname__, __appdesc__, __ctxname__, __ctxdesc__ ) \
        log_initFunc( __configpath__, __logpath__, __appname__, __appdesc__, __ctxname__, __ctxdesc__ )
#endif

#endif // #ifdef LOG_INIT_MAIN

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
