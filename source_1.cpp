#include "source_1.h"

pthread_t testThread3;
static void* testThreadFunc3(void*)
{
    while (1) {
        LOG_FUNC_START;
        usleep(5000);
        LOG_FUNC_END;
    }
    return nullptr;
}

void createthread3()
{
    pthread_create(&testThread3, NULL, testThreadFunc3, NULL );
}

