#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"

#define M_PI 3.14159265358979323846

// HW1 #2.1
// Image& im: image to L1-normalize
void l1_normalize(Image &im) {

    // TODO: Normalize each channel
    float sum;
    for (int c=0; c<im.c; c++) {
        sum = 0;
        for (int y=0; y<im.h; y++) {
            for (int x=0; x<im.w; x++) {
                sum += im.clamped_pixel(x,y,c);
            }
        }
        
        for (int y=0; y<im.h; y++) {
            for (int x=0; x<im.w; x++) {
                if (sum > 0) im.set_pixel(x,y,c,(float)(im.clamped_pixel(x,y,c)/sum));
                else im.set_pixel(x,y,c,(float)(1/(im.h*im.w)));
            }
        }
    }


}

// HW1 #2.1
// int w: size of filter
// returns the filter Image of size WxW

/*Questa funzione restituisce un filtro di dimensioni 
w x w con tutti i valori impostati a 1/(w*w), 
garantendo che w sia un numero dispari.*/

Image make_box_filter(int w) {
    assert(w % 2); // w needs to be odd

    // TODO: Implement the filter
    
    Image ret(w,w,1);
    
    for (int y=0; y<w; y++) {
        for (int x=0; x<w; x++) {
            ret.set_pixel(x,y,0, 1.0/(w*w));
        }
    }
    return ret;
}

// HW1 #2.2
// const Image&im: input image
// const Image& filter: filter to convolve with
// bool preserve: whether to preserve number of channels
// returns the convolved image

/*Questa funzione esegue l'operazione di convoluzione 
tra un'immagine e un filtro, restituendo un'immagine 
convoluta. Il parametro "preserve" controlla se deve 
preservare il numero di canali o convertirlo a una 
singola immagine.*/
Image convolve_image(const Image &im, const Image &filter, bool preserve) {
    assert(filter.c == 1);
    
    
    // This is the case when we need to use the function clamped_pixel(x,y,c).
    // Otherwise you'll have to manually check whether the filter goes out of bounds

    // TODO: Make sure you set the sizes of ret properly. Use ret=Image(w,h,c) to reset ret
    // TODO: Do the convolution operator
    Image ret(im.w, im.h, im.c);
    Image nopres_ret(im.w, im.h, 1);

    for (int y=0; y<im.h; y++) {
        for (int x=0; x<im.w; x++) {
            float nopres_val = 0;
            for (int c=0; c<im.c; c++) {
                
                float val = 0;
                
                for (int fy=0; fy<filter.h; fy++) {
                    for (int fx=0; fx<filter.w; fx++) {
                        int start_x = x + fx - filter.w/2;
                        int start_y = y + fy - filter.h/2;
                        val += im.clamped_pixel(start_x,start_y,c) * filter(fx,fy,0);
                    }
                }
                if (!preserve) nopres_val += val;
                else ret.set_pixel(x,y,c,val);
            }
            if (!preserve) nopres_ret.set_pixel(x,y,0,nopres_val);
        }
    }
    
    // Make sure to return ret and not im. This is just a placeholder
    if (!preserve) return nopres_ret;
    else return ret;
}

// HW1 #2.3
// returns basic 3x3 high-pass filter
Image make_highpass_filter() {
    // TODO: Implement the filter
    
    Image hp_filter(3,3,1);
    
    for (int x=0; x<3; x++) {
        for (int y=0; y<3; y++) {
            hp_filter.set_pixel(x,y,0,0);
            int addr = hp_filter.pixel_address(x,y,0);
            if (addr % 2 != 0) hp_filter.set_pixel(x,y,0,-1);
            if (addr == 4) hp_filter.set_pixel(x,y,0,4);
            //printf("%d  %f\n",addr,hp_filter.data[addr]);
        }
    }
    return hp_filter;

}

// HW1 #2.3
// returns basic 3x3 sharpen filter
/*Questo filtro è progettato per accentuare i dettagli 
e migliorare la nitidezza dell'immagine. La matrice del 
filtro è simile a quella del filtro di passaggio alto,
ma con un peso maggiore sull'elemento centrale (5) e pesi
negativi sui pixel circostanti. Questo contribuisce a 
enfatizzare i bordi e a produrre un effetto di maggiore 
nitidezza nell'immagine risultante.*/
Image make_sharpen_filter() {
    // TODO: Implement the filter
    Image sp_filter(3,3,1);
    
    for (int x=0; x<3; x++) {
        for (int y=0; y<3; y++) {
            sp_filter.set_pixel(x,y,0,0);
            int addr = sp_filter.pixel_address(x,y,0);
            if (addr % 2 != 0) sp_filter.set_pixel(x,y,0,-1);
            if (addr == 4) sp_filter.set_pixel(x,y,0,5);
            //printf("%d  %f\n",addr,hp_filter.data[addr]);
        }
    }
    return sp_filter;

}

// HW1 #2.3
// returns basic 3x3 emboss filter
/*L'obiettivo di questo filtro è creare un effetto di 
rilievo sull'immagine. La matrice del filtro è progettata 
per evidenziare le transizioni di intensità nell'immagine,
simulando l'aspetto di un'immagine tridimensionale 
incisa o rilevata.*/
Image make_emboss_filter() {
    
    Image eb_filter(3,3,1);
    
    eb_filter.data[0] = -2;
    eb_filter.data[1] = -1;
    eb_filter.data[2] = 0;
    eb_filter.data[3] = -1;
    eb_filter.data[4] = 1;
    eb_filter.data[5] = 1;
    eb_filter.data[6] = 0;
    eb_filter.data[7] = 1;
    eb_filter.data[8] = 2;
    
    return eb_filter;
    
}

// HW1 #2.4
// float sigma: sigma for the gaussian filter
// returns basic gaussian filter

/*Questa funzione genera un filtro gaussiano, che è 
comunemente utilizzato per sfocare un'immagine. 
Il filtro gaussiano viene utilizzato per ridurre il 
rumore e sfocare leggermente i dettagli, creando un 
effetto di "morbidezza". La dimensione del kernel del 
filtro gaussiano è determinata da un parametro sigma, 
che controlla la larghezza della distribuzione gaussiana.*/
Image make_gaussian_filter(float sigma) {
    // TODO: Implement the filter
    int kernel = ceil(sigma*6) + 1;
    Image gauss(kernel,kernel,1);
    
    for (int x=0; x<kernel; x++) {
        for (int y=0; y<kernel; y++) {
            float val = (1.0/(2*powf(sigma, 2.0)*M_PI)) * exp(-(powf(x-kernel/2, 2.0)+powf(y-kernel/2, 2.0))/(2*powf(sigma, 2.0)));
            gauss.set_pixel(x,y,0,val);
        }
    }
    l1_normalize(gauss);

    return gauss;

}


// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their sum
Image add_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size

    Image sum(a.w,a.h,a.c);

    // TODO: Implement subtraction
    for (int x=0; x<a.w; x++) {
        for (int y=0; y<a.h; y++) {
            for (int c=0; c<a.c; c++) {
                sum.set_pixel(x,y,c,a.clamped_pixel(x,y,c)+b.clamped_pixel(x,y,c));
            }
        }
    }

    return sum;


}

// HW1 #3
// const Image& a: input image
// const Image& b: input image
// returns their difference res=a-b
Image sub_image(const Image &a, const Image &b) {
    assert(a.w == b.w && a.h == b.h &&
           a.c == b.c); // assure images are the same size
    
    Image sub(a.w,a.h,a.c);

    // TODO: Implement subtraction
    for (int x=0; x<a.w; x++) {
        for (int y=0; y<a.h; y++) {
            for (int c=0; c<a.c; c++) {
                sub.set_pixel(x,y,c,a(x,y,c)-b(x,y,c));
            }
        }
    }

    return sub;

}

// HW1 #4.1
// returns basic GX filter
Image make_gx_filter() {
    // TODO: Implement the filter
    Image gx_filter(3,3,1);
    
    gx_filter.data[0] = -1;
    gx_filter.data[1] = 0;
    gx_filter.data[2] = 1;
    gx_filter.data[3] = -2;
    gx_filter.data[4] = 0;
    gx_filter.data[5] = 2;
    gx_filter.data[6] = -1;
    gx_filter.data[7] = 0;
    gx_filter.data[8] = 1;
    
    return gx_filter;
}

// HW1 #4.1
// returns basic GY filter
Image make_gy_filter() {
    // TODO: Implement the filter
    Image gy_filter(3,3,1);
    
    gy_filter.data[0] = -1;
    gy_filter.data[1] = -2;
    gy_filter.data[2] = -1;
    gy_filter.data[3] = 0;
    gy_filter.data[4] = 0;
    gy_filter.data[5] = 0;
    gy_filter.data[6] = 1;
    gy_filter.data[7] = 2;
    gy_filter.data[8] = 1;
    
    return gy_filter;
}

// HW1 #4.2
// Image& im: input image
/*Questa funzione normalizza le caratteristiche di 
ciascun canale dell'immagine. Per ciascun canale, calcola
 il valore massimo e minimo all'interno dell'immagine e 
 normalizza i pixel di quel canale in modo che i valori 
 si trovino nell'intervallo [0, 1].*/
void feature_normalize(Image &im) {
    assert(im.w * im.h); // assure we have non-empty image

    // TODO: Normalize the features for each channel
    for (int c=0; c<im.c; c++) {
        
        float max_val = -INFINITY;
        float min_val = INFINITY;
        
        //Trova i valori massimo e minimo del range reale dell'immagine
        for (int y=0; y<im.h; y++) {
            for (int x=0; x<im.w; x++) {
                if (im(x,y,c) > max_val) max_val = im(x,y,c);
                else if (im(x,y,c) < min_val) min_val = im(x,y,c);
            }
        }
        
        //Se il range [max-min] == 0, l'intera immagine va settata a 0
        if (max_val == min_val) {
            for (int y=0; y<im.h; y++) {
                for (int x=0; x<im.w; x++) {
                    im.set_pixel(x,y,c,0);
                }
            }
        }
        //Altrimenti normalizziamo il range
        else {
            float diff = max_val - min_val;
            for (int y=0; y<im.h; y++) {
                for (int x=0; x<im.w; x++) {
                    float pix = im(x,y,c);
                    im.set_pixel(x,y,c,(pix-min_val)/diff);
                }
            }
        }
        
    }
    
}


// Normalizes features across all channels
/*Questa funzione normalizza le caratteristiche 
attraverso tutti i canali. Prima di normalizzare, 
combina tutti i canali in un unico canale. Quindi, 
normalizza i pixel di questo canale.
*/
void feature_normalize_total(Image &im) {
    assert(im.w * im.h * im.c); // assure we have non-empty image

    int nc = im.c;
    im.c = 1;
    im.w *= nc;

    feature_normalize(im);

    im.w /= nc;
    im.c = nc;

}


// HW1 #4.3
// Image& im: input image
// return a pair of images of the same size

/*
Questa funzione utilizza i filtri Sobel (generati con le
funzioni make_gx_filter e make_gy_filter) per calcolare 
il gradiente dell'immagine in termini di magnitudine e 
direzione. La magnitudine rappresenta l'intensità del 
gradiente, mentre la direzione indica la direzione del 
gradiente.
*/

pair<Image, Image> sobel_image(const Image &im) {
    // TODO: Your code here
    Image gx_filter = make_gx_filter();
    Image gy_filter = make_gy_filter();
    
    Image gx = convolve_image(im,gx_filter,false);
    Image gy = convolve_image(im,gy_filter,false);
    
    Image magnitude(im.w, im.h, 1);
    Image direction(im.w, im.h, 1);
    
    for (int y=0; y<im.h; y++) {
        for (int x=0; x<im.w; x++) {
            
            magnitude.set_pixel(x, y, 0, sqrtf(powf(gx(x,y,0),2.0) + powf(gy(x,y,0),2.0)));
            
            direction.set_pixel(x, y, 0, atan2(gy(x,y,0),gx(x,y,0)));
        }
    }

    return {magnitude, direction};
}


// HW1 #4.4
// const Image& im: input image
// returns the colorized Sobel image of the same size

/*Questa funzione colora l'immagine basandosi sui 
risultati della funzione sobel_image. Assegna la 
direzione del gradiente alla componente Hue di 
un'immagine in formato HSV e imposta la saturazione 
sulla magnitudine del gradiente. Quindi converte 
l'immagine risultante da HSV a RGB.*/

Image colorize_sobel(const Image &im) {

    // TODO: Your code here
    Image res(im.w, im.h, im.c);
    
    pair<Image, Image> sobel = sobel_image(im);
    Image saturation = sobel.first;
    Image hue = sobel.second;
    
    feature_normalize(saturation);
    
    for (int y=0; y<hue.h; y++) {
        for (int x=0; x<hue.w; x++) {
            hue.set_pixel(x, y, 0, hue(x,y,0)/(2*M_PI) + 0.5);
        }
    }
    
    for (int y=0; y<im.h; y++) {
        for (int x=0; x<im.w; x++) {
            res.set_pixel(x,y,0,hue(x,y,0));
            res.set_pixel(x,y,1,saturation(x,y,0));
            res.set_pixel(x,y,2,saturation(x,y,0));
        }
    }
    
    hsv_to_rgb(res);
    
    Image gauss = make_gaussian_filter(4);

    return convolve_image(res,gauss,true);
}


// HW1 #4.5
// const Image& im: input image
// float sigma1,sigma2: the two sigmas for bilateral filter
// returns the result of applying bilateral filtering to im

/*Questa funzione implementa il filtro bilaterale, che è 
utilizzato per il miglioramento dell'immagine preservando 
i bordi. Utilizza due parametri sigma, uno per il dominio 
spaziale e uno per il dominio dei colori, per pesare 
le differenze tra i pixel durante il filtraggio. 
In breve, il filtro bilaterale sfoca l'immagine 
mantenendo i dettagli nei pressi dei bordi.*/
Image bilateral_filter(const Image &im, float sigma1, float sigma2) {
    
    Image res(im.w,im.h,im.c);

    // TODO: Your bilateral code
    int kernel = ceil(sigma1*6) + 1;
    
    Image space_gauss = make_gaussian_filter(sigma1);
    
    for (int x=0; x<im.w; x++) {
        for (int y=0; y<im.h; y++) {
            for (int c=0; c<im.c; c++) {
                
                float sum = 0;
                
                //Calcolo il fattore di normalizzazione N(i,j)
                for (int i=0; i<kernel; i++) {
                    for (int j=0; j<kernel;j++) {
                        float curr_pixel = im.clamped_pixel(x,y,c);
                        float dist_pixel = im.clamped_pixel(x+i-(int)kernel/2,y+j-(int)kernel/2,c);
                        float curr_gauss_space = space_gauss(i,j,0);
                        float curr_gauss_color = (1.0/(2*powf(sigma2, 2.0)*M_PI)) * exp(-(powf(curr_pixel-dist_pixel, 2.0))/(2*powf(sigma2, 2.0)));
                        sum += curr_gauss_space * curr_gauss_color;
                    }
                }
                
                //Computo il filtro bilaterale
                float new_pixel = 0;
                for (int i=0; i<kernel; i++) {
                    for (int j=0; j<kernel;j++) {
                        float curr_pixel = im.clamped_pixel(x,y,c);
                        float dist_pixel = im.clamped_pixel(x+i-kernel/2,y+j-kernel/2,c);
                        float curr_gauss_space = space_gauss(i,j,0);
                        float curr_gauss_color = (1.0/(2*powf(sigma2, 2.0)*M_PI)) * exp(-(powf(curr_pixel-dist_pixel, 2.0))/(2*powf(sigma2, 2.0)));
                        new_pixel += im.clamped_pixel(x+i-kernel/2,y+j-kernel/2,c) * curr_gauss_space * curr_gauss_color / sum;
                    }
                }
                res.set_pixel(x,y,c,new_pixel);
            }
        }
    }

    return res;
}


// HM #5
//

/*Le funzioni compute_histogram, compute_CDF, histogram_equalization_hsv, 
e histogram_equalization_rgb sono utilizzate per eseguire l'equalizzazione 
dell'istogramma. L'equalizzazione dell'istogramma è una tecnica utilizzata 
per migliorare la distribuzione delle intensità nei pixel di un'immagine, 
aumentando il contrasto.*/

float *compute_histogram(const Image &im, int ch, int num_bins) {
    float *hist = (float *) malloc(sizeof(float) * num_bins);
    for (int i = 0; i < num_bins; ++i) {
        hist[i] = 0;
    }

    // TODO: Your histogram code
    for (int x=0; x<im.w; x++) {
        for (int y=0; y<im.h; y++) {
            int val = round(im(x,y,ch) * (num_bins - 1));
            hist[val]++;
        }
    }

    return hist;
}

float *compute_CDF(float *hist, int num_bins) {
    float *cdf = (float *) malloc(sizeof(float) * num_bins);

    cdf[0] = hist[0];

    // TODO: Your cdf code
    for (int i=1; i<num_bins; i++) {
        int curr = 0;
        for (int j=0; j<=i; j++) {
            curr += hist[j];
        }
        cdf[i] = curr;
    }

    return cdf;
}

Image histogram_equalization_hsv(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // TODO: Your histogram equalization code
    
    // *** Convert to hsv ***
    rgb_to_hsv(new_im);
    
    /*for (int x=0; x<im.w; x++) {
        for (int y=0; y<im.h; y++) {
            printf("Pixel[%d;%d]: %f\n",x,y,new_im(x,y,2));
        }
    }*/
    
    
    // *** Compute histograms for the luminance channel ***
    float* hist = compute_histogram(new_im,2,num_bins);
    
    // *** Compute cdf ***
    float* cdf = compute_CDF(hist,num_bins);
    
    
    // DEBUG
    /*printf("\n\n\n");
    for (int i=0; i<num_bins; i++) {
        printf("hist[%d]: %f\n", i, hist[i]);
    }
    printf("\n\n\n");
    for (int i=0; i<num_bins; i++) {
        printf("cdf[%d]: %f\n", i, cdf[i]);
    }
    printf("\n\n\n");*/
    
    
    // *** Equalization ***
    
    //  find cdf_min, the minimum value of the cdf
    float cdf_min = 0;
    for (int i=0; i<num_bins; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }
    
    // compute new pixel values after equalization
    int im_area = im.w * im.h;
    int range = num_bins - 1;
    float* eq_pixels = (float*) malloc(sizeof(float) * num_bins);
    const float k = (float)(255.0/(im_area-cdf_min));
    for (int i=0; i<num_bins; i++) {
        if (cdf[i] != 0) {
            //int new_val = round(((cdf[i] - cdf_min)/(im_area - cdf_min)) * range);
            eq_pixels[i] = (float)(k * (cdf[i] - cdf_min));
        }
        else eq_pixels[i] = 0;
        //printf("eq_pixels[%d]: %f\n", i, eq_pixels[i]);
    }
    
    // set V channel values to equalized values
    for (int x=0; x<im.w; x++) {
        for (int y=0; y<im.h; y++) {
            int val = round(new_im(x,y,2)*255);
            new_im.set_pixel(x,y,2,(float)(eq_pixels[val]/255.0));
        }
    }
    
    // DEBUG
    /*for (int x=0; x<im.w; x++) {
        for (int y=0; y<im.h; y++) {
            printf("Pixel[%d;%d]: %f\n",x,y,new_im(x,y,2));
        }
    }*/
    
    // *** Convert back to rgb ***
    hsv_to_rgb(new_im);

    // *** Delete the allocated memory! ***
    
    free(eq_pixels);
    
    //  delete hist;
    free(hist);
    //  delete cdf;
    free(cdf);

    return new_im;
}

Image histogram_equalization_rgb(const Image &im, int num_bins) {
    Image new_im(im);
    float eps = 1.0 / (num_bins * 1000);

    // compute histograms for each color channel
    for (int c = 0; c < im.c; ++c) {

        // *** Compute histograms for the luminance channel ***
        float* hist = compute_histogram(new_im,c,num_bins);
        
        // *** Compute cdf ***
        float* cdf = compute_CDF(hist,num_bins);
        
        
        // DEBUG
        /*printf("\n\n\n");
        for (int i=0; i<num_bins; i++) {
            printf("hist[%d]: %f\n", i, hist[i]);
        }
        printf("\n\n\n");
        for (int i=0; i<num_bins; i++) {
            printf("cdf[%d]: %f\n", i, cdf[i]);
        }
        printf("\n\n\n");*/
        
        
        // *** Equalization ***
        
        //  find cdf_min, the minimum value of the cdf
        float cdf_min = 0;
        for (int i=0; i<num_bins; i++) {
            if (cdf[i] != 0) {
                cdf_min = cdf[i];
                break;
            }
        }
        
        // compute new pixel values after equalization
        int im_area = im.w * im.h;
        int range = num_bins - 1;
        float* eq_pixels = (float*) malloc(sizeof(float) * num_bins);
        for (int i=0; i<num_bins; i++) {
            if (cdf[i] != 0) {
                float new_val = (float)((cdf[i] - cdf_min)/(im_area - cdf_min)) * range;
                eq_pixels[i] = new_val;
            }
            else eq_pixels[i] = 0;
            //printf("eq_pixels[%d]: %f\n", i, eq_pixels[i]);
        }
        
        // set V channel values to equalized values
        for (int x=0; x<im.w; x++) {
            for (int y=0; y<im.h; y++) {
                int val = round(new_im(x,y,c)*255);
                new_im.set_pixel(x,y,c,(float)(eq_pixels[val]/255.0));
            }
        }
        
        // DEBUG
        /*for (int x=0; x<im.w; x++) {
            for (int y=0; y<im.h; y++) {
                printf("Pixel[%d;%d]: %f\n",x,y,new_im(x,y,2));
            }
        }*/

    }

    return new_im;
}


// HELPER MEMBER FXNS

void Image::feature_normalize(void) { ::feature_normalize(*this); }

void Image::feature_normalize_total(void) { ::feature_normalize_total(*this); }

void Image::l1_normalize(void) { ::l1_normalize(*this); }

Image operator-(const Image &a, const Image &b) { return sub_image(a, b); }

Image operator+(const Image &a, const Image &b) { return add_image(a, b); }
