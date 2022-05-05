#include "input.h"

buttons_buffer_t buttons = { 0 };

bool bGameDebounceButton(char currentState, bool* previousState)
{
    bool result = false;
    if(currentState && !(*previousState))
    {
        result = true;
        *previousState = true;
    }
    else if(!currentState)
        *previousState = false;

    return result;
}

void vGetButtonInput(void)
{
    if (xSemaphoreTake(buttons.lock, 0) == pdTRUE)
    {
        xQueueReceive(buttonInputQueue, &buttons.buttons, 0);
        xSemaphoreGive(buttons.lock);
    }
}

bool bGUIPushButton(const coord_t lowBound, const coord_t highBound)
{
    return  tumEventGetMouseX() > lowBound.x    && 
            tumEventGetMouseX() < highBound.x   &&
            tumEventGetMouseY() > lowBound.y    && 
            tumEventGetMouseY() < highBound.y   &&
            tumEventGetMouseLeft();
}

int iInputInit()
{
    buttons.lock = xSemaphoreCreateMutex();
    if(!buttons.lock)
    {
        PRINT_ERROR("Failed to create buttons lock");
        return -1;
    }

    return 0;
}