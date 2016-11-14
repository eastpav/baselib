/**
 *  @file  dllist.h
 *  @version 1.00.00
 *  @date  2011-07-18
 *  @brief 双向链表库，该库提供双向链表的创建和维护，用户提供一个双向链表
 *         的描述符【DL_LIST】。在双向链表中的节点可以为任何用户自定义的结构
 *         但是必须保留第一个元素为DL_NODE类型的成员。链表中的第一个节点的
 *         前驱为NULL。
 *         为了保证链表的有效性，链表头中的节点计数被去掉。
 */

#ifndef _DLLIST_H
#define _DLLIST_H

/**
 *  @brief 双向链表的节点
 */
typedef struct dlnode
{
    struct dlnode *next; /**< 双向节点的后继 */
    struct dlnode *prev; /**< 双向节点的前驱 */
} 
DL_NODE;

/**
 *  @brief 双向链表的头
 */
typedef struct dl_list
{
    DL_NODE *head;	/**< 双向链表的头指针 */
    DL_NODE *tail;	/**< 双向链表的尾指针 */
}
DL_LIST;

/* Linked list macros */

/**
 *  @brief 双向链表的首节点
 */
#define DLL_FIRST(pList) (((DL_LIST *)(pList))->head)

/**
 *  @brief 双向链表的尾节点
 */
#define DLL_LAST(pList)	 (((DL_LIST *)(pList))->tail)

/**
 *  @brief 双向链表节点后继
 */
#define DLL_NEXT(pNode)	 (((DL_NODE *)(pNode))->next)

/**
 *  @brief 双向链表节点前驱
 */
#define DLL_PREV(pNode)	 (((DL_NODE *)(pNode))->prev)

/**
 *  @brief 双向链表是否为空
 */
#define DLL_EMPTY(pList) (((DL_LIST *)pList)->head == NULL)

/* function declarations */

/**
 * @brief  初始化双向链表。
 * @param  pList - 双向链表指针。
 * @return 无。
 */
IMPORT void dllInit( DL_LIST *pList );

/**
 * @brief  清除指定双向链表。
 * @param  pList - 双向链表指针。
 * @return 无。
 */
IMPORT void dllCleanup( DL_LIST *pList );

/**
 * @brief  获取双向链表的首节点，并从双向链表中删除。
 * @param  pList - 双向链表指针。
 * @return 链表的首节点指针，NULL - 链表为空。
 */
IMPORT DL_NODE* dllGet( DL_LIST *pList );

/**
 * @brief  向双向链表添加指定的节点，添加中链表末尾。
 * @param  pList - 双向链表指针。
 * @param  pNode - 添加的节点指针。
 * @return 无。
 */
IMPORT void dllAdd( DL_LIST *pList, DL_NODE *pNode );

/**
 * @brief  向双向链表的指定节点后插入节点，如果指定的节点为NULL，则
 *         将节点作为当前链表的首节点。
 * @param  pList - 双向链表指针。
 * @param  pPrev - 指定节点指针，作为新插入节点的前驱。
 * @param  pNode - 待插入的节点指针。
 * @return 无。
 */
IMPORT void dllInsert( DL_LIST *pList, DL_NODE *pPrev, DL_NODE *pNode );

/**
 * @brief  从双向链表删除指定的节点。
 * @param  pList - 双向链表指针。
 * @param  pNode - 待删除的节点指针。
 * @return 无。
 */
IMPORT void dllRemove( DL_LIST *pList, DL_NODE *pNode );

/**
 * @brief  获取双向链表中节点的数量。
 * @param  pList - 双向链表指针。
 * @return 节点的数量。
 */
IMPORT int  dllCount( DL_LIST *pList );

/**
 * @brief  对双向链表的所有节点调用指定的回调函数，对用户提供的回调函数的申明为
 *         @code
 *          BOOL routine (pNode, arg)
 *				DL_NODE *pNode; /@ pointer to a linked list node @/
 *				void* arg; /@ arbitrary user-supplied argument @/
 *         @endcode
 *         针对用户提供的回调函数，如果针对剩余的节点调用该回调函数，那么就需要
 *         返回TRUE，否则返回FALSE并退出dllEach，返回结束节点的指针。
 * @param  pList - 双向链表指针。
 * @param  routine - 用户回调函数。
 * @param  arg - 回调函数参数。
 * @return dllEach返回时结束遍历的节点指针，NULL - 完成链表中所有节点的遍历。
 */
IMPORT DL_NODE* dllEach( DL_LIST *pList, int(*routine)(DL_NODE*,void*), void* arg );

#endif /*_DLLIST_H*/

/*
// End of file 
*/
