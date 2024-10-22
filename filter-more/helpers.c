#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int grey = round(((float)image[i][j].rgbtRed + (float)image[i][j].rgbtBlue + (float)image[i][j].rgbtGreen) / 3);
            image[i][j].rgbtRed = grey;
            image[i][j].rgbtBlue = grey;
            image[i][j].rgbtGreen = grey;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE buffer[width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            buffer[j].rgbtRed = image[i][width - 1 - j].rgbtRed;
            buffer[j].rgbtBlue = image[i][width - 1 - j].rgbtBlue;
            buffer[j].rgbtGreen = image[i][width - 1 - j].rgbtGreen;
        }
        for (int j = 0; j < width; j++)
        {
            image[i][j] = buffer[j];
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE buffer[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == 0)
            {
                if (j == 0)
                {
                    // corner up left
                    buffer[i][j].rgbtRed = round((image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j+1].rgbtRed) / 4.0);
                    buffer[i][j].rgbtBlue = round((image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j+1].rgbtBlue) / 4.0);
                    buffer[i][j].rgbtGreen = round((image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j+1].rgbtGreen) / 4.0);
                }
                else if (j == width - 1)
                {
                    // corner up right
                    buffer[i][j].rgbtRed = round((image[i][j].rgbtRed + image[i][j-1].rgbtRed + image[i+1][j-1].rgbtRed + image[i+1][j].rgbtRed) / 4.0);
                    buffer[i][j].rgbtBlue = round((image[i][j].rgbtBlue + image[i][j-1].rgbtBlue + image[i+1][j-1].rgbtBlue + image[i+1][j].rgbtBlue) / 4.0);
                    buffer[i][j].rgbtGreen = round((image[i][j].rgbtGreen + image[i][j-1].rgbtGreen + image[i+1][j-1].rgbtGreen + image[i+1][j].rgbtGreen) / 4.0);
                }
                else
                {
                    // upper edge
                    buffer[i][j].rgbtRed = round((image[i][j-1].rgbtRed + image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i+1][j-1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j+1].rgbtRed) / 6.0);
                    buffer[i][j].rgbtBlue = round((image[i][j-1].rgbtBlue + image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i+1][j-1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j+1].rgbtBlue) / 6.0);
                    buffer[i][j].rgbtGreen = round((image[i][j-1].rgbtGreen + image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i+1][j-1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j+1].rgbtGreen) / 6.0);
                }
            }
            else if (i == height - 1)
            {
                if (j == 0)
                {
                    // corner low left
                    buffer[i][j].rgbtRed = round((image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i-1][j].rgbtRed + image[i-1][j+1].rgbtRed) / 4.0);
                    buffer[i][j].rgbtBlue = round((image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i-1][j].rgbtBlue + image[i-1][j+1].rgbtBlue) / 4.0);
                    buffer[i][j].rgbtGreen = round((image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i-1][j].rgbtGreen + image[i-1][j+1].rgbtGreen) / 4.0);
                }
                else if (j == width - 1)
                {
                    // corner low right
                    buffer[i][j].rgbtRed = round((image[i][j].rgbtRed + image[i][j-1].rgbtRed + image[i-1][j].rgbtRed + image[i-1][j-1].rgbtRed) / 4.0);
                    buffer[i][j].rgbtBlue = round((image[i][j].rgbtBlue + image[i][j-1].rgbtBlue + image[i-1][j].rgbtBlue + image[i-1][j-1].rgbtBlue) / 4.0);
                    buffer[i][j].rgbtGreen = round((image[i][j].rgbtGreen + image[i][j-1].rgbtGreen + image[i-1][j].rgbtGreen + image[i-1][j-1].rgbtGreen) / 4.0);
                }
                else
                {
                    // lower edge
                    buffer[i][j].rgbtRed = round((image[i][j].rgbtRed + image[i][j-1].rgbtRed + image[i][j+1].rgbtRed + image[i-1][j].rgbtRed + image[i-1][j-1].rgbtRed + image[i-1][j+1].rgbtRed) / 6.0);
                    buffer[i][j].rgbtBlue = round((image[i][j].rgbtBlue + image[i][j-1].rgbtBlue + image[i][j+1].rgbtBlue + image[i-1][j].rgbtBlue + image[i-1][j-1].rgbtBlue + image[i-1][j+1].rgbtBlue) / 6.0);
                    buffer[i][j].rgbtGreen = round((image[i][j].rgbtGreen + image[i][j-1].rgbtGreen + image[i][j+1].rgbtGreen + image[i-1][j].rgbtGreen + image[i-1][j-1].rgbtGreen + image[i-1][j+1].rgbtGreen) / 6.0);
                }
            }
            else
            {
                if (j == 0)
                {
                    // left edge
                    buffer[i][j].rgbtRed = round((image[i-1][j].rgbtRed + image[i-1][j+1].rgbtRed + image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j+1].rgbtRed) / 6.0);
                    buffer[i][j].rgbtBlue = round((image[i-1][j].rgbtBlue + image[i-1][j+1].rgbtBlue + image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j+1].rgbtBlue) / 6.0);
                    buffer[i][j].rgbtGreen = round((image[i-1][j].rgbtGreen + image[i-1][j+1].rgbtGreen + image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j+1].rgbtGreen) / 6.0);
                }
                else if (j == width - 1)
                {
                    // right edge
                    buffer[i][j].rgbtRed = round((image[i-1][j].rgbtRed + image[i-1][j-1].rgbtRed + image[i][j].rgbtRed + image[i][j-1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j-1].rgbtRed) / 6.0);
                    buffer[i][j].rgbtBlue = round((image[i-1][j].rgbtBlue + image[i-1][j-1].rgbtBlue + image[i][j].rgbtBlue + image[i][j-1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j-1].rgbtBlue) / 6.0);
                    buffer[i][j].rgbtGreen = round((image[i-1][j].rgbtGreen + image[i-1][j-1].rgbtGreen + image[i][j].rgbtGreen + image[i][j-1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j-1].rgbtGreen) / 6.0);
                }
                else
                {
                    // stnd
                    buffer[i][j].rgbtRed = round((image[i-1][j-1].rgbtRed + image[i-1][j].rgbtRed + image[i-1][j+1].rgbtRed + image[i][j-1].rgbtRed + image[i][j].rgbtRed + image[i][j+1].rgbtRed + image[i+1][j-1].rgbtRed + image[i+1][j].rgbtRed + image[i+1][j+1].rgbtRed) / 9.0);
                    buffer[i][j].rgbtBlue = round((image[i-1][j-1].rgbtBlue + image[i-1][j].rgbtBlue + image[i-1][j+1].rgbtBlue + image[i][j-1].rgbtBlue + image[i][j].rgbtBlue + image[i][j+1].rgbtBlue + image[i+1][j-1].rgbtBlue + image[i+1][j].rgbtBlue + image[i+1][j+1].rgbtBlue) / 9.0);
                    buffer[i][j].rgbtGreen = round((image[i-1][j-1].rgbtGreen + image[i-1][j].rgbtGreen + image[i-1][j+1].rgbtGreen + image[i][j-1].rgbtGreen + image[i][j].rgbtGreen + image[i][j+1].rgbtGreen + image[i+1][j-1].rgbtGreen + image[i+1][j].rgbtGreen + image[i+1][j+1].rgbtGreen) / 9.0);
                }
            }
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = buffer[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE buffer[height][width];
    RGBTRIPLE neighbours[3][3];
    typedef struct
    {
        int  rgbtBlue;
        int  rgbtGreen;
        int  rgbtRed;
    }
    RGBTRIPLEINT;

    RGBTRIPLEINT x;
    RGBTRIPLEINT y;

    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    neighbours[k][l].rgbtBlue  = 0;
                    neighbours[k][l].rgbtGreen = 0;
                    neighbours[k][l].rgbtRed  = 0;
                }
            }

            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    int ni = i + k;
                    int nj = j + l;

                    if (ni >= 0 && ni < height && nj >= 0 && nj < width)
                    {
                        neighbours[k + 1][l + 1] = image[ni][nj];
                    }
                }
            }

            x.rgbtBlue  = 0;
            x.rgbtGreen = 0;
            x.rgbtRed   = 0;
            y.rgbtBlue  = 0;
            y.rgbtGreen = 0;
            y.rgbtRed   = 0;

            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    x.rgbtBlue  += Gx[k][l] * neighbours[k][l].rgbtBlue;
                    x.rgbtGreen += Gx[k][l] * neighbours[k][l].rgbtGreen;
                    x.rgbtRed   += Gx[k][l] * neighbours[k][l].rgbtRed;

                    y.rgbtBlue  += Gy[k][l] * neighbours[k][l].rgbtBlue;
                    y.rgbtGreen += Gy[k][l] * neighbours[k][l].rgbtGreen;
                    y.rgbtRed   += Gy[k][l] * neighbours[k][l].rgbtRed;
                }
            }

            int B = round(sqrt(x.rgbtBlue * x.rgbtBlue + y.rgbtBlue * y.rgbtBlue));
            int G = round(sqrt(x.rgbtGreen * x.rgbtGreen + y.rgbtGreen * y.rgbtGreen));
            int R = round(sqrt(x.rgbtRed * x.rgbtRed + y.rgbtRed * y.rgbtRed));

            buffer[i][j].rgbtBlue = (B > 255) ? 255 : B;
            buffer[i][j].rgbtGreen = (G > 255) ? 255 : G;
            buffer[i][j].rgbtRed = (R > 255) ? 255 : R;
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = buffer[i][j];
        }
    }
    return;
}
