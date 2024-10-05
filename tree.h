/* tree.h */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <errno.h>

#define TagRoot 1 /* 00 01 */
#define TagNode 2 /* 00 10 */
#define TagLeaf 4 /* 01 00 */

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;
typedef unsigned char Tag;

struct s_node {
    Tag tag;
    struct s_node *north; // parent node
    struct s_node *west; // nodes that hang from this
    struct s_leaf *east; // data
    int8 path[256];
};

typedef struct s_node Node;

struct s_leaf {
    Tag tag;
    union u_tree *west; // either a node or a leaf
    struct s_leaf *east;
    int8 key[128]; // each leaf has a key that distiguishes it
    int8 *value;
    int16 size;
};

typedef struct s_leaf Leaf;

union u_tree {
    Node n;
    Leaf l;
};

typedef union u_tree Tree;
