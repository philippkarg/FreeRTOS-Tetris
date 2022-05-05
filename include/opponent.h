/*!
 * \file opponent.h
 * \authors Philipp Karg (philipp.karg@tum.de)
 * 
 * \brief Header file for opponent.c.
 * \date 04.02.2021
 */

/*!
 * \defgroup opponent Opponent Module
 * \ingroup tetris
 * \brief Module handling incoming/outgoing messages to an "oppnent".
 * 
 * This module contains functionality to interact with the "opponent". The opponent is an executable,
 * that generates Tetrominos. It can run in various modes. For more information on the opponent see.
 * 
 */

#ifndef OPPONENT_H
#define OPPONENT_H

#include "tetrisConfig.h"

extern QueueHandle_t TetrominoQueue;
extern SemaphoreHandle_t NextTetrominoSignal;
extern TaskHandle_t UDPControlTask;

/*!
 * \brief Initialize the opponent functionality.
 * \return (int): 0 if initialization was successful, -1 otherwise.
 */
int iOpponentInit();

#endif // OPPONENT_H