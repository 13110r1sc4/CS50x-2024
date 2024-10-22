#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int check_validity(string key);

char encrypt(char letter, string key);

int main(int argc, string argv[])
{
    if (argc == 2)
    {
        int validity = check_validity(argv[1]);

        if (validity == 0)
        {
            string pt = get_string("plaintext: ");

            printf("ciphertext: ");

            for (int i = 0; i < strlen(pt); i++)
            {
                printf("%c", encrypt(pt[i], argv[1]));
            }

            printf("\n");
        }
        else if (validity == 1)
        {
            printf("Key not valid\n");
            return 1;
        }
    }
    else
    {
        printf("Wrong number of command-line arguments\n");
        return 1;
    }
}

int check_validity(string key)
{
    if (strlen(key) != 26)
    {
        return 1;
    }

    // int count = 0;

    for (int i = 0; i < strlen(key); i++)
    {
        if (isalpha(key[i]))
        {
            for (int j = i + 1; j < strlen(key); j++)
            {
                if (tolower(key[i]) == tolower(key[j]))
                {
                    return 1;
                }
                else
                {
                    continue;
                }
            }
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

char encrypt(char letter, string key)
{
    if (isalpha(letter))
    {
        if (isupper(letter))
        {
            return toupper(key[letter - 'A']);
        }
        else
        {
            return tolower(key[letter - 'a']);
        }
    }
    else
    {
        return letter;
    }
}
