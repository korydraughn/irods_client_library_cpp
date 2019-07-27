/* For copyright information please refer to files in the COPYRIGHT directory
 */
#include "irods_list.h"
#include <cstdlib>

List *newList( Region *r ) {
    List *l = ( List * )region_alloc( r, sizeof( List ) );
    l->head = l->tail = NULL;
    l->size = 0;
    return l;
}

List *newListNoRegion() {
    List *l = ( List * )malloc( sizeof( List ) );
    l->head = l->tail = NULL;
    l->size = 0;
    return l;
}

ListNode *newListNodeNoRegion( void *value ) {
    ListNode *l = ( ListNode * )malloc( sizeof( ListNode ) );
    l->next = NULL;
    l->value = value;
    return l;
}
ListNode *newListNode( void *value, Region *r ) {
    ListNode *l = ( ListNode * )region_alloc( r, sizeof( ListNode ) );
    l->next = NULL;
    l->value = value;
    return l;
}

void listRemoveNoRegion( List *list, ListNode *node ) {
    ListNode *prev = NULL, *curr = list->head;
    while ( curr != NULL ) {
        if ( curr == node ) {
            if ( prev == NULL ) {
                list->head = node->next;
            }
            else {
                prev->next = node->next;
            }
            free( node );
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    if ( list->tail == node ) {
        list->tail = prev;
    }
    list->size--;
}

