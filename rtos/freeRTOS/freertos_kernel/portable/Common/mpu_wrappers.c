/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Implementation of the wrapper functions used to raise the processor privilege
 * before calling a standard FreeRTOS API function.
 */

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "mpu_prototypes.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if ( portUSING_MPU_WRAPPERS == 1 )

API_CODE void portRAISE_PRIVILEGE(void)
{	
	uint32_t ret = 0;
	ret = raise_priv( );	
}

API_CODE void portRESET_PRIVILEGE(void)
{
	/* This function shall only be invoked after portRAISE_PRIVILEGE() call. If invoked from a user level code,
	it might hang the system */
	
	uint32_t ret = 0;
	ret = reset_priv ( );
}

/**
 * @brief Calls the port specific code to raise the privilege.
 *
 * @return pdFALSE if privilege was raised, pdTRUE otherwise.
 */
BaseType_t xPortRaisePrivilege( void ) FREERTOS_SYSTEM_CALL;

/**
 * @brief If xRunningPrivileged is not pdTRUE, calls the port specific
 * code to reset the privilege, otherwise does nothing.
 */
void vPortResetPrivilege( BaseType_t xRunningPrivileged );
/*-----------------------------------------------------------*/

BaseType_t xPortRaisePrivilege( void ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xRunningPrivileged;

	/* Check whether the processor is already privileged. */
	xRunningPrivileged = portIS_PRIVILEGED();

	/* If the processor is not already privileged, raise privilege. */
	if( xRunningPrivileged != pdTRUE )
	{
		portRAISE_PRIVILEGE();
	}

	return xRunningPrivileged;
}
/*-----------------------------------------------------------*/

void vPortResetPrivilege( BaseType_t xRunningPrivileged )
{
	if( xRunningPrivileged != pdTRUE )
	{
		portRESET_PRIVILEGE();
	}
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	BaseType_t MPU_xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskCreateRestricted( pxTaskDefinition, pxCreatedTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif /* conifgSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	BaseType_t MPU_xTaskCreateRestrictedStatic( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskCreateRestrictedStatic( pxTaskDefinition, pxCreatedTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif /* conifgSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	BaseType_t MPU_xTaskCreate( TaskFunction_t pvTaskCode, const char * const pcName, uint16_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	TaskHandle_t MPU_xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName, const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const pxTaskBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskCreateStatic( pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

void MPU_vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const xRegions ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vTaskAllocateMPURegions( xTask, xRegions );
	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )
	void MPU_vTaskDelete( TaskHandle_t pxTaskToDelete ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskDelete( pxTaskToDelete );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelayUntil == 1 )
	void MPU_vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, TickType_t xTimeIncrement ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskDelayUntil( pxPreviousWakeTime, xTimeIncrement );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskAbortDelay == 1 )
	BaseType_t MPU_xTaskAbortDelay( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskAbortDelay( xTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelay == 1 )
	void MPU_vTaskDelay( TickType_t xTicksToDelay ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskDelay( xTicksToDelay );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )
	UBaseType_t MPU_uxTaskPriorityGet( const TaskHandle_t pxTask ) /* FREERTOS_SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	portRAISE_PRIVILEGE();

		uxReturn = uxTaskPriorityGet( pxTask );
		portRESET_PRIVILEGE();
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskPrioritySet == 1 )
	void MPU_vTaskPrioritySet( TaskHandle_t pxTask, UBaseType_t uxNewPriority ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskPrioritySet( pxTask, uxNewPriority );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )
	eTaskState MPU_eTaskGetState( TaskHandle_t pxTask ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();
	eTaskState eReturn;

		eReturn = eTaskGetState( pxTask );
		portRESET_PRIVILEGE();
		return eReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TRACE_FACILITY == 1 )
	void MPU_vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskGetInfo( xTask, pxTaskStatus, xGetFreeStackSpace, eState );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
	TaskHandle_t MPU_xTaskGetIdleTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGetIdleTaskHandle();
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
API_CODE void MPU_vTaskSuspend( TaskHandle_t pxTaskToSuspend ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskSuspend( pxTaskToSuspend );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )
API_CODE void MPU_vTaskResume( TaskHandle_t pxTaskToResume ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskResume( pxTaskToResume );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSuspendAll( void ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vTaskSuspendAll();
	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskResumeAll( void ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xTaskResumeAll();
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

TickType_t MPU_xTaskGetTickCount( void ) /* FREERTOS_SYSTEM_CALL */
{
TickType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xTaskGetTickCount();
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxTaskGetNumberOfTasks( void ) /* FREERTOS_SYSTEM_CALL */
{
UBaseType_t uxReturn;
portRAISE_PRIVILEGE();

	uxReturn = uxTaskGetNumberOfTasks();
	portRESET_PRIVILEGE();
	return uxReturn;
}
/*-----------------------------------------------------------*/

char * MPU_pcTaskGetName( TaskHandle_t xTaskToQuery ) /* FREERTOS_SYSTEM_CALL */
{
char *pcReturn;
portRAISE_PRIVILEGE();

	pcReturn = pcTaskGetName( xTaskToQuery );
	portRESET_PRIVILEGE();
	return pcReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetHandle == 1 )
	TaskHandle_t MPU_xTaskGetHandle( const char *pcNameToQuery ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGetHandle( pcNameToQuery );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	void MPU_vTaskList( char *pcWriteBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskList( pcWriteBuffer );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS > 0 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	void MPU_vTaskGetRunTimeStats( char *pcWriteBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskGetRunTimeStats( pcWriteBuffer );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configGENERATE_RUN_TIME_STATS == 1 ) && ( INCLUDE_xTaskGetIdleTaskHandle == 1 ) )
	uint32_t MPU_ulTaskGetIdleRunTimeCounter( void ) /* FREERTOS_SYSTEM_CALL */
	{
	uint32_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = ulTaskGetIdleRunTimeCounter();
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	void MPU_vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxTagValue ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskSetApplicationTaskTag( xTask, pxTagValue );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	TaskHookFunction_t MPU_xTaskGetApplicationTaskTag( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHookFunction_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGetApplicationTaskTag( xTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
	void MPU_vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTaskSetThreadLocalStoragePointer( xTaskToSet, xIndex, pvValue );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if ( configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0 )
	void *MPU_pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex ) /* FREERTOS_SYSTEM_CALL */
	{
	void *pvReturn;
	portRAISE_PRIVILEGE();

		pvReturn = pvTaskGetThreadLocalStoragePointer( xTaskToQuery, xIndex );
		portRESET_PRIVILEGE();
		return pvReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )
	BaseType_t MPU_xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskCallApplicationTaskHook( xTask, pvParameter );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )
	UBaseType_t MPU_uxTaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t *pulTotalRunTime ) /* FREERTOS_SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	portRAISE_PRIVILEGE();

		uxReturn = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, pulTotalRunTime );
		portRESET_PRIVILEGE();
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCatchUpTicks( TickType_t xTicksToCatchUp ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xTaskCatchUpTicks( xTicksToCatchUp );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
	UBaseType_t MPU_uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
	{
	UBaseType_t uxReturn;
	portRAISE_PRIVILEGE();

		uxReturn = uxTaskGetStackHighWaterMark( xTask );
		portRESET_PRIVILEGE();
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark2 == 1 )
	configSTACK_DEPTH_TYPE MPU_uxTaskGetStackHighWaterMark2( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
	{
	configSTACK_DEPTH_TYPE uxReturn;
	portRAISE_PRIVILEGE();

		uxReturn = uxTaskGetStackHighWaterMark2( xTask );
		portRESET_PRIVILEGE();
		return uxReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )
	TaskHandle_t MPU_xTaskGetCurrentTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGetCurrentTaskHandle();
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetSchedulerState == 1 )
	BaseType_t MPU_xTaskGetSchedulerState( void ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGetSchedulerState();
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vTaskSetTimeOutState( pxTimeOut );
	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xTaskCheckForTimeOut( pxTimeOut, pxTicksToWait );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskGenericNotify( TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskGenericNotify( xTaskToNotify, uxIndexToNotify, ulValue, eAction, pulPreviousNotificationValue );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskNotifyWait( ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, xTicksToWait );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	uint32_t MPU_ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
	{
	uint32_t ulReturn;
	portRAISE_PRIVILEGE();

		ulReturn = ulTaskNotifyTake( xClearCountOnExit, xTicksToWait );
		portRESET_PRIVILEGE();
		return ulReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TASK_NOTIFICATIONS == 1 )
	BaseType_t MPU_xTaskNotifyStateClear( TaskHandle_t xTask ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTaskNotifyStateClear( xTask );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	QueueHandle_t MPU_xQueueGenericCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize, uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueGenericCreate( uxQueueLength, uxItemSize, ucQueueType );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	QueueHandle_t MPU_xQueueGenericCreateStatic( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueGenericCreateStatic( uxQueueLength, uxItemSize, pucQueueStorage, pxStaticQueue, ucQueueType );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericReset( QueueHandle_t pxQueue, BaseType_t xNewQueue ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xQueueGenericReset( pxQueue, xNewQueue );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, BaseType_t xCopyPosition ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xQueueGenericSend( xQueue, pvItemToQueue, xTicksToWait, xCopyPosition );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueMessagesWaiting( const QueueHandle_t pxQueue ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();
UBaseType_t uxReturn;

	uxReturn = uxQueueMessagesWaiting( pxQueue );
	portRESET_PRIVILEGE();
	return uxReturn;
}
/*-----------------------------------------------------------*/

UBaseType_t MPU_uxQueueSpacesAvailable( const QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();
UBaseType_t uxReturn;

	uxReturn = uxQueueSpacesAvailable( xQueue );
	portRESET_PRIVILEGE();
	return uxReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueReceive( QueueHandle_t pxQueue, void * const pvBuffer, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();
BaseType_t xReturn;

	xReturn = xQueueReceive( pxQueue, pvBuffer, xTicksToWait );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueuePeek( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();
BaseType_t xReturn;

	xReturn = xQueuePeek( xQueue, pvBuffer, xTicksToWait );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xQueueSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();
BaseType_t xReturn;

	xReturn = xQueueSemaphoreTake( xQueue, xTicksToWait );
	portRESET_PRIVILEGE();
	return xReturn;
}
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) )
	TaskHandle_t MPU_xQueueGetMutexHolder( QueueHandle_t xSemaphore ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();
	void * xReturn;

		xReturn = xQueueGetMutexHolder( xSemaphore );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateMutex( const uint8_t ucQueueType ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueCreateMutex( ucQueueType );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_MUTEXES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateMutexStatic( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueCreateMutexStatic( ucQueueType, pxStaticQueue );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueHandle_t MPU_xQueueCreateCountingSemaphore( UBaseType_t uxCountValue, UBaseType_t uxInitialCount ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueCreateCountingSemaphore( uxCountValue, uxInitialCount );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( configSUPPORT_STATIC_ALLOCATION == 1 ) )

	QueueHandle_t MPU_xQueueCreateCountingSemaphoreStatic( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueCreateCountingSemaphoreStatic( uxMaxCount, uxInitialCount, pxStaticQueue );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )
	BaseType_t MPU_xQueueTakeMutexRecursive( QueueHandle_t xMutex, TickType_t xBlockTime ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueTakeMutexRecursive( xMutex, xBlockTime );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )
	BaseType_t MPU_xQueueGiveMutexRecursive( QueueHandle_t xMutex ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueGiveMutexRecursive( xMutex );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configUSE_QUEUE_SETS == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
	QueueSetHandle_t MPU_xQueueCreateSet( UBaseType_t uxEventQueueLength ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueSetHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueCreateSet( uxEventQueueLength );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	QueueSetMemberHandle_t MPU_xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t xBlockTimeTicks ) /* FREERTOS_SYSTEM_CALL */
	{
	QueueSetMemberHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueSelectFromSet( xQueueSet, xBlockTimeTicks );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	BaseType_t MPU_xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueAddToSet( xQueueOrSemaphore, xQueueSet );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )
	BaseType_t MPU_xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xQueueRemoveFromSet( xQueueOrSemaphore, xQueueSet );
		portRESET_PRIVILEGE();
		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	void MPU_vQueueAddToRegistry( QueueHandle_t xQueue, const char *pcName ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vQueueAddToRegistry( xQueue, pcName );

		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	void MPU_vQueueUnregisterQueue( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vQueueUnregisterQueue( xQueue );

		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if configQUEUE_REGISTRY_SIZE > 0
	const char *MPU_pcQueueGetName( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();
	const char *pcReturn;

		pcReturn = pcQueueGetName( xQueue );

		portRESET_PRIVILEGE();
		return pcReturn;
	}
#endif
/*-----------------------------------------------------------*/

void MPU_vQueueDelete( QueueHandle_t xQueue ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vQueueDelete( xQueue );

	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void *MPU_pvPortMalloc( size_t xSize ) /* FREERTOS_SYSTEM_CALL */
	{
	void *pvReturn;
	portRAISE_PRIVILEGE();

		pvReturn = pvPortMalloc( xSize );

		portRESET_PRIVILEGE();

		return pvReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void MPU_vPortFree( void *pv ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vPortFree( pv );

		portRESET_PRIVILEGE();
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	void MPU_vPortInitialiseBlocks( void ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vPortInitialiseBlocks();

		portRESET_PRIVILEGE();
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	size_t MPU_xPortGetFreeHeapSize( void ) /* FREERTOS_SYSTEM_CALL */
	{
	size_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xPortGetFreeHeapSize();

		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
	TimerHandle_t MPU_xTimerCreate( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction ) /* FREERTOS_SYSTEM_CALL */
	{
	TimerHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_TIMERS == 1 ) )
	TimerHandle_t MPU_xTimerCreateStatic( const char * const pcTimerName, const TickType_t xTimerPeriodInTicks, const UBaseType_t uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	TimerHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerCreateStatic( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction, pxTimerBuffer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	void *MPU_pvTimerGetTimerID( const TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
	{
	void * pvReturn;
	portRAISE_PRIVILEGE();

		pvReturn = pvTimerGetTimerID( xTimer );
		portRESET_PRIVILEGE();

		return pvReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	void MPU_vTimerSetTimerID( TimerHandle_t xTimer, void *pvNewID ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTimerSetTimerID( xTimer, pvNewID );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	BaseType_t MPU_xTimerIsTimerActive( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerIsTimerActive( xTimer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TaskHandle_t MPU_xTimerGetTimerDaemonTaskHandle( void ) /* FREERTOS_SYSTEM_CALL */
	{
	TaskHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerGetTimerDaemonTaskHandle();
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( ( INCLUDE_xTimerPendFunctionCall == 1 ) && ( configUSE_TIMERS == 1 ) )
	BaseType_t MPU_xTimerPendFunctionCall( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerPendFunctionCall( xFunctionToPend, pvParameter1, ulParameter2, xTicksToWait );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	void MPU_vTimerSetReloadMode( TimerHandle_t xTimer, const UBaseType_t uxAutoReload ) /* FREERTOS_SYSTEM_CALL */
	{
	portRAISE_PRIVILEGE();

		vTimerSetReloadMode( xTimer, uxAutoReload );
		portRESET_PRIVILEGE();
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	const char * MPU_pcTimerGetName( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
	{
	const char * pcReturn;
	portRAISE_PRIVILEGE();

		pcReturn = pcTimerGetName( xTimer );
		portRESET_PRIVILEGE();

		return pcReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TickType_t MPU_xTimerGetPeriod( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
	{
	TickType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerGetPeriod( xTimer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	TickType_t MPU_xTimerGetExpiryTime( TimerHandle_t xTimer ) /* FREERTOS_SYSTEM_CALL */
	{
	TickType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerGetExpiryTime( xTimer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configUSE_TIMERS == 1 )
	BaseType_t MPU_xTimerGenericCommand( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
	{
	BaseType_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xTimerGenericCommand( xTimer, xCommandID, xOptionalValue, pxHigherPriorityTaskWoken, xTicksToWait );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	EventGroupHandle_t MPU_xEventGroupCreate( void ) /* FREERTOS_SYSTEM_CALL */
	{
	EventGroupHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xEventGroupCreate();
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	EventGroupHandle_t MPU_xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	EventGroupHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xEventGroupCreateStatic( pxEventGroupBuffer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
EventBits_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xEventGroupWaitBits( xEventGroup, uxBitsToWaitFor, xClearOnExit, xWaitForAllBits, xTicksToWait );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear ) /* FREERTOS_SYSTEM_CALL */
{
EventBits_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xEventGroupClearBits( xEventGroup, uxBitsToClear );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet ) /* FREERTOS_SYSTEM_CALL */
{
EventBits_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xEventGroupSetBits( xEventGroup, uxBitsToSet );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

EventBits_t MPU_xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
EventBits_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xEventGroupSync( xEventGroup, uxBitsToSet, uxBitsToWaitFor, xTicksToWait );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vEventGroupDelete( EventGroupHandle_t xEventGroup ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vEventGroupDelete( xEventGroup );
	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSend( StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
size_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferSend( xStreamBuffer, pvTxData, xDataLengthBytes, xTicksToWait );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferNextMessageLengthBytes( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
size_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferNextMessageLengthBytes( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferReceive( StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
{
size_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferReceive( xStreamBuffer, pvRxData, xBufferLengthBytes, xTicksToWait );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

void MPU_vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
portRAISE_PRIVILEGE();

	vStreamBufferDelete( xStreamBuffer );
	portRESET_PRIVILEGE();
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsFull( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferIsFull( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferIsEmpty( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferIsEmpty( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferReset( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferReset( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferSpacesAvailable( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
size_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferSpacesAvailable( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

size_t MPU_xStreamBufferBytesAvailable( StreamBufferHandle_t xStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
{
size_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferBytesAvailable( xStreamBuffer );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

BaseType_t MPU_xStreamBufferSetTriggerLevel( StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel ) /* FREERTOS_SYSTEM_CALL */
{
BaseType_t xReturn;
portRAISE_PRIVILEGE();

	xReturn = xStreamBufferSetTriggerLevel( xStreamBuffer, xTriggerLevel );
	portRESET_PRIVILEGE();

	return xReturn;
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	StreamBufferHandle_t MPU_xStreamBufferGenericCreate( size_t xBufferSizeBytes, size_t xTriggerLevelBytes, BaseType_t xIsMessageBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	StreamBufferHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xStreamBufferGenericCreate( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif /* configSUPPORT_DYNAMIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	StreamBufferHandle_t MPU_xStreamBufferGenericCreateStatic( size_t xBufferSizeBytes, size_t xTriggerLevelBytes, BaseType_t xIsMessageBuffer, uint8_t * const pucStreamBufferStorageArea, StaticStreamBuffer_t * const pxStaticStreamBuffer ) /* FREERTOS_SYSTEM_CALL */
	{
	StreamBufferHandle_t xReturn;
	portRAISE_PRIVILEGE();

		xReturn = xStreamBufferGenericCreateStatic( xBufferSizeBytes, xTriggerLevelBytes, xIsMessageBuffer, pucStreamBufferStorageArea, pxStaticStreamBuffer );
		portRESET_PRIVILEGE();

		return xReturn;
	}
#endif /* configSUPPORT_STATIC_ALLOCATION */
/*-----------------------------------------------------------*/

#if ( configUSE_TASK_NOTIFICATIONS == 1 )

uint32_t MPU_ulTaskGenericNotifyTake( UBaseType_t uxIndexToWaitOn,
                                      BaseType_t xClearCountOnExit,
                                      TickType_t xTicksToWait ) /* FREERTOS_SYSTEM_CALL */
         {
         	uint32_t ulReturn;
         	portRAISE_PRIVILEGE();
         	
         	ulReturn = ulTaskGenericNotifyTake(uxIndexToWaitOn, xClearCountOnExit, xTicksToWait);
         	portRESET_PRIVILEGE();
         	
         	return ulReturn;
         	
         }  
#endif  /* configUSE_TASK_NOTIFICATIONS */                                

/* Functions that the application writer wants to execute in privileged mode
can be defined in application_defined_privileged_functions.h.  The functions
must take the same format as those above whereby the privilege state on exit
equals the privilege state on entry.  For example:

void MPU_FunctionName( [parameters ] )
{
portRAISE_PRIVILEGE();

	FunctionName( [parameters ] );

	portRESET_PRIVILEGE();
}
*/

#if configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS == 1
	#include "application_defined_privileged_functions.h"
#endif

#endif
