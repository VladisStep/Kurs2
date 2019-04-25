#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <png.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>


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
    int width;
    png_byte color[4];
};


int openPNG(struct png* image, char* fileName)
{
    int x,y;
    char heder[8];

    FILE *fp = fopen(fileName, "rb");

    if(!fp) 
    {
        return 1;
    }

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
    return 0;
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


void drawCircle(struct png* image, int Sx, int Sy, int R, int width, png_byte *rgb)
{
    int Ex = Sx + R - 1;
    int Ey = Sy + R - 1;
    int i,j;

    for(i = 0; i <= R; i++)
    {
        for(j = 0; j <= R; j++){

            if( (i*i + j*j) <= R*R && (i*i + j*j) >= (R -width)*(R - width))
            {
               
                setPixel(image, Sx + i, Sy + j, rgb);
                setPixel(image, Sx + i, Sy - j, rgb);
                setPixel(image, Sx - i, Sy + j, rgb);
                setPixel(image, Sx - i, Sy - j, rgb);
            }
        }
    } 
    
}


void printHelp()
{
    printf("Справка\n");
    printf("--copy(-c) - делает копию заданного участка. Участок задается при помощи -S и -E.\n");
    printf("(Пример) --copy -S 100 200 -E 500 600 fileOut.png\n");
    printf("--reflect(-r) - отображает выбранный участок относительно оси. Участок задается при помощи -S и -E, ось задется при попщи -O\n");
    printf("(Пример) --reflect -S 100 200 -E 500 700 -O x fileOut.png\n");
    printf("--draw_circle(-d) - рисует окружность вписанную в квадарат. Квадарт задается при помощи координат левого верхнего угла -S и  радиуса окружности -R ");
    printf("*расчет координат правого нижнего угла делается автомотически*, так же выберается ширина окружности -W *если окружно должны быть залита, то параметры -W и -R должны совпадать* ");
    printf("цвет окржуносит можно выбрать при помощи -С.\n");
    printf("(Пример) --draw_circle -S 500 500 -R 100 -W 10 -C red fileOut.png");
    printf("--start(-S) - считывет координаты *целый числа* верхнего левого угла прямоугольной области (-С 32 23)\n");
    printf("--end(-E) - считывет координаты *целый числа* нижнего правого угла прямоугольной области (-E 32 23)\n");
    printf("--color(-C) - считывет цвет. Доступные цвета: red, green, blue, white, black*стоит по умолчанию*(-С red)\n");
    printf("--os(-O) - считывает ось x или ось y (-O x)\n");
    printf("--width(-W) - считывает значение ширины (-W 10)\n");
    printf("--radius(-R) - считывает заначение радиуса (-R 100)\n");
    printf("--info(-i) - выводит информацию о входном файле\n");
    printf("Строка команд должна заканчиваться названием файла вывода\n");

}


int isNum(char *n)
{
    int i;

    for(i = 0; i < strlen(n); i++)
    {
        if(isdigit(n[i]))
            continue;
        else 
            return 0;
    }

    return 1;
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

void randSquare(struct png* image, int N){
	srand(time(NULL));
	int maxA;
	
	if( image->width >= image->height)
		maxA = image->width;
	else
		maxA = image->height;
		
	
	int x, y, a;
	int i = 0;
	png_byte color[4];

	while(i < N){
	 	x = rand() %image->width;
		y = rand() %image->height;
		a = rand() %maxA;
		


		if(x + a < image->width &&
			y + a < image->height){
			color[0] = rand()%255; 
			color[1] = rand()%255;
			color[2] = rand()%255;
			color[3] = 255;
	
			for(int j = 0; j < a; j++){
				for( int k = 0; k < a; k++){
				setPixel(image, x+j , y+k, color);
					}
			}	

		i++;
		}
	


	
	} 
	
	
	


}


int main(int argc, char **argv){

    int flag = 0;
    struct png image;
    int i;

    struct args a;
    int longIndex = 0;
    int opt = 0;
    
    a.start[0] = -1;
    a.start[1] = -1;
    a.end[0] = -1;
    a.end[1] = -1;
    a.R = -1;
    a.Os = 0;
    a.width = 1;

    a.color[0] = 0;
    a.color[1] = 0;
    a.color[2] = 0;
    a.color[3] = 255;

    char *optstring = "drcsE:S:R:C:O:W:ih";

    if(openPNG(&image, argv[1]) || !strstr(argv[1], ".png"))
    {
        printf("Невозможно открыть файл\n");
        return 0;
    }
    
    struct option longOpts[] = {
        {"reflect", no_argument, &flag, 'r'},
        {"copy", no_argument, &flag, 'c'},
        {"draw_circle", no_argument, &flag, 'd'},
        {"help", required_argument, &flag, 'h'},
        {"start", required_argument, NULL, 'S'}, 
        {"end", required_argument, NULL, 'E'},
        {"color", required_argument, NULL, 'C'},
        {"os", required_argument, NULL, 'O'},
        {"width", required_argument, NULL, 'W'},
        {"radius", required_argument, NULL, 'R'},
        {"info", required_argument, &flag, 'i'},
		{"randSquare", no_argument, &flag, 's'},
        {NULL, 0, NULL, 0}
    };

    opt = getopt_long(argc, argv, optstring , longOpts, &longIndex);
    
    while( opt != -1 ) {
        switch( opt ) {
            case 'E':
                
                if(!isNum(optarg))
                {
                    printf("%s - это не целое число\n", optarg);
                    return 0;
                }
                
                if(!isNum(argv[optind]))
                {
                    printf("%s - это не целое число\n", argv[optind]);
                    return 0;
                }

                a.end[0] = atoi(optarg);
                a.end[1] = atoi(argv[optind]);
 
                break;
            
            case 'S':

                if(!isNum(optarg))
                {
                    printf("%s - это не целое число\n", optarg);
                    return 0;
                }
                if(!isNum(argv[optind]))
                {
                    printf("%s - это не целое число\n", argv[optind]);
                    return 0;
                }
              
                a.start[0] = atoi(optarg);
                a.start[1] = atoi(argv[optind]);
;
                break;

            case 'R':
                
                if(!isNum(optarg))
                {
                    printf("%s - это не целое число\n", optarg);
                    return 0;
                }
               
                a.R = atoi(optarg);
                
                break;
            
            case 'W':
                
                if(!isNum(optarg))
                {
                    printf("%s - это не целое число\n", optarg);
                    return 0;
                }
                
                a.width = atoi(optarg);
                
                break;

            case 'C':
                
                if(!strcmp("red", optarg))
                {
                    a.color[0] = 255;
                }
                else if(!strcmp("green", optarg))
                {
                    a.color[1] = 255;
                }
                else if(!strcmp("blue", optarg))
                {
                    a.color[3] = 255;
                }
                else if(!strcmp("white", optarg))
                {
                    a.color[0] = 255;
                    a.color[1] = 255;
                    a.color[2] = 255;
                }
                else if(!strcmp("black", optarg))
                {

                }
                else
                {
                    printf("Цвета %s  не существует",  optarg);
                }
                break;

            case 'O':

                if(!strcmp("x", optarg) || !strcmp("y", optarg))
                {
                    a.Os =  121 - *optarg;
                }
                else
                {
                    printf("Не верная ось координат\n");
                }
                
                break;
            
            case 'h':
                printHelp();
                break;
                
            case 'i':
                flag = 'i';
                break;

            case 'r':
                flag = 'r';
                break;
            
            case 'c':
                flag = 'c';
                break;
            
            case 'd':
                flag = 'd';
                break;
			
			case 's':
                flag = 's';
                break;
        }
           
        opt = getopt_long(argc, argv, optstring , longOpts, &longIndex);
        
    }

    switch(flag){
        case'r':

            if(a.start[0] < 0 || 
               a.start[1] < 0 || 
               a.end[0] > image.width || 
               a.end[1] > (image.height - 1) ||
               a.start[0] >= a.end[0] || 
               a.start[1] >= a.end[1] )
               {
                   printf("Область отражениея не является корректной\n");
                   break;
               }
            else
            {
                reflect(&image, a.start[0], a.start[1], a.end[0], a.end[1], a.Os);
            }
            
        break;

        case'c':

            if(a.start[0] < 0 || 
               a.start[1] < 0 || 
               a.end[0] > image.width || 
               a.end[1] > (image.height - 1) ||
               a.start[0] > a.end[0] || 
               a.start[1] > a.end[1])
               {
                   printf("Область копирования не является корректной\n");
                   break;
               }
            else
            {
                doCopy(&image, a.start[0], a.start[1], a.end[0], a.end[1]);
            }
            
        break;

        case'd':

            if(a.start[0] < 0 || 
               a.start[1] < 0 || 
               a.start[0] + a.R > image.width || 
               a.start[1] + a.R > (image.height - 1)||
               a.start[0] - a.R < 0 ||
               a.start[1] - a.R < 0 ||
               a.R <= 0 ||
               a.width > a.R ||
               a.width < 0)
               {
                   printf("Невозможно нарисовать круг по введенным данным\n");
                   break;
               }
            else
            {
                drawCircle(&image,a.start[0], a.start[1], a.R, a.width, a.color);
            }
            
        break;

        case 'h':
            printHelp();
        break;

        case 'i':
            printf("Width = %d\n", image.width);
            printf("Heigth = %d\n", image.height);
        break;
		
		case 's':
            randSquare(&image, a.R);
        break;
    }
	
  
    doNewFile(&image, argv[argc - 1]);

}
