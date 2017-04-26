#include "source_3.h"

pthread_t testThread5;
static void* testThreadFunc5(void*)
{
    while (1) {
        LOG_FUNC_START;
//        cout << log_level.to_string() << endl;
//        LOG_INFO << " This is threadfunc :  " << __func__ ;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(500000);
        LOG_FUNC_END;
    }
    return nullptr;
}

void createthread5()
{
    pthread_create(&testThread5, NULL, testThreadFunc5, NULL );
}
