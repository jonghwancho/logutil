#include "source_3.h"

pthread_t testThread5;
static void* testThreadFunc5(void*)
{
    while (1) {
        LOG_FUNC_START;
        usleep(5000);
        LOG_FUNC_END;
    }
    return nullptr;
}

void createthread5()
{
    pthread_create(&testThread5, NULL, testThreadFunc5, NULL );
}
