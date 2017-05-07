#include "source_2.h"

pthread_t testThread4;
static void* testThreadFunc4(void*)
{
    while (1) {
        LOG_FUNC_START;
        usleep(40000);
        LOG_FUNC_END;
    }
    return nullptr;
}

void createthread4()
{
    pthread_create(&testThread4, NULL, testThreadFunc4, NULL );
}
