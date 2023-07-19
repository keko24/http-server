typedef struct node {
    char *key;
    char *value;
    struct node *next;
} Node;

typedef struct linkedList {
    Node *head;
    Node *tail;
    int size;
} LinkedList;

void initLinkedList(LinkedList *list);

void add(LinkedList *list, char *key, char *value);

void addAt(LinkedList *list, int index, char *key, char *value);

Node *find(LinkedList *list, int index);

void removeFirst(LinkedList *list);

void removeAt(LinkedList *list, int index);

void printLinkedList(LinkedList *list);
