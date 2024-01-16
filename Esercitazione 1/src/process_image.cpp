#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>

#include "image.h"

using namespace std;

// Converte un'immagine RGB in un'immagine in scala di 
// grigi.
// Parametri:
//   const Image& im: immagine di input
// Ritorna:
//   Image: l'immagine in scala di grigi risultante
Image rgb_to_grayscale(const Image& im)
  {
  assert(im.c == 3); // only accept RGB images
  Image gray(im.w,im.h,1); // create a new grayscale image (note: 1 channel)
  
  // TODO: calculate the pixels of 'gray'
  
    float luma;
  
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            
            luma = 0.299 * get_clamped_pixel(im, x, y, 0) + 0.587 * get_clamped_pixel(im, x, y, 1) + 0.114 * get_clamped_pixel(im, x, y, 2);
            
            set_pixel(gray, x, y, 0, luma);
            
        }
    }
  
    return gray;
  }



// Cambia il colore di un'immagine in scala di grigi.
// Parametri:
//   const Image& im: immagine di input
//   float r, g, b: valori RGB per il nuovo colore
// Ritorna:
//   Image: l'immagine RGB risultante
Image grayscale_to_rgb(const Image& im, float r, float g, float b)
  {
  assert(im.c == 1);
  Image rgb(im.w,im.h,3);
  
    for(int q2=0;q2<im.h;q2++) {
        for(int q1=0;q1<im.w;q1++) {
            rgb(q1,q2,0)=r*im(q1,q2);
            rgb(q1,q2,1)=g*im(q1,q2);
            rgb(q1,q2,2)=b*im(q1,q2);
        }
    }
  
  return rgb;
  }



// Sposta tutti i pixel nel canale specificato di un'immagine.
// Parametri:
//   Image& im: immagine di input da modificare
//   int c: quale canale spostare
//   float v: quanto spostare
// Ritorna:
//   void
void shift_image(Image& im, int c, float v)
  {
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  // TODO: shift all the pixels at the specified channel
  
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            set_pixel(im, x, y, c, get_clamped_pixel(im,x,y,c)+v);
        }
    }
  
  }

// Clampa tutti i pixel in tutti i canali per essere compresi tra 0 e 1.
// Parametri:
//   Image& im: immagine di input da modificare
// Ritorna:
//   void
void clamp_image(Image& im)
  {
  // TODO: clamp all the pixels in all channel to be between 0 and 1
  
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            for (int c=0; c < im.c; c++) {
            
                if (get_clamped_pixel(im,x,y,c) < 0) set_pixel(im, x, y, c, 0);
                else if (get_clamped_pixel(im,x,y,c) > 1) set_pixel(im, x, y, c, 1);
            }
        }
    }

  
  }

// These might be handy
float max(float a, float b, float c)
  {
  return max({a,b,c});
  }

float min(float a, float b, float c)
  {
  return min({a,b,c});
  }


// Converte tutti i pixel da formato RGB a formato HSV in un'immagine.
// Parametri:
//   Image& im: immagine di input da modificare
// Ritorna:
//   void
void rgb_to_hsv(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from RGB format to HSV format
  
    float H, S, V;
    float C;
    
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            float R, G, B;
            R = get_clamped_pixel(im,x,y,0);
            G = get_clamped_pixel(im,x,y,1);
            B = get_clamped_pixel(im,x,y,2);
            
            V = max(R, G, B);
            set_pixel(im, x, y, 2, V);
            
            if (V == 0) S = 0;
            else {
                C = V - min(R, G, B);
                S = C/V;
            }
            set_pixel(im, x, y, 1, S);
            
            float temp;
            if (C == 0) H = 0;
            else {
                if (V == R) temp = (G-B)/C;
                else if (V == G) temp = (B-R)/C + 2;
                else if (V == B) temp = (R-G)/C + 4;
                
                if (temp < 0) H = temp/6 + 1;
                else H = temp/6;
            }
            set_pixel(im, x, y, 0, H);
        }
    }
  
  }

// Converte tutti i pixel da formato HSV a formato RGB in un'immagine.
// Parametri:
//   Image& im: immagine di input da modificare
// Ritorna:
//   void
void hsv_to_rgb(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from HSV format to RGB format
  
    float C, X, m;
    float H, S, V;
    
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            H = get_clamped_pixel(im,x,y,0);
            S = get_clamped_pixel(im,x,y,1);
            V = get_clamped_pixel(im,x,y,2);
            
            C = V * S;
            X = C * (1 - fabs(fmod(6*H,2) - 1));
            m = V - C;
            
            float r, g, b;
            if (H >= 0 && H < (float)1/6) {
                r = C;
                g = X;
                b = 0;
            }
            else if (H >= (float)1/6 && H < (float)2/6) {
                r = X;
                g = C;
                b = 0;
            }
            else if (H >= (float)2/6 && H < (float)3/6) {
                r = 0;
                g = C;
                b = X;
            }
            else if (H >= (float)3/6 && H < (float)4/6) {
                r = 0;
                g = X;
                b = C;
            }
            else if (H >= (float)4/6 && H < (float)5/6) {
                r = X;
                g = 0;
                b = C;
            }
            else {
                r = C;
                g = 0;
                b = X;
            }
            set_pixel(im, x, y, 0, r+m);
            set_pixel(im, x, y, 1, g+m);
            set_pixel(im, x, y, 2, b+m);
        }
    }
      
  }

// Scala tutti i pixel nel canale specificato di un'immagine.
// Parametri:
//   Image& im: immagine di input da modificare
//   int c: quale canale scalare
//   float v: quanto scalare
// Ritorna:
//   void
void scale_image(Image& im, int c, float v)
  {
  assert(c>=0 && c<im.c); // needs to be a valid channel
  
  // TODO: scale all the pixels at the specified channel
  
    for (int y=0; y < im.h; y++) {
        for (int x=0; x < im.w; x++) {
            set_pixel(im, x, y, c, im.data[pixel_address(im,x,y,c)]*v);
        }
    }
  
  
  }

// HW0 #9
// Image& im: input image to be modified in-place
void rgb_to_lch(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from RGB format to LCH format
  
  
  NOT_IMPLEMENTED();
  
  }

// HW0 #9
// Image& im: input image to be modified in-place
void lch_to_rgb(Image& im)
  {
  assert(im.c==3 && "only works for 3-channels images");
  
  // TODO: Convert all pixels from LCH format to RGB format
  
  NOT_IMPLEMENTED();
  
  }



// Implementation of member functions
void Image::clamp(void) { clamp_image(*this); }
void Image::shift(int c, float v) { shift_image(*this,c,v); }
void Image::scale(int c, float v) { scale_image(*this,c,v); }

void Image::HSVtoRGB(void) { hsv_to_rgb(*this); }
void Image::RGBtoHSV(void) { rgb_to_hsv(*this); }
void Image::LCHtoRGB(void) { lch_to_rgb(*this); }
void Image::RGBtoLCH(void) { rgb_to_lch(*this); }
