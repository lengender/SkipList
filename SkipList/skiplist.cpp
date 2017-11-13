#include<iostream>
using namespace std;

//max level
const int MAX_LEVEL = 5;

//skiplistNode
typedef struct skiplistNode
{
	int key;
	//forward level
	struct skiplistNode *forward[MAX_LEVEL];
}skiplistNode;

//skiplist
typedef struct skiplist
{
	struct skiplistNode *head;
}skiplist;

//create a skiplist
skiplist *createSkipList()
{
	skiplist *sl = new skiplist;
	sl->head = new skiplistNode;
	sl->head->key = INT_MIN;
	for (int i = 0; i < MAX_LEVEL; ++i)
		sl->head->forward[i] = NULL;

	return sl;
}

//search a key from skiplist
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
//rand a level
int randLevel()
{
	int i = 1;
	while (rand() % 2)
		i++;

	if (i >= MAX_LEVEL)
		i = MAX_LEVEL;

	return i;
}

//insert a skiplistNode to skiplist
bool insertNode(skiplist *sl, int key)
{
	skiplistNode *ptr = sl->head;
	struct skiplistNode *update[MAX_LEVEL];
	int newlevel = randLevel();
	for (int i = MAX_LEVEL - 1; i >= 0; --i)
	{
		while (ptr->forward[i] && ptr->forward[i]->key < key)
			ptr = ptr->forward[i];

		update[i] = ptr;
	}

	ptr = ptr->forward[0];
	if (ptr && ptr->key == key)
		return false;

	skiplistNode *tmp = new skiplistNode;
	tmp->key = key;
	for (int i = 0; i < MAX_LEVEL; ++i)
		tmp->forward[i] = NULL;

	for (int i = newlevel - 1; i >= 0; --i)
	{
		ptr = update[i];
		tmp->forward[i] = ptr->forward[i];
		ptr->forward[i] = tmp;
	}

	return true;
}

//delete a node from skiplist
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

int main()
{
	skiplist *sl = createSkipList();
	for (int i = 0; i < 10; ++i)
	{
		insertNode(sl, i + 1);
	}

	skiplistNode *t1 = searchNode(sl, 1);
	skiplistNode *t2 = searchNode(sl, 5);
	skiplistNode *t3 = searchNode(sl, 9);

	if (t1)
	cout << t1->key << " " << t2->key << " " << t3->key << endl;

	for (int i = 0; i < 7; ++i)
		deleteNode(sl, i + 1);

	if (searchNode(sl, 5) == NULL)
		cout << "nice" << endl;
	else cout << "fuck" << endl;

	return 0;
}