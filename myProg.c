#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>
#include <unistd.h>
#include <getopt.h>

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

struct args{
    int isReflect;
    int start[2];
    int end[2];
    int R;
    int Os;
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

void swapPixels(png_byte *ptr1, png_byte *ptr2)
{
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

void setPixel(struct png *image, int x, int y, char *rgb)
{
   png_byte *row = image->row_pointers[y];
    png_byte *ptr = &(row[x*4]);
      
    ptr[0] = rgb[0];
    ptr[1] = rgb[1];
    ptr[2] = rgb[2];
	ptr[3] = rgb[3]; 
}

void drawCircle(struct png* image, int Cx, int Cy, int R, char *rgb)
{
    int x1 = 0;
    int y1 = R;
    int delta1 = 1 - 2 * R;
    int error1 = 0;
   
    int x2 = 0;
    int y2 = R-500;
    int delta2 = 1 - 2 * (R-500);
    int error2 = 0;
   
    while (y1 >= 0 )
    {     
        
        setPixel(image,Cx + x1, Cy + y1, rgb);
        setPixel(image,Cx + x1, Cy - y1, rgb);
        setPixel(image,Cx - x1, Cy + y1, rgb);
        setPixel(image,Cx - x1, Cy - y1, rgb);
       
        for(int i = x2; i <= x1 ; i++)
        {
            for(int j = y2; j <= y1; j++)
            {
                setPixel(image, Cx + i, Cy + j, rgb);
                setPixel(image, Cx + i, Cy - j, rgb);
                setPixel(image, Cx - i, Cy + j, rgb);
                setPixel(image, Cx - i, Cy - j, rgb);
            }
                  
        }
       
        error1 = 2 * (delta1 + y1) - 1;
       
        if ((delta1 < 0) && (error1 <= 0))
        {
            delta1 += 2 * ++x1 + 1;
            goto next;
        }
       
        if ((delta1 > 0) && (error1 > 0))
        {
            delta1 -= 2 * --y1 + 1;
            goto next;
        }
       
        delta1 += 2 * (++x1 - y1--);

        next:
            
        if( y2 >= 0){
            error2 = 2 * (delta2 + y2) - 1;
       
            if ((delta2 < 0) && (error2 <= 0)){
                delta2 += 2 * ++x2 + 1;
                continue;
            }
       
            if ((delta2 > 0) && (error2 > 0))
            {
                delta2 -= 2 * --y2 + 1;
                continue;
            }

            delta2 += 2 * (++x2 - y2--);

        }

    }
      
}

void doCopy(struct png *image, int Sx, int Sy, int Fx, int Fy)
{
    int x, y;
    int lenX = Fx - Sx + 1;
    int lenY = Fy - Sy + 1;
    
    png_bytep *new_row_pointers =(png_bytep *) malloc(sizeof(png_bytep) * lenY);
     
    for(y = 0; y < lenY; y++)
    {
        new_row_pointers[y] = (png_byte *)malloc(sizeof(png_byte)*4 * lenX); 

        png_byte *torow = new_row_pointers[y]; 
        png_byte *row = image->row_pointers[Sy + y]; 

        for (x = 0; x < lenX; x++)
        {
            png_byte *toptr = &(torow[x*4]); 
            png_byte *ptr = &(row[((Sx+x)*4)]);

            toptr[0] = ptr[0];
            toptr[1] = ptr[1]; 
            toptr[2] = ptr[2];
            toptr[3] = ptr[3];
        }
    }

    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    image->row_pointers = new_row_pointers;
    image->width = lenX; 
    image->height = lenY;
}

int main(int argc, char **argv){


	
    int flag= 0;
    struct png image;
    int i;

    struct args a;
    int longIndex = 0;
    int opt = 0;
    
    a.start[0] = -1;
    a.start[1] = -1;
    a.end[0] = -1;
    a.end[1] = -1;
    a.R = 0;
    a.Os = 0;

    char *optstring = "rE:S:R:C:O:";
    
    struct option longOpts[] = {
        {"reflect", no_argument, &flag, 'r'},
        {"start", required_argument, NULL, 'S'}, 
        {"end", required_argument, NULL, 'E'},
        {"color", required_argument, NULL, 'C'},
        {"os", required_argument, NULL, 'O'},
        {"radius", required_argument, NULL, 'R'}
    };
   
    
    
    opt = getopt_long(argc, argv, optstring , longOpts, &longIndex);
    i = optind;
    while( opt != -1 ) {
        switch( opt ) {
            case 'E':
                
                a.end[0] = atoi(optarg);
                a.end[1] = atoi(argv[optind]);
                
                
                break;
            
            case 'S':
              
                a.start[0] = atoi(optarg);
                a.start[1] = atoi(argv[optind]);
                printf("%d\n", a.start[1]);
                break;
            case 'R':
                
                printf("R = %s\n", optarg);

                break;

            case 'C':
                
                printf("C = %s\n", optarg);

                break;

            case 'O':
                
                
                a.Os = 121 - *optarg;
                break;
   
            default:
                /* сюда попасть невозможно. */
                break;
        }
        
        
        opt = getopt_long(argc, argv, optstring , longOpts, &longIndex);
        
    }
    

    openPNG(&image, FILE_NAME);

    switch(flag){
        case'r':

            if(a.start[0] < 0 || a.start[1] < 0 || 
               a.end[0] > image.width || a.end[1] > (image.height - 1) ||
               a.start[0] >= a.end[0] || a.start[1] >= a.end[1] )
               {
                   printf("ERROR\n");
                   break;
               }
            else
            {
                reflect(&image, a.start[0], a.start[1], a.end[0], a.end[1], a.Os);
            }
            
        break;
    }


    
    //drawCircle(&image, Cx, Cy, R, rgb);
    //doSquare(&image, Sx, Sy, Fx, Fy);  
    
   // doCopy(&image, Sx , Sy, Fx, Fy);
  
    doNewFile(&image, argv[argc - 1]);
    
}
