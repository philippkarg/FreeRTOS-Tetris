/**
 * @file tetrisConfig.h
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * 
 * @brief File containing some game configurations.
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 */

/**
 * @defgroup tetris Tetris Modules
 * @brief All modules that were created in the scope of this project.
 * 
 * This module contains all submodules that were created in the scope of this project, 
 * e.g. the @ref logic "Logic" or @ref gui "GUI" modules.
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 */

/**
 * @defgroup config Config Module
 * @ingroup tetris
 * @brief Module that allows some configurations of the game.
 * @authors Philipp Karg (philipp.karg@tum.de)
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 * @{
 */

#ifndef TETRIS_CONFIG_H
#define TETRIS_CONFIG_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL_scancode.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

#include "TUM_Draw.h"
#include "TUM_Font.h"
#include "TUM_Event.h"
#include "TUM_Sound.h"
#include "TUM_Utils.h"
#include "TUM_FreeRTOS_Utils.h"
#include "TUM_Print.h"

#include "enum.h"
#include "EmulatorConfig.h"

/**
 * @name User Names
 * @{
 */
#define USER_NAME_1 "tetrisdominator420"
#define USER_NAME_2 "xXTetrisGodXx"
#define USER_NAME_3 "iplaytetrislul"
#define USER_NAME_4 "isuckattetris69"
#define USER_NAME_5 "xX360NOSCOPEXx"
#define USER_NAME_6 "fischlipp"
#define DEFAULT_USER_NAME USER_NAME_1 // If the player does not select a user name, the default one is given
///@}

/**
 * @name Game window configuration
 * @{
 */
#define FIGURE_SIZE 5   ///< Maximum length/width of a Tetromino
#define COLS 10         ///< Number of columns, that the board contains
#define ROWS 16         ///< Number of rows, that the board contains
#define SQUARE_WIDTH 40 ///< Pixel width/height of one square
#define NUMBER_OF_TETRIS_COLORS 6 ///< Number of colors
#define BACKGROUND_COLOR ((unsigned int) 0x656565)  ///< Background color (can be any HEX color)
///@}

/**
 * @name Sound effects.
 * 
 * Set ENABLE_SOUND_EFFECTS to 1 to enable sound effects.
 * Sound effects may not be synchronized at all time.
 */
#define ENABLE_SOUND_EFFECTS 0  ///< Whether sound effects should be enabled

/**
 * @name Sound effect files
 * @{
 */
#define FALLING_SOUND   "../resources/waveforms/user_waveforms/falling.wav"
#define GAME_OVER_SOUND "../resources/waveforms/user_waveforms/gameover.wav"
#define ROW_FULL_SOUND  "../resources/waveforms/user_waveforms/linefull.wav" 
#define THUMP_SOUND     "../resources/waveforms/user_waveforms/thump.wav"
///@}

/**
 * @name Square image files
 * @{
 */
#define BLUE_SQUARE         "../resources/images/blue_square.png"
#define GREEN_SQUARE        "../resources/images/green_square.png" 
#define YELLOW_SQUARE       "../resources/images/yellow_square.png" 
#define RED_SQUARE          "../resources/images/red_square.png" 
#define LIGHT_BLUE_SQUARE   "../resources/images/light_blue_square.png" 
#define PURPLE_SQUARE       "../resources/images/purple_square.png"
#define GREY_SQUARE         "../resources/images/grey_square.png"
///@}

///@}

#endif // TETRIS_CONFIG_H