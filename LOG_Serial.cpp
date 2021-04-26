//Including the header files

#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <omp.h>
#include <string>
#include <iostream>

int imageWidth, imageHeight, imageDimensions;

png_byte imageColorType;
png_byte imageBitDepth;
png_bytep *imageRowPointer = NULL;

void readImageFile(char filename[])
{

  //Opens the file as read only in binary format
  FILE *imageFile = fopen(filename, "rb");
  if (imageFile == NULL)
  {
    std::cout<<"Image File cannot be opened";
    return;
  }

  //png_create_read_struct :- allocates and initialize a png_struct structure for reading PNG File
  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
  {
    std::cout<<"Failed to create png struct";
    //Terminates the program immediately
    abort();
  }
  
  // png_create_info_struct :- allocates and initialize a png_info structure
  png_infop info = png_create_info_struct(png);
  if (!info)
  {
    std::cout<<"Failed to create info struct";
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

  //read the png image info
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

  //free the memory asscoiated with read png_struct
  png_destroy_read_struct(&png, &info, NULL);
}

void writeImageFile(char filename[])
{

  //Opens the file as write only in binary format
  FILE *imageFile = fopen(filename, "wb");
  if (imageFile == NULL){
    std::cout<<"Image File cannot be opened";
    abort();
  }

  //png_create_write_struct :- allocates and initialize a png_struct structure for writing into PNG File
  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png){
    std::cout<<"Failed to create png struct";
    abort();
  }

// png_create_info_struct :- allocates and initialize a png_info structure
  png_infop info = png_create_info_struct(png);
  if (!info){
    std::cout<<"Failed to create info struct";
    abort();
  }
  if (setjmp(png_jmpbuf(png)))
    abort();
 
 //Initialize input/output for PNG File
  png_init_io(png, imageFile);

 // sets image header information in info_ptr
  png_set_IHDR(
      png,
      info,
      imageWidth, imageHeight,
      8,
      PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  // write PNG info to file
  png_write_info(png, info);

  //write image data to file
  png_write_image(png, imageRowPointer);
  
  png_write_end(png, NULL);

  //Closes the opened file
  fclose(imageFile);

  //free the memory asscoiated with write png_struct
  png_destroy_write_struct(&png, &info);
}

void maskRGB()
{
  int maskDimensions = 5;
  int maskMatrix[5][5] = {0, 0, 1, 0, 0,
                          0, 1, 2, 1, 0,
                          1, 2, -16, 2, 1,
                          0, 1, 2, 1, 0,
                          0, 0, 1, 0, 0};
  int size = imageHeight * imageWidth;

  // The library function malloc is used to allocate a block of memory on the heap. 
  int *maskedImageRed = (int *)malloc(size * sizeof(int));
  int *maskedImageGreen = (int *)malloc(size * sizeof(int));
  int *maskedImageBlue = (int *)malloc(size * sizeof(int));

  for (int i = 0; i < size; i++)
  {
    maskedImageRed[i] = 0;
    maskedImageGreen[i] = 0;
    maskedImageBlue[i] = 0;
  }
  int averageRed = 0, averageGreen = 0, averageBlue = 0;
  int val = 0;
  for (int i = 0; i < imageHeight; i++)
  {

    for (int j = 0; j < imageWidth; j++)
    {
      averageRed = 0;
      averageGreen = 0;
      averageBlue = 0;

      for (int itImageX = i - maskDimensions / 2, itMaskX = 0; itImageX <= (i + maskDimensions / 2) && itMaskX < maskDimensions; itImageX++, itMaskX++)
      {
        if (itImageX >= 0 && itImageX <= imageHeight)
        {
          png_bytep row = imageRowPointer[itImageX];
          for (int itImageY = j - maskDimensions / 2, itMaskY = 0; itImageY <= (j + maskDimensions / 2) && itMaskY < maskDimensions; itImageY++, itMaskY++)
          {
            png_bytep px = &(row[itImageY * 4]);
            if (itImageX >= 0 && itImageX < imageHeight && itImageY >= 0 && itImageY < imageWidth)
            {
              averageRed += (px[0] * maskMatrix[itMaskX][itMaskY]);
              averageGreen += (px[1] * maskMatrix[itMaskX][itMaskY]);
              averageBlue += (px[2] * maskMatrix[itMaskX][itMaskY]);
            }
          }
        }
      }
      if (averageRed < 0)
        averageRed = 0;
      else if (averageRed > 255)
        averageRed = 255;
      maskedImageRed[i * imageWidth + j] += averageRed / 9;

      if (averageGreen < 0)
        averageGreen = 0;
      else if (averageGreen > 255)
        averageGreen = 255;
      maskedImageGreen[i * imageWidth + j] += averageGreen / 9;

      if (averageBlue < 0)
        averageBlue = 0;
      else if (averageBlue > 255)
        averageBlue = 255;
      maskedImageBlue[i * imageWidth + j] += averageBlue / 9;
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
  free(maskedImageRed);
  free(maskedImageGreen);
  free(maskedImageBlue);
}

void laplacianOfGaussian()
{
  maskRGB();
}

int main()
{
  double startTime = omp_get_wtime();
  char inputImage[] = "./Image.png";
  char outputImage[] = "./LOG_Serial_Result.png";

  readImageFile(inputImage);
  laplacianOfGaussian();
  writeImageFile(outputImage);
  double endTime = omp_get_wtime();
  printf("%lf", endTime - startTime);
  
  return 0;
}