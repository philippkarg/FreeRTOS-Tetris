/*!
 * \file game.c
 * \authors Philipp Karg (philipp.karg@tum.de)
 * 
 * \brief File containing the Game's Tasks.
 * \date 04.02.2021
 */

#include "game.h"
#include "input.h"
#include "stateMachine.h"
#include "logic.h"
#include "gui.h"
#include "opponent.h"

/*!
 * \name Delays
 * @{
 */
#define POS_UPDATE_DELAY 500    ///< Initial value for the delay for updating a Tetromino's position
#define DELAY_AT_BOTTOM 300     ///< Initial value for the delay when a Tetromino hits the bottom
///@}

// **********************************************************************************
// Global Variables *****************************************************************
// **********************************************************************************
/*!
 * \addtogroup game
 * @{
 */
// **********************************************************************************
/// \name Task Handles
///@{
TaskHandle_t MainMenuTask                   = NULL; ///< \ref TaskHandle_t "Task" for the Main Menu Task
TaskHandle_t GameTask                       = NULL; ///< \ref TaskHandle_t "Task" for the Game Task
TaskHandle_t PauseTask                      = NULL; ///< \ref TaskHandle_t "Task" for the Pause Task
TaskHandle_t ScoreTask                      = NULL; ///< \ref TaskHandle_t "Task" for the Score Task
///@}

// **********************************************************************************
/// \name Timer Handles
///@{
TimerHandle_t PosUpdateTimer                = NULL; ///< \ref TimerHandle_t "Timer" for updating a Tetromino's position
TimerHandle_t DelayAtGroundTimer            = NULL; ///< \ref TimerHandle_t "Timer" for the delay, when a Tetromino hits the bottom
///@}

// **********************************************************************************
/// \name Semaphore Handles
///@{
SemaphoreHandle_t ScreenLock                = NULL; ///< \ref SemaphoreHandle_t "Semaphore" for locking the screen
SemaphoreHandle_t DrawSignal                = NULL; ///< \ref SemaphoreHandle_t "Signal" for drawing
SemaphoreHandle_t YSignal             = NULL; ///< \ref SemaphoreHandle_t "Signal" for updating a Tetromino's y-position
SemaphoreHandle_t XSignal             = NULL; ///< \ref SemaphoreHandle_t "Signal" for updating a Tetromino's x-position
SemaphoreHandle_t FallSignal         = NULL; ///< \ref SemaphoreHandle_t "Signal" if the down-key is held
SemaphoreHandle_t RotationSignal            = NULL; ///< \ref SemaphoreHandle_t "Signal" for rotating a Tetromino
SemaphoreHandle_t InitNextSignal   = NULL; ///< \ref SemaphoreHandle_t "Signal" for initializing the next Tetromino
SemaphoreHandle_t ResetGameSignal           = NULL; ///< \ref SemaphoreHandle_t "Signal" for resetting the game
SemaphoreHandle_t ResetUDPSignal            = NULL; ///< \ref SemaphoreHandle_t "Signal" for resetting the UDP socket
SemaphoreHandle_t NoConnectionSignal        = NULL; ///< \ref SemaphoreHandle_t "Signal" if the binary is not connected
///@}

// **********************************************************************************
/// \name Queue Handles
///@{
QueueHandle_t LeftRightQueue                = NULL; ///< \ref QueueHandle_t "Queue" for left & right input
QueueHandle_t ConnectionQueue               = NULL; ///< \ref QueueHandle_t "Queue" for the connection status
QueueHandle_t GameModeQueue                 = NULL; ///< \ref QueueHandle_t "Queue" for the game mode
QueueHandle_t PlayerModeQueue               = NULL; ///< \ref QueueHandle_t "Queue" for the player mode
QueueHandle_t RotationModeQueue             = NULL; ///< \ref QueueHandle_t "Queue" for the rotation mode
QueueHandle_t ScoreQueue                    = NULL; ///< \ref QueueHandle_t "Queue" for the current score
QueueHandle_t HighScoresQueue               = NULL; ///< \ref QueueHandle_t "Queue" for the highscores 
QueueHandle_t LevelQueue                    = NULL; ///< \ref QueueHandle_t "Queue" for the current level
QueueHandle_t GameOverQueue                 = NULL; ///< \ref QueueHandle_t "Queue" for the game-over status
///@}
///@}

// **********************************************************************************
// Forward Declarations *************************************************************
// **********************************************************************************
/*!
 * \ingroup game
 * \brief Check for button input, give semaphores and write the pressed buttons in a queue.
 * \param[inout] buttonPressed (bool): whether a button was pressed.
 */
static void buttonInput(bool *buttonPressed);

/*!
 * \ingroup game
 * \brief Change a timer's period depending on the current level & start the timer.
 * \param[inout] timer ( \ref TimerHandle_t): Timer to change period for. 
 * \param[in] level (uint8_t): Current level. 
 * \param[in] delay (int): The delay for changing the period. 
 */
static void changeTimerPeriod(TimerHandle_t timer, uint8_t level, int delay);

/*!
 * \ingroup game
 * \brief Task that handles the Main Menu.
 * 
 * This Task draws the following textures:
 * - The Main Menu (vGUIDrawMainMenu())
 * - The player mode selection (bGUIDrawPlayerModeSelection())
 * - The rotation mode selection (bGUIDrawRotationSelection())
 * - The button to switch to the level selection (bGUIDrawLevelMenuSelection())
 * - And if the button was clicked, the actual level selection screen (bGUIDrawLevelScreen())
 * 
 * Furthermore, this task:
 * - sets the #LevelQueue
 * - checks if the opponent is connected
 * - stops/resumes the UDP task depending on the opponents connection
 */
static void mainMenuTask()
{
    // Init *************************************************************************
    // Queues & Semaphores
    GameModeQueue       = xQueueCreate(10, sizeof(game_mode_t));
    PlayerModeQueue     = xQueueCreate(1, sizeof(player_mode_t));
    RotationModeQueue   = xQueueCreate(1, sizeof(rotation_t));
    ConnectionQueue     = xQueueCreate(1, sizeof(bool));
    LevelQueue          = xQueueCreate(1, sizeof(uint8_t));
    NoConnectionSignal  = xSemaphoreCreateBinary();
    
    if(!GameModeQueue)      exit(EXIT_FAILURE);
    if(!PlayerModeQueue)    exit(EXIT_FAILURE);
    if(!RotationModeQueue)  exit(EXIT_FAILURE);
    if(!ConnectionQueue)    exit(EXIT_FAILURE);
    if(!LevelQueue)         exit(EXIT_FAILURE);
    if(!NoConnectionSignal) exit(EXIT_FAILURE);

    game_mode_t     mode            = NO_MODE;
    player_mode_t   playerMode      = NO_PLAYER;
    rotation_t      rotationMode    = NO_ROTATION;
    bool            isConnected     = false;
    bool            drawLevelScreen = false;
    uint8_t         currentLevel    = 0;
    score_t         *highScores     = (score_t*) calloc(3, sizeof(score_t));

    // Loop *************************************************************************
    while(1)
    {
        if(DrawSignal && xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE)
        {
            // Get button input
            vGetButtonInput();
            
            // If isConnected is false, check for a connection by giving NoConnectionSignal
            if(!isConnected) xSemaphoreGive(NoConnectionSignal);
            // Read queues
            if(ConnectionQueue)
                xQueueReceive(ConnectionQueue, &isConnected, 0);
            if(GameModeQueue)
                xQueueReceive(GameModeQueue, &mode, 0);
            if(HighScoresQueue)
                xQueueReceive(HighScoresQueue, &highScores, 0);

            // Entering critical section that cannot be interrupted *****************
            taskENTER_CRITICAL();

            if(xSemaphoreTake(ScreenLock, portMAX_DELAY) == pdTRUE)
            {
                // Draw *************************************************************
                if(!drawLevelScreen)
                {
                    tumDrawClear(BACKGROUND_COLOR);
                    vGUIDrawMainMenu(mode, playerMode, rotationMode, isConnected);
                    // Selections for the different modes/menus
                    if(bGUIDrawPlayerModeSelection(&playerMode))
                    {
                        xQueueReset(PlayerModeQueue);
                        xQueueSend(PlayerModeQueue, &playerMode, 0);
                    }
                    if(bGUIDrawRotationSelection(&rotationMode))
                    {
                        xQueueReset(RotationModeQueue);
                        xQueueSend(RotationModeQueue, &rotationMode, 0);
                    }
                    if(bGUIDrawLevelMenuSelection())
                        drawLevelScreen = true;
                }
                else if(bGUIDrawLevelScreen(&currentLevel, highScores))
                    drawLevelScreen = false;
            }
            // Return Screen Lock
            xSemaphoreGive(ScreenLock);
            // Exiting critical section *********************************************
            taskEXIT_CRITICAL();

            // Set level queue
            if(LevelQueue)
            {
                xQueueReset(LevelQueue);
                xQueueSend(LevelQueue, &currentLevel, 0);
            }
            // Depending on the player mode, suspend or resume the UDP Task
            if(playerMode == SINGLE_PLAYER && eTaskGetState(UDPControlTask) == eRunning) 
                vTaskSuspend(UDPControlTask);
            if(playerMode == MULTI_PLAYER && eTaskGetState(UDPControlTask) == eSuspended)
                vTaskResume(UDPControlTask);

            iCheckStateInput(playerMode, isConnected, rotationMode, false);
        }
    }
}

/*!
 * \ingroup game
 * \brief Callback function for the Timer that updates the position.
 * \param[in] PosUpdateTimer ( \ref TimerHandle_t): Timer to callback. 
 */
static void posUpdateTimerCallback(TimerHandle_t PosUpdateTimer)
{
    xSemaphoreGive(YSignal);
}

/*!
 * \ingroup game
 * \brief Callback function for the Timer that starts a delay if a Tetromino hits the bottom.
 * \param[in] DelayAtGroundTimer ( \ref TimerHandle_t): Timer to callback. 
 */
static void delayAtBottomTimerCallback(TimerHandle_t DelayAtGroundTimer)
{
    xSemaphoreGive(InitNextSignal);
}

/*!
 * \ingroup game
 * \brief Task that handles the Tetris gameplay.
 * 
 * -# If the #ResetGameSignal has been received, reset the game.
 * -# At the beginning/if the game is reset, the first Tetromino is initialized.
 * -# The Tetrominos position is updated, including x & y position & rotation.
 * -# If a Tetromino hits the ground, start a Timer, so that the player can move around the Tetromino further.
 * -# If the Tetromino is not moved anymore, initialize the next one & add the old one to the landed Tetrominos.
 * -# Draw all aspects of the game, e.g. the falling Tetromino & the static elements.
 * -# If the game is over, save the score & switch to the pause task.
 */
static void gameTask()
{
    // ******************************************************************************
    // Init *************************************************************************
    // ******************************************************************************
    // Timer ************************************************************************
    PosUpdateTimer      = xTimerCreate( "PosUpdateTimer",
                                        pdMS_TO_TICKS(POS_UPDATE_DELAY),
                                        pdTRUE, 
                                        NULL,
                                        posUpdateTimerCallback);
    DelayAtGroundTimer  = xTimerCreate( "DelayAtGroundTimer",
                                        pdMS_TO_TICKS(DELAY_AT_BOTTOM),
                                        pdFALSE, 
                                        NULL,
                                        delayAtBottomTimerCallback);
    if(!PosUpdateTimer)     exit(EXIT_FAILURE);
    if(!DelayAtGroundTimer) exit(EXIT_FAILURE);

    // Queues ***********************************************************************
    LeftRightQueue  = xQueueCreate(1, sizeof(int));
    GameOverQueue   = xQueueCreate(1, sizeof(bool));
    ScoreQueue      = xQueueCreate(1, sizeof(score_t));
    HighScoresQueue = xQueueCreate(1, sizeof(score_t*));
    
    if(!LeftRightQueue)     exit(EXIT_FAILURE);
    if(!ScoreQueue)         exit(EXIT_FAILURE);
    if(!HighScoresQueue)    exit(EXIT_FAILURE);
    if(!GameOverQueue)      exit(EXIT_FAILURE);

    // Semaphores *******************************************************************
    YSignal         = xSemaphoreCreateBinary();
    XSignal         = xSemaphoreCreateBinary();
    FallSignal      = xSemaphoreCreateBinary();
    RotationSignal  = xSemaphoreCreateBinary();
    InitNextSignal  = xSemaphoreCreateBinary();
    
    if(!YSignal)        exit(EXIT_FAILURE);
    if(!XSignal)        exit(EXIT_FAILURE);
    if(!FallSignal)     exit(EXIT_FAILURE);
    if(!RotationSignal) exit(EXIT_FAILURE);
    if(!InitNextSignal) exit(EXIT_FAILURE);

    // Flags ************************************************************************
    bool okNext             = false;
    bool initFirstTetromino = true;
    bool startDelayTimer    = true;
    bool gameOver           = false;
    bool isConnected        = true;
    bool buttonPressed      = false;
    
    // Array of landed tetrominos ***************************************************
    color_t landed[ROWS][COLS] = { 0 };

    // Modes ************************************************************************
    player_mode_t playerMode = NO_PLAYER;
    rotation_t rotationMode = NO_ROTATION;

    // Sequence of tetrominos for a fairer game *************************************
    tetromino_type_t tetrominoSequence[7] = { 0 };
    int sequenceIndex = 0;

    // Current & upcoming tetrominos ************************************************
    tetromino_t *tetromino  = malloc(sizeof *tetromino);
    tetromino_t *next       = malloc(sizeof *next);
    
    // Score ************************************************************************
    score_t *score = malloc(sizeof *score);
    
    // Images ***********************************************************************
    image_handle_t squares[NUMBER_OF_TETRIS_COLORS] = { NULL };
    vGUISetImageHandle(squares);

    // Loop *************************************************************************
    while(1)
    {
        if(DrawSignal && xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE)
        {
            // Reset ****************************************************************
            if(xSemaphoreTake(ResetGameSignal, 0) == pdTRUE)
            {
                xTimerStop(PosUpdateTimer, 0);
                xQueueReset(LeftRightQueue);

                initFirstTetromino = true;
                sequenceIndex = 0;
                gameOver = false;
                score->score = 0;
                score->rows = 0; 
                
                for(int i=0; i<7; i++)
                    tetrominoSequence[i] = NO_TYPE;

                for(int row=0; row<ROWS; row++)
                    for(int col=0; col<COLS; col++)
                        landed[row][col] = 0;     
            }

            // Initialize the game **************************************************
            if(initFirstTetromino)
            {
                initFirstTetromino = false;
                // Read Queues
                if(PlayerModeQueue)
                    xQueueReceive(PlayerModeQueue, &playerMode, 0);
                if(RotationModeQueue)
                    xQueueReceive(RotationModeQueue, &rotationMode, 0);
                if(LevelQueue)
                    xQueuePeek(LevelQueue, &score->level, 0);

                // When initalizing the game in multiplayer mode,
                // check if the binary sends the upcoming tetromino types.
                // If so, the binary is connected, if not it is not.
                // If the binary is connected, the connection status is sent via the
                // ConnectionQueue to the PauseTask.
                if(playerMode == MULTI_PLAYER)
                {
                    tetromino_type_t buf[2] = {NO_TYPE};
                    if(TetrominoQueue)
                        for(int i=0; i<2; i++)
                        {
                            if(xQueueReceive(TetrominoQueue, &buf[i], 0) == pdTRUE)
                                isConnected = true;
                            else
                                isConnected = false;
                        }
                    
                    // Set type for the current Tetromino & the upcoming one
                    if(buf[0] != NO_TYPE) tetromino->type = buf[0];
                    if(buf[1] != NO_TYPE) next->type = buf[1];

                    if(isConnected)
                    {
                        xSemaphoreGive(NextTetrominoSignal);
                        xQueueReset(ConnectionQueue);
                        xQueueSend(ConnectionQueue, &isConnected, 0);
                    }
                }
                // Initialize both Tetrominos
                vLogicInitTetromino(tetromino, tetrominoSequence, &sequenceIndex, playerMode);
                vLogicInitTetromino(next, tetrominoSequence, &sequenceIndex, playerMode);
                
                // This Semaphore is taken, so that when resetting the game,
                // the tetromino starts at the top and does not increase it's y-position
                // before drawing it the first time
                xSemaphoreTake(YSignal, 0);

                // Init the period of the timer that updates the Tetromino position with the current level
                changeTimerPeriod(PosUpdateTimer, score->level, POS_UPDATE_DELAY);
            }

            // Start of the main gameplay *******************************************
            // Handle button input
            vGetButtonInput();
            buttonInput(&buttonPressed);
            
            // Checking if the next tetromino will cause the game to be over,
            // before doing any movement.
            gameOver = bLogicCheckGameOver(tetromino, landed);
            if(!gameOver)
            {
                // If the DelayAtGroundTimer is active, it is reset, if:
                // - a button is pressed
                // - the tetromino can be moved further downwards
                if(xTimerIsTimerActive(DelayAtGroundTimer) != pdFALSE)
                {
                    coord_t down = {tetromino->position.x, tetromino->position.y + 1};
                    if(buttonPressed || bLogicCheckMove(tetromino->shape, down, landed))
                        xTimerStart(DelayAtGroundTimer, 0);
                }
                // Update Tetromino *************************************************
                // Move the Tetromino on the x-axis
                if(xSemaphoreTake(XSignal, 0) == pdTRUE)
                {
                    int pressedButton = 0;
                    if(xQueueReceive(LeftRightQueue, &pressedButton, 0) == pdTRUE)
                        vLogicUpdateXCoord(tetromino, landed, pressedButton);
                }

                // Rotate the Tetromino
                if(xSemaphoreTake(RotationSignal, 0) == pdTRUE)
                    vLogicRotate(tetromino, landed, rotationMode);

                // Move the Tetromino down
                if(xSemaphoreTake(FallSignal, 0) == pdTRUE)
                    okNext = !bLogicUpdateYCoord(tetromino, landed);
                else if(xSemaphoreTake(YSignal, 0) == pdTRUE)
                {
                    okNext = !bLogicUpdateYCoord(tetromino, landed);
                    if(!okNext && ENABLE_SOUND_EFFECTS)
                        tumSoundPlayUserSample(FALLING_SOUND); 
                }
                        
                // If a Tetromino hits the ground, 
                // a delay is started, so that the player has some time
                // to move the tetromino around after hitting the ground.
                // The timespan of this delay also decreases when the level gets higher
                if(okNext && startDelayTimer)
                {
                    if(ENABLE_SOUND_EFFECTS) tumSoundPlayUserSample(THUMP_SOUND);
                    // This function changes the timer's period and starts it as well
                    changeTimerPeriod(DelayAtGroundTimer, score->level, DELAY_AT_BOTTOM);
                    startDelayTimer = false;
                }
                // Initialize next Tetromino ****************************************
                // If the timer for the delay at ground has run out, this signal is given
                // If this signal is given, the next Tetromino can be initialized
                if(xSemaphoreTake(InitNextSignal, 0) == pdTRUE)
                {
                    xTimerStop(PosUpdateTimer, 0);                        
                    // Add the Tetromino to the array of landed Tetrominos
                    vLogicAddToLanded(tetromino, landed);
                    // Check if row(s) are full
                    if(vLogicRowFull(landed, score) && ENABLE_SOUND_EFFECTS)
                        tumSoundPlayUserSample(ROW_FULL_SOUND);
                    // The initialize the new (current) Tetromino
                    *tetromino = *next;
                    // If in multiplayer mode, read the next tetromino type from the opponent
                    if(playerMode == MULTI_PLAYER)
                    {
                        tetromino_type_t buf = NO_TYPE;
                        if(xQueueReceive(TetrominoQueue, &buf, 0) == pdTRUE)
                        {
                            isConnected = true;
                            if(buf != NO_TYPE) next->type = buf;
                            xSemaphoreGive(NextTetrominoSignal);
                        }
                        else
                            isConnected = false;
                    }

                    // Initialize the next Tetromino & reset flags
                    vLogicInitTetromino(next, tetrominoSequence, &sequenceIndex, playerMode);
                    okNext = false;
                    startDelayTimer = true;
                    // Restart the timer for updating the position
                    changeTimerPeriod(PosUpdateTimer, score->level, POS_UPDATE_DELAY);
                }
            }

            // Entering a critical section, that cannot be interrupted **************
            taskENTER_CRITICAL();
            // Draw *****************************************************************
            if(xSemaphoreTake(ScreenLock, portMAX_DELAY) == pdTRUE)
            {
                tumDrawClear(BACKGROUND_COLOR);
                // Draw static elements: score, level, rows
                vGUIDrawStatic(squares, score);
                vGUIDrawFPS();
                // Once again check if the game is over after moving the Tetromino
                if(!bLogicCheckGameOver(tetromino, landed))
                {
                    vGUIDrawTetromino(tetromino, squares);
                    vGUIDrawNextTetromino(next, squares);
                } 
                else if(ENABLE_SOUND_EFFECTS)
                    tumSoundPlayUserSample(GAME_OVER_SOUND);
                vGUIDrawLanded(landed, squares);
            }
            xSemaphoreGive(ScreenLock);
            // Exiting critical section *********************************************
            taskEXIT_CRITICAL();

            // Send game over status
            xQueueReset(GameOverQueue);
            xQueueSend(GameOverQueue, &gameOver, 0);

            // If the connection to the binary stops,
            // the connection status is sent to the pause task,
            // to which the state machine also switches.
            if(!isConnected && playerMode == MULTI_PLAYER)
            {  
                xQueueReset(ConnectionQueue);
                xQueueSend(ConnectionQueue, &isConnected, 0);  
                if(StateQueue)
                    xQueueSend(StateQueue, &nextStateSignal, 0);
            }

            // If the game is over, current score is put into its queue
            if(gameOver)
            {
                gameOver = false;
                xQueueReset(ScoreQueue);
                xQueueSend(ScoreQueue, score, 0);
                xTimerStop(PosUpdateTimer, 0);

                // The pause task is resumed
                if(StateQueue)
                    xQueueSend(StateQueue, &nextStateSignal, 0);
            }
        
            iCheckStateInput(NO_PLAYER, false, NO_ROTATION, false);
        }
    }
}

/*!
 * \ingroup game 
 * \brief Task that handles the pause & game-over screen.
 * 
 * -# Read from the #GameOverQueue, #ScoreQueue & #ConnectionQueue.
 * -# If the game is not over, call vGUIDrawPauseMenu().
 * -# Otherwise, call vGUIDrawGameOverMenu().
 */
static void pauseTask()
{
    // Init *************************************************************************
    score_t score       = { 0 };
    char *lastUserName  = DEFAULT_USER_NAME;
    bool gameOver       = false;
    bool isConnected    = true;
    // Loop *************************************************************************
    while(1)
    {
        if(DrawSignal && xSemaphoreTake(DrawSignal, portMAX_DELAY) == pdTRUE)
        {
            vGetButtonInput();

            // Read Queues
            if(GameOverQueue)
                xQueueReceive(GameOverQueue, &gameOver, 0);
            if(ScoreQueue)
                xQueueReceive(ScoreQueue, &score, 0);
            if(ConnectionQueue)
                xQueuePeek(ConnectionQueue, &isConnected, 0);

            // Entering a critical section that cannot be interrupted ***********
            taskENTER_CRITICAL();
            if(xSemaphoreTake(ScreenLock, portMAX_DELAY) == pdTRUE)
            {
                // If the game is still going, draw the pause menu
                if(!gameOver) vGUIDrawPauseMenu(isConnected);
                // Otherwise draw the game over menu
                else
                {
                    vGUIDrawGameOverMenu(&score, lastUserName);
                    xQueueReset(ScoreQueue);
                    xQueueSend(ScoreQueue, &score, 0);
                    vTaskResume(ScoreTask);
                }
            }
            xSemaphoreGive(ScreenLock);
            // Exiting critical section *********************************************
            taskEXIT_CRITICAL();

            iCheckStateInput(NO_PLAYER, isConnected, NO_ROTATION, gameOver);
        }
    }
}

#define HIGHSCORES_SIZE 3

/*!
 * \ingroup game 
 * \brief Low priority task, running in the background, that handles the score.
 */
static void scoreTask()
{
    // Init *************************************************************************
    score_t score = { 0 };
    score_t *highScores = (score_t*) calloc(HIGHSCORES_SIZE, sizeof(score_t));
    // Loop *************************************************************************
    while(1)
    {
        // This Task notification is given when the player switches from 
        // the game over screen to the main menu/game screen
        if(ulTaskNotifyTake(pdTRUE, 0) && xQueueReceive(ScoreQueue, &score, 0) == pdTRUE)
        {
            // Put the new score in the correct position of the highscores list
            for(int i=0; i<HIGHSCORES_SIZE; i++)
            {
                if(score.score > highScores[i].score)
                {
                    if(i+2 < HIGHSCORES_SIZE)
                        highScores[i+2] = highScores[i+1];
                    if(i+1 < HIGHSCORES_SIZE)
                        highScores[i+1] = highScores[i];
                    
                    highScores[i] = score;
                    break;
                }
            }
            if(HighScoresQueue)
            {
                xQueueReset(HighScoresQueue);
                xQueueSend(HighScoresQueue, &highScores, 0);
            }
        }
        // After arranging the array of highscores, suspend this task
        vTaskSuspend(NULL);
    }
}

static void buttonInput(bool *buttonPressed)
{
    int buf = 0;
    *buttonPressed = false;
    static debounce_button_t debounceUp = { 0 }, debounceRight = { 0 }, debounceLeft = { 0 };

    if(xSemaphoreTake(buttons.lock, portMAX_DELAY) == pdTRUE)
    {
        // Left arrow ***************************************************************
        if(bGameDebounceButton(buttons.buttons[SDL_SCANCODE_LEFT], &debounceLeft.lastState))
        {
            buf = LEFT_PRESSED;
            xQueueReset(LeftRightQueue);
            if(xQueueSend(LeftRightQueue, &buf, 0) == pdTRUE)
            {
                xSemaphoreGive(XSignal);
                *buttonPressed = true;
            }
        }
        // Right arrow **************************************************************
        if(bGameDebounceButton(buttons.buttons[SDL_SCANCODE_RIGHT], &debounceRight.lastState))
        { 
            buf = RIGHT_PRESSED;
            if(xQueueSend(LeftRightQueue, &buf, 0) == pdTRUE)
            {
                xSemaphoreGive(XSignal);
                *buttonPressed  = true;
            }
        }
        // Up arrow *****************************************************************
        if(bGameDebounceButton(buttons.buttons[SDL_SCANCODE_UP], &debounceUp.lastState))
        {
            xSemaphoreGive(RotationSignal);
            *buttonPressed = true;
        }
        // Down arrow ***************************************************************
        // Down-button not debounced, so that it can be held down
        if(buttons.buttons[SDL_SCANCODE_DOWN])
        {
            buttons.buttons[SDL_SCANCODE_DOWN] = 0;
            xSemaphoreGive(FallSignal);
        }
        xSemaphoreGive(buttons.lock);
    }
}

static void changeTimerPeriod(TimerHandle_t timer, uint8_t level, int delay)
{
    if(timer)
    {
        if(level > 1)
            xTimerChangePeriod(timer, pdMS_TO_TICKS(delay - (level * 40)), 0);
        else
            xTimerStart(timer, 0);
    }
}

int iGameInit()
{
    /*Set seed for rand()*/
    srand(time(NULL));

    /*Load sound waveforms*/
    tumSoundLoadUserSample(FALLING_SOUND);
    tumSoundLoadUserSample(GAME_OVER_SOUND);
    tumSoundLoadUserSample(ROW_FULL_SOUND);
    tumSoundLoadUserSample(THUMP_SOUND);

    DrawSignal = xSemaphoreCreateBinary(); // Screen buffer locking
    if(!DrawSignal)
    {
        PRINT_ERROR("Failed to create draw signal");
        goto err_draw_signal;
    }
    ScreenLock = xSemaphoreCreateMutex();
    if(!ScreenLock)
    {
        PRINT_ERROR("Failed to create screen lock");
        goto err_screen_lock;
    }

    if(xTaskCreate(mainMenuTask, "MainMenuTask", mainGENERIC_STACK_SIZE, NULL, configMAX_PRIORITIES-3, &MainMenuTask) != pdPASS)
    {
        PRINT_TASK_ERROR("MainMenuTask");
        goto err_main_menu_task;
    }
    
    if(xTaskCreate(pauseTask, "PauseTask", mainGENERIC_STACK_SIZE, NULL, configMAX_PRIORITIES-3, &PauseTask) != pdPASS)
    {
        PRINT_TASK_ERROR("PauseTask");
        goto err_pause_task;
    }

    if(xTaskCreate(gameTask, "GameTask", mainGENERIC_STACK_SIZE, NULL, configMAX_PRIORITIES-3, &GameTask) != pdPASS)
    {
        PRINT_TASK_ERROR("GameTask");
        goto err_game_task;
    }

    if(xTaskCreate(scoreTask, "ScoreTask", mainGENERIC_STACK_SIZE, NULL, mainGENERIC_PRIORITY, &ScoreTask) != pdPASS)
    {
        PRINT_TASK_ERROR("ScoreTask");
        goto err_score_task;
    }
    else
        vTaskSuspend(ScoreTask);

    ResetGameSignal = xSemaphoreCreateBinary();
    if(!ResetGameSignal)
    {
        PRINT_ERROR("Failed to create ResetGameSignal");
        goto err_reset_game_signal;
    }

    ResetUDPSignal = xSemaphoreCreateBinary();
    if(!ResetUDPSignal)
    {
        PRINT_ERROR("Failed to create ResetUDPSignal");
        goto err_reset_udp_signal;
    }

    return 0;
    
        vSemaphoreDelete(ResetUDPSignal);
    err_reset_udp_signal:
        vSemaphoreDelete(ResetGameSignal);
    err_reset_game_signal:
        vTaskDelete(ScoreTask);
    err_score_task:
        vTaskDelete(GameTask);
    err_game_task:
        vTaskDelete(PauseTask);
    err_pause_task:
        vTaskDelete(MainMenuTask);
    err_main_menu_task:
        vSemaphoreDelete(ScreenLock);
    err_screen_lock:
        vSemaphoreDelete(DrawSignal);
    err_draw_signal:
        return -1;
}

