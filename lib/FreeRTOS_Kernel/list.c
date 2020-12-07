/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#include <stdlib.h>
#include "FreeRTOS.h"
#include "list.h"

/*-----------------------------------------------------------
 * PUBLIC LIST API documented in list.h
 *----------------------------------------------------------*/

void vListInitialise(List_t *const pxList)
{
    /* The list structure contains a list item which is used to mark the
    end of the list.  To initialise the list the list end is inserted
    as the only list entry. */
    pxList->pxIndex = (ListItem_t *) & (pxList->xListEnd);            /*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    /* The list end value is the highest possible value in the list to
    ensure it remains at the end of the list. */
    pxList->xListEnd.xItemValue = portMAX_DELAY;

    /* The list end next and previous pointers point to itself so we know
    when the list is empty. */
    pxList->xListEnd.pxNext = (ListItem_t *) & (pxList->xListEnd);    /*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
    pxList->xListEnd.pxPrevious = (ListItem_t *) & (pxList->xListEnd);   /*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */

    pxList->uxNumberOfItems = (UBaseType_t) 0U;

    /* Write known values into the list if
    configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_LIST_INTEGRITY_CHECK_1_VALUE(pxList);
    listSET_LIST_INTEGRITY_CHECK_2_VALUE(pxList);
}
/*-----------------------------------------------------------*/

void vListInitialiseItem(ListItem_t *const pxItem)
{
    /* Make sure the list item is not recorded as being on a list. */
    pxItem->pvContainer = NULL;

    /* Write known values into the list item if
    configUSE_LIST_DATA_INTEGRITY_CHECK_BYTES is set to 1. */
    listSET_FIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE(pxItem);
    listSET_SECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE(pxItem);
}
/*-----------------------------------------------------------*/

void vListInsertEnd(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    ListItem_t *const pxIndex = pxList->pxIndex;

    /* Only effective when configASSERT() is also defined, these tests may catch
    the list data structures being overwritten in memory.  They will not catch
    data errors caused by incorrect configuration or use of FreeRTOS. */
    listTEST_LIST_INTEGRITY(pxList);
    listTEST_LIST_ITEM_INTEGRITY(pxNewListItem);

    /* Insert a new list item into pxList, but rather than sort the list,
    makes the new list item the last item to be removed by a call to
    listGET_OWNER_OF_NEXT_ENTRY(). */
    pxNewListItem->pxNext = pxIndex;
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;

    /* Only used during decision coverage testing. */
    mtCOVERAGE_TEST_DELAY();

    pxIndex->pxPrevious->pxNext = pxNewListItem;
    pxIndex->pxPrevious = pxNewListItem;

    /* Remember which list the item is in. */
    pxNewListItem->pvContainer = (void *) pxList;

    (pxList->uxNumberOfItems)++;
}
/*-----------------------------------------------------------*/

void vListInsert(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    ListItem_t *pxIterator;
    const TickType_t xValueOfInsertion = pxNewListItem->xItemValue;

    /* Only effective when configASSERT() is also defined, these tests may catch
    the list data structures being overwritten in memory.  They will not catch
    data errors caused by incorrect configuration or use of FreeRTOS. */
    listTEST_LIST_INTEGRITY(pxList);
    listTEST_LIST_ITEM_INTEGRITY(pxNewListItem);

    /* Insert the new list item into the list, sorted in xItemValue order.

    If the list already contains a list item with the same item value then the
    new list item should be placed after it.  This ensures that TCB's which are
    stored in ready lists (all of which have the same xItemValue value) get a
    share of the CPU.  However, if the xItemValue is the same as the back marker
    the iteration loop below will not end.  Therefore the value is checked
    first, and the algorithm slightly modified if necessary. */
    if (xValueOfInsertion == portMAX_DELAY) {
        pxIterator = pxList->xListEnd.pxPrevious;
    }
    else {
        /* *** NOTE ***********************************************************
        If you find your application is crashing here then likely causes are
        listed below.  In addition see http://www.freertos.org/FAQHelp.html for
        more tips, and ensure configASSERT() is defined!
        http://www.freertos.org/a00110.html#configASSERT

            1) Stack overflow -
               see http://www.freertos.org/Stacks-and-stack-overflow-checking.html
            2) Incorrect interrupt priority assignment, especially on Cortex-M
               parts where numerically high priority values denote low actual
               interrupt priorities, which can seem counter intuitive.  See
               http://www.freertos.org/RTOS-Cortex-M3-M4.html and the definition
               of configMAX_SYSCALL_INTERRUPT_PRIORITY on
               http://www.freertos.org/a00110.html
            3) Calling an API function from within a critical section or when
               the scheduler is suspended, or calling an API function that does
               not end in "FromISR" from an interrupt.
            4) Using a queue or semaphore before it has been initialised or
               before the scheduler has been started (are interrupts firing
               before vTaskStartScheduler() has been called?).
        **********************************************************************/

        for (pxIterator = (ListItem_t *) & (pxList->xListEnd); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext) {   /*lint !e826 !e740 The mini list structure is used as the list end to save RAM.  This is checked and valid. */
            /* There is nothing to do here, just iterating to the wanted
            insertion position. */
        }
    }

    pxNewListItem->pxNext = pxIterator->pxNext;
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    /* Remember which list the item is in.  This allows fast removal of the
    item later. */
    pxNewListItem->pvContainer = (void *) pxList;

    (pxList->uxNumberOfItems)++;
}
/*-----------------------------------------------------------*/

UBaseType_t uxListRemove(ListItem_t *const pxItemToRemove)
{
    /* The list item knows which list it is in.  Obtain the list from the list
    item. */
    List_t *const pxList = (List_t *) pxItemToRemove->pvContainer;

    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    /* Only used during decision coverage testing. */
    mtCOVERAGE_TEST_DELAY();

    /* Make sure the index is left pointing to a valid item. */
    if (pxList->pxIndex == pxItemToRemove) {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    else {
        mtCOVERAGE_TEST_MARKER();
    }

    pxItemToRemove->pvContainer = NULL;
    (pxList->uxNumberOfItems)--;

    return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/

void vTimerInsert(List_t *const pxList, List_t *const pxBucket, ListItem_t *const pxNewListItem)
{
    TickType_t xNextExpireTime;
    /* If the bucket is empty, fill it to let the following procedure work */
    if ( listLIST_IS_EMPTY( pxBucket ) != pdFALSE ) {
        vListInsertEnd(pxBucket, pxNewListItem);
    }
    else {
        /* Here assume that the bucket is sorted once or there is only one item in the bucket */
        xNextExpireTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxBucket);

        /* Depending on e_i decides if timer should insert to list or append to bucket */
        if ( listGET_LIST_ITEM_VALUE(pxNewListItem) < xNextExpireTime ) {
            vListInsert(pxList, pxNewListItem);
        } 
        else {
            /* simply append timer to bucket*/
            vListInsertEnd(pxBucket, pxNewListItem);
            if ( listLIST_IS_EMPTY( pxList ) != pdFALSE )
                vTimerRefill(pxList, pxBucket);
            /* printf("After refilling:\n"); */
            /* ListItem_t *pxTimerItem; */
            /* pxTimerItem = listGET_HEAD_ENTRY(pxList); */
            /* UBaseType_t i = pxList->uxNumberOfItems; */
            /* while (i != 0){ */
            /*     printf("item: %d\n", pxTimerItem->xItemValue); */
            /*     pxTimerItem = pxTimerItem->pxNext; */ 
            /*     i--; */
            /* } */
            /* printf("\n"); */
        }
    }
}
/*-----------------------------------------------------------*/

void vTimerRefill(List_t *const pxList, List_t *const pxBucket)
{
    /* The idea here is to refill list until the length of the list is s (if there is no repeated period) 
     * s can be calculated by pxBucket->uxNumberOfItems / 2, since this moment list is empty but bucket is full */
    
    /* ad-hoc Sorting Algorithm on bucket */
    vSortBucket ( pxBucket );

    TickType_t xNextExpireTime;
    TickType_t e_tail = 0;

    ListItem_t *pxTimerItem;

    /* fill the first s items into list */
    for (int i = 0; i < (pxBucket->uxNumberOfItems)/2; i++){
        pxTimerItem = listGET_HEAD_ENTRY(pxBucket);
        (void) uxListRemove(pxTimerItem);
        vListInsert(pxList, (ListItem_t *) pxTimerItem);
    }
    e_tail = pxTimerItem->xItemValue;

    /* if there are timers with the same periods */
    while( listLIST_IS_EMPTY( pxBucket ) == pdFALSE ){        
        xNextExpireTime = listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxBucket);
        if( xNextExpireTime == e_tail){
            pxTimerItem = listGET_HEAD_ENTRY(pxBucket);
            (void) uxListRemove(pxTimerItem);
            vListInsert(pxList, (ListItem_t *) pxTimerItem);
        }
        else
            break;
    }
}
/*-----------------------------------------------------------*/

void vSortBucket(List_t *const pxBucket)
{
    ListItem_t *pxTimerItem;
    /* Merge Sort */
    /* The pxNext of the last item is set to NULL for merge-sort design */ 
    pxBucket->pxIndex->pxPrevious->pxNext = NULL;
    vMergeSort( &listGET_HEAD_ENTRY(pxBucket) );
    pxTimerItem = listGET_HEAD_ENTRY(pxBucket);
    while (pxTimerItem->pxNext != NULL){
        /* traverse items forward and fix the backward links*/
        ListItem_t *pxTmpItem = pxTimerItem;
        pxTimerItem = pxTimerItem->pxNext; 
        pxTimerItem->pxPrevious = pxTmpItem;
    }
    /* pxTimerItem is the end item, mark it with the link to pxIndex */
    pxBucket->pxIndex->pxPrevious = pxTimerItem;
    pxBucket->pxIndex->pxPrevious->pxNext=pxBucket->pxIndex;

}
/*-----------------------------------------------------------*/

void vMergeSort(ListItem_t ** pxHeadItem) {
    ListItem_t *head = *pxHeadItem;
    ListItem_t *a;
    ListItem_t *b;
    /* Base case: length 0 or 1 */
    if ( head == NULL || head->pxNext == NULL )
        return;

    /* Split head into 'a' and 'b' sublists */
    vFrontBackSplit(head, &a, &b);

    /* Recursively sort the sublists */
    vMergeSort(&a);
    vMergeSort(&b); 

    /* Merge the two sorted lists together */
    *pxHeadItem = vSortedMerge(a, b);

}
/*-----------------------------------------------------------*/

ListItem_t* vSortedMerge(ListItem_t *a, ListItem_t *b)
{
    /* merge the two sorted lists together */
    ListItem_t *result = NULL;

    /* Base cases */
    if (a == NULL){
        return (b);
    }
    else if (b == NULL){
        return (a);
    }

    /* Pick either a or b, and recur */
    if(a->xItemValue <= b->xItemValue){
        result = a;
        result->pxNext = vSortedMerge(a->pxNext, b);
    }else{
        result = b;
        result->pxNext = vSortedMerge(a, b->pxNext);
    }

    return(result);
}
/*-----------------------------------------------------------*/

void vFrontBackSplit(ListItem_t *const pxSourceItem, ListItem_t **pxFrontRef, ListItem_t **pxBackRef)
{
    /* utility function */
    ListItem_t *pxFast;  
    ListItem_t *pxSlow;  
    pxSlow = pxSourceItem;
    pxFast = pxSourceItem->pxNext;
    
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while(pxFast != NULL){
        pxFast = pxFast->pxNext;
        if(pxFast != NULL){
            pxSlow = pxSlow->pxNext;
            pxFast = pxFast->pxNext;
        }
    }
    /* 'slow' is before the midpoint in the list, so split it in two at that point */
    *pxFrontRef = pxSourceItem;
    *pxBackRef = pxSlow->pxNext;
    pxSlow->pxNext = NULL;
}
/*-----------------------------------------------------------*/
