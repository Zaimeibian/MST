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

struct Edge{
	int src, dest, weight;
};


struct GraphK{
	int V, E;
	struct Edge* edge;
};

struct GraphK* createGraphK(int V, int E)
{
	struct GraphK* graph = new GraphK;
	graph->V = V;
	graph->E = E;

	graph->edge = new Edge[E];

	return graph;
}

struct subset{
	int parent;
	int rank;
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
	int key[V], w[V];
	int c;
	char a[V], b[V];
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
		//printf("w is %d\n", heapNode->key);
		if(u!=0){
			a[c] = parent[u]+97;
			b[c] = u+97;
			w[c] = heapNode->key;
			c++;
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
	outFile<<c<<"\n";
	for(int i=0; i<c; i++){
		outFile<<"("<<a[i]<<","<<b[i]<<")"<<"="<<w[i]<<"\n";
	}
	outFile.close();
}
struct Graph* readGraphP(char* path){
	char buffer[256];
	char weig[3];
	int V, E;
	int src, dest, weigh;
	int i=0;
	fstream file;
	file.open(path,ios::in);
	file.getline(buffer, 256, '\n');
	sscanf(buffer, "%d", &V);
	//printf("the V is %d\n",V);
	struct Graph* graph = createGraph(V);
	file.getline(buffer, 256, '\n');
	file.getline(buffer, 256, '\n');
	sscanf(buffer, "%d", &E);
	//printf("the E is %d\n",E);
	while(i<E){
		file.getline(buffer, 256, '\n');
		src = buffer[1]-97;
		dest = buffer[3]-97;
		weig[0] = buffer[6];
		weig[1] = buffer[7];
		weig[2] = buffer[8];
		weigh = atoi(weig);
		//printf("src is %d, dest is %d, weight is %d\n", src, dest, weigh);
		addEdge(graph, src, dest, weigh);
		i++;
	}
	file.close();
	return graph;
}



int find(struct subset subsets[], int i)
{
	if (subsets[i].parent != i)
		subsets[i].parent = find(subsets, subsets[i].parent);

	return subsets[i].parent;
}


void Union(struct subset subsets[], int x, int y)
{
	int xroot = find(subsets, x);
	int yroot = find(subsets, y);


	if (subsets[xroot].rank < subsets[yroot].rank)
		subsets[xroot].parent = yroot;
	else if (subsets[xroot].rank > subsets[yroot].rank)
		subsets[yroot].parent = xroot;

	else
	{
		subsets[yroot].parent = xroot;
		subsets[xroot].rank++;
	}
}


int myComp(const void* a, const void* b)
{
	struct Edge* a1 = (struct Edge*)a;
	struct Edge* b1 = (struct Edge*)b;
	return a1->weight - b1->weight;
}


struct GraphK* readGraphK(char* path){
	char buffer[256];
	int src, dest, weigh;
	int V, E;
	char weig[3];
	int i=0;
	fstream file;
	file.open(path,ios::in);
	file.getline(buffer, 256, '\n');
	sscanf(buffer, "%d", &V);
	//printf("the V is %d\n",V);
	file.getline(buffer, 256, '\n');
	file.getline(buffer, 256, '\n');
	sscanf(buffer, "%d", &E);
	struct GraphK* graph = createGraphK(V, E);
	//printf("the E is %d\n",E);
	while(i<E){
		file.getline(buffer, 256, '\n');
		src = buffer[1]-97;
		dest = buffer[3]-97;
		weig[0] = buffer[6];
		weig[1] = buffer[7];
		weig[2] = buffer[8];
		weigh = atoi(weig);
		//printf("src is %d, dest is %d, weight is %d\n", src, dest, weigh);
		graph->edge[i].src = src;
		graph->edge[i].dest = dest;
		graph->edge[i].weight = weigh;
		i++;
	}
	file.close();
	return graph;
}

void KruskalMST(struct GraphK* graph)
{
	int V = graph->V;
	struct Edge result[V];
	int e = 0; 
	int i = 0; 
	int n, m;
	char a[V], b[V];
	ofstream outFile;
	outFile.open("kruskalMst.txt", ios::trunc);
	//printf("the size of element is %d\n", sizeof(int));
	qsort(graph->edge, graph->E, sizeof(graph->edge[0]), myComp);

	
	struct subset *subsets =
		(struct subset*) malloc( V * sizeof(struct subset) );


	for (int v = 0; v < V; ++v)
	{
		subsets[v].parent = v;
		subsets[v].rank = 0;
	}

	while (e < V - 1)
	{

		struct Edge next_edge = graph->edge[i++];
		//printf("the weight is %d\n", next_edge.weight);
		n = find(subsets, next_edge.src);
		m = find(subsets, next_edge.dest);

		if (n != m)
		{
			result[e++] = next_edge;
			Union(subsets, n, m);
		}

	}

	outFile<<e<<"\n";
	for (i = 0; i < e; ++i){
		a[i] = result[i].src+97;
		b[i] = result[i].dest+97;
		outFile<<"("<<a[i]<<","<<b[i]<<")"<<"="<<result[i].weight<<"\n";
	}
	return;
}



int main(){
	char path[256];
	char rpath[] = "graph.txt";
	printf("please enter the path of 'graph.in'\n");
	cin>>path;
	if(strcmp(path, rpath))
		printf("input file must be 'graph.txt',please run again\n");
	else{
		struct GraphK* graphK = readGraphK(path);
		struct Graph* graphP = readGraphP(path);
		PrimMst(graphP);
		KruskalMST(graphK);
		printf("program done, you can check the two output file now\n");
	}
	return 0;
}
