#include <cs50.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int count_letters(string text);

int count_words(string text);

int count_sentences(string text);

int main(void)
{
    string text = get_string("Text: ");

    int letters = count_letters(text);
    int words = count_words(text);
    int sentences = count_sentences(text);

    int index = round(0.0588 * ((float) letters / (float) words * 100) -
                      0.296 * ((float) sentences / (float) words * 100) - 15.8);

    if (index < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (index >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", index);
    }
}

int count_letters(string text)
{
    int count = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (isalpha(text[i]))
        {
            count += 1;
        }
    }
    return count;
}

int count_words(string text)
{
    int count = 1;
    for (int i = 0; i < strlen(text); i++)
    {
        if (isspace(text[i]))
        {
            count += 1;
        }
    }
    return count;
}

int count_sentences(string text)
{
    int count = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] == '.' || text[i] == '?' || text[i] == '!')
        {
            count += 1;
        }
    }
    return count;
}
