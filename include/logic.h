/*!
 * \file logic.h
 * 
 * \authors Philipp Karg (philipp.karg@tum.de)
 * 
 * \brief Header file for logic.c.
 * \date 04.02.2021
 * \copyright AED Engineering GmbH 2022
 * \details Vertraulich/Confidential
 */

/*!
 * \defgroup logic Logic Module
 * \ingroup tetris
 * \brief Module handling the logic components of the game.
 * 
 * This includes calculating new coordinates, checking if a Tetromino can move/rotate,
 * checking for complete rows and more.
 * 
 * @{
 */

#ifndef LOGIC_H
#define LOGIC_H

#include "tetrisConfig.h"

/*!
 * \brief Function to calculate the size of an array.
 * \param[in] x: Array to calculate the size of.
 */
#define SIZEARR(x) (sizeof(x)/sizeof((x)[0]))

#define EMPTY_SPACE 0 ///< If an tetromino does not contain a block in specific position, the value is set to 0

/*!
 * \name Initial coordinates
 * @{
 */
#define INIT_COORDS_X COLS/2 -1 ///< Initial x coordinate
#define INIT_COORDS_Y 0 ///< Initial y coordinate
///@}

/*!
 * \name Pressed keys
 * @{
 */
#define LEFT_PRESSED 1  ///< Left-arrow key
#define RIGHT_PRESSED 2 ///< Right-arrow key
#define DOWN_PRESSED 3  ///< Down-arrow key
///@}

/*!
 * \brief Structure representing a Tetromino.
 */
typedef struct tetromino
{
    coord_t position;       ///< Position of top left square
    coord_t newPosition;    ///< New position to check
    tetromino_type_t type;  ///< Tetromino type
    int rotation;           ///< Tetrominos rotation
    color_t color;          ///< Tetrominos color

    /// Array containing the Tetrominos shape.
    color_t shape[FIGURE_SIZE][FIGURE_SIZE];
    /// Array to check a potential shape
    color_t newShape[FIGURE_SIZE][FIGURE_SIZE];

} tetromino_t;

/*!
 * \brief Structure for the score.
 */
typedef struct score
{
    uint32_t score;     ///< The current score
    uint8_t level;      ///< The current level
    uint16_t rows;      ///< Number of rows cleared
    char *userName;     ///< The selected User-Name
} score_t;

/*!
 * \brief Initialize a Tetromino.
 * 
 * -# Set the Tetromino's type by calling setTetrominoType().
 * -# Set rotation
 * -# Set color
 * -# Init the position
 * -# Set the \ref tetromino_t::shape "shape array".
 * 
 * \param[out] tetromino ( \ref tetromino_t *): Tetromino to initialize.
 * \param[in] types ( \ref tetromino_type_t [][]): Array of possible types.
 * \param[in] index (int*): Index of the types-array. 
 * \param[in] playerMode ( \ref player_mode_t): Current player mode. 
 */
void vLogicInitTetromino(   tetromino_t *tetromino, 
                            tetromino_type_t types[], 
                            int *index, 
                            player_mode_t playerMode);

/*!
 * \brief Check if a move to a \p newPosition is possible.
 * 
 * Check for possible collisions on the side borders, 
 * the ground and with the landed tetrominos.
 * \param[in] newShape (const \ref color_t): New shape to check. 
 * \param[in] newPosition ( \ref coord_t): New position to check.
 * \param[in] landed (const \ref landed): Array of landed Tetrominos.
 * \return (bool): whether a move to a \p newPosition is possible.
 */
bool bLogicCheckMove(   const color_t newShape[FIGURE_SIZE][FIGURE_SIZE], 
                        coord_t newPosition,
                        const color_t landed[ROWS][COLS]);

/*!
 * \brief Check if the game will be over if the new \p tetromino is created.
 * \param[in] tetromino (const \ref tetromino_t *): Tetromino to check.
 * \param[in] landed (const \ref color_t [][]): Array of landed Tetrominos.
 * \return (bool): whether the game will be over.
 */
bool bLogicCheckGameOver(const tetromino_t *tetromino, const color_t landed[ROWS][COLS]);

/*!
 * \brief Update the x coordinate of \p tetromino.
 * 
 * If the \p pressedButton is #LEFT_PRESSED/#RIGHT_PRESSED, try to decrease/increase the position by 1.
 * \param[inout] tetromino ( \ref tetromino_t *): Tetromino object to increase x coordinate of. 
 * \param[in] landed (const \ref color_t [][]): Array of landed Tetrominos. 
 * \param[in] pressedButton (int): The button that was pressed. 
 */
void vLogicUpdateXCoord(tetromino_t *tetromino, const color_t landed[ROWS][COLS], int pressedButton);

/*!
 * \brief Update the y coordinate of \p tetromino.
 * \param[inout] tetromino ( \ref tetromino_t *): Tetromino object to increase y coordinate of. 
 * \param[in] landed (const \ref color_t [][]): Array of landed Tetrominos. 
 * \return (bool): true if the y coordinate could be increased.
 */
bool bLogicUpdateYCoord(tetromino_t *tetromino, const color_t landed[ROWS][COLS]);

/*!
 * \brief Rotate \p tetromino, if possible.
 * 
 * -# Increase/decrease \p tetromino's rotation counter, depending on \p rotationMode.
 * -# Set \p tetromino's \ref tetromino_t::newShape "new shape", by calling setTetrominoShape().
 * -# Check if that new shape is possible with bLogicCheckMove().
 * -# If it is, copy the new shape to \ref tetromino_t::shape "the old shape".
 * 
 * \param[inout] tetromino ( \ref tetromino_t *): Tetromino object to rotate. 
 * \param[in] landed (const \ref color_t [][]): Array of landed Tetrominos. 
 * \param[in] rotationMode ( \ref rotation_t): Rotation mode, either left or right. 
 */
void vLogicRotate(tetromino_t *tetromino, const color_t landed[ROWS][COLS], rotation_t rotationMode);

/*!
 * \brief Add \p tetromino to the \p landed array.
 * \param[in] tetromino (const \ref tetromino_t *): Tetromino to add 
 * \param[out] landed ( \ref color_t [][]): Array of landed Tetrominos. 
 */
void vLogicAddToLanded(const tetromino_t *tetromino, color_t landed[ROWS][COLS]);

/*!
 * \brief Check how many rows are full and remove them.
 * \param[inout] landed ( \ref color_t [][]): Array of landed Tetrominos. 
 * \param[inout] score ( \ref score_t *): Score object to increase if rows are full. 
 * \return (bool): true if one or more rows are full. False otherwise.
 */
bool vLogicRowFull(color_t landed[ROWS][COLS], score_t *score);

///@}
#endif //LOGIC_H