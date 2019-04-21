#include <stdio.h>
#include <stdlib.h>

#include <png.h>

#define FILE_NAME "spongebob.png"

struct png{

    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
};

void openPNG(struct png* image, char* fileName)
{

    int x,y;
    char heder[8];

    FILE *fp = fopen(fileName, "rb");

    if(!fp) printf("file is not opend\n");

    fread(heder, 1, 8, fp);

    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    image->info_ptr = png_create_info_struct(image->png_ptr);


   png_init_io(image->png_ptr, fp);

   png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);
    
    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);


    for (y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);
}

void doNewFile(struct png* image, char* fileName)
{

    int x, y;

    FILE *fp = fopen(fileName, "wb");
    
    if(!fp) printf("file is not open {doNewFile}\n");

    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);


    image->info_ptr = png_create_info_struct(image->png_ptr);

    png_init_io(image->png_ptr, fp);

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);

    png_write_image(image->png_ptr, image->row_pointers);

    png_write_end(image->png_ptr, NULL);

    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);

}



void doCopy(struct png *image, int Sx, int Sy, int Fx, int Fy)
{

    int x, y;
    int lenX = Fx - Sx + 1;
    int lenY = Fy - Sy + 1;

    printf("lenX = %d  lenY = %d\n", lenX, lenY);
    
    png_bytep *new_row_pointers =(png_bytep *) malloc(sizeof(png_bytep) * lenY); // выделили память для строк

    for(y = 0; y < lenY; y++){
        new_row_pointers[y] = (png_byte *)malloc(sizeof(png_byte *) * lenX); // выделили память под пиксели в строке

        png_byte *torow = new_row_pointers[y]; // начало записываемой области
        png_byte *row = image->row_pointers[Sy + y]; // начало считываемой области из файла

        for (x = 0; x < lenX; x++){
            
            
            png_byte *toptr = &(torow[x*4]); // выбор пикселей
            png_byte *ptr = &(row[(Sx+x*4)]);


           /* toptr[0] = ptr[0];
            toptr[1] = ptr[1]; // при копировании пикселей вылетает ошибка
            toptr[2] = ptr[2];
            toptr[3] = ptr[3];*/

            ptr[0] = 0;
            ptr[1] = 255;      // закрашивание области на картинке работает
            ptr[2] = 255;
            ptr[3] = 255;

            /*toptr[0] = 0;
            toptr[1] = 255;   // рисование области однотонным цветом в новом
            toptr[2] = 255;   // файле тоже работает
            toptr[3] = 255;*/

            
        
        }
       /* image->row_pointers = new_row_pointers; // запись нового массива пикселей 
        image->width = lenX; // измнеие длины и шиирины картинки
        image->height = lenY;*/

    }
}


int main(){


	int Sx = 100;
	int Sy = 100;
    
    int Fx = 2000;
    int Fy = 1000;

    
    struct png image;
    
    openPNG(&image, FILE_NAME);
    doCopy(&image, Sx , Sy, Fx, Fy);
    doNewFile(&image, "newFile.png");
    
}
