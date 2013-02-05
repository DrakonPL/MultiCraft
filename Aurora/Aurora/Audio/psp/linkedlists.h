typedef struct node1{
       NodeData data;
       struct node1 * next;
} Node1, * Node1Ptr;

typedef struct node2{
       NodeData data;
       struct node2 * previous, * next;
} Node2, * Node2Ptr;
