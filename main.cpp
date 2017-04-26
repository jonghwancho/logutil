#include <iostream>
#include "logutil_main.h"
#include "source_1.h"
#include "source_2.h"
#include "source_3.h"
//#include <unistd.h>

using namespace std;

pthread_t testThread1;
static void* testThreadFunc1(void*)
{
    while (1) {
        LOG_INFO << " This is threadfunc :  " << __func__ ;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(100000);
    }
    return nullptr;
}

pthread_t testThread2;
static void* testThreadFunc2(void*)
{
    while (1) {
        LOG_INFO << " This is threadfunc :  " << __func__ ;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(30000);
    }
    return nullptr;
}


int main( int argc, char* argv[] )
{
    (void)argc; (void)argv;
    LOG_INIT("/app/config","/app/log","logtest","","","");
//    pthread_create(&log_workingThread, NULL, log_workerThreadFunc, NULL );
    pthread_create(&testThread1, NULL, testThreadFunc1, NULL );
    pthread_create(&testThread2, NULL, testThreadFunc2, NULL );
    createthread3();
    createthread4();
    createthread5();
    while(1){
//        LOG_ERROR;
//        log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        if( true )  LOG_ERROR << "aaaa";
          LOG_DEBUG << "aaaa";
          LOG_WARN  << "aaaa";
          LOG_INFO  << "aaaa";
//          if( log_queue.size() ) cout << log_queue.front() << endl;
//        cout << log_queue.front() << endl;
//        log_class << 1 << 2 << 3;
        usleep(2000000);
    };
    return 0;
}
