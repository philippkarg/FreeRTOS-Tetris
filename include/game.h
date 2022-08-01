/*!
 * \file game.h
 * \authors Philipp Karg (philipp.karg@tum.de)
 * 
 * \brief Header file for game.c.
 * \date 04.02.2021
 */

/*!
 * \defgroup game Main Game Module
 * \ingroup tetris
 * \brief Module that contains the primary game functionality
 * 
 * This module contains the game's main functionality. It contains the different tasks, for the main game, main menu & pause screen.
 * It interacts with the \ref logic "Logic Module", the \ref gui "GUI Module" , the \ref state "State Machine Module" and the \ref opponent "Opponent Module".
 * 
 * @{
 */
#ifndef GAME_H
#define GAME_H

#include "tetrisConfig.h"

/*!
 * \brief Print error message for \p task.
 * \param[in] task (const char*): Task to print error message for.
 */
#define PRINT_TASK_ERROR(task) PRINT_ERROR("Failed to print task ##task");
///@}

// Task Handles *********************************************************************
extern TaskHandle_t MainMenuTask;
extern TaskHandle_t GameTask;
extern TaskHandle_t PauseTask;
extern TaskHandle_t ScoreTask;

// Queue Handles ********************************************************************
extern QueueHandle_t ConnectionQueue;
extern QueueHandle_t GameModeQueue;
extern QueueHandle_t TetrominoQueue;

// Semaphore Handles ****************************************************************
extern SemaphoreHandle_t ScreenLock;
extern SemaphoreHandle_t DrawSignal;
extern SemaphoreHandle_t ResetGameSignal;
extern SemaphoreHandle_t ResetUDPSignal;
extern SemaphoreHandle_t NoConnectionSignal;

/*!
 * \ingroup game
 * \brief Initialize the game.
 * 
 * - Load the sound samples.
 * - Create #ScreenLock & #DrawSignal Semaphores.
 * - Create #MainMenuTask, #GameTask, #PauseTask & #ScoreTask.
 * 
 * \return (int): 0 upon successful initialization, -1 otherwise.
 */
int iGameInit();

#endif //GAME_H