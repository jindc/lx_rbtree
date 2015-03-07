#ifndef LX_RBTREE_H
#define LX_RBTREE_H
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum lx_rbtree_rcode lx_rbtree_rcode;
enum lx_rbtree_rcode
{
    LX_RBTREE_OK = 0,
    LX_RBTREE_NOMEM = -100,
    LX_RBTREE_KEY_EXIST,
    LX_RBTREE_NO_EXIST,
    LX_RBTREE_ERROR
};

typedef enum lx_rbtree_color lx_rbtree_color;
enum lx_rbtree_color
{
    LX_BLACK ,
    LX_RED
};

typedef struct lx_rbtree_node lx_rbtree_node;
struct lx_rbtree_node
{
    uint64_t key;
    void * data;
    lx_rbtree_color color;
    
    lx_rbtree_node * left;
    lx_rbtree_node * right;
    lx_rbtree_node * parent;
};

typedef struct lx_rbtree_t lx_rbtree_t;
struct lx_rbtree_t 
{
    lx_rbtree_node * root;
    lx_rbtree_node  nil;
    
    void *(*mem_malloc)(size_t size);
    void (*mem_free)(void * ptr);
};

#define lx_rbtree_zero(pnode)  memset((pnode),0,sizeof(lx_rbtree_node));
#define lx_rbtree_init(tree,m_malloc, m_free)\
    lx_rbtree_zero( &(tree)->nil)\
    (tree)->root = &(tree)->nil;\
    (tree)->mem_malloc = (m_malloc);\
    (tree)->mem_free = (m_free);

int lx_rbtree_free(lx_rbtree_t *tree);

lx_rbtree_node * lx_rbtree_min(lx_rbtree_t * tree,lx_rbtree_node * n);
lx_rbtree_node * lx_rbtree_find(lx_rbtree_t * tree, uint64_t key);

int lx_rbtree_insert(lx_rbtree_t * tree, uint64_t key, void * data);
int lx_rbtree_delete(lx_rbtree_t * tree, uint64_t key);

#endif

