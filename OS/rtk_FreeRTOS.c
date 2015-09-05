// -----------------------------------------------------------------------------
//
// rtk_FreeRTOS.c
//
// -----------------------------------------------------------------------------
//  (c) NXP B.V. 2010. All rights reserved.
//  Disclaimer
//  1.   The NXP Software/Source Code is provided to Licensee "AS IS"
//  without any warranties of any kind. NXP makes no warranties to Licensee
//  and shall not indemnify Licensee or hold it harmless for any reason
//  related to the NXP Software/Source Code or otherwise be liable to the
//  NXP customer. The NXP customer acknowledges and agrees that the NXP
//  Software/Source Code is provided AS-IS and accepts all risks of
//  utilizing the NXP Software under the conditions set forth according to
//  this disclaimer.
//
//  2.   NXP EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
//  OF INTELLECTUAL PROPERTY RIGHTS. NXP SHALL HAVE NO LIABILITY TO THE NXP
//  CUSTOMER, OR ITS SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY FOR
//  ANY DAMAGES, INCLUDING WITHOUT LIMITATION, DAMAGES RESULTING OR ALLEGDED
//  TO HAVE RESULTED FROM ANY DEFECT, ERROR OR OMMISSION IN THE NXP
//  SOFTWARE/SOURCE CODE, THIRD PARTY APPLICATION SOFTWARE AND/OR
//  DOCUMENTATION, OR AS A RESULT OF ANY INFRINGEMENT OF ANY INTELLECTUAL
//  PROPERTY RIGHT OF ANY THIRD PARTY. IN NO EVENT SHALL NXP BE LIABLE FOR
//  ANY INCIDENTAL, INDIRECT, SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING LOST PROFITS) SUFFERED BY NXP CUSTOMER OR ITS
//  SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY ARISING OUT OF OR
//  RELATED TO THE NXP SOFTWARE/SOURCE CODE EVEN IF NXP HAS BEEN ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGES.
// -----------------------------------------------------------------------------

#include "assert.h"
#include "rtk.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "semphr.h"

/********************/
/*      Queues      */
/********************/

rtk_queue_t rtk_que_create( unsigned int queue_length, unsigned int item_size )
{
    xQueueHandle queue;
    queue = xQueueCreate( ( unsigned portBASE_TYPE ) queue_length, ( unsigned portBASE_TYPE ) item_size );
    return (rtk_queue_t)queue;
}

int rtk_que_receive( rtk_queue_t queue, void *pbuf, int ticks_to_wait )
{
    return (int)xQueueReceive( ( xQueueHandle ) queue, pbuf, ( portTickType ) ticks_to_wait);
}

int rtk_que_send( rtk_queue_t queue, void *pbuf )
{
    return (int)xQueueSend( ( xQueueHandle ) queue, pbuf, ( portTickType ) 0 );
}

/*********************/
/*    Semaphores     */
/*********************/

rtk_sem_t rtk_sem_create( int val, char * name )
{
    xSemaphoreHandle sem;
    
    sem = xQueueCreate( ( unsigned portBASE_TYPE ) 1, semSEMAPHORE_QUEUE_ITEM_LENGTH );
//    assert( sem != NULL );
    if( val==1 ) 
        xSemaphoreGive( sem );
    else if( val== 0 )
        ; // nothing
    else;
//        assert(false);// Todo: make it a counting semaphore instead of a binary semaphore

#ifdef DEBUG
//    vQueueAddToRegistry( sem, name );
#endif
    return (rtk_sem_t)sem;
}

void rtk_sem_acquire( rtk_sem_t sem )
{
    xSemaphoreTake( (xSemaphoreHandle) sem, portMAX_DELAY );
}

bool rtk_sem_acquire_timeout( rtk_sem_t sem, int mstimeout )
{
    // Blocks until semaphore 'sem' is acquired, or until 'mstimeout' milliseconds have past.
    // Returns true when acquired, false on a timeout
    if ( xSemaphoreTake( (xSemaphoreHandle) sem, (portTickType) mstimeout/portTICK_RATE_MS ) == pdTRUE )
        return true;
    return false;
}

void rtk_sem_release( rtk_sem_t sem )
{
    xSemaphoreGive( (xSemaphoreHandle) sem );
}

void rtk_sem_release_isr( rtk_sem_t sem )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( (xSemaphoreHandle) sem, &xHigherPriorityTaskWoken );
    if ( xHigherPriorityTaskWoken ) vPortYieldFromISR();
}

/*********************/
/*      Tasks        */
/*********************/

void rtk_tsk_sleep( int ms )
{
    // Let this task sleep for 'ms' milliseconds
    vTaskDelay( ms / portTICK_RATE_MS );
}

/*********************/
/* Critical sections */
/*********************/

void rtk_int_disable( void )
{
    vPortEnterCritical();
}

void rtk_int_enable( void )
{
    vPortExitCritical();
}

/* eof */
