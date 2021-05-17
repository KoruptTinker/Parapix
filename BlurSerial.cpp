#include "./headers/Parapix.h"
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<png.h>
#include<omp.h>
using namespace std;

void blurSerial(char inputName[],char outputName[]) 
{
    int width,height;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep *row_pointers = NULL;
    int maskDim=6;
    int maskMat[6][6]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    FILE *fp = fopen(inputName, "rb");
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) 
        abort();
    png_infop info = png_create_info_struct(png);
    if(!info) 
        abort();
    if(setjmp(png_jmpbuf(png))) 
        abort();
    png_init_io(png, fp);
    png_read_info(png, info);
    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBa format.
    if(bit_depth == 16)
        png_set_strip_16(png);
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth<8)
        png_set_expand_gray_1_2_4_to_8(png);
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);
    png_read_update_info(png, info);
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) 
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    png_read_image(png, row_pointers); 
    fclose(fp);
    png_destroy_read_struct(&png, &info, NULL);

    int size =  height * width;
    int maskD = maskDim * maskDim;
    int averageR,averageG,averageB;
    int *maskedImgR = (int*)malloc(size*sizeof(int));
    int *maskedImgG = (int*)malloc(size*sizeof(int));
    int *maskedImgB = (int*)malloc(size*sizeof(int));
    for(int i=0;i<size;i++)
    {
        maskedImgB[i] = 0;
        maskedImgG[i] = 0;
        maskedImgB[i] = 0;
    }
    int val=0;
    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            averageR = 0;
            averageG = 0;
            averageB = 0;
            for(int itImageX=i-maskDim/2,itMaskX=0;itImageX<=(i+maskDim/2)&&itMaskX<maskDim;itImageX++,itMaskX++)
            {
                if(itImageX>=0 && itImageX<=height)
                {
                    png_bytep row = row_pointers[itImageX];
                    for(int itImageY=j-maskDim/2,itMaskY=0;itImageY<=(j+maskDim/2 )&&itMaskY<maskDim;itImageY++,itMaskY++)
                    {
                        png_bytep px = &(row[itImageY * 4]);
                        if(itImageX>=0&&itImageX<height&&itImageY>=0&&itImageY<width)
                        {
                            averageR+=(px[0]*maskMat[itMaskX][itMaskY]);
                            averageG+=(px[1]*maskMat[itMaskX][itMaskY]);
                            averageB+=(px[2]*maskMat[itMaskX][itMaskY]);
                        }                
                    }
                }       
            }
            //storing Red
            if(averageR/maskD > 255)
                averageR = 255*maskD;
            else if(averageR/maskD < 0)
                averageR = 0;
            maskedImgR[i*width + j] = averageR/maskD;

            //storing green 
            if(averageG/maskD> 255)
                averageG = 255*maskD;
            else if(averageG/maskD< 0)
                averageG = 0;
            maskedImgG[i*width + j] = averageG/maskD;

            //storing blue
            if(averageB/maskD> 255)
                averageB = 255*maskD;
            else if(averageB/maskD< 0)
                averageB = 0;
            maskedImgB[i*width + j] = averageB/maskD;
        }
    }
    int x=0;
    //storing to the image
    for(int i=0;i<height;i++)
    { 
        png_bytep row = row_pointers[i];
        for(int j=0;j<width;j++)
        {
            png_bytep px = &(row[j * 4]);
            px[0] = maskedImgR[x];
            px[1] = maskedImgG[x];
            px[2] = maskedImgB[x];
            x++;
        }
    }

    
    fp = fopen(outputName, "wb");
    if (!fp)
        abort();
    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) 
        abort();
    info = png_create_info_struct(png);
    if (!info) 
        abort();
    if (setjmp(png_jmpbuf(png))) 
        abort();
    png_init_io(png, fp);

    // Output is 8bit depth, RGBa format.
    png_set_IHDR(png,info,width,height,8,PNG_COLOR_TYPE_RGBA,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    if (!row_pointers) 
        abort();
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);
    fclose(fp);
    png_destroy_write_struct(&png, &info);
    free(maskedImgR);
    free(maskedImgG);
    free(maskedImgB);
}

// int main() 
// {
//     cout<<"Enter the size of Mask:"<<endl;
//     cin>>maskDim;
//     double start = omp_get_wtime();
//     for(int i=1;i<=8;i++)
//     {
//         string str="D://Workplace//BlurImage//Testing//photo"+to_string(i)+".png";
//         char p[str.size()+1];
//         str.copy(p,str.size()+1);
//         p[str.size()]='\0';
//         string res="D://Workplace//BlurImage//Blurred//result"+to_string(i)+".png";
//         char s[res.size()+1];
//         res.copy(s,res.size()+1);
//         s[res.size()]='\0';
//         read_png_file(p);
//         process_png_file();
//         write_png_file(s);
//     }
//     free(maskMat);
//     double end = omp_get_wtime();
//     cout<<"Serial"<<endl;
//     cout<<"Time taken is "<<(end-start)<<endl;
//     system("pause");
//     return 0;
// }