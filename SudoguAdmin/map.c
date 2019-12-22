/**
 * @file map.c
 *
 * This file implements the map.h interface, which implements maps using the balanced binary tree abstraction exported
 * by the bst.h interface.
 */

#include <string.h>
#include "bst.h"
#include "cslib.h"
#include "iterator.h"
#include "itertype.h"
#include "map.h"

 /**
  * @brief This type is the concrete type used to represent the map.
  */

struct MapCDT {
    IteratorHeader header;              /* Header to enable iteration   */
    BST bst;                            /* BST that does all the work   */
};

/* Private function prototypes */

static Iterator newMapIterator(void* collection);
static void addKeyToIterator(BSTNode node, void* data);

/* Exported entries */

Map newMap() {
    Map map;

    map = newBlock(Map);
    enableIteration(map, newMapIterator);
    map->bst = newBST(string);
    return map;
}

void freeMap(Map map) {
    freeBST(map->bst);
    freeBlock(map);
}

int sizeMap(Map map) {
    return sizeBST(map->bst);
}

bool isEmptyMap(Map map) {
    return isEmptyBST(map->bst);
}

void clearMap(Map map) {
    clearBST(map->bst);
}

Map cloneMap(Map map) {
    Map newmap;

    newmap = newBlock(Map);
    enableIteration(newmap, newMapIterator);
    newmap->bst = cloneBST(map->bst);
    return newmap;
}

void putMap(Map map, string key, void* value) {
    BSTNode node;

    node = insertBSTNode(map->bst, key);
    setNodeValue(node, value);
}

void* getMap(Map map, string key) {
    BSTNode node;

    node = findBSTNode(map->bst, key);
    return (node == NULL) ? NULL : getNodeValue(node);
}

bool containsKeyMap(Map map, string key) {
    return findBSTNode(map->bst, key) != NULL;
}

void removeMap(Map map, string key) {
    removeBSTNode(map->bst, key);
}

void mapMap(Map map, proc fn, void* data) {
    Iterator it;
    BSTNode node;

    it = newNodeIterator(map->bst, INORDER);
    while (stepIterator(it, &node)) {
        fn(getKeyString(node), getNodeValue(node), data);
    }
}

/* Private functions */

static Iterator newMapIterator(void* collection) {
    Iterator iterator;

    iterator = newListIterator(sizeof(string), NULL);
    mapBST(((Map)collection)->bst, addKeyToIterator, INORDER, iterator);
    return iterator;
}

/**
 * @brief Adds the key from the specified node to the iterator, which is passed as the data parameter.
 */

static void addKeyToIterator(BSTNode node, void* data) {
    string key;

    key = (string)getKey(node).pointerRep;
    addToIteratorList((Iterator)data, &key);
}
