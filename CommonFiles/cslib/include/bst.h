/**
 * @file bst.h
 *
 * This interface supports a general abstraction for binary search trees. A binary search tree (or BST for short) is a
 * binary tree in which the key in each node is greater than the keys descending to the left and less than the keys
 * descending to the right.
 *
 * Although the BST and BSTNode types are available to clients, most applications will find the Set and HashMap types
 * instead, which use the BST type in their implementation.
 */

#ifndef _bst_h
#define _bst_h

#include <stdarg.h>
#include "cmpfn.h"
#include "cslib.h"
#include "generic.h"
#include "iterator.h"

 /**
  * @brief The abstract type for a binary search tree.
  */

typedef struct BSTCDT* BST;

/**
 * @brief This type allows clients to control the order of iteration.
 *
 * Specifying PREORDER means that the root node is processed before its children, INORDER means that the root is
 * processed between the processing of the left and right children, and POSTORDER means that the root is processed after
 * its children.
 */

typedef enum {
    PREORDER, INORDER, POSTORDER
} TraversalOrder;

/**
 * @brief The abstract type for a tree node.
 */

typedef struct BSTNodeCDT* BSTNode;

/**
 * @brief Creates a new empty binary search tree for keys with the specified base type. The type parameter must be an
 * explicit type name like int or string.
 * Usage: @code bst = newBST(type); @endcode
 */

#define newBST(type) newBSTFromType(#type)

 /**
  * @brief Creates a new empty binary search tree for keys with the specified base type expressed as a string.
  * Usage: @code bst = newBSTFromType(baseType); @endcode
  */

BST newBSTFromType(string baseType);

/**
 * @brief Frees the storage for a binary search tree.  If nodes contain data fields with allocated storage, the client
 * must free this storage by traversing the tree prior to calling freeBST.
 * Usage: @code freeBST(bst); @endcode
 */

void freeBST(BST bst);

/**
 * @brief Returns the number of elements in the binary search tree.
 * Usage:
 * @code n = size(bst); @endcode
 */

int sizeBST(BST bst);

/**
 * @brief Returns true if the binary search tree has no entries.
 * Usage:
 * @code if (isEmpty(bst)) . . . @endcode
 */

bool isEmptyBST(BST bst);

/**
 * @brief Removes all nodes from the BST.
 * Usage:
 * @code clearBST(bst); @endcode
 */

void clearBST(BST bst);

/**
 * @brief Creates a copy of the BST.  The clone function copies
 * only the first level of the structure and does not copy the individual
 * elements.
 * Usage:
 * @code newbst = clone(bst); @endcode
 */

BST cloneBST(BST bst);

/**
 * @brief Applies the binary search algorithm to find a particular key in the tree represented by bst.  If the key
 * appears in the tree, findBSTNode returns that node; if not, findBSTNode returns NULL.
 * Usage:
 * @code node = findBSTNode(bst, key); @endcode
 */

BSTNode findBSTNode(BST bst, ...);

/**
 * @brief Identical to findBSTNode except that the key is taken
 * from the generic argument.
 * Usage: @code node = findBSTNodeFromArg(bst, any); @endcode
 */

BSTNode findBSTNodeFromArg(BST bst, GenericType any);

/**
 * @brief Inserts a new node into a binary search tree, if it does not already exist.  If a node already exists in the
 * tree, insertBSTNode returns that node; if not, insertBSTNode creates a new node, copies the key value, and returns
 * the newly created node.
 * Usage:
 * @code node = insertBSTNode(bst, key); @endcode
 */

BSTNode insertBSTNode(BST bst, ...);

/**
 * @brief Identical to insertBSTNode except that the key is taken from the generic argument.
 * Usage: @code node = insertBSTNodeFromArg(bst, any); @endcode
 */

BSTNode insertBSTNodeFromArg(BST bst, GenericType any);

/**
 * @brief Removes a node in the tree that matches the specified key.
 * Usage: @code removeBSTNode(bst, key); @endcode
 */

void removeBSTNode(BST bst, ...);

/**
 * @brief Identical to removeBSTNode except that the key is taken from the generic argument.
 * Usage: @code node = removeBSTNodeFromArg(bst, any); @endcode
 */

void removeBSTNodeFromArg(BST bst, GenericType any);

/**
 * @brief Calls a function on every node in the binary search tree using the specified iteration order.  The arguments
 * to the callback function are a pointer to the node and the data pointer.
 * Usage: @code mapBST(bst, fn, order, data); @endcode
 */

void mapBST(BST bst, proc fn, TraversalOrder order, void* data);

/**
 * @brief Returns an iterator for traversing the nodes in a binary search tree in the specified order.  The foreach
 * statement automatically uses an INORDER traversal.
 * Usage: @code iterator = newNodeIterator(bst, order); @endcode
 *
 * This function creates an iterator that maps over the nodes in the binary search tree using the specified iteration
 * order.  Using the bst value itself in a foreach construct creates a key iterator with an INORDER traversal. For
 * details on the general iterator strategy, see the comments in the itertype.h interface.
 */

Iterator newNodeIterator(BST bst, TraversalOrder order);

/**
 * @brief Returns the root node of the binary search tree.
 * Usage: @code root = getRootBST(bst); @endcode
 */

BSTNode getRootBST(BST bst);

/**
 * @brief Returns the left child of the specified node in a binary search tree.
 * Usage: @code child = getLeftChild(node); @endcode
 */

BSTNode getLeftChild(BSTNode node);

/**
 * Returns the right child of the specified node in a binary search tree.
 * Usage: @code child = getRightChild(node); @endcode
 */

BSTNode getRightChild(BSTNode node);

/**
 * Returns the key as a generic object.
 * Usage: @code any = getKey(node); @endcode
 */

GenericType getKey(BSTNode node);

/**
 * Returns a string representation of the key.
 * Usage: @code str = getKeyString(node); @endcode
 */

string getKeyString(BSTNode node);

/**
 * Sets the value pointer associated with a BSTNode.
 * Usage: @code setNodeValue(node, value); @endcode
 */

void setNodeValue(BSTNode node, void* value);

/**
 * Returns the value pointer associated with a BSTNode.
 * Usage: @code value = getNodeValue(node); @endcode
 */

void* getNodeValue(BSTNode node);

/**
 * Returns the name of the key type.
 * Usage: @code baseType = getBaseTypeBST(bst); @endcode
 */

string getBaseTypeBST(BST bst);

/**
 * Returns the size of the base type in bytes.
 * Usage: @code size = getBaseTypeSizeBST(bst); @endcode
 */

int getBaseTypeSizeBST(BST bst);

/**
 * Sets the comparison function for keys.  This method need not be called for any of the standard types.
 * Usage: @code setCompareFnBST(bst, cmpFn); @endcode
 */

void setCompareFnBST(BST bst, CompareFn cmpFn);

/**
 * Returns the comparison function for keys.
 * Usage: @code cmpFn = getCompareFnBST(bst); @endcode
 */

CompareFn getCompareFnBST(BST bst);

#endif
