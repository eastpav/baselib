/**
 *  @file  sllist.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 单向链表库，该库提供单向链表的创建和维护，用户提供一个单向链表
 *         的描述符【SL_LIST】。在单向链表中的节点可以为任何用户自定义的结构
 *         但是必须保留第一个元素为SL_NODE类型的成员。链表中的第一个节点的
 *         前驱为NULL。
 */

#ifndef _SLLIST_H
#define _SLLIST_H

#include <rawtypes.h>

/**
 *  @brief 单向链表的节点
 */
typedef struct slnode
{
    struct slnode *next; /**< 单向节点的后继 */
} 
SL_NODE;

/**
 *  @brief 单向链表的头
 */
typedef struct sl_list
{
    SL_NODE *head;	/**< 单向链表的头指针 */
    SL_NODE *tail;	/**< 单向链表的尾指针 */
} 
SL_LIST;

/*
// Singly linked list macros
*/
/**
 *  @brief 单向链表的首节点
 */
#define SLL_FIRST(pList) (((SL_LIST *)pList)->head)

/**
 *  @brief 单向链表的尾节点
 */
#define SLL_LAST(pList)	 (((SL_LIST *)pList)->tail)

/**
 *  @brief 单向链表节点后继
 */
#define SLL_NEXT(pNode)	 (((SL_NODE *)pNode)->next)

/**
 *  @brief 单向链表是否为空
 */
#define SLL_EMPTY(pList) (((SL_LIST *)pList)->head == NULL)

/* function declarations */

/**
 * @brief  初始化单向链表。
 * @param  pList - 单向链表指针。
 * @return 无。
 */
extern void sllInit( SL_LIST *pList );

/**
 * @brief  清除指定单向链表。
 * @param  pList - 单向链表指针。
 * @return 无。
 */
extern void sllCleanup( SL_LIST *pList );

/**
 * @brief  获取单向链表的首节点，并从单向链表中删除。
 * @param  pList - 单向链表指针。
 * @return 链表的首节点指针，NULL - 链表为空。
 */
extern SL_NODE* sllGet( SL_LIST *pList );

/**
 * @brief  获取单向链表中指定节点的前驱。
 * @param  pList - 单向链表指针。
 * @param  pNode - 参考节点指针。
 * @return 指定节点的前驱节点指针，NULL - 链表为空。
 */
extern SL_NODE* sllPrevious( SL_LIST *pList, SL_NODE *pNode );

/**
 * @brief  获取单向链表中节点的数量。
 * @param  pList - 单向链表指针。
 * @return 节点的数量。
 */
extern int 	sllCount( SL_LIST *pList );

/**
 * @brief  将指定节点添加到单向链表头。
 * @param  pList - 单向链表指针。
 * @param  pNode - 被添加节点指针。
 * @return 无。
 */
extern void sllPutAtHead( SL_LIST *pList, SL_NODE *pNode );

/**
 * @brief  将指定节点添加到单向链表尾。
 * @param  pList - 单向链表指针。
 * @param  pNode - 被添加节点指针。
 * @return 无。
 */
extern void sllPutAtTail( SL_LIST *pList, SL_NODE *pNode );

/**
 * @brief  从单向链表删除指定的节点。
 * @param  pList - 单向链表指针。
 * @param  pDeleteNode - 被删除节点指针。
 * @param  pPrevNode - 单被删除节点的前驱指针。
 * @return 无。
 */
extern void sllRemove( SL_LIST *pList, SL_NODE *pDeleteNode, SL_NODE *pPrevNode );

/**
 * @brief  对双向链表的所有节点调用指定的回调函数，对用户提供的回调函数的申明为
 *         @code
 *          BOOL routine (pNode, arg)
 *				SL_NODE *pNode; /@ pointer to a linked list node @/
 *				void* arg; /@ arbitrary user-supplied argument @/
 *         @endcode
 *         针对用户提供的回调函数，如果针对剩余的节点调用该回调函数，那么就需要
 *         返回TRUE，否则返回FALSE并退出sllEach，返回结束节点的指针。
 * @param  pList - 双向链表指针。
 * @param  routine - 用户回调函数。
 * @param  arg - 回调函数参数。
 * @return sllEach返回时结束遍历的节点指针，NULL - 完成链表中所有节点的遍历。
 */
extern SL_NODE* sllEach( SL_LIST *pList, int(*routine)(SL_NODE*,void*), void* arg );

#endif /*_SLLIST_H*/

/*
// End of file 
*/

