#include <cs50.h>
#include <math.h>
#include <stdio.h>

int luhn_algo(long n);

int count(long n);

int amex_mast(long n);

int visa(long n);

int main(void)
{
    long n = get_long("Credit card number: ");

    if ((luhn_algo(n) % 10) == 0)
    {
        if ((visa(n) == 4) && (count(n) == 13 || count(n) == 16))
        {
            printf("VISA\n");
        }
        else if ((amex_mast(n) == 51 || amex_mast(n) == 52 || amex_mast(n) == 53 ||
                  amex_mast(n) == 54 || amex_mast(n) == 55) &&
                 (count(n) == 16))
        {
            printf("MASTERCARD\n");
        }
        else if ((amex_mast(n) == 34 || amex_mast(n) == 37) && (count(n) == 15))
        {
            printf("AMEX\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}

int luhn_algo(long n)
{
    long m = n / 10;
    int step = pow(10, 2);
    int sum = 0;

    while (m > 0)
    {
        int digit = (m % 10) * 2;

        while (digit > 0)
        {
            sum += digit % 10;
            digit /= 10;
        }

        m /= step;
    }

    while (n > 0)
    {
        sum += n % 10;
        n /= step;
    }
    return sum;
}

int count(long n)
{
    int i = 0;
    while (n > 0)
    {
        i++;
        n /= 10;
    }
    return i;
}

int amex_mast(long n)
{
    while (n >= 100)
    {
        n /= 10;
    }
    return n;
}

int visa(long n)
{
    while (n >= 10)
    {
        n /= 10;
    }
    return n;
}
