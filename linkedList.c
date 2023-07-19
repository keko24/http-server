#include "linkedList.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

void initLinkedList(LinkedList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void add(LinkedList *list, char *key, char *value) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    list->size++;
}

void addAt(LinkedList *list, int index, char *key, char *value) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        Node *temp = list->head;
        Node *prev = NULL;

        for (int i = 0; i < index; i++) {
            prev = temp;
            temp = temp->next;
        }

        if (prev == NULL) {
            newNode->next = list->head;
            list->head = newNode;
        } else {
            prev->next = newNode;
            newNode->next = temp;
        }

        if (temp == NULL) {
            list->tail = newNode;
        }
    }

    list->size++;
}

Node *find(LinkedList *list, int index) {
    if (list->head == NULL) {
        return NULL;
    }

    Node *temp = list->head;

    for (int i = 0; i < index; i++) {
        temp = temp->next;
    }

    return temp;
}

void removeFirst(LinkedList *list) {
    if (list->head == NULL) {
        return;
    }

    Node *temp = list->head;
    list->head = list->head->next;
    free(temp);

    list->size--;
}

void removeAt(LinkedList *list, int index) {
    if (list->head == NULL) {
        return;
    }

    Node *temp = list->head;
    Node *prev = NULL;

    for (int i = 0; i < index; i++) {
        prev = temp;
        temp = temp->next;
    }

    if (prev == NULL) {
        list->head = temp->next;
    } else {
        prev->next = temp->next;
    }

    if (temp->next == NULL) {
        list->tail = prev;
    }

    free(temp);
    list->size--;
}

void printLinkedList(LinkedList *list) {
    Node *temp = list->head;

    while (temp != NULL) {
        printf("%s -> %s ", temp->key, temp->value);
        temp = temp->next;
    }

    printf("\n");
}
