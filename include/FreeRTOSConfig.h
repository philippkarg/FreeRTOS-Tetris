/*
 FreeRTOS.org V5.4.2 - Copyright (C) 2003-2009 Richard Barry.

 This file is part of the FreeRTOS.org distribution.

 FreeRTOS.org is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License (version 2) as published
 by the Free Software Foundation and modified by the FreeRTOS exception.

 FreeRTOS.org is distributed in the hope that it will be useful,    but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.

 You should have received a copy of the GNU General Public License along
 with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
 Temple Place, Suite 330, Boston, MA  02111-1307  USA.

 A special exception to the GPL is included to allow you to distribute a
 combined work that includes FreeRTOS.org without being obliged to provide
 the source code for any proprietary components.  See the licensing section
 of http://www.FreeRTOS.org for full details.


 ***************************************************************************
 *                                                                         *
 * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
 *                                                                         *
 * This is a concise, step by step, 'hands on' guide that describes both   *
 * general multitasking concepts and FreeRTOS specifics. It presents and   *
 * explains numerous examples that are written using the FreeRTOS API.     *
 * Full source code for all the examples is provided in an accompanying    *
 * .zip file.                                                              *
 *                                                                         *
 ***************************************************************************

 1 tab == 4 spaces!

 Please ensure to read the configuration and relevant port sections of the
 online documentation.

 http://www.FreeRTOS.org - Documentation, latest information, license and
 contact details.

 http://www.SafeRTOS.com - A version that is certified for use in safety
 critical systems.

 http://www.OpenRTOS.com - Commercial support, development, porting,
 licensing and training services.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#include <stdint.h>

/**
 * @ingroup freeRTOS
 * @defgroup freeRTOSconfig FreeRTOS Configurations
 * @brief Some FreeRTOS configurations.
 * @{
 */

#define configUSE_PREEMPTION            1   ///< Whether to use preemption or not
#define configUSE_IDLE_HOOK             1   ///< Whether to use the idle hook.  
#define configUSE_TICK_HOOK             0   ///< Whether to use the tick rate hook.
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 ) ///< The tick rate
#define configMINIMAL_STACK_SIZE        ( ( unsigned short ) 4 ) ///< The minimal stack size. This can be made smaller if required.
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 32 * 1024 ) ) ///< The total heap size.
#define configMAX_TASK_NAME_LEN         ( 16 )  ///< The maximum length of a task name
#define configUSE_TRACE_FACILITY        1       ///< Whether to use the trace facility
#define configUSE_STATS_FORMATTING_FUNCTIONS 1  ///< Whether use stats formatting functions
#define configGENERATE_RUN_TIME_STATS   1   ///< Enable/Disable runtime stats generation
#define configUSE_16_BIT_TICKS          0   ///< Enable/Disable 16-bit ticks (32-bit default)
#define configIDLE_SHOULD_YIELD         1   ///< Enable/Disable idle task yields
#define configUSE_CO_ROUTINES           1   ///< Enable/Disable co-routines
#define configUSE_MUTEXES               1   ///< Enable/Disable Mutexes
#define configUSE_TASK_NOTIFICATIONS    1   ///< Enable/Disable task notifications
#define configUSE_COUNTING_SEMAPHORES   1   ///< Enable/Disable counting semaphores
#define configUSE_ALTERNATIVE_API       0   ///< Enable/Disable an alternative API
#define configUSE_RECURSIVE_MUTEXES     1   ///< Enable/Disable recursive Mutexes   
#define configCHECK_FOR_STACK_OVERFLOW  0   ///< Whether to check for stack overflow. Do not use this option on the PC port.

#define configUSE_APPLICATION_TASK_TAG  1   ///< Enable/Disable application task tags
#define configQUEUE_REGISTRY_SIZE       0   ///< Set the queue registry size
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 1  ///< Set the maximum syscall interrupt priority

#define configMAX_PRIORITIES        ( 10 )      ///< Set the maximum number of priorities
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )   ///< Set the maximum number of co-routines

/* Set the following definitions to 1 to include the API function, or zero
 to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1   ///< Enable/Disable vTaskPrioritySet()
#define INCLUDE_uxTaskPriorityGet           1   ///< Enable/Disable uxTaskPriorityGet()  
#define INCLUDE_vTaskDelete                 1   ///< Enable/Disable vTaskDelete()
#define INCLUDE_vTaskCleanUpResources       1   ///< Enable/Disable vTaskCleanUpResources()
#define INCLUDE_vTaskSuspend                1   ///< Enable/Disable vTaskSuspend()
#define INCLUDE_vTaskDelayUntil             1   ///< Enable/Disable vTaskDelayUntil()
#define INCLUDE_vTaskDelay                  1   ///< Enable/Disable vTaskDelay()
#define INCLUDE_uxTaskGetStackHighWaterMark 0   ///< Enable/Disable vTaskGetStackHighWaterMark(). Do not use this option on the PC port.
#define INCLUDE_xTaskGetSchedulerState      1   ///< Enable/Disable xTaskGetSchedulerState()   

extern void vMainQueueSendPassed(void);
#define traceQUEUE_SEND( pxQueue ) vMainQueueSendPassed()

#define configTIMER_SERVICE_TASK_NAME "Tmr Svc" ///< Set the name of the timer service task
/// Set the priority of the timer task
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
/// Set the stack depth of the timer task
#define configTIMER_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE*2)
/// Set the queue length of the timer
#define configTIMER_QUEUE_LENGTH 10

///@}

#endif /* FREERTOS_CONFIG_H */
