#include "input.h"
#include "game.h"
#include "opponent.h"
#include "stateMachine.h"

static TaskHandle_t BufferSwap = NULL; ///< \ref TaskHandle_t "Task Handle" for swapBuffers() task

/*!
 * \ingroup game
 * \brief Task that updates the screen every 20ms & gives #ScreenLock & #DrawSignal.
 */
static void swapBuffers()
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t frameratePeriod = 20;

    tumDrawBindThread(); // Setup Rendering handle with correct GL context

    while (1)
    {
        if (xSemaphoreTake(ScreenLock, portMAX_DELAY) == pdTRUE)
        {
            tumDrawUpdateScreen();
            tumEventFetchEvents(FETCH_EVENT_NONBLOCK);
            xSemaphoreGive(ScreenLock);
            xSemaphoreGive(DrawSignal);
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(frameratePeriod));
        }
    }
}

int main(int argc, char *argv[])
{
    char *bin_folder_path = tumUtilGetBinFolderPath(argv[0]);
    
    prints("Initializing: ");

    // Init Emulator libs
    if (tumDrawInit(bin_folder_path))
    {
        PRINT_ERROR("Failed to initialize drawing");
        goto err_init_drawing;
    }
    else
        prints("drawing");

    if (tumEventInit())
    {
        PRINT_ERROR("Failed to initialize events");
        goto err_init_events;
    }
    else
        prints(", events");

    if (tumSoundInit(bin_folder_path))
    {
        PRINT_ERROR("Failed to initialize audio");
        goto err_init_audio;
    }
    else
        prints(", and audio\n");

    if(xTaskCreate(swapBuffers, "BufferSwapTask",mainGENERIC_STACK_SIZE*2, NULL, configMAX_PRIORITIES, BufferSwap) != pdPASS)
    {
        PRINT_TASK_ERROR("BufferSwapTask");
        goto err_bufferswap;
    }

    // Init modules
    iInputInit();
    iStateMachineInit();
    iGameInit();
    iOpponentInit();

    vTaskStartScheduler();

    return EXIT_SUCCESS;

    vTaskDelete(BufferSwap);
err_bufferswap:
    tumSoundExit();
err_init_audio:
    tumEventExit();
err_init_events:
    tumDrawExit();
err_init_drawing:
    return EXIT_FAILURE;
}

/*Application support for Static Task Creation*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    
}

/*Application support for timers*/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vMainQueueSendPassed(void)
{
    /* This is just an example implementation of the "queue send" trace hook. */
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vApplicationIdleHook(void)
{
#ifdef __GCC_POSIX__
    struct timespec xTimeToSleep, xTimeSlept;
    /* Makes the process more agreeable when using the Posix simulator. */
    xTimeToSleep.tv_sec = 1;
    xTimeToSleep.tv_nsec = 0;
    nanosleep(&xTimeToSleep, &xTimeSlept);
#endif
}
