from cs50 import get_string


def count_letters(text):

    count = 0
    i = 0
    while i < len(text):

        if text[i].isalpha():
            count += 1

        i += 1

    return count


def count_words(text):

    count = 1
    i = 0
    while i < len(text):

        if text[i].isspace():
            count += 1

        i += 1

    return count


def count_sentences(text):

    count = 0
    i = 0
    while i < len(text):

        if text[i] in [".", "?", "!"]:

            count += 1

        i += 1

    return count


text = get_string("Text: ")

letters = count_letters(text)
words = count_words(text)
sentences = count_sentences(text)

index = round(0.0588 * (float(letters) / float(words) * 100) -
              0.296 * (float(sentences) / float(words) * 100) - 15.8)

if index < 1:

    print("Before Grade 1")

elif index >= 16:

    print("Grade 16+")

else:

    print(f"Grade {index}")
