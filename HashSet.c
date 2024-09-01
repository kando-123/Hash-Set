#include "HashSet.h"

#include <stdlib.h>
#include <string.h>

#define NODE_SUCCESS 0
#define NODE_FAILURE 1

/* ---------- Node ---------- */

int Node_Init(Node_S* node, const String_T key)
{
	if (node)
	{
		size_t length = strlen(key) + 1;
		String_T copy = malloc(length);
		if (copy)
		{
			memcpy(copy, key, length);
			node->key = copy;
			node->next = NULL;
			return NODE_SUCCESS;
		}
	}
	return NODE_FAILURE;
}

void Node_DeInit(Node_S* node)
{
	if (node)
	{
		Node_DeInit(node->next);
		free(node->next);
		free(node->key);
	}
}

/* ---------- Bucket ---------- */

void Bucket_Clear(Bucket_S* bucket)
{
	Node_DeInit(bucket->head);
}

/* ---------- Table ---------- */

#define INITIAL_CAPACITY 8

int HashSet_Init(HashSet_S* set)
{
	if (!set)
	{
		return HASHSET_FAILURE;
	}

	set->size = 0;
	set->capacity = INITIAL_CAPACITY;
	set->buckets = calloc(set->capacity, sizeof(Bucket_S));

	return set->buckets ? HASHSET_SUCCESS : HASHSET_FAILURE;
}

int HashSet_DeInit(HashSet_S* set)
{
	if (!set)
	{
		return HASHSET_FAILURE;
	}

	for (size_t i = 0; i < set->capacity; ++i)
	{
		Bucket_Clear(&set->buckets[i]);
	}
	free(set->buckets);
	set->buckets = NULL;
	set->capacity = 0;
	set->size = 0;

	return HASHSET_SUCCESS;
}

size_t Hash(String_T key)
{
	size_t sum = 0;
	for (size_t factor = 3; *key; ++key, factor *= factor)
	{
		sum += (size_t)(*key) * factor;
	}
	return sum;
}

int Rehash(HashSet_S* set)
{
	/* Prepare the array for the new buckets. */
	size_t newCapacity = set->capacity << 1;
	Bucket_S* newBuckets = calloc(newCapacity, sizeof(Bucket_S));
	if (!newBuckets)
	{
		return HASHSET_FAILURE;
	}

	/* Transfer the nodes by re-pinning them. */
	for (size_t i = 0; i < set->capacity; ++i)
	{
		Node_S* head = set->buckets[i].head;
		while (head)
		{
			/* Extract the first node in the list. */
			Node_S* node = head;
			head = head->next;

			/* Re-pin the node. */
			size_t newIndex = Hash(node->key) & (newCapacity - 1);
			node->next = newBuckets[newIndex].head;
			newBuckets[newIndex].head = node;
		}
	}

	/* Release the previous array of pointers. */
	free(set->buckets);
	set->buckets = newBuckets;
	set->capacity = newCapacity;

	return HASHSET_SUCCESS;
}

int Dehash(HashSet_S* set)
{
	/* Prepare the array for the new buckets. */
	size_t newCapacity = set->capacity >> 1;
	Bucket_S* newBuckets = calloc(newCapacity, sizeof(Bucket_S));
	if (!newBuckets)
	{
		return HASHSET_FAILURE;
	}

	/* Transfer the nodes by re-pinning them. */
	for (size_t i = 0; i < set->capacity; ++i)
	{
		Node_S* head = set->buckets[i].head;
		while (head)
		{
			/* Extract the first node in the list. */
			Node_S* node = head;
			head = head->next;

			/* Re-pin the node. */
			size_t newIndex = Hash(node->key) & (newCapacity - 1);
			node->next = newBuckets[newIndex].head;
			newBuckets[newIndex].head = node;
		}
	}

	/* Release the previous array of pointers. */
	free(set->buckets);
	set->buckets = newBuckets;
	set->capacity = newCapacity;

	return HASHSET_SUCCESS;
}

#define REHASH_FACTOR 2.0
#define DEHASH_FACTOR 0.5

double LoadFactor(HashSet_S* set)
{
	return (double)set->size / (double)set->capacity;
}

int HashSet_Insert(HashSet_S* set, const String_T key)
{
	if (!set || !key)
	{
		return HASHSET_FAILURE;
	}

	if (LoadFactor(set) >= REHASH_FACTOR)
	{
		if (Rehash(set) != HASHSET_SUCCESS)
		{
			return HASHSET_FAILURE;
		}
	}

	size_t index = Hash(key) & (set->capacity - 1);
	if (set->buckets[index].head)
	{
		Node_S* node = set->buckets[index].head;
		Node_S* prev = NULL;
		while (node)
		{
			if (!strcmp(node->key, key))
			{
				return HASHSET_SUCCESS;
			}
			prev = node;
			node = node->next;
		}
		node = malloc(sizeof(Node_S));
		if (node)
		{
			Node_Init(node, key);
			prev->next = node;
			++set->size;
			return HASHSET_SUCCESS;
		}
		else
		{
			return HASHSET_FAILURE;
		}
	}
	else
	{
		Node_S* node = malloc(sizeof(Node_S));
		if (node)
		{
			Node_Init(node, key);
			set->buckets[index].head = node;
			++set->size;
			return HASHSET_SUCCESS;
		}
		else
		{
			return HASHSET_FAILURE;
		}
	}
}

int HashSet_Search(HashSet_S* set, const String_T key, int* value)
{
	if (!set || !key || !value)
	{
		return HASHSET_FAILURE;
	}

	size_t index = Hash(key) & (set->capacity - 1);
	Node_S* node = set->buckets[index].head;
	while (node)
	{
		if (!strcmp(node->key, key))
		{
			*value = TRUE;
			return HASHSET_SUCCESS;
		}
		node = node->next;
	}
	*value = FALSE;
	return HASHSET_SUCCESS;
}

int HashSet_Remove(HashSet_S* set, const String_T key)
{
	if (!set || !key)
	{
		return HASHSET_FAILURE;
	}

	size_t index = Hash(key) & (set->capacity - 1);
	Node_S* node = set->buckets[index].head;
	Node_S* prev = NULL;
	while (node)
	{
		if (!strcmp(node->key, key))
		{
			if (prev)
			{
				prev->next = node->next;
			}
			else
			{
				set->buckets[index].head = node->next;
			}
			node->next = NULL;
			Node_DeInit(node);
			free(node);
			--set->size;
			if (LoadFactor(set) <= DEHASH_FACTOR && set->capacity > INITIAL_CAPACITY)
			{
				Dehash(set);
			}
			return HASHSET_SUCCESS;
		}
		prev = node;
		node = node->next;
	}
	return HASHSET_FAILURE;
}
