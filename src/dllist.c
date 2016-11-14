/* dllist.c - efficient doubly linked list subroutine library */

/*
modification history
-------------------- 
1.00, 2011-2-15, youyq initial 
*/

/*
DESCRIPTION
This subroutine library supports the creation and maintenance of a
doubly linked list.  The user supplies a list descriptor (type DL_LIST)
that will contain pointers to the first and last nodes in the list.
The nodes in the list can be any user-defined structure, but they must reserve
space for a pointer as their first element.  The forward chain is terminated
with a NULL pointer.

For efficiency, the count field has been eliminated, and enqueueing and dequeueing
functions have been hand optimized.

NON-EMPTY LIST:

   ---------             --------          --------
   | head--------------->| next----------->| next---------
   |       |             |      |          |      |      |
   |       |        ------ prev |<---------- prev |      |
   |       |       |     |      |          |      |      |
   | tail------    |     | ...  |    ----->| ...  |      |
   |-------|  |    v                 |                   v
              |  -----               |                 -----
              |   ---                |                  ---
              |    -                 |                   -
              ------------------------

EMPTY LIST:

	-----------
    |  head------------------
    |         |             |
    |  tail----------       |
    |         |     v       v
    |         |   -----   -----
    -----------    ---     ---
                    -       -
*/

#include <rawtypes.h>
#include <dllist.h>

/*
// dllInit - initialize doubly linked list descriptor.
//
// Initialize the specified list to an empty list.
//
// RETURNS: N/A
*/
void 
dllInit( DL_LIST *pList )
{
    pList->head	= NULL;
    pList->tail = NULL;
}

/*
// dllCleanup - cleanup doubly linked list descriptor.
//
// Cleanup the specified list to an empty list.
//
// RETURNS: N/A
*/
void 
dllCleanup( DL_LIST *pList )
{
    pList->head	= NULL;
    pList->tail = NULL;
}

/*
// dllInsert - insert node in list after specified node
//
// This routine inserts the specified node in the specified list.
// The new node is placed following the specified 'previous' node in the list.
// If the specified previous node is NULL, the node is inserted at the head
// of the list.
*/
void 
dllInsert( DL_LIST *pList, DL_NODE *pPrev, DL_NODE *pNode )
{
    DL_NODE *pNext;

    if (pPrev == NULL) {
		/* 
		// new node is to be first in list 
		*/
		pNext = pList->head;
		pList->head = pNode;
	}
    else {
		/* 
		// make prev node point fwd to new 
		*/
		pNext = pPrev->next;
		pPrev->next = pNode;
	}

    if (pNext == NULL) {
		/* new node is to be last in list */
		pList->tail = pNode;
	}
    else {
		/* make next node point back to new */
		pNext->prev = pNode;
	}

    /* set pointers in new node */
    pNode->next		= pNext;
    pNode->prev	= pPrev;
}

/*
// dllAdd - add node to end of list
//
// This routine adds the specified node to the end of the specified list.
//
// RETURNS: N/A
*/
void 
dllAdd( DL_LIST *pList, DL_NODE *pNode )
{
    dllInsert (pList, pList->tail, pNode);
}

/*
// dllRemove - remove specified node in list
//
// Remove the specified node in the doubly linked list.
//
// RETURNS: N/A
*/
void 
dllRemove( DL_LIST *pList, DL_NODE *pNode )
{
    if (pNode->prev == NULL) 
		pList->head = pNode->next;
    else 
		pNode->prev->next = pNode->next;

    if (pNode->next == NULL) 
		pList->tail = pNode->prev;
    else 
		pNode->next->prev = pNode->prev;
}

/*
// dllGet - get (delete and return) first node from list
//
// This routine gets the first node from the specified list, deletes the node
// from the list, and returns a pointer to the node gotten.
//
// RETURNS: Pointer to the node gotten, or NULL if the list is empty.
*/
DL_NODE*
dllGet( DL_LIST *pList )
{
    DL_NODE *pNode = pList->head;

    if (pNode != NULL)
	{
        pList->head = pNode->next; /* make next node be 1st */

        if (pNode->next == NULL)  /* is there any next node? */
            pList->tail = NULL; /* no - list is empty */
        else
            pNode->next->prev = NULL; /* yes - make it 1st node */
	}

    return (pNode);
}

/*
// dllCount - report number of nodes in list
//
// This routine returns the number of nodes in the given list.
//
// CAVEAT
// This routine must actually traverse the list to count the nodes.
// If counting is a time critical fuction, consider using lstLib(1) which
// maintains a count field.
//
// RETURNS: Number of nodes in specified list.
*/
int 
dllCount( DL_LIST *pList )
{
    DL_NODE *pNode = DLL_FIRST (pList);
    int count = 0;

    while (pNode != NULL) 
	{
		count++;
		pNode = DLL_NEXT (pNode);
	}

    return (count);
}

/*
// dllEach - call a routine for each node in a linked list
//
// This routine calls a user-supplied routine once for each node in the
// linked list.  The routine should be declared as follows:
//
// .CS
//  BOOL routine (pNode, arg)
//      DL_NODE *pNode;/@ pointer to a linked list node    @/
//      void*	arg;	/@ arbitrary user-supplied argument @/
// .CE
//
// The user-supplied routine should return TRUE if dllEach() is to
// continue calling it with the remaining nodes, or FALSE if it is done and
// dllEach() can exit.
//
// RETURNS: NULL if traversed whole linked list, or pointer to DL_NODE that
//          dllEach ended with.
*/
DL_NODE*
dllEach( DL_LIST *pList, int(*routine)(DL_NODE*, void*), void* arg )
{
    DL_NODE *pNode = DLL_FIRST (pList);

    while((pNode != NULL) && ((*routine) (pNode, arg))) 
		pNode = DLL_NEXT (pNode);

    return (pNode);	/* return node we ended with */
}

/*
// End of file
*/
