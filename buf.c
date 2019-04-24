void drawCircle(struct png* image, int Cx, int Cy, int R, int width, png_byte *rgb)
{
    int x1 = 0;
    int y1 = R;
    int delta1 = 1 - 2 * R;
    int error1 = 0;
   
    width = (width%2 == 0?width:width-1);
    printf("%d\n", width);
    int x2 = 0;
    int y2 = R-width;
    int delta2 = 1 - 2 * (R - width);
    int error2 = 0;
    
    
   
    while (y1 >= 0)
    {         
        setPixel(image,Cx + x1, Cy + y1, rgb);
        setPixel(image,Cx + x1, Cy - y1, rgb);
        setPixel(image,Cx - x1, Cy + y1, rgb);
        setPixel(image,Cx - x1, Cy - y1, rgb);

        setPixel(image,Cx + x2, Cy + y2, rgb);
        setPixel(image,Cx + x2, Cy - y2, rgb);
        setPixel(image,Cx - x2, Cy + y2, rgb);
        setPixel(image,Cx - x2, Cy - y2, rgb);
       
        for(int i = x1; i >= x2; i--)
        {
            for(int j = y1; j >= y2; j--)
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