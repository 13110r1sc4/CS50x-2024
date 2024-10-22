// Implements a dictionary's functionality
#include "dictionary.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

unsigned int count_words_in_file(const char *dictionary);
bool isPrime(int n);
node **table = NULL;

bool check(const char *word)
{
    int l = strlen(word);
    char tolower_w[l + 1];
    for (int i = 0; i < l; i++)
    {
        tolower_w[i] = tolower(word[i]);
    }
    tolower_w[l] = '\0';

    if (table[hash(tolower_w)] != NULL)
    {
        node *current = table[hash(tolower_w)];
        while (current != NULL)
        {
            if (strcmp(current->word, tolower_w) == 0)
            {
                return true; // Word found
            }
            current = current->next;
        }
    }
    return false;
}

unsigned long N;
unsigned int hash(const char *word)
{
    unsigned int h = 0;
    int length = strlen(word);

    for (int i = 0; i < length; i++)
    {
        unsigned int ascii_value = (unsigned long long int) tolower(word[i]);
        h = (h * 31 + ascii_value) % N; // 1000000007
    }
    return h;
}

unsigned int count_words_in_file(const char *dictionary)
{
    unsigned int count = 0;
    FILE *source = fopen(dictionary, "r");
    if (source == NULL)
    {
        printf("Error reading dictionary\n");
        return 1;
    }
    char word[LENGTH + 1];
    while (fscanf(source, "%s", word) != EOF)
    {
        count++;
    }
    fclose(source);
    return count;
}

bool isPrime(int n)
{
    if (n <= 1)
    {
        return false;
    }
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

int count;
bool load(const char *dictionary)
{
    count = count_words_in_file(dictionary);
    const float load_factor = 0.5;
    N = round(count / load_factor);
    unsigned long next = N;
    bool isP = true;
    while (isP == true)
    {
        if (isPrime(next))
        {
            isP = false;
        }
        next++;
    }
    N = next;

    if (table != NULL)
    {
        free(table);
    }

    table = (node **) malloc(N * sizeof(node *));
    if (table == NULL)
    {
        return false;
    }
    for (unsigned long i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    FILE *source = fopen(dictionary, "r");
    if (source == NULL)
    {
        printf("Error reading dictionary\n");
        return false;
    }
    else
    {
        char word[LENGTH + 1];
        while (fscanf(source, "%s", word) != EOF)
        {
            node *new_node = malloc(sizeof(node));
            if (new_node == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                return 1;
            }

            strcpy(new_node->word, word);
            new_node->next = NULL;
            unsigned int hash_value = hash(word);
            if (hash_value >= N)
            {
                fprintf(stderr, "Hash value out of bounds\n");
                return false;
            }
            if (table[hash_value] == NULL)
            {
                table[hash_value] = new_node;
            }
            else
            {
                node *current = table[hash_value];
                while (current->next != NULL)
                {
                    current = current->next;
                }
                current->next = new_node;
            }
        }
    }
    fclose(source);
    return true;
}

unsigned int size(void)
{

    return count;
}

bool unload(void)
{
    if (table == NULL)
    {
        return false;
    }
    // TODO
    for (int i = 0; i < N; i++)
    {
        node *current = table[i];
        while (current != NULL)
        {
            node *to_free = current;
            current = current->next;
            free(to_free);
        }
    }
    free(table);
    return true;
}
