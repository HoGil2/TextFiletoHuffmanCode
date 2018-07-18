#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  #define DEBUG
#define MAX_ELEMENT 200
#define ALPHABET -('A'-'z')
#define MAX_HUFFCODE 20

typedef struct Alphabet {
	char alpha;
	int freq;
}Alphabet;
typedef struct char_table {
	Alphabet alphabet[ALPHABET];
	int index;
}char_table;
typedef struct TreeNode {
	Alphabet weight;
	char Huffcode[MAX_HUFFCODE];
	struct TreeNode *left_child;
	struct TreeNode *right_child;
}TreeNode;
typedef struct {
	TreeNode *ptree;
	int key;
}element;
typedef struct HeapType {
	element heap[MAX_ELEMENT];
	int heap_size;
}HeapType;
void init_heap(HeapType *heap) {
	heap->heap_size = 0;
}
void error(char *message) {
	fprintf(stderr, "%s\n", message);
	exit(1);
}

//  count the frequency of alphabet in the text
char_table* count_frequency(FILE *fp) {
	char_table *table = (char_table *) malloc (sizeof(char_table));
	Alphabet alphabet[58];
	char alpha = 0;
	int i = 0, j, k = 0;
	int count = 0;
#ifdef DEBUG
	int count_alphabet = 0;
	int count_all = 0;
#endif

	//  init array alphabet
	for (i = 0; i < ALPHABET; i++) {
		alphabet[i].freq = 0;
	}
	//  Input all text char to array
	while (alpha != EOF) {
		alpha = getc(fp);
		//  needed to improvement
		if ('A' <= alpha <= 'Z' || 'a' <= alpha <= 'z') {
#ifdef DEBUG
			count_all++;
			printf("present_char:%c\n", alpha);
#endif
			//  -65 need to that the 'A' is 65 in ASCII
			if (alphabet[alpha - 65].freq == 0) {
				alphabet[alpha - 65].alpha = alpha;
				alphabet[alpha - 65].freq = 1;
				count++;
#ifdef DEBUG
				count_alphabet++;
#endif
			}
			else {
				alphabet[alpha - 65].freq++;
			}
		}
	}
#ifdef DEBUG
	for (i = 0; i < ALPHABET; i++) {
		if (i == 26)
			i += 6;
		printf("alphabet[%d] = {alpha: %c, freq: %d}\n", i, i + 65, alphabet[i].freq);
	}
	printf("NumofAllChar: %d, table->index: %d\n", count_all, count);
#endif
	for (j = 0; j < ALPHABET; j++) {
		if (alphabet[j].freq != 0)
			table->alphabet[k++] = alphabet[j];
	}
	table->index = count;
	//  return the pointer of char_table
	return table;
}
//  Delete minimal value at the heap
element delete_min_heap(HeapType *heap) {
	element item, temp;
	int parent, child;

	item = heap->heap[1];
	temp = heap->heap[(heap->heap_size)--];
	parent = 1;
	child = 2;

	while (child <= heap->heap_size) {
		//  Search for a small child out of two children.
		if ((child < heap->heap_size) &&
			heap->heap[child].key > heap->heap[child + 1].key) {
			child++;
		}
		if (temp.key <= heap->heap[child].key) break;

		heap->heap[parent] = heap->heap[child];
		parent = child;
		child *= 2;
	}
	heap->heap[parent] = temp;
	//  return minimal value
	return item;
}
//  Insert at the heap
void insert_min_heap(HeapType *heap, element item) {
	int i;

	i = ++(heap->heap_size);
	//  Comparing with the parent node.
	//  If (i == 1) means the item's key is least or the heap has not other item.
	while ((i != 1) && (item.key < heap->heap[i / 2].key)) {
		heap->heap[i] = heap->heap[i / 2];
		i /= 2;
	}
	// Insert the new item.
	heap->heap[i] = item;
}
//  get child node and make parent node
TreeNode* make_node(TreeNode *left_child, TreeNode *right_child) {
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	if (node == NULL) {
		error("Error of set the memory of node.\n");
	}
	node->left_child = left_child;
	node->right_child = right_child;

	return node;
}
//  Insert the array at Hufftree
element make_Hufftree(char_table *table) {
	HeapType heap;
	TreeNode *node;
	element e, temp1, temp2;
	int i;
	//  init heap
	heap.heap_size = 0;

	for (i = 0; i < table->index; i++) {
		//  add exist alphabet to the heap 
		if (table->alphabet[i].freq != 0) {
			node = make_node(NULL, NULL);
			node->weight.alpha = table->alphabet[i].alpha;
			e.key = node->weight.freq = table->alphabet[i].freq;
			e.ptree = node;
#ifdef DEBUG
			printf("alpha:%c, freq:%d\n", e.ptree->weight.alpha, e.ptree->weight.freq);
#endif
			insert_min_heap(&heap, e);
		}
	}
	for (i = 0; i < table->index - 1; i++) {
		temp1 = delete_min_heap(&heap);
#ifdef DEBUG
		printf("temp1.char:%c, ", temp1.ptree->weight.alpha);
#endif
		temp2 = delete_min_heap(&heap);
#ifdef DEBUG
		printf("temp2.char:%c\n", temp2.ptree->weight.alpha);
#endif

		node = make_node(temp1.ptree, temp2.ptree);
		node->weight.alpha = node->weight.freq = NULL;
		e.key = temp1.key + temp2.key;
		e.ptree = node;
		insert_min_heap(&heap, e);
	}
	//  'e' is the head of heap
	e = delete_min_heap(&heap);

	return e;
}
//  set Huffmancode moving around the tree
void set_HuffCode(TreeNode *head, int level, char *code) {
	if (head) {
		level++;
		code[level] = '0';
		set_HuffCode(head->left_child, level, code);
		code[level] = '1';
		set_HuffCode(head->right_child, level, code);
		code[level] = '\0';
	
		if (head->left_child == NULL && head->right_child == NULL) {
			for (int i = 0; i < level + 1; i++)
				head->Huffcode[i] = code[i];
#ifdef DEBUG
			printf("alpha:%c, freq:%d, code:", head->weight.alpha, head->weight.freq);
			for (int i = 0; i < level + 1; i++)
				printf("%c", head->Huffcode[i]);
			printf("\n");
#endif
		}
	}
}
void make_Huffcode(FILE *fp) {
	char_table *table;
	element e;
	char code[20];

	table = count_frequency(fp);

	e = make_Hufftree(table);

	set_HuffCode(e.ptree, -1, code);

	free(table);
}
void main(int argc, char **argv) {

	FILE *fp;
	//  file open
	if ((fp = fopen("sample.txt", "r")) == NULL) {
		error("Can't open the file.\n");
	}

	make_Huffcode(fp);

	fclose(fp);
}