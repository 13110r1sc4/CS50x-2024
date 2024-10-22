from cs50 import get_int


def math(n_str):

    i = len(n_str) - 2
    m_1 = 0
    m_2 = 0

    while i >= 0:

        m1 = str(2 * int(n_str[i]))
        m2 = str(int(n_str[i+1]))
        m_1 += sum(list(int(x) for x in m1))
        m_2 += sum(list(int(x) for x in m2))
        if i == 1:
            m_2 += int(n_str[0])
        i -= 2

    if ((m_1 + m_2) % 10) == 0:

        return True


def card():

    while True:

        number = get_int("Number: ")
        n_str = str(number)

        if math(n_str):

            if n_str[0:2] in ["34", "37"] and len(n_str) == 15:
                print("AMEX\n")
                return False

            elif n_str[0:2] in ["51", "52", "53", "54", "55"] and len(n_str) == 16:
                print("MASTERCARD\n")
                return False

            elif n_str[0] in ["4"] and len(n_str) in [13, 16]:
                print("VISA\n")
                return False

            else:
                print("INVALID\n")
                return False

        else:
            print("INVALID\n")
            return False


card()
