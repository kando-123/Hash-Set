#pragma once

typedef char* String_T;
typedef int Boolean_T;

#define TRUE  1
#define FALSE 0

#define HASHSET_SUCCESS 0
#define HASHSET_FAILURE 1

/* ---------- Node ---------- */

struct Node
{
	String_T key;

	struct Node* next;
};

typedef struct Node Node_S;

/* ---------- Bucket ---------- */

struct Bucket
{
	Node_S* head;
};

typedef struct Bucket Bucket_S;

/* ---------- Table ---------- */

struct HashSet
{
	size_t size;
	size_t capacity;

	Bucket_S* buckets;

};

typedef struct HashSet HashSet_S;

int HashSet_Init(HashSet_S* table);
int HashSet_DeInit(HashSet_S* table);
		
int HashSet_Insert(HashSet_S* table, const String_T key);
int HashSet_Search(HashSet_S* table, const String_T key, Boolean_T* value);
int HashSet_Remove(HashSet_S* table, const String_T key);
