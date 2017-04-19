#include <iostream>
#include "logutil.h"
//#include <unistd.h>

using namespace std;

//class Logger {
//public:
//    string stream;
//    Logger() { stream.clear(); }

//    friend Logger& operator<<( Logger&, const char* );
//    friend Logger& operator<<( Logger&, const unsigned char& );

//    friend Logger& operator<<( Logger&, int&  );
//    friend Logger& operator<<( Logger&, const unsigned int&  );

//    friend Logger& operator<<( Logger&, const short&  );
//    friend Logger& operator<<( Logger&, const unsigned short&  );

//    friend Logger& operator<<( Logger&, const float&  );
//    friend Logger& operator<<( Logger&, const double&  );

//    friend Logger& operator<<( Logger&, const long&  );
//    friend Logger& operator<<( Logger&, const unsigned long&  );
//    friend Logger& operator<<( Logger&, const long long&  );

//    friend Logger& operator<<( Logger&, const string&  );
//    friend Logger& operator<<( Logger&, const bool&  );
//};

//Logger &operator<<( Logger&, const char* );
//Logger &operator<<( Logger&, const unsigned char* );

//Logger &operator<<( Logger&, const int  );
//Logger &operator<<( Logger&, const unsigned int  );

//Logger &operator<<( Logger&, const short  );
//Logger &operator<<( Logger&, const unsigned short  );

//Logger &operator<<( Logger&, const float  );
//Logger &operator<<( Logger&, const double  );

//Logger &operator<<( Logger&, const long  );
//Logger &operator<<( Logger&, const unsigned long  );
//Logger &operator<<( Logger&, const long long  );

//Logger &operator<<( Logger&, const string&  );
//Logger &operator<<( Logger&, const bool  );
//static pthread_t        log_workingThread;

//static void* log_workerThreadFunc(void* )
//{
//    if( !log_fileOpen("./test.log") )  {
//        cout << " FAILED to open logfile..!!" << endl;
//        return NULL;
//    }

//    while(1) {
////        cout << "running...." << endl;
//        while( !log_queue.empty() ) {
//            pthread_mutex_lock(&log_mutex_lock);
//            string ctx(log_queue.front());
////            cout << " ctx : " << ctx << endl;
////            cout << " log_queue.size() : " << log_queue.size() << endl;
//            log_queue.pop();
//            pthread_mutex_unlock(&log_mutex_lock);
//            log_fileWrite(ctx);
//            log_stdoutWrite(ctx);
//        }
//    }

//    if( log_filebuf.is_open() == true )  log_filebuf.close();
//    pthread_mutex_destroy(&log_mutex_lock);
//}

pthread_t testThread1;
static void* testThreadFunc1(void*)
{
    while (1) {
//        LOG_INFO;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(10000);
    }
}
pthread_t testThread2;
static void* testThreadFunc2(void*)
{
    while (1) {
//        LOG_WARN;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(30000);
    }
}
pthread_t testThread3;
static void* testThreadFunc3(void*)
{
    while (1) {
//        LOG_DEBUG;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(50000);
    }
}

int main( int argc, char* argv[] )
{
    (void)argc; (void)argv;
    LOG_INIT("logutil","","");
//    pthread_create(&log_workingThread, NULL, log_workerThreadFunc, NULL );
    pthread_create(&testThread1, NULL, testThreadFunc1, NULL );
    pthread_create(&testThread2, NULL, testThreadFunc2, NULL );
    pthread_create(&testThread3, NULL, testThreadFunc3, NULL );
    while(1){
//        LOG_ERROR;
//        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        if( true ) LOG_ERROR << "aaaa";
          LOG_DEBUG << "aaaa";
          LOG_WARN  << "aaaa";
          LOG_INFO  << "aaaa";
//          if( log_queue.size() ) cout << log_queue.front() << endl;
//        cout << log_queue.front() << endl;
//        log_class << 1 << 2 << 3;
        usleep(200);
    };
    return 0;
}
