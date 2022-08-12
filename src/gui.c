/**
 * @file gui.h
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * 
 * @brief Header file for gui.c.
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 */
#include "gui.h"
#include "input.h"

#define FPS_AVERAGE_COUNT 50
#define CENTERED(x) (SCREEN_WIDTH/2 - x/2)

// **********************************************************************************
// Forward Declarations *************************************************************
// **********************************************************************************
/**
 * @ingroup gui
 * @brief Check if drawing is possible.
 * 
 * Checks if tumDrawing functions return 0 (success) or 1 (failure)
 * and sends and error message in case of failure.
 * @param[in] status (uint8_t): Function to check.
 * @param[in] msg (const char *): Error message.
 */
static void checkDraw(uint8_t status, const char *msg);

/**
 * @ingroup gui
 * @brief Draw the User-Name selection.
 * 
 * Function that draws & lets the player select one of 6 usernames.
 * Usernames are defined in the @ref config "Config module".
 * @param[inout] score (const @ref score_t *): Scores to draw. 
 * @param[in] lastUserName (char*): Selected User-Name.
 */
static void drawUserNameSelection(score_t *score, char *lastUserName);

/**
 * @ingroup gui
 * @brief Draw the three hightest scores.
 * @param[in] highScores (const @ref score_t []): Sorted high-scores array. 
 */
static void drawHighScores(const score_t highScores[]);

/**
 * @ingroup gui
 * @brief Draw the available game modes.
 * @param[in] currentMode ( @ref game_mode_t): Currently selected game mode. 
 */
static void drawGameModes(game_mode_t currentMode);

/**
 * @ingroup gui
 * @brief Draw text on the screen.
 * @param[in] str (char *): Text to draw. 
 * @param[in] x (int16_t): X-coordinate. 
 * @param[in] y (int16_t): Y-coordinate. 
 * @param[in] color (uint32_t): Color (HEX). 
 */
static void drawText(char *str, int16_t x, int16_t y, uint32_t color);

// **********************************************************************************
// Menus & Screens ******************************************************************
// **********************************************************************************
void vGUIDrawMainMenu(  game_mode_t mode, 
                        player_mode_t playerMode, 
                        rotation_t rotationMode, 
                        bool isConnected)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 25);
    
    // Select player mode ***********************************************************
    char *str1 = "SELECT PLAYER MODE"; 
    int width = 0, y = 50;
    if(!tumGetTextSize(str1, &width, NULL))
        drawText(str1, CENTERED(width), y, Black);
    
    // Press S to start / Error: No connection **************************************
    char *str3 = { 0 };
    y = 200;
    // Only draw "PRESS S TO START" if the selected mode is either
    // single player, or if it is multi player and the connection is established.
    // Do not draw "PRESS S TO START" info rotation is selected.
    if(playerMode != NO_PLAYER && rotationMode != NO_ROTATION)
    {
        if((playerMode == MULTI_PLAYER && isConnected) || playerMode == SINGLE_PLAYER)
        {
            str3 = malloc(sizeof("PRESS S TO START"));
            strcpy(str3, "PRESS S TO START");
        }
        if(playerMode == MULTI_PLAYER && !isConnected)
        {
            str3 = malloc(sizeof("ERROR: NO CONNECTION"));
            strcpy(str3, "ERROR: NO CONNECTION");
        }
    }
    if(!tumGetTextSize(str3, &width, NULL))
        drawText(str3, CENTERED(width), y, Black);

    // Game modes *******************************************************************
    if(isConnected && playerMode == MULTI_PLAYER)
        drawGameModes(mode);

    // Press Q to quit **************************************************************
    char *str2 = "PRESS Q TO QUIT";
    y = 575;
    if(!tumGetTextSize(str2, &width, NULL))
        drawText(str2, CENTERED(width), y, Black);

    // Reset font size
    tumFontSetSize(prevFontSize);
}

void drawGameModes(game_mode_t currentMode)
{
    // Select mode ******************************************************************
    char *str = "SELECT MODE:";
    int width = 0, y = MODES_HEIGHT - 35;
    if(!tumGetTextSize(str, &width, NULL))
        drawText(str, CENTERED(width), y, Black);
    
    // Game modes *******************************************************************
    char *strings[5] = { 
                            "EASY",
                            "FAIR",
                            "HARD",
                            "RANDOM",
                            "DETERMINISTIC"
                        };
    int widths[5] = { 0 };
    uint32_t strColors[5] = { 0 };

    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 15);

    // Set color for current mode to white
    switch(currentMode)
    {
        case EASY: strColors[0] = White; break;
        case FAIR: strColors[1] = White; break;
        case HARD: strColors[2] = White; break;
        case RANDOM: strColors[3] = White; break;
        case DETERMINISTIC: strColors[4] = White; break;
        default: break;
    }

    // Draw strings
    for(int i=0; i<4; i++)
        if(!tumGetTextSize(strings[i], &widths[i], NULL))
            drawText(strings[i], SCREEN_WIDTH *i/5 - widths[i]/2, MODES_HEIGHT, strColors[i-1]);
    if(!tumGetTextSize(strings[4], &widths[4], NULL))
        drawText(strings[4], CENTERED(widths[4]), MODES_HEIGHT + 25, strColors[4]);

    // Reset font size
    tumFontSetSize(prevFontSize);
}

bool bGUIDrawLevelScreen(uint8_t *currentLevel, const score_t highScores[])
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 30);
    // Background
    checkDraw(tumDrawClear(BACKGROUND_COLOR), __FUNCTION__);
    // Select a level ***************************************************************
    char *topStr = "SELECT A LEVEL: ";
    int width = 0, y = 100;
    if(!tumGetTextSize(topStr, &width, NULL))
        drawText(topStr, CENTERED(width), y, Black);

    // Levels ***********************************************************************
    // Set strings & widths
    char *strings[10] = { 0 };
    int widths[10] = { 0 };
    int y1 = 200, y2 = 300;
    for(int i=0; i<10; i++)
    {
        strings[i] = malloc(sizeof(i));
        sprintf(strings[i], "%d", i);
        tumGetTextSize(strings[i], &widths[i], NULL);
    }
    // Colors (level 0 is white by default)
    static uint32_t strColors[10] = {White, 0 };
    // Set boundaries
    coord_t lowBoundsRow1[5] = { 0 }, highBoundsRow1[5] = { 0 },
            lowBoundsRow2[5] = { 0 }, highBoundsRow2[5] = { 0 };
    for(int i=0; i<5; i++)
    {
        lowBoundsRow1[i].x = SCREEN_WIDTH*(i+1)/6 - widths[i];
        lowBoundsRow1[i].y = y1 - 5;
        highBoundsRow1[i].x = SCREEN_WIDTH*(i+1)/6 + widths[i];
        highBoundsRow1[i].y = y1 + 30;

        lowBoundsRow2[i].x = SCREEN_WIDTH*(i+1)/6 - widths[i];
        lowBoundsRow2[i].y = y2 - 5;
        highBoundsRow2[i].x = SCREEN_WIDTH*(i+1)/6 + widths[i];
        highBoundsRow2[i].y = y2 + 30;
    }

    // Draw the number for each level & check if it was clicked
    // First row
    for(int i=0; i<5; i++)
    {
        drawText(strings[i], lowBoundsRow1[i].x, y1, strColors[i]);
        if(bGUIPushButton(lowBoundsRow1[i], highBoundsRow1[i]))
        {
            for(int j=0; j<10; j++) strColors[j] = Black;
            strColors[i] = White;
            *currentLevel = i;
        }
    }
    // Second row
    for(int i=5; i<10; i++)
    {
        drawText(strings[i], lowBoundsRow2[i-5].x, y2, strColors[i]);
        if(bGUIPushButton(lowBoundsRow2[i-5], highBoundsRow2[i-5]))
        {
            for(int j=0; j<10; j++) strColors[j] = Black;
            strColors[i] = White;
            *currentLevel = i;
        }
    }

    // High scores ******************************************************************
    drawHighScores(highScores);
    
    // Back to main menu ************************************************************
    char *backStr = "BACK TO MAIN MENU";
    y = 550; 
    coord_t lowBound = {SCREEN_WIDTH/2 - width/2, y - 5},
            highBound = {SCREEN_WIDTH/2 + width/2, y + 30};

    if(!tumGetTextSize(backStr, &width, NULL))
        drawText(backStr, CENTERED(width), y, Black);

    // Check if the button was clicked
    bool back = bGUIPushButton(lowBound, highBound);

    // Reset font size
    tumFontSetSize(prevFontSize);
    return back;
}

void drawHighScores(const score_t highScores[])
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 15);

    // Highscores: ******************************************************************
    char *str = "HIGHSCORES: ";
    int width = 0, topHeight = 400;

    if(highScores[0].score && !tumGetTextSize(str, &width, NULL))
        drawText(str, CENTERED(width), topHeight, Black);

    // Scores ***********************************************************************    
    for(int i=0; i<3; i++)
    {
        if(highScores[i].score)
        {
            // User name
            char str[200];
            sprintf(str, "%d. %s",   (i+1), highScores[i].userName);
            int y = topHeight + (i+1) * 25;
            int x = SCREEN_WIDTH * 1/12;
            drawText(str, width, y, Black);

            // Score
            sprintf(str, "%d POINTS", highScores[i].score);
            tumGetTextSize(str, &width, NULL);
            x = SCREEN_WIDTH * 11/12 - width;
            drawText(str, x, y, Black);
        }
    }
    // Reset font size
    tumFontSetSize(prevFontSize);
}

void vGUIDrawStatic(const image_handle_t squares[], const score_t *score)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 12);

    // Wall between field & displays ************************************************
    for(int row=0; row<ROWS; row++)
        tumDrawImage(GREY_SQUARE, COLS * SQUARE_WIDTH, row*SQUARE_WIDTH);

    // Press Esc to pause ***********************************************************
    char *pauseStr = "Press Esc to pause";
    int width = 0;

    if(!tumGetTextSize(pauseStr, &width, NULL))
        drawText(pauseStr, SCREEN_WIDTH - width - 10, DEFAULT_FONT_SIZE * 0.5, White);
    
    tumFontSetSize((ssize_t) 20);
    // Score ************************************************************************
    char str[100] = { 0 };
    int x = SCREEN_WIDTH - 200, y = 50;
    sprintf(str, "SCORE:  %d  ", score->score);
    drawText(str, x, y, White);

    // Level ************************************************************************
    sprintf(str, "LEVEL:  %d  ", score->level);
    drawText(str, x, y+=40, White); 

    // Lines ************************************************************************
    sprintf(str, "LINES:  %d  ", score->rows);
    drawText(str, x, y+=40, White);
    
    // Next tetromino ***************************************************************
    char *nextTetrominoStr = "NEXT:";
    drawText(nextTetrominoStr, x, y+=40, White);

    // Reset font size
    tumFontSetSize(prevFontSize);
}

void vGUIDrawPauseMenu(bool isConnected)
{
    // Draw boxes *******************************************************************
    int innerBoxWidth = 577, innerBoxHeight = 225; 
    int outerBoxWidth = 617, outerBoxHeight = 265;
    coord_t innerBoxCoords = {SCREEN_WIDTH/2 - 477/2 - 50, 200},
            outerBoxCoords = {SCREEN_WIDTH/2 - 477/2 - 70,  180};

    checkDraw(  tumDrawFilledBox(outerBoxCoords.x, outerBoxCoords.y, outerBoxWidth, outerBoxHeight, Black),
                __FUNCTION__);
   
    checkDraw(  tumDrawFilledBox(innerBoxCoords.x, innerBoxCoords.y, innerBoxWidth, innerBoxHeight, BACKGROUND_COLOR),
                __FUNCTION__);

    // Draw Strings *****************************************************************
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 25);

    char strs[3][50] = { 0 };
    int width = 0, height = 225;

    // Set string input depending on connection
    if(isConnected)
    {
        strcpy(strs[0], "PRESS ESC TO RESUME");
        strcpy(strs[1], "PRESS R TO RESTART");
        strcpy(strs[2], "PRESS M FOR MAIN MENU");
    }
    else
    {
        strcpy(strs[0], "ERROR");
        strcpy(strs[1], "CONNECTION LOST");
        strcpy(strs[2], "PRESS M FOR MAIN MENU");
    }          

    for(int i=0; i<3; i++)
        if(!tumGetTextSize(strs[i], &width, NULL))
            drawText(strs[i], CENTERED(width), height + i*50, Black);
    
    // Reset font size
    tumFontSetSize(prevFontSize);
}

void vGUIDrawGameOverMenu(score_t *score, char *lastUserName)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 30);
    checkDraw(tumDrawClear(BACKGROUND_COLOR), __FUNCTION__);

    // Game over ********************************************************************
    char *str = "GAME OVER!";
    int width, y = 50;
    if(!tumGetTextSize(str, &width, NULL))
        drawText(str, CENTERED(width), y, Black);

    // Rest of Game Over Menu *******************************************************
    tumFontSetSize((ssize_t) 25);
    char strings[5][50] = { 0 };

    sprintf(strings[0], "YOUR SCORE: %d", score->score);
    sprintf(strings[1], "YOUR LEVEL: %d", score->level);
    if(score->score) sprintf(strings[2], "SELECT A NAME:");
    sprintf(strings[3], "PRESS R TO RESTART");
    sprintf(strings[4], "PRESS M FOR MAIN MENU");

    for(int i=0; i<5; i++)
    {
        if(i<4 && !tumGetTextSize(strings[i], &width, NULL))
                drawText(strings[i], CENTERED(width), y + 100 + (i-1)*50, Black);
        else if(!tumGetTextSize(strings[i], &width, NULL))
                drawText(strings[i], SCREEN_WIDTH/2 - width/2, y + 450 + (i-4)*50, Black);
    }

    // User Name Selection **********************************************************
    if(score->score) drawUserNameSelection(score, lastUserName);

    // Reset font size
    tumFontSetSize(prevFontSize);
}

// **********************************************************************************
// Selections ***********************************************************************
// **********************************************************************************
bool bGUIDrawPlayerModeSelection(player_mode_t *playerMode)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 20);
    
    // Player mode selection ********************************************************
    char *strs[2] = {"1 PLAYER", "2 PLAYERS"};
    int width = 0, y = 100;
    bool playerModeChanged = false;
    static uint32_t strColors[2] = { 0 };

    for(int i=0; i<2; i++)
    {
        tumGetTextSize(strs[i], &width, NULL);

        // Boundaries
        coord_t lowBounds   = {SCREEN_WIDTH * (i+1)/3 - width/2, y - 5};
        coord_t highBounds  = {SCREEN_WIDTH * (i+1)/3 + width/2, y + 20};

        if(bGUIPushButton(lowBounds, highBounds))
        {
            *playerMode = i+1;
            playerModeChanged = true;
            for(int j=0; j<2; j++) strColors[j] = Black;
            strColors[i] = White;
        }
        drawText(strs[i], lowBounds.x, y, strColors[i]);
    }

    // Reset font size
    tumFontSetSize(prevFontSize);
    return playerModeChanged;
}

bool bGUIDrawRotationSelection(rotation_t *rotationMode)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 20);

    // Rotation *********************************************************************
    char *str = "ROTATION:";
    int width, y = 150;
    if(!tumGetTextSize(str, &width, NULL))
        drawText(str, SCREEN_WIDTH/4 - width/2, y, Black);

    // Rotation Mode Selection ******************************************************
    char *strs[2] = {"LEFT", "RIGHT"};
    bool rotationChanged = false;
    static uint32_t strColors[2] = { 0 };

    for(int i=0; i<2; i++)
    {
        // Boundaries
        tumGetTextSize(strs[i], &width, NULL);
        coord_t lowBounds   = {SCREEN_WIDTH * (i+2)/4 - width/2, y - 5};
        coord_t highBounds  = {SCREEN_WIDTH * (i+2)/4 + width/2, y + 20};
        
        // Push button
        if(bGUIPushButton(lowBounds, highBounds))
        {
            *rotationMode = i+1;
            rotationChanged = true;
            for(int j=0; j<2; j++) strColors[j] = Black;
            strColors[i] = White;
        }
        
        // Draw
        drawText(strs[i], lowBounds.x, y, strColors[i]);
    }
    // Reset font size
    tumFontSetSize(prevFontSize);

    return rotationChanged;
}

bool bGUIDrawLevelMenuSelection(void)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 20);

    // Select level or see highscores ***********************************************
    char *str = "SELECT LEVEL OR SEE HIGHSCORES";
    int width = 0, y = 350;

    // Boundaries
    tumGetTextSize(str, &width, NULL);
    coord_t lowBound = {SCREEN_WIDTH/2 - width/2, y - 5};
    coord_t highBound = {SCREEN_WIDTH/2 + width/2, y + 25};
    // Push button
    bool pushed = bGUIPushButton(lowBound, highBound);
    // Draw
    drawText(str, CENTERED(width), y, Black);
    // Reset font size 
    tumFontSetSize(prevFontSize);

    return pushed;
}

void drawUserNameSelection(score_t *score, char *lastUserName)
{
    // Font size
    ssize_t prevFontSize = tumFontGetCurFontSize();
    tumFontSetSize((ssize_t) 15);
 
    // User Name Selection **********************************************************
    char *userNames[6] = 
    {  
        USER_NAME_1, 
        USER_NAME_2,
        USER_NAME_3, 
        USER_NAME_4, 
        USER_NAME_5, 
        USER_NAME_6
    };
    int width, heights[3] = {300, 325, 350};
    static uint32_t strColors[6] = { 0 };

    for(int i=0; i<6; i++)
    {
        // Boundaries
        tumGetTextSize(userNames[i], &width, NULL);
        coord_t lowBounds   = {SCREEN_WIDTH * (1+(i%2*10)) /12 - (i%2 * width), heights[(int)floor(i/2)]-5};
        coord_t highBounds  = {SCREEN_WIDTH * (1+(i%2*10)) /12 + ((i+1)%2 * width), heights[(int)floor(i/2)] + 20};
        // Push button
        if(bGUIPushButton(lowBounds, highBounds))
        {
            score->userName = malloc(sizeof(userNames[i]));
            strcpy(score->userName, userNames[i]);
            lastUserName = malloc(sizeof(userNames[i]));
            strcpy(lastUserName, userNames[i]);
            for(int j=0; j<6; j++) strColors[j] = Black;
            strColors[i] = White;
        }
        else
        {
            score->userName = malloc(sizeof(lastUserName));
            strcpy(score->userName, lastUserName);
        }
        // Draw
        drawText(userNames[i], lowBounds.x, heights[(int)floor(i/2)], strColors[i]);
    }
    // Reset font size
    tumFontSetSize(prevFontSize);
}

// **********************************************************************************
// Tetrominos ***********************************************************************
// **********************************************************************************
void vGUIDrawTetromino(const tetromino_t *tetromino, const image_handle_t squares[])
{
    for(int row=0; row<FIGURE_SIZE; row++)
        for(int col=0; col<FIGURE_SIZE; col++)
            if(tetromino->shape[row][col] != 0)
                tumDrawLoadedImage
                (   
                    squares[tetromino->color-1],
                    (tetromino->position.x+col)*SQUARE_WIDTH, 
                    (tetromino->position.y+row)*SQUARE_WIDTH
                );
}

void vGUIDrawLanded(const color_t landed[ROWS][COLS], const image_handle_t squares[])
{
    for(int row=0; row<ROWS; row++)
        for(int col=0; col<COLS; col++)
            if(landed[row][col] != EMPTY_SPACE)
                tumDrawLoadedImage
                ( 
                    squares[landed[row][col]-1],
                    col*SQUARE_WIDTH,
                    SCREEN_HEIGHT - (row+1)* SQUARE_WIDTH
                );
}

void vGUIDrawNextTetromino(const tetromino_t *tetromino, const image_handle_t squares[])
{
    for(int row=0; row<FIGURE_SIZE; row++)
        for(int col=0; col<FIGURE_SIZE; col++)
            if(tetromino->shape[row][col] != 0)
                tumDrawLoadedImage
                (   
                    squares[tetromino->color-1],
                    (COLS+col)*SQUARE_WIDTH + 105, 
                    row*SQUARE_WIDTH + 250
                );
}

// **********************************************************************************
// Helper functions *****************************************************************
// **********************************************************************************
void drawText(char *str, int16_t x, int16_t y, uint32_t color)
{
    checkDraw(tumDrawText(str, x, y, color), __FUNCTION__);
}

void checkDraw(uint8_t status, const char *msg)
{
    if(status)
    {
        if(msg)
            fprints(stderr, "[ERROR] %s, %s\n", msg, tumGetErrorMessage());
        else
            fprints(stderr, "[ERROR] %s\n", tumGetErrorMessage());
    }
}

void vGUIDrawFPS(void)
{
    static uint32_t periods[FPS_AVERAGE_COUNT] = { 0 };
    static uint32_t periodsTotal = 0;
    static uint32_t index = 0;
    static uint32_t averageCount = 0;
    static TickType_t xLastWakeTime = 0, prevWakeTime = 0;
    static char str[10] = { 0 };
    static int width;
    int fps = 0;

    if(averageCount < FPS_AVERAGE_COUNT)
        averageCount++;
    else
        periodsTotal -= periods[index];

    xLastWakeTime = xTaskGetTickCount();

    if(prevWakeTime != xLastWakeTime)
    {
        periods[index] = configTICK_RATE_HZ / (xLastWakeTime - prevWakeTime);
        prevWakeTime = xLastWakeTime;
    }
    else
        periods[index] = 0;

    periodsTotal += periods[index];

    if(index == (FPS_AVERAGE_COUNT - 1))
        index = 0;
    else
        index++;

    fps = periodsTotal / averageCount;

    // Draw
    sprintf(str, "FPS: %2d", fps);
    if(!tumGetTextSize(str, &width, NULL))
        drawText(str, SCREEN_WIDTH - width - 10, SCREEN_HEIGHT - DEFAULT_FONT_SIZE * 1.5, White);
}

void vGUISetImageHandle(image_handle_t squares[])
{
    squares[TETRIS_BLUE-1]          = tumDrawLoadImage(BLUE_SQUARE);
    squares[TETRIS_GREEN-1]         = tumDrawLoadImage(GREEN_SQUARE); 
    squares[TETRIS_YELLOW-1]        = tumDrawLoadImage(YELLOW_SQUARE); 
    squares[TETRIS_RED-1]           = tumDrawLoadImage(RED_SQUARE); 
    squares[TETRIS_LIGHT_BLUE-1]    = tumDrawLoadImage(LIGHT_BLUE_SQUARE); 
    squares[TETRIS_PURPLE-1]        = tumDrawLoadImage(PURPLE_SQUARE); 
}



