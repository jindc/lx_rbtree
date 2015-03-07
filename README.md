# lx_rbtree
    lx_rbtree是用c语言实现的红黑树。算法参考了Cormen, Leiserson and Rivest写的《算法导论》（第三版）。
    
    lx_rbtree的定义和实现在lx_rbtree.h和lx_rbtree.c中,lx_rbtree_test.c是使用例子和测试文件。
    
    lx_rbtree使用uint64_t作为key，支持定制的内存分配和释放函数。
    
    lx_rbtre支持查找，插入，删除，找到最小可以节点操作，使用例子见测试文件。
    
    lx_rbtree_test.c包含简单树形打印，红黑树黑高打印，红黑树有序性验证，红黑树五条性质验证，根据工作需要修改lx_rbtree后可以用里面的方法验证正确性。
    
    lx_rbtree的测试包括valgrind内存测试和百万次随机插入，删除，查找测试，程序的正确性有一定保证。
    
    编译及运行：
        ./build.sh
        ./test
    
  作者：德才
  email:jindc@163.com

