/**
 * @file bst.c
 *
 * This file implements the bst.h interface, which provides a general implementation of binary search trees.  It is used
 * in the implementations of the Map and Set types.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "bst.h"
#include "cmpfn.h"
#include "cslib.h"
#include "generic.h"
#include "iterator.h"
#include "itertype.h"

 /**
  * @brief This type is the concrete type used to represent the BST.
  */

struct BSTCDT {
    IteratorHeader header;      /**< Header to enable iteration            */
    string baseType;            /**< The name of the base type             */
    int baseTypeSize;           /**< Size of the base type in bytes        */
    CompareFn cmpFn;            /**< Function to compare two keys          */
    FetchFn fetchFn;            /**< Function to fetch an argument         */
    StoreFn storeFn;            /**< Function to store a value             */
    ToStringFn toStringFn;      /**< Function to convert key to a string   */
    BSTNode root;               /**< Root of the tree                      */
    int count;                  /**< Number of entries in the BST          */
    FILE* debugLog;             /**< Debugging log                         */
};

/**
 * @brief This type defines the structure of a BST node.
 */

struct BSTNodeCDT {
    GenericType key;            /**< Space for the key                     */
    void* value;                /**< Value field for maps                  */
    struct BSTNodeCDT* left;    /**< Left child                            */
    struct BSTNodeCDT* right;   /**< Right child                           */
    int bf;                     /**< Balance factor for node (-1, 0, +1)   */
    BST bst;                    /**< Back pointer to the bst               */
};

/* Private function prototypes */

static BSTNode copyTree(BST newbst, BSTNode t);

static BSTNode findTreeNode(BST bst, BSTNode t, void* kp);

static int insertTreeNode(BST bst, BSTNode* tp, void* kp, BSTNode* rp);

static int removeTreeNode(BST bst, BSTNode* tp, void* kp);

static int removeTargetNode(BST bst, BSTNode* tp);

static void freeTree(BSTNode t);

static void freeNode(BSTNode node);

static void adjustBF(BST bst, BSTNode* tp, int delta);

static void fixLeftImbalance(BST bst, BSTNode* tp);

static void fixRightImbalance(BST bst, BSTNode* tp);

static void rotateLeft(BST bst, BSTNode* tp);

static void rotateRight(BST bst, BSTNode* tp);

static void mapTree(BSTNode t, proc fn, TraversalOrder order, void* data);

static void addNodeToIterator(BSTNode node, void* data);

static Iterator newForeachIterator(void* collection);

/* Exported entries */

BST newBSTFromType(string baseType) {
    BST bst;

    bst = newBlock(BST);
    enableIteration(bst, newForeachIterator);
    bst->baseType = baseType;
    bst->baseTypeSize = getTypeSizeForType(baseType);
    bst->fetchFn = getFetchFnForType(baseType);
    bst->storeFn = getStoreFnForType(baseType);
    bst->cmpFn = getCompareFnForType(baseType);
    bst->toStringFn = getToStringFn(baseType);
    bst->count = 0;
    bst->root = NULL;
    bst->debugLog = NULL;
    return bst;
}

void freeBST(BST bst) {
    clearBST(bst);
    freeBlock(bst);
}

int sizeBST(BST bst) {
    return bst->count;
}

bool isEmptyBST(BST bst) {
    return bst->count == 0;
}

void clearBST(BST bst) {
    freeTree(bst->root);
    bst->count = 0;
}

BST cloneBST(BST bst) {
    BST newbst;

    newbst = newBlock(BST);
    enableIteration(newbst, newForeachIterator);
    newbst->baseType = bst->baseType;
    newbst->baseTypeSize = bst->baseTypeSize;
    newbst->fetchFn = bst->fetchFn;
    newbst->storeFn = bst->storeFn;
    newbst->cmpFn = bst->cmpFn;
    newbst->toStringFn = bst->toStringFn;
    newbst->count = bst->count;
    newbst->root = copyTree(newbst, bst->root);
    newbst->debugLog = NULL;
    return newbst;
}

BSTNode findBSTNode(BST bst, ...) {
    va_list args;
    GenericType any;

    va_start(args, bst);
    bst->fetchFn(args, &any);
    va_end(args);
    return findBSTNodeFromArg(bst, any);
}

BSTNode findBSTNodeFromArg(BST bst, GenericType any) {
    return findTreeNode(bst, bst->root, &any);
}

BSTNode insertBSTNode(BST bst, ...) {
    va_list args;
    GenericType any;

    va_start(args, bst);
    bst->fetchFn(args, &any);
    va_end(args);
    return insertBSTNodeFromArg(bst, any);
}

BSTNode insertBSTNodeFromArg(BST bst, GenericType any) {
    BSTNode node;

    insertTreeNode(bst, &bst->root, &any, &node);
    return node;
}

void removeBSTNode(BST bst, ...) {
    va_list args;
    GenericType any;

    va_start(args, bst);
    bst->fetchFn(args, &any);
    va_end(args);
    removeBSTNodeFromArg(bst, any);
}

void removeBSTNodeFromArg(BST bst, GenericType any) {
    removeTreeNode(bst, &bst->root, &any);
}

void mapBST(BST bst, proc fn, TraversalOrder order, void* data) {
    mapTree(bst->root, fn, order, data);
}

Iterator newNodeIterator(BST bst, TraversalOrder order) {
    Iterator iterator;

    iterator = newListIterator(sizeof(void*), NULL);
    mapBST(bst, addNodeToIterator, order, iterator);
    return iterator;
}

BSTNode getRootBST(BST bst) {
    return bst->root;
}

BSTNode getLeftChild(BSTNode node) {
    return node->left;
}

BSTNode getRightChild(BSTNode node) {
    return node->right;
}

GenericType getKey(BSTNode node) {
    return node->key;
}

string getKeyString(BSTNode node) {
    return node->bst->toStringFn(node->key);
}

int getBF(BSTNode node) {
    return node->bf;
}

void setNodeValue(BSTNode node, void* value) {
    node->value = value;
}

void* getNodeValue(BSTNode node) {
    return node->value;
}

string getBaseTypeBST(BST bst) {
    return bst->baseType;
}

int getBaseTypeSizeBST(BST bst) {
    return bst->baseTypeSize;
}

void setCompareFnBST(BST bst, CompareFn cmpFn) {
    bst->cmpFn = cmpFn;
}

CompareFn getCompareFnBST(BST bst) {
    return bst->cmpFn;
}

void setDebugLog(BST bst, FILE* outfile) {
    bst->debugLog = outfile;
}

/* Private functions */

/**
 * @brief This function implements a recursive walk on the tree to preserve the structure.
 */

static BSTNode copyTree(BST newbst, BSTNode t) {
    BSTNode node;

    if (t == NULL) return NULL;
    node = newBlock(BSTNode);
    node->key = t->key;
    node->value = t->value;
    node->left = copyTree(newbst, t->left);
    node->right = copyTree(newbst, t->right);
    node->bf = t->bf;
    node->bst = newbst;
    return node;
}

/**
 * @brief Finds the specified key by walking recursively over the BST.
 */

static BSTNode findTreeNode(BST bst, BSTNode t, void* kp) {
    int sign;

    if (t == NULL) return NULL;
    sign = bst->cmpFn(kp, &t->key);
    if (sign == 0) return t;
    if (sign < 0) {
        return findTreeNode(bst, t->left, kp);
    }
    else {
        return findTreeNode(bst, t->right, kp);
    }
}

/**
 * @brief Enters the key into the binary search tree, starting the recursive search at the tree whose address is passed
 * as the tp parameter. The rp parameter specifies the location in which the node (which may be either an existing node
 * or a newly allocated one) is stored.  The return value is an integer that indicates the change in the height of the
 * subtree, which is then used to correct the balance factors in ancestor nodes.
 */

static int insertTreeNode(BST bst, BSTNode* tp, void* kp, BSTNode* rp) {
    BSTNode t;
    int sign;

    t = *tp;
    if (t == NULL) {
        t = newBlock(BSTNode);
        memcpy(&t->key, kp, bst->baseTypeSize);
        t->bst = bst;
        t->bf = 0;
        t->left = t->right = NULL;
        t->value = NULL;
        *tp = t;
        *rp = t;
        bst->count++;
        return +1;
    }
    sign = bst->cmpFn(kp, &t->key);
    if (sign == 0) {
        *rp = t;
        return 0;
    }
    if (sign < 0) {
        if (insertTreeNode(bst, &t->left, kp, rp) > 0) {
            switch (t->bf) {
            case +1:
                t->bf = 0;
                return 0;
            case 0:
                t->bf = -1;
                return +1;
            case -1:
                fixLeftImbalance(bst, tp);
                return 0;
            }
        }
    }
    else {
        if (insertTreeNode(bst, &t->right, kp, rp) > 0) {
            switch (t->bf) {
            case -1:
                t->bf = 0;
                return 0;
            case 0:
                t->bf = +1;
                return +1;
            case +1:
                fixRightImbalance(bst, tp);
                return 0;
            }
        }
    }
    return 0;
}

/**
 * @brief Removes the node with the specified key from the tree whose address is passed as the tp parameter. The return
 * value is the change in height of the tree, which is either 0 if the height is unchanged or -1 if the height decreases
 * by one.  This value is then used to correct the balance factors in ancestor nodes.
 */

static int removeTreeNode(BST bst, BSTNode* tp, void* kp) {
    BSTNode t;
    int sign, hDelta, bfDelta;

    t = *tp;
    if (t == NULL) return 0;
    bfDelta = 0;
    sign = bst->cmpFn(kp, &t->key);
    if (sign == 0) return removeTargetNode(bst, tp);
    if (sign < 0) {
        hDelta = removeTreeNode(bst, &t->left, kp);
        if (hDelta < 0) bfDelta = +1;
    }
    else {
        hDelta = removeTreeNode(bst, &t->right, kp);
        if (hDelta < 0) bfDelta = -1;
    }
    adjustBF(bst, tp, bfDelta);
    return ((bfDelta != 0 && t->bf == 0) ? -1 : 0);
}

/**
 * @brief Removes the node whose address is passed as the tp parameter.  This method returns the change in height of the
 * tree rooted at that node, which is either 0 if the height is unchanged or -1 if the height decreases by one.  This
 * value is then used to correct the balance factors in ancestor nodes.
 *
 * This implementation is divided into several cases.  The easy case occurs when either of the children is NULL; in that
 * case, all you need to do is replace the node with its non-NULL child.  If both children are non-NULL, this code finds
 * the rightmost descendent of the left child; this node may not be a leaf, but will have no right child.  Its left
 * child replaces it in the tree, after which the replacement data is moved to the position occupied by the target node.
 */

static int removeTargetNode(BST bst, BSTNode* tp) {
    BSTNode t, np;

    t = *tp;
    if (t->left == NULL) {
        *tp = t->right;
        freeNode(t);
        bst->count--;
        return -1;
    }
    else if (t->right == NULL) {
        *tp = t->left;
        freeNode(t);
        bst->count--;
        return -1;
    }
    else {
        np = t->left;
        while (np->right != NULL) {
            np = np->right;
        }
        t->key = np->key;
        t->value = np->value;
        if (removeTreeNode(bst, &t->left, &t->key) < 0) {
            adjustBF(bst, tp, +1);
            return (t->bf == 0) ? -1 : 0;
        }
        else {
            return 0;
        }
    }
}

/**
 * @brief The freeTree function frees all nodes by conducting a postorder walk. The freeNode function makes a
 * special-case check to free the key string.
 */

static void freeTree(BSTNode t) {
    if (t != NULL) {
        freeTree(t->left);
        freeTree(t->right);
        freeNode(t);
    }
}

static void freeNode(BSTNode node) {
    freeBlock(node);
}

/**
 * @brief Adjusts the balance factors in a node by the specified delta, rebalancing the tree as needed.
 */

static void adjustBF(BST bst, BSTNode* tp, int delta) {
    BSTNode t;

    t = *tp;
    t->bf += delta;
    if (t->bf < -1) {
        fixLeftImbalance(bst, tp);
    }
    else if (t->bf > +1) {
        fixRightImbalance(bst, tp);
    }
}

/**
 * @brief Repairs the balance factor when a node has been found that is out of balance with the longer subtree on the
 * left. Depending on the balance factor of the left child, the code performs a single or double rotation.
 */

static void fixLeftImbalance(BST bst, BSTNode* tp) {
    BSTNode t, parent, child, * cp;
    int oldBF;

    parent = *tp;
    cp = &parent->left;
    child = *cp;
    if (child->bf == +1) {
        oldBF = child->right->bf;
        rotateLeft(bst, cp);
        rotateRight(bst, tp);
        t = *tp;
        t->bf = 0;
        switch (oldBF) {
        case -1:
            t->left->bf = 0;
            t->right->bf = +1;
            break;
        case 0:
            t->left->bf = t->right->bf = 0;
            break;
        case +1:
            t->left->bf = -1;
            t->right->bf = 0;
            break;
        }
    }
    else if (child->bf == 0) {
        rotateRight(bst, tp);
        t = *tp;
        t->bf = +1;
        t->right->bf = -1;
    }
    else {
        rotateRight(bst, tp);
        t = *tp;
        t->right->bf = t->bf = 0;
    }
}

/**
 * @brief Repairs the balance factor when a node has been found that is out of balance with the longer subtree on the
 * right. Depending on the balance factor of the right child, the code performs a single or double rotation.
 */

static void fixRightImbalance(BST bst, BSTNode* tp) {
    BSTNode t, parent, child, * cp;
    int oldBF;

    parent = *tp;
    cp = &parent->right;
    child = *cp;
    if (child->bf == -1) {
        oldBF = child->left->bf;
        rotateRight(bst, cp);
        rotateLeft(bst, tp);
        t = *tp;
        t->bf = 0;
        switch (oldBF) {
        case -1:
            t->left->bf = 0;
            t->right->bf = +1;
            break;
        case 0:
            t->left->bf = t->right->bf = 0;
            break;
        case +1:
            t->left->bf = -1;
            t->right->bf = 0;
            break;
        }
    }
    else if (child->bf == 0) {
        rotateLeft(bst, tp);
        t = *tp;
        t->bf = -1;
        t->left->bf = +1;
    }
    else {
        rotateLeft(bst, tp);
        t = *tp;
        t->left->bf = t->bf = 0;
    }
}

/**
 * @brief Performs a single left rotation of the tree whose address is passed as an argument.  The balance factors are
 * unchanged by this function and must be corrected at a higher level of the algorithm.
 */

static void rotateLeft(BST bst, BSTNode* tp) {
    BSTNode parent, child;
    string key;

    parent = *tp;
    if (bst->debugLog != NULL) {
        key = bst->toStringFn(parent->key);
        fprintf(bst->debugLog, "rotateLeft around %s\n", key);
        freeBlock(key);
    }
    child = parent->right;
    parent->right = child->left;
    child->left = parent;
    *tp = child;
}

/**
 * @brief Performs a single right rotation of the tree whose address is passed as an argument.  The balance factors are
 * unchanged by this function and must be corrected at a higher level of the algorithm.
 */

static void rotateRight(BST bst, BSTNode* tp) {
    BSTNode parent, child;
    string key;

    parent = *tp;
    if (bst->debugLog != NULL) {
        key = bst->toStringFn(parent->key);
        fprintf(bst->debugLog, "rotateRight around %s\n", key);
        freeBlock(key);
    }
    child = parent->left;
    parent->left = child->right;
    child->right = parent;
    *tp = child;
}

/**
 * @brief Implements a recursive walk over the tree.
 */

static void mapTree(BSTNode t, proc fn, TraversalOrder order, void* data) {
    if (t != NULL) {
        if (order == PREORDER) fn(t, data);
        mapTree(t->left, fn, order, data);
        if (order == INORDER) fn(t, data);
        mapTree(t->right, fn, order, data);
        if (order == POSTORDER) fn(t, data);
    }
}

/**
 * @brief Adds the specified node to the iterator, which is passed as the data parameter.
 */

static void addNodeToIterator(BSTNode node, void* data) {
    addToIteratorList((Iterator)data, &node);
}

/**
 * @brief The foreach iterator always uses an INORDER walk.
 */

static Iterator newForeachIterator(void* collection) {
    return newNodeIterator((BST)collection, INORDER);
}
