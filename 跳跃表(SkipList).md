# 跳跃表(SkipList)

阅读Redis源码之际，想对SkipList做一个系统的深入研究，输出过程如下：

### 定义
1. 跳表(SkipList)是一种数据结构，允许快速查询一个有序连续元素的链表。是基于并联的链表，其效率可比拟二叉查找树(平均时间复杂度为O(lgN))。基本上，跳跃列表是对有序的链表增加上附加的前进链接，增加是以随机化的方式进行的，所以在列表中的查找可以快速的跳过部分列表，因此得名。所有操作都以对数随机化的时间进行。
2. 跳跃表的核心思想是:空间换时间，每个节点维护数目随机的指向后续节点的指针(层数)，从而提高查找效率
3. 跳跃表性质如下：
    * 一个跳表由很多层组成
    * 每一层都是一个有序的链表
    * 最底层的链表(Level 1)包含所有的元素
    * 如果一个元素出现在Level i层的链表中，则在Level i层以下的所有层都将包含该元素
    * 每个节点包含key及其对应的value和一个指向同一层链表的下个节点的指针数组

### SkipList结构及相关操作
1. 跳表节点定义如下：

    ```cpp
    typedef struct skiplistNode
    {
	    int key;
	    //forward level
	    struct skiplistNode *forward[MAX_LEVEL];
    }skiplistNode;
    ```
2. 跳表数据结构如下：

    ```cpp
    typedef struct skiplist
    {
	    struct skiplistNode *head;
    }skiplist;
    ```
注意：
* 跳表每个节点包含不同的前进层数，本来应该是 `struct skiplistNode *forward`然后进行动态分配，但是在此为了方便，直接指定了最大层数
* 跳表结构包含一个头结点，头结点的层数最大，初始都为NULL， 然后，其key为负无穷，即为最小值

3. 查找操作：总结下来就是一个“折半查找”的过程，从头结点的最高层往右下走，找到相应节点为止。

    ```cpp
    skiplistNode *searchNode(skiplist *sl, int key)
    {
    	skiplistNode *ptr = sl->head;
    	for (int i = MAX_LEVEL - 1; i >= 0; --i)
    	{
    		while(ptr && ptr->forward[i] && ptr->forward[i]->key < key)
    			ptr = ptr->forward[i];
    	}
    	ptr = ptr->forward[0];
    
    	if (ptr && ptr->key == key)
    		return ptr;
    	return NULL;
    }
    ```
    
4. 插入操作：
* 定义一个`update[MAX_LEVEL]`数组，然后在查找对应位置的时候，更新数组，`update[MAX_LEVEL]`的最后位置，就是插入点的前一个位置

    ```cpp
    bool insertNode(skiplist *sl, int key)
    {
    	skiplistNode *ptr = sl->head;
    	struct skiplistNode *update[MAX_LEVEL];
    	int newlevel = randLevel();
    	for (int i = MAX_LEVEL - 1; i >= 0; --i)
    	{
    		while (ptr->forward[i] && ptr->forward[i]->key < key)
    			ptr = ptr->forward[i];
            //每层更新update
    		update[i] = ptr;
    	}
    
    	ptr = ptr->forward[0];
    	if (ptr && ptr->key == key)
    		return false;
    
    	skiplistNode *tmp = new skiplistNode;
    	tmp->key = key;
    	for (int i = 0; i < MAX_LEVEL; ++i)
    		tmp->forward[i] = NULL;
    
        //每层插入
    	for (int i = newlevel - 1; i >= 0; --i)
    	{
    		ptr = update[i];
    		tmp->forward[i] = ptr->forward[i];
    		ptr->forward[i] = tmp;
    	}
    	return true;
    }
    ```
5. 删除操作
* 删除操作和插入差不多，不同的是，再最后一步，删除操作是每层的单链表删除操作

    ```cpp
    void deleteNode(skiplist *sl, int key)
    {
    	skiplistNode *ptr = sl->head;
    	skiplistNode *update[MAX_LEVEL];
    	for (int i = MAX_LEVEL - 1; i >= 0; --i)
    	{
    		while (ptr && ptr->forward[i] && ptr->forward[i]->key < key)
    			ptr = ptr->forward[i];
    
    		update[i] = ptr;
    	}
    
    	ptr = ptr->forward[0];
    	if (!ptr || ptr->key != key)
    		return;
    
    	for (int i = MAX_LEVEL - 1; i >= 0; --i)
    	{
    		if (update[i]->forward[i] == ptr){
    			update[i]->forward[i] = ptr->forward[i];
    		}
    	}
    
    	delete ptr;
    }
    ```

### 总结
* 总的来说，skiplist比较简单，而且易于实现，关键在于理解插入和删除时的update数组的作用。
* 源代码见：[SkipList简单实现及测试][1]


  [1]: https://github.com/lengender/SkipList