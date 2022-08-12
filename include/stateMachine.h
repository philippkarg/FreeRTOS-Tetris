/**
 * @file stateMachine.h
 * 
 * @authors Philipp Karg (philipp.karg@tum.de)
 *  
 * @brief Header file for stateMachine.c.
 * @date 04.02.2021
 * @copyright Philipp Karg 2022
 */

/**
 * @defgroup state State Machine Module
 * @ingroup tetris
 * 
 * @brief State Machine handling changes between tasks.
 * 
 * This module contains a state machine task. This task is responsible for 
 * switching from one state (task) to another.
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "tetrisConfig.h"

extern const uint8_t nextStateSignal;
extern const uint8_t prevStateSignal;
extern QueueHandle_t StateQueue;

/**
 * @ingroup state
 * @brief Function that checks if the player switched to a different state, depending on different parameters.
 * 
 * - If the game is running & Esc is pressed, switch to the next state (pause menu).
 * - If the pause task is running, the game is not over, the opponent is connected 
 * & Esc is pressed, switch to the previous state (game task).
 * - If the pause task is running & R is pressed, reset the game & switch to the previous state (game task).
 * - If the pause task is running & M is pressed, reset the game & switch to the next state (Main Menu).
 * - If the main menu task is running, valid settings have been selected 
 * & S is pressed, switch to the next state (game task).
 * 
 * @param[in] playerMode ( @ref player_mode_t): The current player mode. 
 * @param[in] isConnected (bool): The current connection status. 
 * @param[in] rotationMode ( @ref rotation_t): The current rotation mode. 
 * @param[in] gameOver (bool): Whether the game is over. 
 * @return (int): 0 if state was successfully changed, -1 otherwise.
 */
int iCheckStateInput(player_mode_t playerMode, bool isConnected, rotation_t rotationMode, bool gameOver);

/**
 * @ingroup state
 * @brief Initialize the state machine.
 * @return (int): 0 if initialization was successful, false otherwise.
 */
int iStateMachineInit();

#endif // STATE_MACHINE_H