#include "source_1.h"

pthread_t testThread3;
static void* testThreadFunc3(void*)
{
    while (1) {
        LOG_FUNC_START;
//        cout << log_level.to_string() << endl;
        //log_queue.push(log_getPreLog(__FILENAME__, __LINE__) + " : " + __func__);
        usleep(500000);
        LOG_FUNC_END;
    }
    return nullptr;
}

void createthread3()
{
    pthread_create(&testThread3, NULL, testThreadFunc3, NULL );
}

