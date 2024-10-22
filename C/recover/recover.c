#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define block_size 512
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover FILE\n");
        return 1;
    }

    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("Could not open file %s.\n", argv[1]);
        return 1;
    }

    uint8_t buffer[block_size];
    FILE *img = NULL;
    char filename[8];
    int count = 0;
    while (fread(buffer, 1, block_size, card) == block_size)
    {
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            sprintf(filename, "%03i.jpg", count);

            if (count == 0)
            {
                img = fopen(filename, "w");
                fwrite(buffer, 1, block_size, img);
            }
            else
            {
                fclose(img);
                img = fopen(filename, "w");
                fwrite(buffer, 1, block_size, img);
            }
            count++;
        }
        else
        {
            if (img != NULL)
            {
                fwrite(buffer, 1, block_size, img);
            }
        }
    }
    if (img != NULL)
    {
        fclose(img);
    }
    fclose(card);
    return 0;
}
