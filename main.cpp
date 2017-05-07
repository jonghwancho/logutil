#include <iostream>
#define LOG_INIT_MAIN
#include "logutil.h"
#include "source_1.h"
#include "source_2.h"
#include "source_3.h"

using namespace std;

pthread_t testThread1;
static void* testThreadFunc1(void*)
{
    while (1) {
//        cout << "thread1" << endl;
        LOG_INFO << " This is threadfunc :  " << __func__ ;
        usleep(10);
    }
    return nullptr;
}

pthread_t testThread2;
static void* testThreadFunc2(void*)
{
    while (1) {
//        cout << "thread2" << endl;
        LOG_INFO << " This is threadfunc :  " << __func__ ;
        usleep(3000);
    }
    return nullptr;
}


int main( int argc, char* argv[] )
{
    (void)argc; (void)argv;

    LOG_INIT("/app/config","/app/log","logtest","","","");

//    thread testThread1;
//    thread testThread2;

//    testThread1 = thread(testThreadFunc1, (void*)nullptr);
//    testThread2 = thread(testThreadFunc2, (void*)nullptr);

//    testThread1.detach();
//    testThread2.detach();

//    pthread_create(&testThread1, NULL, testThreadFunc1, NULL );
//    pthread_create(&testThread2, NULL, testThreadFunc2, NULL );

//    createthread3();
//    createthread4();
//    createthread5();

    while(1){
        if( true )  LOG_ERROR << "aaaa";
          LOG_DEBUG << "b";
          LOG_WARN  << "c";
          LOG_INFO  << "e";
          usleep(200000);
    };
    return 0;
}
