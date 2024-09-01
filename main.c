#pragma warning(disable : 4996)

#include <stdio.h>
#include "HashSet.h"

void PrintHashSet(HashSet_S* set)
{
	puts("Hash Set:");
	if (!set)
	{
		puts("\t(null)");
	}
	else
	{
		printf("\tsize = %llu\n", set->size);
		printf("\tcapacity = %llu\n", set->capacity);
		puts("\tbuckets:");
		for (size_t i = 0; i < set->capacity; ++i)
		{
			printf("\t\tb. #%llu: ", i);
			for (Node_S* node = set->buckets[i].head; node; node = node->next)
			{
				printf("[%s] -> ", node->key);
			}
			puts("(null)");
		}
	}
}

void PrintMenu(void)
{
	puts("insert: i <key:string>");
	puts("search: s <key:string>");
	puts("remove: r <key:string>");
	puts("print:  p");
	puts("quit:   q");
	puts("menu:   m");
}

int main()
{
	HashSet_S hashSet;
	HashSet_Init(&hashSet);

	char key[100] = { 0 };

	PrintMenu();
	for (char ch = getchar(); ch != 'q' && ch != 'Q'; ch = getchar())
	{
		switch (ch)
		{
		case 'I':
		case 'i':
			if (scanf("%s", key) == 1)
			{
				printf("Inserting: \"%s\"\n", key);
				HashSet_Insert(&hashSet, key);
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'S':
		case 's':
			if (scanf("%s", key) == 1)
			{
				printf("Searching for: %s\n", key);
				Boolean_T value = FALSE;
				if (HashSet_Search(&hashSet, key, &value) == HASHSET_SUCCESS)
				{
					printf(value ? "Present" : "Absent");
				}
				else
				{
					puts("Searching error!");
				}
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'R':
		case 'r':
			if (scanf("%s", key) == 1)
			{
				printf("Removing: %s\n", key);
				HashSet_Remove(&hashSet, key);
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'P':
		case 'p':
			PrintHashSet(&hashSet);
			break;
		case 'M':
		case 'm':
			PrintMenu();
			break;
		case 'C':
		case 'c':
			HashSet_DeInit(&hashSet);
			HashSet_Init(&hashSet);
			break;
		}
	}

	HashSet_DeInit(&hashSet);

	return 0;
}