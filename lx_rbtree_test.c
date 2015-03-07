#include <assert.h>
#include "lx_rbtree.h"

#define pspace(size) {int i = (size); while(i--)printf(" "); }

int prekey; 
int isfirst = 1;
void traversal_lmr( lx_rbtree_node * node,lx_rbtree_node * nil,int is_print )
{
    if(node == nil)
        return;

    if(node->left != nil)
        traversal_lmr(node->left,nil,is_print);

    if(is_print) printf("%d ",(int)node->key );

    if(isfirst){ 
       prekey = (int)node->key;
       isfirst = 0;
    }else{
        assert(prekey < (int)node->key);
    }
    prekey = (int)node->key;

    if(node->right != nil)
       traversal_lmr(node->right,nil,is_print);
}

void print_rbtree(lx_rbtree_node * node,lx_rbtree_node * nil,int bspace,int* vspace)
{
    int temp;

    if(node == nil)
        return;

    temp = printf("----%d:%s:%p",(int)node->key,node->color == LX_BLACK?"b":"r",node->data);
    
    if(node->right != nil){
    
        bspace += temp;
        print_rbtree(node->right,nil,bspace,vspace);
        bspace -= temp;
    }

    if(node->left != nil){
        int i = 0;
        bspace += temp;

        for(;i < *vspace;++i){
            printf("\n");
            pspace(bspace);
            printf("|");
        }
        printf("\n");
        pspace(bspace);
        print_rbtree(node->left,nil,bspace,vspace);
        //*vspace += 2;
        bspace -= temp;
    }
}

void check_rbtree_node(lx_rbtree_node * n,lx_rbtree_t * tree,int * bh)
{
    lx_rbtree_node *nil;
    int lbh,rbh;
    nil = &tree->nil;
    
    //rule 2
    if(n == tree->root)
        assert(n->color == LX_BLACK);
    
    //rule 3
    if(n == nil){
        assert(nil->color == LX_BLACK);
    }
    
    //rule 4
    if(n != nil){
        if( n->color == LX_RED){
            assert(n->left->color == LX_BLACK && n->right->color == LX_BLACK);    
        }
    }

    //check point
    if(n != nil){
        if(n == tree->root)
            assert(n->parent == nil);
        if(n->left != nil)
            assert(n->left->parent == n);
        if(n->right != nil)
            assert(n->right->parent == n);
    }

    if(n != nil){
        check_rbtree_node(n->left,tree,&lbh);
        check_rbtree_node(n->right,tree,&rbh);
    }

    //rule 5
    if(n == nil){
        *bh = 1;    
    }else{
        //printf("%d:lbh %d,rbh %d\n",(int)n->key,lbh,rbh);    
        assert( lbh == rbh);
        if(bh != NULL)
            if(n->color == LX_BLACK)
                *bh = lbh + 1;
            else
                *bh = lbh;
    }
}


void check_rbtree(lx_rbtree_t * tree,int p_bh)
{
    lx_rbtree_node *nil;
    int bh;

    //make sure a sort tree
    isfirst = 1;
    traversal_lmr(tree->root,&tree->nil,0);

    //make sure a rbtree
    check_rbtree_node(tree->root,tree,&bh);
    if(p_bh)printf("the black high is %d\n",bh);
}

int simple_test( )
{
    int ret;
    unsigned int i = 0,j = 0;

    int ia[] = {41,38,31,12,19,8,45,49,488};
    int da[] = {8,12,19,31,38,41,488,49,45};
    
    lx_rbtree_t tree, * ptree;
    ptree = &tree;
    lx_rbtree_init(ptree,malloc,free);
    
    for(; i < sizeof(ia)/sizeof(int); ++i)
    {
        int vspace = 2;
        printf("insert %d \n",ia[i]);
        ret = lx_rbtree_insert(ptree,ia[i],(void *)(long)(i+5));
        assert(ret == 0);

        ret = lx_rbtree_insert(ptree,ia[i],(void *)(long)(i+5));
        assert(ret == LX_RBTREE_KEY_EXIST);
        
        isfirst = 1;
        traversal_lmr(ptree->root,&ptree->nil,1);
        printf("\n");
        
        printf("the tree is: \n");
        print_rbtree(ptree->root,&ptree->nil,0,&vspace);
        printf("\n");

        check_rbtree(ptree,1);

    }
        
    assert( lx_rbtree_find(ptree,ia[3]));
    assert(! lx_rbtree_find(ptree,-1));
    
    for(i = 0; i < sizeof(da)/sizeof(int); ++i)
    {
        int vspace = 2;

        printf("delete %d \n",da[i]);
        ret = lx_rbtree_delete(ptree,da[i]);
        assert(ret == 0);

        ret = lx_rbtree_delete(ptree,da[i]);
        assert(ret == LX_RBTREE_NO_EXIST);
        
        isfirst = 1;
        traversal_lmr(ptree->root,&ptree->nil,1);
        printf("\n");
        
        printf("the tree is: \n");
        print_rbtree(ptree->root,&ptree->nil,0,&vspace);
        printf("\n");

        check_rbtree(ptree,1);
    }

    lx_rbtree_free(ptree);
    
    return 0;    
}

#define lx_rand( max_num) ( 1 + (int)( (max_num) * (rand()/(RAND_MAX + 1.0)) ))
void batch_test(int size)
{
    lx_rbtree_t tree, *ptree;
    int i,ret,temp;
    int vspace = 2;
    int max_num = 10000000;
    int is_detail = 0;
    ptree = &tree;
    lx_rbtree_init(ptree,malloc,free);

    for(i = 0; i < size;i++){
        temp = lx_rand(max_num); 
        ret = lx_rbtree_insert(ptree,temp,(void *)(long)(i+5));
        assert( lx_rbtree_find(ptree,temp));
    } 
    check_rbtree(ptree,1);
    
    printf("the tree is: \n");
    //print_rbtree(ptree->root,&ptree->nil,0,&vspace);
    printf("\n");
    
    for(i = 0; i < max_num;i++){
        //temp = i%2 == 0?i:i+1 ;
        //temp = i;
        temp = lx_rand(max_num);

    //    printf("delete %d \n",temp);
        
       //if(temp > 3800) print_rbtree(ptree->root,&ptree->nil,0,&vspace);
        
        ret = lx_rbtree_delete(ptree,temp);
        //check_rbtree(ptree,0);
        assert( !lx_rbtree_find(ptree,temp));
    }

    printf("after delete\n");
    isfirst = 1;
    //traversal_lmr(ptree->root,&ptree->nil,1);
    printf("\n");
    
   //printf("the tree is: \n");
   //print_rbtree(ptree->root,&ptree->nil,0,&vspace);
   // printf("\n");
    check_rbtree(ptree,1);
    

    lx_rbtree_free(ptree);

}

int main(int argc,char * argv[])
{
    simple_test();  
    batch_test(1000000);
}
