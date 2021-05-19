//Including the header files

#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <omp.h>
#include <string>
#include <iostream>
#include "./headers/Parapix.h"

void LOGParallel(char filename[], char outputFileName[])
{

  int imageWidth, imageHeight, imageDimensions;
  png_byte imageColorType;
  png_byte imageBitDepth;
  png_bytep *imageRowPointer = NULL;

  fflush(stdin);

  //Opens the file as read only in binary format
  FILE *imageFile = fopen(filename, "rb");
  //If fopen failed to open the file, it will return null
  if (imageFile == NULL)
  {
    return;
  }

  //png_create_read_struct :- allocates and initialize a png_struct structure for reading PNG File
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
  {
    //Terminates the program immediately
    abort();
  }
  //png_create_info_struct :- allocates and initialize a png_info structure
  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    //Terminates the program immediately
    abort();
  }

  if (setjmp(png_jmpbuf(png)))
  {
    //Terminates the program immediately
    abort();
  }

  //Initialize input/output for png file
  png_init_io(png, imageFile);

  //Read the png image info
  png_read_info(png, info);

  imageWidth = png_get_image_width(png, info);
  imageHeight = png_get_image_height(png, info);

  imageColorType = png_get_color_type(png, info);
  imageBitDepth = png_get_bit_depth(png, info);

  imageDimensions = imageHeight;
  if (imageBitDepth == 16)
    png_set_strip_16(png);

  if (imageColorType == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  if (imageColorType == PNG_COLOR_TYPE_GRAY && imageBitDepth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if (imageColorType == PNG_COLOR_TYPE_RGB || imageColorType == PNG_COLOR_TYPE_GRAY || imageColorType == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if (imageColorType == PNG_COLOR_TYPE_GRAY || imageColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  imageRowPointer = (png_bytep *)malloc(sizeof(png_bytep) * imageHeight);
  for (int y = 0; y < imageHeight; y++)
  {
    imageRowPointer[y] = (png_byte *)malloc(png_get_rowbytes(png, info));
  }

  png_read_image(png, imageRowPointer);

  //Close the opened file
  fclose(imageFile);
  //Free the memory asscoiated with read png_struct
  png_destroy_read_struct(&png, &info, NULL);

  int averageR, averageG, averageB;
  int size = imageHeight * imageWidth;

  //The library function malloc is used to allocate a block of memory on the heap.
  int *maskedImageRed = (int *)malloc(size * sizeof(int));
  int *maskedImageGreen = (int *)malloc(size * sizeof(int));
  int *maskedImageBlue = (int *)malloc(size * sizeof(int));

  for (int i = 0; i < size; i++)
  {
    maskedImageRed[i] = 0;
    maskedImageGreen[i] = 0;
    maskedImageRed[i] = 0;
  }

  int val = 0;
  int maskDimensions = 5;
  int maskMatrix[5][5] = {0, 0, 1, 0, 0,
                          0, 1, 2, 1, 0,
                          1, 2, -16, 2, 1,
                          0, 1, 2, 1, 0,
                          0, 0, 1, 0, 0};

  int i, j, ImageX, ImageY, MaskX, MaskY;
#pragma omp parallel shared(maskedImageBlue) private(i, j, ImageX, ImageY, MaskX, MaskY, averageR, averageB, averageG)
  {
#pragma omp for schedule(static)

    for (i = 0; i < imageHeight; i++)
    {

      for (j = 0; j < imageWidth; j++)
      {
        averageR = 0;
        averageG = 0;
        averageB = 0;
        for (ImageX = i - maskDimensions / 2, MaskX = 0; ImageX <= (i + maskDimensions / 2) && MaskX < maskDimensions; ImageX++, MaskX++)
        {
          if (ImageX != -1)
          {
            png_bytep row = imageRowPointer[ImageX];
            for (ImageY = j - maskDimensions / 2, MaskY = 0; ImageY <= (j + maskDimensions / 2) && MaskY < maskDimensions; ImageY++, MaskY++)
            {
              png_bytep px = &(row[ImageY * 4]);
              if (ImageX >= 0 && ImageX < imageHeight && ImageY >= 0 && ImageY < imageWidth)
              {
                averageR += (px[0] * maskMatrix[MaskX][MaskY]);
                averageG += (px[1] * maskMatrix[MaskX][MaskY]);
                averageB += (px[2] * maskMatrix[MaskX][MaskY]);
              }
            }
          }
        }
        if (averageR < 0)
          averageR = 0;
        else if (averageR > 255)
          averageR = 255;
        maskedImageRed[i * imageWidth + j] += averageR / 9;

        if (averageG < 0)
          averageG = 0;
        else if (averageG > 255)
          averageG = 255;
        maskedImageGreen[i * imageWidth + j] += averageG / 9;

        if (averageB < 0)
          averageB = 0;
        else if (averageB > 255)
          averageB = 255;
        maskedImageBlue[i * imageWidth + j] += averageB / 9;
      }
    }
  }

  int x = 0;
  for (int i = 0; i < imageHeight; i++)
  {
    png_bytep row = imageRowPointer[i];
    for (int j = 0; j < imageWidth; j++)
    {
      png_bytep px = &(row[j * 4]);
      px[0] = maskedImageRed[x];
      px[1] = maskedImageGreen[x];
      px[2] = maskedImageBlue[x];
      x++;
    }
  }

  //Opens the file as write only in binary format
  imageFile = fopen(outputFileName, "wb");
  if (!imageFile)
    abort();

  //png_create_write_struct :- allocates and initialize a png_struct structure for writing into PNG File
  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
  {
    std::cout << "Failed to create png struct";
    //Terminates the program immediately
    abort();
  }

  info = png_create_info_struct(png);
  if (!info)
  {
    std::cout << "Failed to create info struct";
    //Terminates the program immediately
    abort();
  }

  if (setjmp(png_jmpbuf(png)))
    abort();

  //Initialize input/output for png file
  png_init_io(png, imageFile);

  //Sets image header information in info_ptr
  png_set_IHDR(
      png,
      info,
      imageWidth, imageWidth,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  //Write PNG info to file
  png_write_info(png, info);

  //Write image data to file
  png_write_image(png, imageRowPointer);
  png_write_end(png, NULL);

  free(maskedImageRed);
  free(maskedImageGreen);
  free(maskedImageBlue);
  for (int y = 0; y < imageHeight; y++)
  {
    free(imageRowPointer[y]);
  }
  free(imageRowPointer);
  fclose(imageFile);

  png_destroy_write_struct(&png, &info);
}