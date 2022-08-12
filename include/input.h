/**
 * @file input.h
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 *  
 * @brief Header file for input.c.
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 */

/**
 * @defgroup input Input Module
 * @ingroup tetris
 * @brief Module handling any interactions with keyboard or mouse inputs.
 *
 * This module provides support for any keyboard or mouse inputs.
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 * @{
 */

#ifndef INPUT_H
#define INPUT_H

#include "tetrisConfig.h"

/**
 * @brief Structure for debouncing buttons.
 */
typedef struct debounce_button
{
    int counter;    ///< Counts the amount of times the key has been pressed
    bool lastState; ///< Stores the last state of the button
} debounce_button_t;

/**
 * @brief Structure for keyboard inputs.
 */
typedef struct buttons_buffer
{
    uint8_t buttons[SDL_NUM_SCANCODES]; ///< Keyboard buttons using the SDL library
    SemaphoreHandle_t lock;             ///< Buttons lock
} buttons_buffer_t;

extern buttons_buffer_t buttons; ///< Object representing the keyboard buttons

/**
 * @brief Read the current button input from #buttonInputQueue
 * & write it into #buttons.
 */
void vGetButtonInput();

/**
 * @brief Simple function for debouncing input buttons.
 * 
 * Take the previous state of the button & the current state.
 * - If the button is currently pressed, and the previous state (meaning the button was not just pressed) is false, return true.
 * - If the button is currently pressed, and the previous state (meaning the button was just pressed) is true, return false.
 * 
 * @param[in] currentState (char): Current state of the button input.
 * @param[in] previousState (bool*): Previous state of the button input. 
 * @return (bool): whether the (debounced) button was pressed.
 */
bool bGameDebounceButton(char currentState, bool* previousState);

/**
 * @brief Create a push-button between 2 boundaries.
 * 
 * Take two boundaries and return true if the mouse is in those boundaries
 * and the left mouse button is pressed.
 * @param[in] lowBound (const @ref coord_t): Lower boundary. 
 * @param[in] highBound (const @ref coord_t): Higher boundary. 
 * @return (bool): Whether the push button was pressed. 
 */
bool bGUIPushButton(const coord_t lowBound, const coord_t highBound);

/**
 * @brief Initialize the input module.
 * @return (int): 0 if initialization was successful, -1 otherwise.
 */
int iInputInit();

///@}
#endif // INPUT_H
