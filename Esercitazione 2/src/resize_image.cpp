#include <cmath>
#include "image.h"

using namespace std;

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the nearest neighbor to pixel (x,y,c)
float Image::pixel_nearest(float x, float y, int c) const
  {
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)
  
  // TODO: Your code here
  
  return clamped_pixel(round(x),round(y),c);
  }

// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image nearest_resize(const Image& im, int w, int h)
  {
  Image ret(w,h,im.c);
  
  // TODO: Your code here
  
    float scale_x = (float) w/im.w;
    float scale_y = (float) h/im.h;
    
    for (int c=0; c<im.c; c++) {
        for (int y=0; y<h; y++) {
            float old_y = (float)(y + 0.5)/scale_y - 0.5;
            for (int x=0; x<w; x++) {
                float old_x = (float)(x + 0.5)/scale_x - 0.5;
                ret.set_pixel(x,y,c,im.pixel_nearest(old_x,old_y,c));
            }
        }
    }
  
  return ret;
  }

// HW1 #1
// float x,y: inexact coordinates
// int c: channel
// returns the bilinearly interpolated pixel (x,y,c)
float Image::pixel_bilinear(float x, float y, int c) const
  {
  // Since you are inside class Image you can
  // use the member function pixel(a,b,c)
  
  
  // TODO: Your code here
      
      int x_floor = floor(x), x_ceil = ceil(x);
      
      int y_floor = floor(y), y_ceil = ceil(y);
      
      float v1 = clamped_pixel(x_floor, y_floor, c);
      float v2 = clamped_pixel(x_ceil, y_floor, c);
      float v3 = clamped_pixel(x_floor, y_ceil, c);
      float v4 = clamped_pixel(x_ceil, y_ceil, c);
      
      float A4 = (x - x_floor) * (y - y_floor);
      float A3 = (x_ceil - x) * (y - y_floor);
      float A2 = (x - x_floor) * (y_ceil - y);
      float A1 = (x_ceil - x) * (y_ceil - y);
      
      return v1 * A1 + v2 * A2 + v3 * A3 + v4 * A4;
      
      /* IN ALTERNATIVA, uguale matematicamente parlando:
    
       float q1 = v1 * (x_ceil - x) + v2 * (x - x_floor);
       float q2 = v3 * (x_ceil - x) + v4 * (x - x_floor);
      
       return q1 * (y_ceil - y) + q2 * (y - y_floor);*/
  }

// HW1 #1
// int w,h: size of new image
// const Image& im: input image
// return new Image of size (w,h,im.c)
Image bilinear_resize(const Image& im, int w, int h)
  {
  
    Image ret(w,h,im.c);
    
  // TODO: Your code here
  
    float scale_x = (float) w/im.w;
    float scale_y = (float) h/im.h;
    
    for (int c=0; c<im.c; c++) {
        for (int y=0; y<h; y++) {
            float old_y = (float)(y + 0.5)/scale_y - 0.5;
            for (int x=0; x<w; x++) {
                float old_x = (float)(x + 0.5)/scale_x - 0.5;
                ret.set_pixel(x,y,c,im.pixel_bilinear(old_x,old_y,c));
            }
        }
    }
  
  return ret;
  }


