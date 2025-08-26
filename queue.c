#include <stdlib.h>
#include <stdio.h>

typedef struct Node
{
    struct Node *next;
    void *value;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
} Queue;

static Node *createNode(void *value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) return NULL;
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

Queue *createQueue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) return NULL;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void enqueue(Queue *queue, void *data)
{
    Node *newNode = createNode(data);
    if (queue->tail == NULL)
    {
        queue->head = newNode;
        queue->tail = newNode;
        return;
    }
    queue->tail->next = newNode;
    queue->tail = newNode;
}

void *dequeue(Queue *queue)
{
    if (queue->head == NULL)
    {
        return NULL;
    }
    Node *temp = queue->head;
    void *value = temp->value;
    queue->head = queue->head->next;
    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }
    free(temp);
    return value;
}

void *front(Queue *queue)
{
    if (queue->head == NULL) return NULL;
    return queue->head->value;
}

void freeQueue(Queue *queue)
{
    while (queue->head != NULL)
    {
        dequeue(queue);
    }
    free(queue);
}

int main(){
    Queue *q = createQueue();
    int t = 1;
    int t2 = 2;
    enqueue(q, &t);
    enqueue(q, &t2);
    printf("%i\n", *((int *)front(q)));
    printf("%i\n", *((int *)dequeue(q)));
    printf("%i\n", *((int *)dequeue(q)));
    freeQueue(q);
    return 0;
}