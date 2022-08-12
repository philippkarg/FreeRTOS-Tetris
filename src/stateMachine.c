#include "stateMachine.h"
#include "input.h"
#include "game.h"
#include "opponent.h"

/**
 * @addtogroup state
 * @{
 * @name State Machine Definitions
 * @{
 */
#define STATE_QUEUE_LENGTH 1        ///< Length of the state queue
#define STATE_COUNT 3               ///< Total number of states
#define STATE_ONE 0                 ///< State 1
#define STATE_TWO 1                 ///< State 2
#define STATE_THREE 2               ///< State 3
#define NEXT_TASK 0                 ///< Next task
#define PREV_TASK 1                 ///< Previous task
#define STARTING_STATE STATE_ONE    ///< Initial state
#define STATE_DEBOUNCE_DELAY 300    ///< Delay between state changes
///@}

const uint8_t nextStateSignal       = NEXT_TASK;    ///< Signal to change to the next state
const uint8_t prevStateSignal       = PREV_TASK;    ///< Signal to change to the previous state
static TaskHandle_t StateMachine    = NULL;         ///< @ref TaskHandle_t "Task" for the State Machine
QueueHandle_t StateQueue            = NULL;         ///< @ref QueueHandle_t "Queue" for the different states

/**
 * @brief Change the state, either forwards of backwards.
 * @param[inout] state (uint8_t*): Current state, to be changed.
 * @param[in] forwards (uint8_t): Whether to change to the next or previous state. 
 */
static void changeState(volatile uint8_t *state, uint8_t forwards)
{
    switch(forwards)
    {
        case NEXT_TASK:
            if(*state == STATE_COUNT - 1)
                *state = 0;
            else
                (*state)++;
            break;
        case PREV_TASK:
            if(*state == 0)
                *state = STATE_COUNT - 1;
            else
                (*state)--;
            break;
        default:
            break;
    }
}

/**
 * @brief Basic state machine with sequential states.
 *
 * -# Read from #StateQueue, whether to switch to the next or previous task.
 * -# Calculate the current state by calling changeState().
 * -# Depending on the current state, resume or suspend specific tasks.
 */
static void stateMachine()
{
    uint8_t currentState = STARTING_STATE; // Default state
    bool stateChanged = true; // Only re-evaluate state if it has changed
    uint8_t input = 0;
    const int stateChangePeriod = STATE_DEBOUNCE_DELAY;
    TickType_t last_change = xTaskGetTickCount();

    while (1)
    {
        if(stateChanged) goto initial_state;

        // Handle state machine input
        if( StateQueue &&
            (xQueueReceive(StateQueue, &input, portMAX_DELAY) == pdTRUE) &&
            (xTaskGetTickCount() - last_change > stateChangePeriod))
        {
            changeState(&currentState, input);
            stateChanged = true;
            last_change = xTaskGetTickCount();
        }

initial_state:
        // Handle current state
        if(stateChanged)
        {
            switch(currentState)
            {
                // Start Main Menu Task
                case STATE_ONE:
                    if(MainMenuTask)
                        vTaskResume(MainMenuTask);
                    if(GameTask)
                        vTaskSuspend(GameTask);
                    if(PauseTask)
                        vTaskSuspend(PauseTask);
                    break;
                // Start Game Task
                case STATE_TWO:
                    if(MainMenuTask)
                        vTaskSuspend(MainMenuTask);
                    if(GameTask)
                        vTaskResume(GameTask);
                    if(PauseTask)
                        vTaskSuspend(PauseTask);
                    break;
                // Start Pause Task
                case STATE_THREE:
                    if(MainMenuTask)
                        vTaskSuspend(MainMenuTask);
                    if(GameTask)
                        vTaskSuspend(GameTask);
                    if(PauseTask)
                        vTaskResume(PauseTask);   
                default:
                    break;
            }
            stateChanged = false;
        }
    }
}

int iCheckStateInput(player_mode_t playerMode, bool isConnected, rotation_t rotationMode, bool gameOver)
{
    static debounce_button_t    debounceEsc = { 0 };
    static debounce_button_t    debounceR   = { 0 };
    static debounce_button_t    debounceM   = { 0 };
    static debounce_button_t    debounceS   = { 0 };

    if(xSemaphoreTake(buttons.lock, 0) == pdTRUE)
    {
        // Switch to PauseTask if GameTask is running
        if( eTaskGetState(GameTask) == eRunning &&
            bGameDebounceButton(buttons.buttons[SDL_SCANCODE_ESCAPE], &debounceEsc.lastState)) 
        {
            if(StateQueue)
            {
                xSemaphoreGive(buttons.lock);
                xQueueSend(StateQueue, &nextStateSignal, 0);
                return 0;
            }
            return -1;
        }

        // Switch to GameTask ifPauseTask is running. 
        // Only possible if not in game over screen
        if(eTaskGetState(PauseTask) == eRunning)
        {
            if( !gameOver &&
                isConnected &&
                bGameDebounceButton(buttons.buttons[SDL_SCANCODE_ESCAPE], &debounceEsc.lastState)) 
            {
                if(StateQueue)
                {
                    xSemaphoreGive(buttons.lock);
                    xQueueSend(StateQueue, &prevStateSignal, 0);
                    return 0;
                }
                return -1;
            }
            
            // Switch to GameTask ifPauseTask is running.
            // Resets game & binary.
            if(bGameDebounceButton(buttons.buttons[SDL_SCANCODE_R], &debounceR.lastState))
            {
                if(StateQueue)
                {
                    xSemaphoreGive(buttons.lock);

                    xSemaphoreGive(ResetUDPSignal);
                    xSemaphoreGive(ResetGameSignal);

                    if(gameOver) xTaskNotifyGive(ScoreTask);

                    xQueueSend(StateQueue, &prevStateSignal, 0);
                    return 0;
                }
                return -1;
            }

            // Switch to MainMenuTask ifPauseTask is running.
            // Resets game & binary.
            if(bGameDebounceButton(buttons.buttons[SDL_SCANCODE_M], &debounceM.lastState)) 
            {
                if(StateQueue)
                {
                    xSemaphoreGive(buttons.lock);
                    xSemaphoreGive(ResetUDPSignal);
                    xSemaphoreGive(ResetGameSignal);
                    if(gameOver) xTaskNotifyGive(ScoreTask);
                    xQueueSend(StateQueue, &nextStateSignal, 0);
                    return 0;
                }
                return -1;
            }
        }
        // Switch to GameTask ifMainMenuTask is running.
        // Only possible if rotation & playerMode are selected (&connected in multiplayer mode)
        if( eTaskGetState(MainMenuTask) == eRunning &&
            ((playerMode == MULTI_PLAYER && isConnected) || playerMode == SINGLE_PLAYER) &&
            rotationMode != NO_ROTATION &&
            bGameDebounceButton(buttons.buttons[SDL_SCANCODE_S], &debounceS.lastState)) 
        {
            if(StateQueue)
            {
                xSemaphoreGive(buttons.lock);

                if(playerMode == MULTI_PLAYER) 
                    xSemaphoreGive(NextTetrominoSignal);

                xQueueSend(StateQueue, &nextStateSignal, 0);
                return 0;
            }
            return -1;
        } 
        xSemaphoreGive(buttons.lock);
    }
    return 0;
}

int iStateMachineInit()
{
    StateQueue = xQueueCreate(STATE_QUEUE_LENGTH, sizeof(uint8_t));
    if(!StateQueue)
    {
        PRINT_ERROR("Could not open state queue");
        goto err_state_queue;
    }

    if(xTaskCreate(stateMachine, "StateMachine", mainGENERIC_STACK_SIZE*2, NULL, configMAX_PRIORITIES-1, StateMachine) != pdPASS)
    {
        PRINT_TASK_ERROR("StateMachine");
        goto err_statemachine;
    }

    return 0;
    
    vTaskDelete(StateMachine);
    err_statemachine:
        vQueueDelete(StateQueue);
    err_state_queue:
        return -1;
}

///@}