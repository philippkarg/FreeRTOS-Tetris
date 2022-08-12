/**
 * @file enum.h
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * 
 * @brief File containing all global enumerations.
 * @date 08.04.2022
 */

/**
 * @defgroup enum Enumerations
 * @ingroup tetris
 * @brief Enumerations that are used throughout the project.
 * @authors Philipp Karg (philipp.karg@tum.de)
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 * @{
 */

#ifndef ENUM_H
#define ENUM_H

/**
 * @brief Game modes supported by the "opponent".
 */
typedef enum game_mode
{
    NO_MODE = 0,        ///< No mode selected.
    FAIR = 1,           ///< Fair mode, the opponent will produce a fair selection of Tetrominos.
    EASY = 2,           ///< Easy mode, the opponent will produce fewer S & Z Tetrominos.
    HARD = 3,           ///< Hard mode, the opponent will produce more S & Z Tetrominos.
    RANDOM = 4,         ///< Random mode, the opponent will produce random Tetromino types.
    DETERMINISTIC = 5   ///< Deterministic mode, the opponent will produce each Tetromino type at least once every 7 types
} game_mode_t;

/**
 * @brief Rotations.
 */
typedef enum rotation
{
    NO_ROTATION = 0,    ///< No rotation.
    LEFT = 1,           ///< Rotating Tetrominos to the left.
    RIGHT = 2           ///< Rotating Tetrominos to the right.  
} rotation_t;

/**
 * @brief Tetromino colors.
 */
typedef enum color
{
    NO_COLOR = 0,
    TETRIS_BLUE = 1, 
    TETRIS_GREEN = 2, 
    TETRIS_YELLOW = 3,
    TETRIS_RED = 4, 
    TETRIS_LIGHT_BLUE = 5, 
    TETRIS_PURPLE = 6
} color_t;

/**
 * @brief Tetromino types.
 */
typedef enum tetromino_type
{
    NO_TYPE = 0,
    S = 1,
    Z = 2,          
    J = 3,
    L = 4,
    T = 5,
    O = 6,
    I = 7
} tetromino_type_t;

/**
 * @brief Player modes.
 */
typedef enum player_mode
{
    NO_PLAYER = 0,
    SINGLE_PLAYER = 1,
    MULTI_PLAYER = 2
} player_mode_t;

///@}
#endif // ENUM_H