#include <string.h>
#include <assert.h>
#include "lx_rbtree.h"

static int lx_rbtree_free_node(lx_rbtree_node * n, lx_rbtree_t * tree)
{
    lx_rbtree_node * nil;

    nil = &tree->nil;
    if(n == nil)
        return;

    lx_rbtree_free_node(n->left,tree);
    lx_rbtree_free_node(n->right,tree);

    tree->mem_free(n);
}

int lx_rbtree_free(lx_rbtree_t *tree)
{
    lx_rbtree_free_node(tree->root, tree);    
}

lx_rbtree_node * lx_rbtree_find(lx_rbtree_t * tree, uint64_t key)
{
    lx_rbtree_node * nil,*n;

    n = tree->root;
    nil = &tree->nil;

    while(n != nil){
        uint64_t ckey = n->key;
        if(ckey == key)
            return n;
        if(key < ckey)
            n = n->left;
        else
            n = n->right;
    }

    return NULL;    
}

lx_rbtree_node * lx_rbtree_min(lx_rbtree_t * tree,lx_rbtree_node * node)
{
    lx_rbtree_node * nil,*n,*p;
    
    n = node;
    nil = &tree->nil;
    if(n == nil)
        return nil;

    while(n->left != nil){
        p = n;
        n = n->left;
    }
    return n;    
}

static lx_rbtree_node * new_node(lx_rbtree_t * tree,uint64_t key,void * data,lx_rbtree_color color,lx_rbtree_node * nil)
{
    lx_rbtree_node * n ;
    
    n= (lx_rbtree_node *)tree->mem_malloc(sizeof(lx_rbtree_node));
    if(n == NULL)
        return NULL;
    
    n->key = key;
    n->data = data;
    n->color = color;
    n->left = n->right = n->parent = nil;

    return n;
}

static inline void left_rotate(lx_rbtree_node * x,lx_rbtree_t * tree)
{
    lx_rbtree_node * y,*nil;
    
    nil = &tree->nil; 
    if(x == nil || x->right == nil )
        assert(0);
    
    y = x->right;

    y->parent = x->parent;
    if(x == tree->root)
        tree->root = y;
    else{
        if(x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }

    x->right = y->left;
    if(y->left != nil)
        y->left->parent = x;

    y->left = x;
    x->parent = y;
}

static inline void right_rotate(lx_rbtree_node * x,lx_rbtree_t * tree)
{
    lx_rbtree_node * y,*nil;
    
    nil = &tree->nil; 
    if(x == nil || x->left == nil )
        assert(0);

    y = x->left;
    
    y->parent = x->parent;
    if(x == tree->root)
        tree->root = y;
    else{
        if(x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    
    x->left = y->right;
    if(y->right != nil)
        y->right->parent = x;

    x->parent = y;
    y->right = x;
}

static void fixup_insert_color(lx_rbtree_node * x,lx_rbtree_t * tree)
{
    lx_rbtree_node * nil,*root,*y;

    nil = &tree->nil;

    while(x->parent->color == LX_RED && x != tree->root)
    {
        if(x->parent == x->parent->parent->left){
            y = x->parent->parent->right;

            if(y->color == LX_RED){
                x->parent->color = y->color = LX_BLACK;
                x->parent->parent->color = LX_RED;
                x = x->parent->parent;
            }else{
                if(x == x->parent->right){
                    left_rotate(x->parent,tree);
                    x = x->left;
                }

                x->parent->color = LX_BLACK;
                x->parent->parent->color = LX_RED;
                right_rotate(x->parent->parent,tree);
            }
        }else{
            y = x->parent->parent->left;

            if(y->color == LX_RED){
                x->parent->color = y->color = LX_BLACK;
                x->parent->parent->color = LX_RED;
                x = x->parent->parent;
            }else{
                if(x == x->parent->left){
                    right_rotate(x->parent,tree);
                    x =  x->right;
                }
                x->parent->color = LX_BLACK;
                x->parent->parent->color = LX_RED;
                left_rotate(x->parent->parent,tree);
            }
        }
    }

    if(x == tree->root) x->color = LX_BLACK;
}

int lx_rbtree_insert(lx_rbtree_t * tree, uint64_t key, void * data)
{
    lx_rbtree_node * node,*next_node,*temp_node;
    
    //root
    if(tree->root == &tree->nil){
        node = new_node(tree,key,data,LX_BLACK,&tree->nil);
        if(node == NULL)
            return LX_RBTREE_NOMEM;
        tree->root = node;
        return 0;
    }

    //find parent
    node = tree->root;
    while(1){

        uint64_t ckey = node->key;
        if(ckey == key)
            return LX_RBTREE_KEY_EXIST;
        if(key < ckey )
            next_node = node->left;
        else
            next_node = node->right;
        
        if(next_node == &tree->nil ){
            break;
        }else{
            node = next_node;
            continue;
        }
   }
    
   temp_node = new_node(tree,key,data,LX_RED,&tree->nil);
   if(temp_node == NULL)
       return LX_RBTREE_NOMEM;

   if(temp_node->key < node->key){
       node->left = temp_node;    
   }else{
       node->right = temp_node;
   }
   temp_node->parent = node;

   fixup_insert_color(temp_node,tree);    
   
   return 0;    
}

static void transplant(lx_rbtree_t *tree, lx_rbtree_node * u, lx_rbtree_node * v)
{
    v->parent = u->parent;
    
    if(u->parent == &tree->nil)
        tree->root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
}

#define check_pc_color(n) assert((n)->left->color == LX_BLACK && (n)->right->color == LX_BLACK &&(n)->parent->color == LX_BLACK);
#define check_c_color(n) assert((n)->left->color == LX_BLACK && (n)->right->color == LX_BLACK );

static void fixup_delete_color(lx_rbtree_t * tree, lx_rbtree_node * x)
{
    lx_rbtree_node *nil, *w;

    nil = &tree->nil;
    while(x->color == LX_BLACK && x != tree->root)
    {
        if(x == x->parent->left){
            w = x->parent->right;
            //case 1:w is red
            if(w->color == LX_RED){
                left_rotate(x->parent,tree);
                x->parent->color = LX_RED;
                w->color = LX_BLACK;
                check_pc_color( x->parent );
                
                w = x->parent->right;
            }
            
            //w is black
            //case 2
            if(w->left->color == LX_BLACK && w->right->color == LX_BLACK){
                w->color = LX_RED;
                check_c_color( w);
                x = x->parent;
            }else{
                //case 3
                if( w->right->color == LX_BLACK){
                    right_rotate(w,tree);
                    w->parent->color = LX_BLACK;
                    w->color = LX_RED;
                
                    check_pc_color( w);
                    w = w->parent;
                }

                //case 4
                left_rotate(x->parent,tree);
                w->right->color = LX_BLACK;
                w->color = x->parent->color;
                x->parent->color = LX_BLACK;
                
                x = tree->root;
            }

        }else{
            w = x->parent->left;
            if( w->color == LX_RED){
                right_rotate(x->parent,tree);
                w->color = LX_BLACK;
                
                x->parent->color = LX_RED;
                check_pc_color( x->parent );
                
                w = x->parent->left;
            }
            
            if(w->left->color == LX_BLACK && w->right->color == LX_BLACK ){
                w->color = LX_RED;
                check_c_color( w);
                x = x->parent;
            }else{

                if(w->left->color == LX_BLACK ){
                    left_rotate(w,tree);
                    w->color = LX_RED;
                    w->parent->color = LX_BLACK;
                    check_pc_color( w);
                    
                    w = w->parent;
                    assert(w == x->parent->left);
                }

                right_rotate(x->parent,tree);
                w->left->color = LX_BLACK;
                w->color = x->parent->color;
                x->parent->color = LX_BLACK;

                x = tree->root;
            }
        }//end x is right
    }//end while

    x->color = LX_BLACK; 
}

int lx_rbtree_delete(lx_rbtree_t * tree, uint64_t key)
{
    lx_rbtree_node * n,*nil,*z,*y,*x;
    uint64_t ckey;
    lx_rbtree_color y_original; 

    nil = &tree->nil;
    n = tree->root;
    while(n != nil){
        ckey = n->key;
        if(key == ckey)
            break;
        else if(key <ckey)
            n = n->left;
        else
            n = n->right;
    }
    if(n == nil)
        return LX_RBTREE_NO_EXIST;

    z = y = n;
    y_original = y->color;
    if(z->left == nil){
        x = z->right;
        transplant(tree,z,x);
    }else if( z->right == nil ){
        x = z->left;
        transplant(tree,z,x);
    }else{
        y = lx_rbtree_min(tree,z->right);
        y_original = y->color;

        x = y->right;
        if(y->parent == z){
            x->parent = y;
        }else{
            transplant(tree,y,x);

            y->right = z->right;
            y->right->parent = y;
        }

        transplant(tree,z,y);
        y->left = z ->left;
        y->left->parent = y; 
        y->color = z->color;
    }
    
    tree->mem_free(z);
    
    if(y_original == LX_BLACK)
        fixup_delete_color(tree,x );

    return 0;    
}

