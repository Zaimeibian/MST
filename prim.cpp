#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <iostream>  
#include <fstream>  
#include <iomanip> 
using namespace std;

struct LinkNode
{
	int dest;
	LinkNode* next;
	int weig;
};

struct HeapNode
{
	int v;
	int key;
};

struct Heap
{
	int size, capacity;
	int *pos;
	struct HeapNode **array;
};

struct Node
{
	LinkNode* head;
};

struct Graph
{
	int V;
	struct Node* array;
};

struct LinkNode* newNode(int dest, int weig){
	struct LinkNode* newNo = (struct LinkNode*)malloc(sizeof(struct LinkNode));
	newNo->dest = dest;
	newNo->weig = weig;
	newNo->next = NULL;
	return newNo;
}

struct Graph* createGraph(int V){
	struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
	graph->V = V;
	graph->array = (struct Node*)malloc(V*sizeof(struct Node));
	for(int i=0; i<V; ++i)
		graph->array[i].head = NULL;
	return graph;
}

void addEdge(struct Graph* graph, int src, int dest, int weig){
	struct LinkNode* newNo = newNode(dest, weig);
	newNo->next = graph->array[src].head;
	graph->array[src].head = newNo;

	newNo = newNode(src, weig);
	newNo->next = graph->array[dest].head;
	graph->array[dest].head = newNo;
}

struct HeapNode* newHeapNode(int v, int key){
	struct HeapNode* heapNode = (struct HeapNode*)malloc(sizeof(struct HeapNode));
	heapNode->v = v;
	heapNode->key = key;
	return heapNode;
}

struct Heap* createHeap(int capacity){
	struct Heap* heap = (struct Heap*)malloc(sizeof(struct Heap));
	heap->pos = (int *)malloc(capacity * sizeof(int));
	heap->size = 0;
	heap->capacity = capacity;
	heap->array = (struct HeapNode**)malloc(capacity * sizeof(struct HeapNode*));
	return heap;
}

void swapNode(struct HeapNode** a, struct HeapNode** b){
	struct HeapNode* t = *a;
	*a = *b;
	*b = t;
}

void heapify(struct Heap* heap, int idx){
	int smallest, left, right;
	smallest = idx;
	left = 2*idx+1;
	right = 2*idx+2;

	if (left < heap->size &&
		heap->array[left]->key < heap->array[smallest]->key)
	smallest = left;

	if (right < heap->size &&
		heap->array[right]->key < heap->array[smallest]->key)
	smallest = right;

	if (smallest != idx)
	{
		HeapNode *smallestNode = heap->array[smallest];
		HeapNode *idxNode = heap->array[idx];

		heap->pos[smallestNode->v] = idx;
		heap->pos[idxNode->v] = smallest;

		swapNode(&heap->array[smallest], &heap->array[idx]);

		heapify(heap, smallest);
	}
}

int isEmpty(struct Heap* heap){
	return heap->size == 0;
}

struct HeapNode* extractMin(struct Heap* heap){
	if(isEmpty(heap))
		return NULL;
	struct HeapNode* root = heap->array[0];
	struct HeapNode* lastNode = heap->array[heap->size-1];
	heap->array[0] = lastNode;

	heap->pos[root->v] = heap->size-1;
	heap->pos[lastNode->v]= 0;

	--heap->size;
	heapify(heap, 0);
	return root;
}

void decreaseKey(struct Heap* heap, int v, int key){
	int i = heap->pos[v];
	heap->array[i]->key = key;
	while(i && heap->array[i]->key < heap->array[(i-1)/2]->key){
		heap->pos[heap->array[i]->v] = (i-1)/2;
		heap->pos[heap->array[(i-1)/2]->v] = i;
		swapNode(&heap->array[i], &heap->array[(i-1)/2]);

		i = (i-1)/2;
	}
}

bool isInHeap(struct Heap* heap, int v){
	if(heap->pos[v] < heap->size)
		return true;
	return false;
}

void PrimMst(struct Graph* graph){
	int V = graph->V;
	int parent[V];
	int key[V];
	ofstream outFile;
	outFile.open("primMst.txt", ios::trunc);
	struct Heap* heap = createHeap(V);
	for(int v=1; v<V; ++v){
		parent[v] = -1;
		key[v] = INT_MAX;
		heap->array[v] = newHeapNode(v, key[v]);
		heap->pos[v] = v;
	}

	key[0] = 0;
	heap->array[0] = newHeapNode(0, key[0]);
	heap->pos[0] = 0;
	heap->size = V;

	while(!isEmpty(heap)){
		struct HeapNode* heapNode = extractMin(heap);
		int u = heapNode->v;
		char a,b;
		if(u!=0){
			a = parent[u]+97;
			b = u+97;
			outFile<<"("<<a<<","<<b<<")"<<"="<<key[u]<<"\n";
		}
		struct LinkNode* pCrawl = graph->array[u].head;
		while(pCrawl != NULL){
			int v = pCrawl->dest;
			if(isInHeap(heap, v) && pCrawl->weig<key[v]){
				key[v] = pCrawl->weig;
				parent[v] = u;
				decreaseKey(heap, v, key[v]);
			}
			pCrawl = pCrawl->next;
		}
	}
	outFile.close();
}
struct Graph* readGraph(char* path){
	char buffer[256];
	int V, E;
	int src, dest, weig;
	fstream file;
	file.open(path,ios::in);
	file.getline(buffer, 256, '\n');
	V =(int)buffer[0];
	struct Graph* graph = createGraph(V);
	file.getline(buffer, 256, '\n');
	file.getline(buffer, 256, '\n');
	while(!file.eof()){
		file.getline(buffer, 256, '\n');
		src = buffer[1]-97;
		dest = buffer[3]-97;
		weig = buffer[6]-0;
		addEdge(graph, src, dest, weig);
	}
	file.close();
	return graph;
}

int main(){
	char path[256];
	printf("please enter the path of 'graph.in'");
	cin>>path;
	struct Graph* graph = readGraph(path);
	PrimMst(graph);
	return 0;
}
