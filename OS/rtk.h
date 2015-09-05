// -----------------------------------------------------------------------------
//
// rtk.h
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

#ifndef __RTK_H 
#define __RTK_H

#include <stdbool.h>

/********************/
/*      Queues      */
/********************/

typedef void * rtk_queue_t;

rtk_queue_t rtk_que_create( unsigned int queue_length, unsigned int item_size );

int rtk_que_receive( rtk_queue_t queue, void *pbuf, int ticks_to_wait );

int rtk_que_send( rtk_queue_t queue, void *pbuf );

/********************/
/*    Semaphores    */
/********************/

typedef void * rtk_sem_t;
// Type holding a semaphore
// NOTE: The rtk abstraction should cast to implementation type.
//       Having a type here means that we only need one header file

rtk_sem_t rtk_sem_create( int val, char * name );
// Returns a new (general, not binary) semaphore.
// It has initial value 'val' and name 'name'.
// NOTE: Can not fail (ie it asserts/reboots when creation fails)

void rtk_sem_acquire( rtk_sem_t sem );
// Blocks until sempahore 'sem' is acquired.
// NOTE: having 'rtk_sem_acquire' and 'rtk_sem_acquire_timeout' means
//       that we don't need a 'rtk_sem_maxtimeout' or 'rtk_sem_indefinite'

bool rtk_sem_acquire_timeout( rtk_sem_t sem, int mstimeout );
// Blocks until semaphore 'sem' is acquired, or until 'mstimeout' milliseconds have past.
// Returns true when acquired, false on a timeout
// NOTE: 'bool' and 'true' are part of ansi C (stdbool.h)

void rtk_sem_release( rtk_sem_t sem );
// Releases semaphore 'sem'
// May not be called from an ISR

void rtk_sem_release_isr( rtk_sem_t sem );
// Releases semaphore 'sem' from the context of an ISR
// May not be called from a task

/********************/
/*      Tasks       */
/********************/

void rtk_tsk_sleep( int ms );

/*********************/
/* Critical sections */
/*********************/

void rtk_int_disable( void );

void rtk_int_enable( void );

#endif /* end __RTK_H */
