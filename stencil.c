
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Define output file name
#define OUTPUT_FILE "stencil.pgm"

void stencil(const int nx, const int ny, double *  image, double *  tmp_image);
void init_image(const int nx, const int ny, double *  image, double *  tmp_image);
void output_image(const char * file_name, const int nx, const int ny, double *image);
double wtime(void);

int main(int argc, char *argv[]) {

  // Check usage
  if (argc != 4) {
    fprintf(stderr, "Usage: %s nx ny niters\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initiliase problem dimensions from command line arguments
  int nx = atoi(argv[1]);
  int ny = atoi(argv[2]);
  int niters = atoi(argv[3]);

  int alloc_ny = ny + 2;

  // Allocate the image
  double *alloc_image = malloc(sizeof(double) * nx * alloc_ny);
  double *alloc_tmp_image = malloc(sizeof(double) * nx * alloc_ny);

  double *image = alloc_image + nx;
  double *tmp_image = alloc_tmp_image + nx;

  // Set the input image
  init_image(nx, alloc_ny, alloc_image, alloc_tmp_image);

  // Call the stencil kernel
  double tic = wtime();
  for (int t = 0; t < niters; ++t) {
    stencil(nx, ny, image, tmp_image);
    stencil(nx, ny, tmp_image, image);
  }
  double toc = wtime();


  // Output
  printf("------------------------------------\n");
  printf(" runtime: %lf s\n", toc-tic);
  printf("------------------------------------\n");

  output_image(OUTPUT_FILE, nx, ny, image);
  free(alloc_image);
  free(alloc_tmp_image);
}

void stencil(const int nx, const int ny, double *  image, double *  tmp_image) {
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      tmp_image[j+i*ny] = 0.6 * image[j+i*ny]
                        + 0.1 * ( image[j  +(i-1)*ny] * (i > 0)
                                + image[j  +(i+1)*ny] * (i < nx-1)
                                + image[j-1+i*ny]     * (j > 0)
                                + image[j+1+i*ny]     * (j < ny-1) );
    }
  }
}

// Create the input image
void init_image(const int nx, const int alloc_ny, double *  alloc_image, double *  alloc_tmp_image) {

  double *image = alloc_image + nx;
  int ny = alloc_ny - 2;

  // Zero everything
  for (int j = 0; j < alloc_ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      alloc_image[j+i*ny] = 0.0;
      alloc_tmp_image[j+i*ny] = 0.0;
    }
  }

  // Checkerboard
  for (int j = 0; j < 8; ++j) {
    for (int i = 0; i < 8; ++i) {
      for (int jj = j*ny/8; jj < (j+1)*ny/8; ++jj) {
        for (int ii = i*nx/8; ii < (i+1)*nx/8; ++ii) {
          if ((i+j)%2)
          image[jj+ii*ny] = 100.0;
        }
      }
    }
  }
}

// Routine to output the image in Netpbm grayscale binary image format
void output_image(const char * file_name, const int nx, const int ny, double *image) {

  // Open output file
  FILE *fp = fopen(file_name, "w");
  if (!fp) {
    fprintf(stderr, "Error: Could not open %s\n", OUTPUT_FILE);
    exit(EXIT_FAILURE);
  }

  // Ouptut image header
  fprintf(fp, "P5 %d %d 255\n", nx, ny);

  // Calculate maximum value of image
  // This is used to rescale the values
  // to a range of 0-255 for output
  double maximum = 0.0;
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      if (image[j+i*ny] > maximum)
        maximum = image[j+i*ny];
    }
  }

  // Output image, converting to numbers 0-255
  for (int j = 0; j < ny; ++j) {
    for (int i = 0; i < nx; ++i) {
      fputc((char)(255.0*image[j+i*ny]/maximum), fp);
    }
  }

  // Close the file
  fclose(fp);

}

// Get the current time in seconds since the Epoch
double wtime(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec*1e-6;
}
