/*void doCopy(struct png *image, int Sx, int Sy, int Fx, int Fy){

    int x, y;
    int lenX = Fx - Sx + 1;
    int lenY = Fy - Sy + 1;

    printf("lenX = %d  lenY = %d\n", lenX, lenY);
    
    png_bytep *new_row_pointers =(png_bytep *) malloc(sizeof(png_bytep) * lenY);

    for(y = 0; y < lenY; y++){
        new_row_pointers[y] = (png_byte *)malloc(sizeof(png_byte *)*lenX);

        png_byte *torow = new_row_pointers[y];
        png_byte *row = image->row_pointers[Sy + y];

        for (x = 0; x < lenX; x++){
            
            png_byte *toptr = &(torow[x*4]);
            png_byte *ptr = &(row[(Sx + x)*4]);


            toptr[0] = ptr[0];
            toptr[1] = ptr[1];
            toptr[2] = ptr[2];
            toptr[3] = ptr[3];

            
        
        }
        image->row_pointers = new_row_pointers;
        image->width = lenX;
        image->height = lenY;

    }
}


void doCircle(struct png *image, int x0, int y0, int radius){
    int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while(y >= 0) {
		setPixel(image, x0 + x, y0 + y);
		setPixel(image, x0 + x, y0 - y);
		setPixel(image, x0 - x, y0 + y);
		setPixel(image, x0 - x, y0 - y);
		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
    
}*/

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


void doCopy(struct png* image, int Sx, int Sy, int Fx, int Fy){

    int x, y;

    int lenY = Fy - Sy +1;
    int lenX = Fx - Sx +1;

    png_bytep *new_row_pointers = (png_bytep *)calloc(sizeof(png_bytep), lenY);

    for(y = 0; y < lenY; y++)
    {
       
        new_row_pointers[y] = (png_byte *)calloc(sizeof(png_byte*), lenX + (4 - lenX/4));
        png_byte* new_row = new_row_pointers[y];
        png_byte* row = image->row_pointers[Sy + y];

        for(x = 0; x < lenX; x++)
        {
            
            png_byte *new_ptr = &(new_row[x*4]);
            png_byte *ptr = &(row[(Sx + x)*4]);

           new_ptr[3] = 245;
            new_ptr[2] = 145;
            new_ptr[1] = 95;
            new_ptr[0] = 48;

           /* ptr[0] = ptr[1];
            ptr[1] = ptr[2];
            ptr[2] = ptr[3];*/
            //printf("{%d = %d} ", x*4, (Sx + x)*4);
            
        }

       // image->row_pointers = new_row_pointers;
        image->width = lenX;

        //image->info_ptr->width = lenX;

        image->height = lenY;
    }
}