#include "image.h"

// Restituisce l'indice 0-based di un valore di pixel nell'array di dati di un'immagine.
// Parametri:
//   const Image& im: immagine di input
//   int x, y: coordinate del pixel
//   int ch: canale di interesse
// Ritorna:
//   int: l'indice del pixel nell'array di dati
int pixel_address(const Image& im, int x, int y, int ch)
  {

  // TODO: calculate and return the index
    return im.w * y + x + (ch * im.w * im.h);
  
  }

// Restituisce il valore pixel clippato alle coordinate e al canale specificati in un'immagine.
// Parametri:
//   const Image& im: immagine di input
//   int x, y, ch: coordinate del pixel e canale di interesse
// Ritorna:
//   float: il valore pixel clippato

float get_clamped_pixel(const Image& im, int x, int y, int ch)
  {
  // TODO: clamp the coordinates and return the correct pixel value
  
    int my_x = x, my_y = y, my_ch = ch;
    
    if (x < 0) my_x = 0;
    else if (x >= im.w) my_x = im.w - 1;
    
    if (y < 0) my_y = 0;
    else if (y >= im.h) my_y = im.h - 1;
    
    if (ch < 0) my_ch = 0;
    else if (ch >= im.c) my_ch = im.c - 1;
  
    return (float) im.data[pixel_address(im, my_x, my_y, my_ch)];
  }

// Imposta il valore pixel alle coordinate e al canale specificati in un'immagine, se all'interno dei limiti.
// Parametri:
//   Image& im: immagine di input
//   int x, y, ch: coordinate del pixel e canale di interesse
//   float value: valore da assegnare al pixel
// Ritorna:
//   void
void set_pixel(Image& im, int x, int y, int c, float value)
  {
  // TODO: Only set the pixel to the value if it's inside the image
  
    if (x < 0 || y < 0 || c < 0 || x >= im.w || y >= im.h || c >= im.c ) return;
    
    im(x,y,c) = value;
  
  }



// Copia il contenuto di un'immagine in un'altra.
// Parametri:
//   Image& to: immagine di destinazione
//   const Image& from: immagine di origine
// Ritorna:
//   void
void copy_image(Image& to, const Image& from)
  {
  // allocating data for the new image
  to.data=(float*)calloc(from.w*from.h*from.c,sizeof(float));
  to.c=from.c;
  
  // TODO: populate the remaining fields in 'to' and copy the data
  // You might want to check how 'memcpy' function works

    
    to.w = from.w;
    to.h = from.h;
  
    memcpy(to.data, from.data, to.h * to.w * to.c * sizeof(float));
  
  }
