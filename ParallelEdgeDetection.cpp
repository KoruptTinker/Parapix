#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include<omp.h>
#include<string.h>
#include <iostream>

void edgeDetection(char filename[],char output[]){

    int imageWidth;
    int imageHeight;
    int imageDimensions;

    png_byte imageCType;
    png_byte imageBDepth;
    png_bytep *imageRows = NULL;

    fflush(stdin);
    FILE *image = fopen(filename, "rb");

    if(image==NULL){
        std::cout<<"Failed to open file";
        return ;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) {
        std::cout<<"Failed to create PNG struct";
        abort();
    }
   
    png_infop info = png_create_info_struct(png);
    if(!info){
        std::cout<<"Failed to create info struct";
        abort();
    } 

    if(setjmp(png_jmpbuf(png))){
        abort();
    }    

    png_init_io(png, image);

    png_read_info(png, info);

    imageWidth=png_get_image_width(png, info);
    imageHeight=png_get_image_height(png, info);
    
    imageCType = png_get_color_type(png, info);
    imageBDepth  = png_get_bit_depth(png, info);

    imageDimensions = imageHeight;

    if(imageBDepth == 16){
      png_set_strip_16(png);
    }

    if(imageCType == PNG_COLOR_TYPE_PALETTE){
      png_set_palette_to_rgb(png);
    }

    if(imageCType == PNG_COLOR_TYPE_GRAY && imageBDepth < 8){
      png_set_expand_gray_1_2_4_to_8(png);
    }

    if(png_get_valid(png, info, PNG_INFO_tRNS)){
      png_set_tRNS_to_alpha(png);
    }

    if(imageCType == PNG_COLOR_TYPE_RGB || imageCType == PNG_COLOR_TYPE_GRAY || imageCType == PNG_COLOR_TYPE_PALETTE){
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }

    if(imageCType == PNG_COLOR_TYPE_GRAY || imageCType == PNG_COLOR_TYPE_GRAY_ALPHA){
      png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    imageRows = (png_bytep*)malloc(sizeof(png_bytep) * imageHeight);

    for(int y = 0; y < imageHeight; y++){
      imageRows[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, imageRows);
   
    fclose(image);

    png_destroy_read_struct(&png, &info, NULL);


    int maskFilter[3][3]={-1,-2,-1,0,0,0,1,2,1};
    
    int averageRed;
    int averageGreen;
    int averageBlue;

    int size  = imageHeight * imageWidth;

    int *maskedImageRed = (int*)malloc(size*sizeof(int));
    int *maskedImageGreen = (int*)malloc(size*sizeof(int));
    int *maskedImageBlue = (int*)malloc(size*sizeof(int));
 
    for(int i=0;i<size;i++){
      maskedImageRed[i]=0;
      maskedImageGreen[i]=0;
      maskedImageBlue[i]=0;
    }

    int val=0;
    int maskDimensions=3;
    for(int i=0 ; i < imageHeight; i++){
            for(int j=0 ; j < imageWidth ; j++){
                averageRed = 0;
                averageGreen = 0;
                averageBlue = 0;
                for(int ImageX = i-1, MaskX = 0; ImageX < (i+maskDimensions) && MaskX < maskDimensions ;ImageX++,MaskX++){
                    if(ImageX!=-1){
                        png_bytep row = imageRows[ImageX];
                        for(int ImageY = j-1, MaskY =0; ImageY < (j+maskDimensions) && MaskY < maskDimensions; ImageY++,MaskY++){
                            png_bytep pixel = &(row[ImageY * 4]);
                            if(ImageX >= 0 && ImageX < imageHeight && ImageY >= 0 && ImageY < imageWidth){
                                averageRed+= (pixel[0] * maskFilter[MaskX][MaskY]);
                                averageGreen += (pixel[1] * maskFilter[MaskX][MaskY]);
                                averageBlue += (pixel[2] * maskFilter[MaskX][MaskY]);
                            }      
                        }
                    }
                }
            if(averageRed < 0){
                averageRed = 0;
            }
            else if(averageRed > 255){
                averageRed = 255;
            }
            maskedImageRed[i*imageWidth + j]+= averageRed/9;

            if(averageGreen < 0)
                averageGreen = 0;
            else if(averageGreen > 255)
                averageGreen = 255;
            maskedImageGreen[i*imageWidth + j] += averageGreen/9;

            if(averageBlue < 0){
                averageBlue = 0;
            }
            else if(averageBlue > 255){
                averageBlue = 255;
            }
            maskedImageBlue[i*imageWidth + j] += averageBlue/9;

          }
    }
    
    val=0;
    maskFilter[0][0] = -1;
    maskFilter[0][1] = 0;
    maskFilter[0][0] = 1;
    maskFilter[1][0] = -2;
    maskFilter[1][1] = 0;
    maskFilter[1][2] = 2;
    maskFilter[2][0] = -1;
    maskFilter[2][1] = 0;
    maskFilter[2][2] = 1;
    
    for(int i=0 ; i < imageHeight; i++){
        for(int j=0 ; j < imageWidth ; j++){
            averageRed = 0;
            averageGreen = 0;
            averageBlue = 0;
            for(int ImageX = i-1, MaskX = 0; ImageX < (i+maskDimensions) && MaskX < maskDimensions ;ImageX++,MaskX++){
                if(ImageX!=-1){
                    png_bytep row = imageRows[ImageX];
                    for(int ImageY = j-1, MaskY =0; ImageY < (j+maskDimensions) && MaskY < maskDimensions; ImageY++,MaskY++){
                        png_bytep pixel = &(row[ImageY * 4]);
                        if(ImageX >= 0 && ImageX < imageHeight && ImageY >= 0 && ImageY < imageWidth){
                            averageRed += (pixel[0] * maskFilter[MaskX][MaskY]);
                            averageGreen += (pixel[1] * maskFilter[MaskX][MaskY]);
                            averageBlue += (pixel[2] * maskFilter[MaskX][MaskY]);
                        }
                    }
                }
            }
            if(averageRed < 0){
                averageRed = 0;
            }
            else if(averageRed > 255){
                averageRed = 255;
            }
            maskedImageRed[i*imageWidth + j]+= averageRed/9;

            if(averageGreen < 0){
                averageGreen = 0;
            }
            else if(averageGreen > 255){
                averageGreen = 255;
            }
            maskedImageGreen[i*imageWidth + j] += averageGreen/9;

            if(averageBlue < 0){
                averageBlue = 0;
            }
            else if(averageBlue > 255){
                averageBlue = 255;
            }
            maskedImageBlue[i*imageWidth + j] += averageBlue/9;
        }
    }
    val=0;

    maskFilter[0][0] = -1;
    maskFilter[0][1] = -1;
    maskFilter[0][0] = -1;
    maskFilter[1][0] = -1;
    maskFilter[1][1] =  8;
    maskFilter[1][2] = -1;
    maskFilter[2][0] = -1;
    maskFilter[2][1] = -1;
    maskFilter[2][2] = -1;
    
    for(int i=0 ; i < imageHeight; i++){
        for(int j=0 ; j < imageWidth ; j++){
            averageRed = 0;
            averageGreen = 0;
            averageBlue = 0;
            for(int ImageX = i-1, MaskX = 0; ImageX < (i+maskDimensions) && MaskX < maskDimensions ;ImageX++,MaskX++){
                if(ImageX!=-1){
                    png_bytep row = imageRows[ImageX];
                    for(int ImageY = j-1, MaskY =0; ImageY < (j+maskDimensions) && MaskY < maskDimensions; ImageY++,MaskY++){
                        png_bytep pixel = &(row[ImageY * 4]);
                        if(ImageX >= 0 && ImageX < imageHeight && ImageY >= 0 && ImageY < imageWidth){
                            averageRed += (pixel[0] * maskFilter[MaskX][MaskY]);
                            averageGreen += (pixel[1] * maskFilter[MaskX][MaskY]);
                            averageBlue += (pixel[2] * maskFilter[MaskX][MaskY]);
                        }
                    }
                }
            }
            if(averageRed < 0){
                averageRed = 0;
            }
            else if(averageRed > 255){
                averageRed= 255;
            }
            maskedImageRed[i*imageWidth + j]+= averageRed/9;

            if(averageGreen < 0){
                averageGreen = 0;
            }
            else if(averageGreen > 255){
                averageGreen = 255;
            }
            maskedImageGreen[i*imageWidth + j] += averageGreen/9;

            if(averageBlue < 0){
                averageBlue = 0;
            }
            else if(averageBlue > 255){
                averageBlue = 255;
            }
            maskedImageBlue[i*imageWidth + j] += averageBlue/9;
        }
    }

    int x=0;
    for(int i=0;i<imageHeight;i++){ 
        png_bytep row = imageRows[i];
        for(int j=0;j<imageWidth;j++){
            png_bytep pixel = &(row[j * 4]);
            pixel[0] = maskedImageRed[x];
            pixel[1] = maskedImageGreen[x];
            pixel[2] = maskedImageBlue[x];
            x++;
        }
    }

    image = fopen(output, "wb");
    if(!image){ 
        std::cout<<"Failed to create output image";
        abort();
    }

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if(!png){
        std::cout<<"Failed to create PNG Write struct";
        abort();
    }

    info = png_create_info_struct(png);

    if(!info){
        std::cout<<"Failed to create info struct";
        abort();
    }

    if(setjmp(png_jmpbuf(png))){
        abort();
    }

  png_init_io(png, image);

  png_set_IHDR(
    png,
    info,
    imageWidth, imageHeight,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );

    png_write_info(png, info);
    png_write_image(png, imageRows);
    png_write_end(png, NULL);

    fclose(image);

    png_destroy_write_struct(&png, &info);
}

int main() {
    char fileName[]="./photo.png";
    char output[]="./output.png";
    edgeDetection(fileName,output);
    return 0;
}