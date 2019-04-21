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

    /*if (!png_sig_cmp(heder, 0, 8)){
        printf("ok\n");
    }*/
    
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    //if(!(image->png_ptr))printf("image->png_ptr read ERROR\n");
    
    image->info_ptr = png_create_info_struct(image->png_ptr);


   png_init_io(image->png_ptr, fp);
   /*
    * read png_struct
    */

   png_set_sig_bytes(image->png_ptr, 8);
   /*
    * If you had previously opened the file and read any 
    * of the signature from the beginning in order to see 
    * if this was a PNG file, you need to let libpng know 
    * that there are some bytes missing from the start of 
    * the file.
    */
    
    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);
    
    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    /*
     * количество строк
     */

    for (y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
    /* 
     * выделение памяти для хранения подписи строки
     */
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


    /* write header */


    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */

    png_write_image(image->png_ptr, image->row_pointers);

    png_write_end(image->png_ptr, NULL);

    /* cleanup heap allocation */
    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);

}

void process_file(struct png *image) {
    int x,y;

    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);

            
            /*printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                   x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

            /* set red value to 0 and green value to the blue one */

            if((x*x + y*y) < 100000){
                
                ptr[0] = 1;
                ptr[1] = 1;
                ptr[3] = 1;
            }
            
        }
    }
}

void doSquare(struct png* image, int Sx, int Sy, int Fx, int Fy)
{
    int x,y;

    printf("{x->%d, y->%d}\n", image->width-1, image->height-1);
	
	

    for(x = Sx-1; x <= Fx+1; x++){
        
        y = Sy-1;
        
        png_byte *row = image->row_pointers[y];
        png_byte *ptr = &(row[x*4]);
      
        ptr[0] = 255;
        ptr[1] = 0;
        ptr[2] = 0;
		ptr[3] = 255;
            
        row = image->row_pointers[Fy +1];
        ptr = &(row[x*4]);

		ptr[0] = 255;
        ptr[1] = 0;
        ptr[2] = 0;
		ptr[3] = 255;   
    }

    for(y = Sy; y <= Fy; y++){
        
            x = Sx-1;
            png_byte *row = image->row_pointers[y];
            png_byte *ptr = &(row[x*4]);
            
            ptr[0] = 255;
            ptr[1] = 0;
            ptr[2] = 0;
			ptr[3] = 255;
            
            row = image->row_pointers[y];
            ptr = &(row[(Fx+1)*4]);
			 
            ptr[0] = 255;
            ptr[1] = 0;
            ptr[2] = 0;
			ptr[3] = 255;
    }

    
}

void swapPixels(png_byte *ptr1, png_byte *ptr2){
    int buf[4];

    buf[0] = ptr2[0];
    buf[1] = ptr2[1];
    buf[2] = ptr2[2];
    buf[3] = ptr2[3];

    ptr2[0] = ptr1[0];
    ptr2[1] = ptr1[1];
    ptr2[2] = ptr1[2];
    ptr2[3] = ptr1[3];

    ptr1[0] = buf[0];
    ptr1[1] = buf[1];
    ptr1[2] = buf[2];
    ptr1[3] = buf[3];

}

void reflect(struct png *image, int Sx, int Sy, int Fx, int Fy, int XorY)
{
	int x,y;
     // 1 - oX, 0- oY
    if(XorY){
        int lenX = Fx - Sx + 1;
        for(x = Sx; x <= Sx +(lenX - 3)/2 + (lenX - 3)%2; x++){

            for(y = Sy; y <= Fy; y++){
            
                png_byte *row1 = image->row_pointers[y];
                png_byte *ptr1 = &(row1[x*4]);

                png_byte *row2 = image->row_pointers[y];
                png_byte *ptr2 = &(row2[(Fx - x + Sx)*4]);

                swapPixels(ptr1, ptr2);
            }
        }
   }
    else{
        int lenY = Fy - Sy + 1;
        for(y = Sy; y <= Sy +(lenY - 3)/2 + (lenY - 3)%2; y++){
            for(x = Sx; x <= Fx; x++){

                png_byte *row1 = image->row_pointers[y];
                png_byte *ptr1 = &(row1[x*4]);

                png_byte *row2 = image->row_pointers[Fy - y + Sy];
                png_byte *ptr2 = &(row2[(x)*4]);

                swapPixels(ptr1, ptr2);
            }
        }
    }
	

}

void setPixel(struct png *image, int x, int y){
   png_byte *row = image->row_pointers[y];
    png_byte *ptr = &(row[x*4]);
      
    ptr[0] = 255;
    ptr[1] = 255;
    ptr[2] = 255;
	ptr[3] = 255; 
}


void drawCircle(struct png* image, int Cx, int Cy, int R){

    
    int x,y;
    printf("R = %d\n", R);

    for(y = 0; y <= Cy + R; y++){
        
        
        for(x = 0; x <= Cx + R; x++){          
            
            

            if(x*x + y*y <= R*R){
               // printf("ok ");
                setPixel(image, Cx + x, Cy + y);
                setPixel(image, Cx - x, Cy - y);
                setPixel(image, Cx + x, Cy - y);
                setPixel(image, Cx - x, Cy + y);
            }


        }
    }



}

void doCircle(struct png *image, int x, int y, int r){

    int x1,y1,yk = 0;
    int sigma,delta,f;

    x1 = 0;
    y1 = r;
    delta = 2*(1-r);

        do
        {
            setPixel(image,x+x1,y+y1);
            setPixel(image,x-x1,y+y1);
            setPixel(image,x+x1,y-y1);
            setPixel(image,x-x1,y-y1);

            f = 0;
            if (y1 < yk)
                break;
            if (delta < 0)
            {
                        sigma = 2*(delta+y1)-1;
                        if (sigma <= 0)
                        {
                                x1++;
                                delta += 2*x1+1;
                                f = 1;
                        }
                }
                else
                if (delta > 0)
                {
                        sigma = 2*(delta-x1)-1;
                        if (sigma > 0)
                        {
                                y1--;
                                delta += 1-2*y1;
                                f = 1;
                        }
                }
                if (!f)
                {
                        x1++;
                        y1--;
                        delta += 2*(x1-y1-1);
                }
        }
        while(1);
}



int main(){


	int Sx = 400;
	int Sy = 400;
    int Fx = 600;
    int Fy = 600;

    int Cx = 500;
    int Cy = 500;
    int R = 100;
;
    int XorY = 1;
    struct png image;
    int i;
    openPNG(&image, FILE_NAME);
    //process_file(&image);
	
    
    
   
        
    doCircle(&image, Cx, Cy, R);
    doCircle(&image, Cx, Cy, R-1);
    doCircle(&image, Cx, Cy, R-2);
    doCircle(&image, Cx, Cy, R-3);

    //doSquare(&image, Sx, Sy, Fx, Fy);
    
    //reflect(&image, Sx, Sy, Fx, Fy, XorY);
    //doCopy(&image, Sx , Sy, Fx, Fy);
    doNewFile(&image, "newFile.png");
    
}
