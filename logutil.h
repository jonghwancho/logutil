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

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static queue<string>    log_queue;
static stringstream     log_ss;
static filebuf          log_filebuf;
static pthread_t        log_workingThread;
static bitset<8>        log_level;
static pthread_mutex_t  log_mutex_lock   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t   log_thread_cond  = PTHREAD_COND_INITIALIZER;

struct log_stringstream_base { };
static struct log_stringstream_base     log_ss_base;   // like the global std::cout

struct logstream {
    bool alive;
    logstream() : alive(true) {  log_ss.str(""); }
    logstream(logstream && rhs) : alive(true) { rhs.alive = false; }
    logstream(logstream const &) = delete;
    ~logstream() { if (alive) { log_queue.push(log_ss.str()); log_ss.str(""); } }//cout << log_ss.str() << endl; log_ss.str(""); } }
};

template <typename T>
logstream operator<<(logstream && o, T const & x)
{
    log_ss << x;
    return move(o);
}

template <typename T>
logstream operator<<(log_stringstream_base &, T const & x)
{
    return move(logstream() << x);
}

typedef enum LOG_LEVEL {
    LOG_LEVEL_DLT       = 0,
    LOG_LEVEL_CONSOLE   = 1,
    LOG_LEVEL_VERB      = 2,
    LOG_LEVEL_DEBUG     = 3,
    LOG_LEVEL_INFO      = 4,
    LOG_LEVEL_WARN      = 5,
    LOG_LEVEL_ERROR     = 6,
    LOG_LEVEL_FATAL     = 7,
    LOG_LEVEL_MAX,
}LOG_LEVEL_E;


//static Logger           log_class;
//template<typename T>
//Logger& operator<<( Logger& rhs, const T lhs ) {
//    string tmp(lhs);
//    rhs.stream.append(tmp);
//    return rhs;
//}
//class Logger {
//public:
//    Logger():logstream(new logstream()) {}

//    struct logstream {
//        string     stream;
//        logstream(){ stream.clear(); }
//        ~logstream() { log_queue.push(stream); }
//    }*logstream;

////    Logger log()  { return new logstream(); }//::_stlogstream(); }

//    friend Logger::logstream operator<<( Logger::logstream&, const char* );
//    friend Logger& operator<<( Logger&, const unsigned char& );

//    friend Logger& operator<<( Logger&, const int& );
//    friend Logger& operator<<( Logger&, const unsigned int& );

//    friend Logger& operator<<( Logger&, const short& );
//    friend Logger& operator<<( Logger&, const unsigned short& );

//    friend Logger& operator<<( Logger&, const float& );
//    friend Logger& operator<<( Logger&, const double& );

//    friend Logger& operator<<( Logger&, const long& );
//    friend Logger& operator<<( Logger&, const unsigned long& );
//    friend Logger& operator<<( Logger&, const long long& );

//    friend Logger& operator<<( Logger&, const string& );
//    friend Logger& operator<<( Logger&, const bool& );
//};



static bool log_fileWrite ( string content )
{
    ostream os(&log_filebuf);
    os << content << "\n";
//    cout  << content << endl;
    return true;
}

static bool log_stdoutWrite( string content )
{
    cout << content + "\n";
    return true;
}

static bool log_fileOpen ( string filename )
{
    return log_filebuf.open(filename.c_str(), ios::out | ios::app);
}

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

static void* log_workerThreadFunc(void* )
{
    if( !log_fileOpen("./test.log") )  {
        cout << " FAILED to open logfile..!!" << endl;
        return NULL;
    }

    while(1) {
//        cout << "running...." << endl;
        while( !log_queue.empty() ) {
            pthread_mutex_lock(&log_mutex_lock);
            string ctx(log_queue.front());
//            cout << " ctx : " << ctx << endl;
//            cout << " log_queue.size() : " << log_queue.size() << endl;
            log_queue.pop();
            pthread_mutex_unlock(&log_mutex_lock);
            log_fileWrite(ctx);
            log_stdoutWrite(ctx);
        }
    }

    if( log_filebuf.is_open() == true )  log_filebuf.close();
    pthread_mutex_destroy(&log_mutex_lock);
}

#if 0
template<typename T1>
Logger& operator<<( Logger& rhs, const T1* lhs ) {
    rhs.stream << lhs;
    return rhs;
}

template<typename T2>
Logger& operator<<( Logger& rhs, const T2& lhs ) {
    rhs.stream << lhs;
    return rhs;
}
#endif

//#if 1
//Logger::logstream operator<<( Logger::stream rhs, const char* lhs )          { rhs->stream + lhs; return rhs; }
//Logger& operator<<( Logger& rhs, const char& lhs )          { rhs.stream + lhs; return rhs; }
//Logger& operator<<( Logger& rhs, const unsigned char& lhs ) { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const int& lhs )           { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const unsigned int& lhs )  { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const short& lhs )         { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const unsigned short& lhs ){ rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const float& lhs )         { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const double& lhs )        { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const long& lhs)           { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const unsigned long& lhs ) { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const long long& lhs )     { rhs.stream + to_string(lhs); return rhs; }
//Logger& operator<<( Logger& rhs, const string& lhs )        { rhs.stream + lhs; return rhs; }
//Logger& operator<<( Logger& rhs, const bool& lhs )          { rhs.stream + ((lhs)?"TRUE":"FALSE"); return rhs; }
//#endif

static bool log_init( string log_config_file_path, string log_file_path, string log_module_name )
{
    /* config file reading */
    DIR*    dir_;
    dir_ = opendir(log_config_file_path.c_str());

    if( dir_ == nullptr ) {
        cout << " ERROR..!! config file is not exist..!!" << endl;
        cout << " So, we will set all log level... ( 0xFFFFFFFF )" << endl;
        log_level.set();    // all level flags are set.
        log_level.reset(LOG_LEVEL_DEBUG);
        log_level.reset(LOG_LEVEL_WARN);
    }
    else {
        /* setting log level */
        log_level.set();
        log_level.reset(LOG_LEVEL_DEBUG);
        log_level.reset(LOG_LEVEL_WARN);
    }
    /* log file settting */

    /* log thread trigger */
    pthread_mutex_init(&log_mutex_lock, NULL);
    pthread_create(&log_workingThread, NULL, log_workerThreadFunc, NULL );

    return true;
}

#define LOG_INIT( _configpath_ , _logfile_ , _module_ ) \
        log_init( _configpath_ , _logfile_ , _module_ )

#ifndef LOG_DEBUG
#define LOG_DEBUG      if(log_level[LOG_LEVEL_DEBUG]) log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [D] " << __func__
#endif

#ifndef LOG_INFO
#define LOG_INFO       if(log_level[LOG_LEVEL_INFO])  log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [I] " << __func__
#endif

#ifndef LOG_ERROR
#define LOG_ERROR      if(log_level[LOG_LEVEL_ERROR]) log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [E] " << __func__
#endif

#ifndef LOG_FATAL
#define LOG_FATAL      if(log_level[LOG_LEVEL_FATAL]) log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [C] " << __func__
#endif

#ifndef LOG_WARN
#define LOG_WARN       if(log_level[LOG_LEVEL_WARN])  log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [W] " << __func__
#endif

#ifndef LOG_VERBOSE
#define LOG_VERB       if(log_level[LOG_LEVEL_VERB])  log_ss_base << log_getPreLog(__FILENAME__, __LINE__) << " : [V] " << __func__
#endif


#endif // LOGUTIL_H
