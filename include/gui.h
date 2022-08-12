/**
 * @file gui.h
 * @authors Philipp Karg (philipp.karg@tum.de)
 * 
 * @brief Header file for gui.c.
 * @date 04.02.2021
 */

/**
 * @defgroup gui GUI Module
 * @ingroup tetris
 * @brief Module handling the Graphical User Interface.
 * 
 * This module handles all functionality related to drawing/displaying things on the screen.
 * It uses the @ref tum_draw "TUM Drawing API".
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 * @{
 */

#ifndef GUI_H
#define GUI_H

#include "tetrisConfig.h"
#include "logic.h"
/**
 * @name Widths & heights for the individual mode texts on the display.
 * 
 * @{
 */
#define MODE_EASY_WIDTH 60 
#define MODE_FAIR_WIDTH 61
#define MODE_HARD_WIDTH 60
#define MODE_RANDOM_WIDTH 90
#define MODE_DETERMINISTIC_WIDTH 198
#define MODES_HEIGHT 500
///@}

// **********************************************************************************
// Menus & Screens ******************************************************************
// **********************************************************************************
/**
 * @brief Draw the main menu.
 * 
 * If in multi-player mode, draw available game modes.
 * @param[in] mode ( @ref game_mode_t): Current game mode.
 * @param[in] playerMode ( @ref player_mode_t): Current player mode (either single or multi).
 * @param[in] rotationMode ( @ref rotation_t): Current rotation mode (either left or right) .
 * @param[in] isConnected (bool): Whether the opponent is connected in multi player mode. 
 */
void vGUIDrawMainMenu(  game_mode_t mode,
                        player_mode_t playerMode,
                        rotation_t rotationMode,
                        bool isConnected);


/**
 * @brief Draw the level selection & highscores screen.
 * @param[in] currentLevel (uint8_t*): The currently selected level. 
 * If no level was selected, it is 0.
 * @param[in] highScores (const @ref score_t): Array of the 3 highest scores. 
 * @return (bool): Return true if the "Back to Main Menu"-button was clicked.
 */
bool bGUIDrawLevelScreen(uint8_t *currentLevel, const score_t highScores[]);

/**
 * @brief Draw the wall, score, level & number of lines completed.
 * @param[in] squares (const @ref image_handle_t): Array containing the square images. 
 * @param[in] score (const @ref score_t): Score object 
 */
void vGUIDrawStatic(const image_handle_t squares[], const score_t *score);

/**
 * @brief Draw the Pause Menu.
 * 
 * If the player pressed "Esc" while playing, this menu is shown.
 * @param[in] isConnected (bool): If the connection was established in multi-player mode. 
 */
void vGUIDrawPauseMenu(bool isConnected);

/**
 * @brief Draw the Game-Over Menu.
 * 
 * If no new Tetromino can be placed, the game is over.
 * Display this menu in this case.
 * @param[in] score ( @ref score_t): Score object that contains the score, level & amount of rows that were reached. 
 * @param[in] lastUserName (char*): User-name that was last selected. 
 */
void vGUIDrawGameOverMenu(score_t *score, char *lastUserName);

// **********************************************************************************
// Selections ***********************************************************************
// **********************************************************************************
/**
 * @brief Draw the player-mode selection in Main Menu.
 * @param[in] playerMode ( @ref player_mode_t *): Player mode object. 
 * @return (bool): Whether the player-mode changed.
 */
bool bGUIDrawPlayerModeSelection(player_mode_t *playerMode);

/**
 * @brief Draw rotation-mode selection in Main Menu.
 * @param[in] rotationMode ( @ref rotation_t): Rotation mode object. 
 * @return (bool): Whether the rotation-mode changed.
 */
bool bGUIDrawRotationSelection(rotation_t *rotationMode);

/**
 * @brief Draws & let the player switch to the level selection/highscores screen.
 * @return (bool): Whether the the button was pressed.
 */
bool bGUIDrawLevelMenuSelection(void);

// **********************************************************************************
// Tetrominos ***********************************************************************
// **********************************************************************************
/**
 * @brief Draw the current Tetromino.
 * 
 * Iterate through the Tetrominos's @ref tetromino_t::shape "shape array" 
 * and draw each existing entry in the Tetromino's @ref color_t "color".
 * @param[in] tetromino (const @ref tetromino_t *): Tetromino object to draw.
 * @param[in] squares (const @ref image_handle_t []): Array containing the square images. 
 */
void vGUIDrawTetromino(const tetromino_t *tetromino, const image_handle_t squares[]);

/**
 * @brief Draw the array of landed tetrominos.
 * @param[in] landed (const @ref color_t [][]): Array of landed Tetrominos. 
 * @param[in] squares (const @ref image_handle_t []): Array containing the square images. 
 */
void vGUIDrawLanded(const color_t landed[ROWS][COLS], const image_handle_t squares[]);

/**
 * @brief Draw the upcoming Tetromino in the upper right corner.
 * @param[in] tetromino (const @ref tetromino_t *): Tetromino to draw.
 * @param[in] squares (const @ref image_handle_t): Array containing the square images. 
 */
void vGUIDrawNextTetromino(const tetromino_t *tetromino, const image_handle_t squares[]);

// **********************************************************************************
// Helper functions *****************************************************************
// **********************************************************************************
/**
 * @brief Draw an FPS counter.
 */
void vGUIDrawFPS(void);

/**
 * @brief Takes an @ref image_handle_t array & initializes it with the correct images
 * @param[out] squares ( @ref image_handle_t []): Array to initialize
 */
void vGUISetImageHandle(image_handle_t squares[]);

/**@}*/

#endif //GUI_H