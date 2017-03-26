#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <queue>
#include <fstream>
#include <pthread.h>
#include <sys/time.h>

using namespace std;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static filebuf          log_filebuf;
static pthread_t        log_workingThread;
static queue<string>    log_queue;
//static pthread_mutex_t  log_mutex_lock   = PTHREAD_MUTEX_INITIALIZER;
//static pthread_cond_t   log_thread_cond  = PTHREAD_COND_INITIALIZER;

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
    snprintf(buf, sizeof buf, "%s.%ld (%-30s, %5d)", tmbuf, tv.tv_usec, file.c_str(), line);

    string rtn(buf);
    return rtn;
}

static void* log_workerThreadFunc(void* )
{
//    pthread_mutex_init(&log_mutex_lock, NULL);
    if( !log_fileOpen("./test.log") )  {
        cout << " FAILED to open logfile..!!" << endl;
        return NULL;
    }

    while(1) {
//        cout << "running...." << endl;
        while( !log_queue.empty() ) {
//            pthread_mutex_lock(&log_mutex_lock);
            string ctx(log_queue.front());
//            cout << " ctx : " << ctx << endl;
//            cout << " log_queue.size() : " << log_queue.size() << endl;
            log_queue.pop();
//            pthread_mutex_unlock(&log_mutex_lock);
            log_fileWrite(ctx);
            log_stdoutWrite(ctx);
        }
    }

    if( log_filebuf.is_open() == true )  log_filebuf.close();
//    pthread_mutex_destroy(&log_mutex_lock);
}

pthread_t testThread1;
static void* testThreadFunc1(void*)
{
    while (1) {
        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(10000);
    }
}
pthread_t testThread2;
static void* testThreadFunc2(void*)
{
    while (1) {
        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(30000);
    }
}
pthread_t testThread3;
static void* testThreadFunc3(void*)
{
    while (1) {
        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(50000);
    }
}

int main( int argc, char* argv[] )
{
    (void)argc; (void)argv;
    pthread_create(&log_workingThread, NULL, log_workerThreadFunc, NULL );
    pthread_create(&testThread1, NULL, testThreadFunc1, NULL );
    pthread_create(&testThread2, NULL, testThreadFunc2, NULL );
    pthread_create(&testThread3, NULL, testThreadFunc3, NULL );
    while(1){
        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        cout << log_getPreLog(__FILENAME__, __LINE__)  << endl;
        usleep(20000);
    };
    return 0;
}
