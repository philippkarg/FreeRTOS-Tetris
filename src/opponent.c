#include "opponent.h"
#include "input.h"
#include "game.h"
#include "gui.h"

#include "AsyncIO.h"
#include "FreeRTOS.h"

/**
 * @addtogroup opponent
 * @{
 * @name UDP Definitions
 * @{
 */
#define UDP_BUFFER_SIZE     1024 ///< Buffer size of the UDP connection
#define UDP_RECEIVE_PORT    1234 ///< UDP port for receiving data
#define UDP_TRANSMIT_PORT   1235 ///< UDP port for transmitting data
///@}

// **********************************************************************************
// Global Variables *****************************************************************
// **********************************************************************************
// Task Handles *********************************************************************
TaskHandle_t UDPControlTask             = NULL; ///< @ref TaskHandle_t "Task" to control the UDP socket
// Semaphore Handles ****************************************************************
static SemaphoreHandle_t HandleUDP      = NULL; ///< @ref SemaphoreHandle_t "Mutex" for handling UDP
SemaphoreHandle_t NextTetrominoSignal   = NULL; ///< @ref SemaphoreHandle_t "Signal" for the next Tetromino
// Queue Handles ********************************************************************
QueueHandle_t TetrominoQueue            = NULL; ///< @ref QueueHandle_t "Queue" for receiving tetromino types from opponent
// aIO Handles **********************************************************************
static aIO_handle_t UDPSocReceive       = NULL; ///< @ref aIO_handle_t "AsyncIO Handle" for receiving data via UDP

// **********************************************************************************
// Forward Declarations *************************************************************
// **********************************************************************************
/**
 * @brief Parse @p buffer into the corresponding @ref game_mode_t.
 * @param[in] buffer (const char*): String to parse. 
 * @param[out] mode ( @ref game_mode_t *): Object to parse game mode into.
 * @return (bool): whether @p buffer could be parsed.
 */
static bool parseMode(const char *buffer, game_mode_t *mode);

/**
 * @brief Parse @p buffer into the corresponding @ref tetromino_type_t.
 * @param[in] buffer (const char*): String to parse. 
 * @param[out] type ( @ref tetromino_type_t *): Object to parse Tetromino type into. 
 * @return (bool): whether @p buffer could be parsed.
 */
static bool parseTetrominoType(const char* buffer, tetromino_type_t *type);

/**
 * @brief Function that reads a game selection from the user.
 * @param[in] buf (char*): String to put the selected game mode in.
 * @return (bool): whether a game mode was selected.
 */
static bool selectGameMode(char *buf);

// **********************************************************************************
// Function Definitions *************************************************************
// **********************************************************************************
/**
 * @brief Interrupt-Servie-Routine that handles incoming UDP messages.
 * @param[in] readSize (size_t): Size of the buffer. 
 * @param[in] buffer (char*): The UDP message.
 * @param[in] args (void*): Additional arguments.
 */
static void UDPHandler(size_t readSize, char *buffer, void *args)
{
    game_mode_t mode = NO_MODE;
    tetromino_type_t type = NO_TYPE;
    bool isConnected = false;

    BaseType_t xHigherPriorityTaskWoken1 = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken2 = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken3 = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken4 = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken5 = pdFALSE;

    if(xSemaphoreTakeFromISR(HandleUDP, &xHigherPriorityTaskWoken1) == pdTRUE)
    {
        bool okSendMode = parseMode(buffer, &mode);
        bool okSendTetromino = parseTetrominoType(buffer, &type);
        isConnected = mode != NO_MODE;

        // Write to queues
        if(ConnectionQueue && isConnected)
            xQueueSendFromISR(ConnectionQueue, (void *)&isConnected, &xHigherPriorityTaskWoken2);
        if(GameModeQueue && okSendMode)
            xQueueSendFromISR(GameModeQueue, (void *)&mode, &xHigherPriorityTaskWoken3);
        if(TetrominoQueue && okSendTetromino)
            xQueueSendFromISR(TetrominoQueue, (void * )&type, &xHigherPriorityTaskWoken4);

        xSemaphoreGiveFromISR(HandleUDP, &xHigherPriorityTaskWoken5);
    
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken1 |
                            xHigherPriorityTaskWoken2 |
                            xHigherPriorityTaskWoken3 |
                            xHigherPriorityTaskWoken4 |
                            xHigherPriorityTaskWoken5);
    }
    else
    {
        PRINT_ERROR("Not able to take HandleUDP mutex");
    }
}

/**
 * @brief Task that controls incoming/outgoing UDP messages.
 */
static void vUDPControlTask()
{
    // Init *************************************************************************
    static char buf[50];
    bool initTetrominoQueue = true;
    // Queue & Signal
    TetrominoQueue      = xQueueCreate(2, sizeof(tetromino_type_t));
    NextTetrominoSignal = xSemaphoreCreateBinary();
    if(!TetrominoQueue)         exit(EXIT_FAILURE);
    if(!NextTetrominoSignal)    exit(EXIT_FAILURE);
    // Socket
    UDPSocReceive = aIOOpenUDPSocket(NULL, UDP_RECEIVE_PORT, UDP_BUFFER_SIZE, UDPHandler, NULL);
    printf("UDP socket opened on port %d\n", UDP_RECEIVE_PORT);
    // Loop *************************************************************************
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(15));

        vGetButtonInput();

        // If the user resets the game, a new seed is generated & the TetrominoQueue is reset
        if(xSemaphoreTake(ResetUDPSignal, 0) == pdTRUE)
        {
            int seed = time(NULL);
            sprintf(buf, "SEED=%d", seed);
            aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
            initTetrominoQueue = true;
            xQueueReset(TetrominoQueue);
            xSemaphoreGive(NextTetrominoSignal);
        }

        // As long as the opponent is not connected, 
        // a new seed is set & the game mode is checked every iteration
        if(xSemaphoreTake(NoConnectionSignal, 0) == pdTRUE)
        {
            int seed = time(NULL);
            sprintf(buf, "SEED=%d", seed);
            aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
            sprintf(buf, "MODE");
            aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
        }
        
        // If the user selects a new gamemode in the main menu, that mode is send to the opponent
        if(selectGameMode(buf))
        {
           aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
           sprintf(buf, "MODE");
           aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
        }

        // If the TetrominoQueue is not full, the next tetromino type is requested from the opponent
        if( xSemaphoreTake(NextTetrominoSignal, 0) == pdTRUE &&
            uxQueueSpacesAvailable(TetrominoQueue) != 0)
        {
            if(initTetrominoQueue)
            {
                initTetrominoQueue = false;
                sprintf(buf, "NEXT");
                aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
                sprintf(buf, "NEXT");
                aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
            }
            else
            {
                sprintf(buf, "NEXT");
                aIOSocketPut(UDP, NULL, UDP_TRANSMIT_PORT, buf, strlen(buf));
            }
        }
    }
}

static bool parseMode(const char *buffer, game_mode_t *mode)
{
    if(!strcmp(buffer, "MODE=FAIR"))            *mode = FAIR;
    if(!strcmp(buffer, "MODE=EASY"))            *mode = EASY;
    if(!strcmp(buffer, "MODE=HARD"))            *mode = HARD;
    if(!strcmp(buffer, "MODE=RANDOM"))          *mode = RANDOM;
    if(!strcmp(buffer, "MODE=DETERMINISTIC"))   *mode = DETERMINISTIC;

    return *mode != NO_MODE;
}

static bool parseTetrominoType(const char* buffer, tetromino_type_t *type)
{
    if(!strcmp(buffer, "NEXT=S"))   *type = S;
    if(!strcmp(buffer, "NEXT=Z"))   *type = Z;
    if(!strcmp(buffer, "NEXT=J"))   *type = J;
    if(!strcmp(buffer, "NEXT=L"))   *type = L;
    if(!strcmp(buffer, "NEXT=T"))   *type = T;
    if(!strcmp(buffer, "NEXT=O"))   *type = O;
    if(!strcmp(buffer, "NEXT=I"))   *type = I;

    return *type != NO_TYPE;
}

static bool selectGameMode(char *buf)
{
    // Bounds for the bGUIPushButton function
    coord_t lowBoundEasy            = {SCREEN_WIDTH *1/5 - MODE_EASY_WIDTH/2, MODES_HEIGHT-5},
            highBoundEasy           = {SCREEN_WIDTH *1/5 + MODE_EASY_WIDTH/2, MODES_HEIGHT+20},
            lowBoundFair            = {SCREEN_WIDTH *2/5 - MODE_FAIR_WIDTH/2, MODES_HEIGHT-5},
            highBoundFair           = {SCREEN_WIDTH *2/5 + MODE_FAIR_WIDTH/2, MODES_HEIGHT+20},
            lowBoundHard            = {SCREEN_WIDTH *3/5 - MODE_HARD_WIDTH/2, MODES_HEIGHT-5},
            highBoundHard           = {SCREEN_WIDTH *3/5 + MODE_HARD_WIDTH/2, MODES_HEIGHT+20},
            lowBoundRandom          = {SCREEN_WIDTH *4/5 - MODE_RANDOM_WIDTH/2, MODES_HEIGHT-5},
            highBoundRandom         = {SCREEN_WIDTH *4/5 + MODE_RANDOM_WIDTH/2, MODES_HEIGHT+20},
            lowBoundDeterministic   = {SCREEN_WIDTH/2 - MODE_DETERMINISTIC_WIDTH/2, MODES_HEIGHT+20},
            highBoundDeterministic  = {SCREEN_WIDTH/2 + MODE_DETERMINISTIC_WIDTH/2, MODES_HEIGHT+45};
    
    // Select the desired mode
    bool modeSelected = false;
    if(bGUIPushButton(lowBoundEasy, highBoundEasy))
    {  
        sprintf(buf, "MODE=EASY");
        modeSelected = true;
    }
    if(bGUIPushButton(lowBoundFair, highBoundFair))
    {  
        sprintf(buf, "MODE=FAIR");
        modeSelected = true;
    }
    if(bGUIPushButton(lowBoundHard, highBoundHard))
    {  
        sprintf(buf, "MODE=HARD");
        modeSelected = true;
    }
    if(bGUIPushButton(lowBoundRandom, highBoundRandom))
    {  
        sprintf(buf, "MODE=RANDOM");
        modeSelected = true;
    }
    if(bGUIPushButton(lowBoundDeterministic, highBoundDeterministic))
    {  
        sprintf(buf, "MODE=DETERMINISTIC");
        modeSelected = true;
    }

    return modeSelected;
}

int iOpponentInit(void)
{
    HandleUDP = xSemaphoreCreateMutex();
    if(!HandleUDP)
    {
        PRINT_ERROR("Failed to create UDPHandle mutex");
        goto err_handle_udp;
    }

    if(xTaskCreate(vUDPControlTask, "UDPControlTask", mainGENERIC_STACK_SIZE, NULL,
                    mainGENERIC_PRIORITY, &UDPControlTask) != pdPASS)
    {
        PRINT_TASK_ERROR("UDPControlTask");
        goto err_udp_control_task;
    }
    
    return 0;

        vTaskDelete(UDPControlTask);
    err_udp_control_task:
        vSemaphoreDelete(HandleUDP);
    err_handle_udp:
        return -1;
}