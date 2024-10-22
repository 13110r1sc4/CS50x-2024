#include <cs50.h>
#include <stdio.h>

void brick(int b);

void no_brick(int nb);

int main(void)
{
    // Get size of grid
    int n;
    do
    {
        n = get_int("Height: ");
    }
    while (n < 1);

    for (int i = n - 1; 0 <= i; i--)
    {
        no_brick(i);
        brick(n - i);
        no_brick(2);
        brick(n - i);
        printf("\n");
    }
}

void brick(int b)
{
    while (b > 0)
    {
        printf("#");
        b--;
    }
}

void no_brick(int nb)
{
    while (nb > 0)
    {
        printf(" ");
        nb--;
    }
}
