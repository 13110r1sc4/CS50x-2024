from cs50 import get_int


def main():

    while True:

        n = get_int("Height: ")

        if n not in range(1, 9):
            pass
        else:
            i = 1
            j = n - 1
            while i <= n and j >= 0:

                print(" " * j + "#" * i + "  " + "#" * i)
                i += 1
                j -= 1

            return False


main()
