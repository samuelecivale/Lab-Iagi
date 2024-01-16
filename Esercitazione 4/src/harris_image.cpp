#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include "image.h"
//#include "matrix.h"

using namespace std;


// crea il descrittore di un pixel
Descriptor describe_index(const Image& im, int x, int y, int w)
{
  Descriptor d;
  d.p = {(double)x,(double)y};
  d.data.reserve(w * w * im.c);
  
  for(int c = 0; c < im.c; c++)
  {
    float cval = im.clamped_pixel(x, y, c);
    for(int dx = -w/2; dx <= w/2; dx++) 
      for(int dy = -w/2; dy <= w/2; dy++)
      d.data.push_back(im.clamped_pixel(x + dx, y + dy, c) - cval);
  }
  return d;
}

// Marks the spot of a point in an image.
void mark_spot(Image& im, const Point& p)
{
  int x = p.x;
  int y = p.y;
  
  for(int i = -9; i < 10; ++i)
  {
    im.set_pixel(x+i, y, 0, 1);
    im.set_pixel(x, y+i, 0, 1);
    im.set_pixel(x+i, y, 1, 0);
    im.set_pixel(x, y+i, 1, 0);
    im.set_pixel(x+i, y, 2, 1);
    im.set_pixel(x, y+i, 2, 1);
  }
}

// Marks corners denoted by an array of descriptors.
Image mark_corners(const Image& im, const vector<Descriptor>& d)
{
  Image im2=im;
  for(auto&e1:d)mark_spot(im2,e1.p);
  return im2;
}


// HW5 1.1b
Image make_1d_gaussian(float sigma)
{
  int dimension = ceil(6 * sigma);
  if (dimension % 2 == 0)
    dimension++;

  Image im(dimension, 1);

  float multiplier = 1.0 / (sqrt(2.0 * M_PI) * sigma);
  for (int x = 0; x < dimension; x++) 
  {
    float k = x - (dimension / 2);
    float exponent = -1.0 * pow(k, 2) / (2 * pow(sigma, 2));
    im(x, 0, 0) = multiplier * exp(exponent);
  }
  return im;
  
}

// HW5 1.1b
Image smooth_image(const Image& im, float sigma)
{
  Image filter = make_1d_gaussian(sigma);

  Image im1 = convolve_image(im, filter, true);

  swap(filter.h, filter.w);

  Image im2 = convolve_image(im1, filter, true);

  return im2;
}


// HW5 1.1 q
Image structure_matrix(const Image& im2, float sigma)
{
 
  // only grayscale or rgb
  assert((im2.c==1 || im2.c==3) && "only grayscale or rgb supported");
  Image im;
  // convert to grayscale if necessary
  if(im2.c==1)im=im2;
  else im=rgb_to_grayscale(im2);
  
  // Matrice di struttura, dove ogni canale conterrà una misura specifica della struttura dell'immagine.
  Image S(im.w, im.h, 3);
 
  Image Ix_im = convolve_image(im, make_gx_filter(), false);
  Image Iy_im = convolve_image(im, make_gy_filter(), false);
  
  //Passa le derivate di Ix e Iy in due matrici
  Matrix Ix(im.w, im.h);
  Matrix Iy(im.w, im.h);
  for (int w = 0; w < im.w; w++)
    for (int h = 0; h < im.h; h++) 
    {
      Ix(w, h) = Ix_im(w, h, 0);
      Iy(w, h) = Iy_im(w, h, 0);
    }

  Matrix IxIx = elementwise_multiply(Ix, Ix); // Misura l'intensità del gradiente lungo l'asse x
  Matrix IyIy = elementwise_multiply(Iy, Iy); // Misura l'intensità del gradiente lungo l'asse y
  Matrix IxIy = elementwise_multiply(Ix, Iy); // Misura la correlazione tra i gradienti lungo gli assi x e y

  for (int w = 0; w < im.w; w++)
      for (int h = 0; h < im.h; h++) 
      {
        S(w, h, 0) = IxIx(w, h);
        S(w, h, 1) = IyIy(w, h);
        S(w, h, 2) = IxIy(w, h);
      }

  // somma pesata dei valori dei pixel vicini tramite una gaussiana
  return smooth_image(S, sigma);
}


// HW5 1.2
// per capire se un punto è un corner, calcoliamo il cornerness response
Image cornerness_response(const Image& S, int method)
{
  Image R(S.w, S.h);

  for (int w = 0; w < S.w; w++)
    for (int h = 0; h < S.h; h++) 
    {
      float IxIx = S(w, h, 0);
      float IyIy = S(w, h, 1);
      float IxIy = S(w, h, 2);
    
      R(w, h) = ((IxIx*IyIy) - pow(IxIy,2)) / (IxIx+IyIy); // Harris det(H) / trace(H)
    }

  return R;
}


// HW5 1.3
Image nms_image(const Image& im, int w)
{
  // non maximum supression con i vicini a distanza w
  Image r = im;
  
  for (int x = 0; x < im.w; x++)
    for (int y = 0; y < im.h; y++)
      for (int neighbor1 = x-w; neighbor1 <= x+w; neighbor1++)
        for (int neighbor2 = y-w; neighbor2 <= y+w; neighbor2++)
          if (im(x, y, 0) < im.clamped_pixel(neighbor1, neighbor2, 0))
            r(x, y, 0) = -1000;

  return r;
}


// HW5 1.4
vector<Descriptor> detect_corners(const Image& im, const Image& nms, float thresh, int window)
{
  vector<Descriptor> d;
  
  for (int x = 0; x < im.w; x++)
    for (int y = 0; y < im.h; y++)
      if (nms(x, y, 0) > thresh) // se il pixel è un corner
        d.push_back(describe_index(im, x, y, window)); // aggiunge il descrittore del punto di interesse all'array di descrittori
  
  return d;
}


vector<Descriptor> harris_corner_detector(const Image& im, float sigma, float thresh, int window, int nms, int corner_method)
{
  // Calculate structure matrix
  Image S = structure_matrix(im, sigma);
  
  // Estimate cornerness
  Image R = cornerness_response(S,corner_method);
  
  // Run NMS on the responses
  Image Rnms = nms_image(R, nms);
  
  return detect_corners(im, Rnms, thresh, window);
}


Image detect_and_draw_corners(const Image& im, float sigma, float thresh, int window, int nms, int corner_method)
{
  vector<Descriptor> d = harris_corner_detector(im, sigma, thresh, window, nms, corner_method);
  return mark_corners(im, d);
}
