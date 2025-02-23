#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// import other functions
#include "exam_utils.h"
#include "save_jpeg.h"
#include "xwin_sdl.h"

// functions
void copy(int width, int x1, int y1, int block_w, int block_h, int x2, int y2, unsigned char *scr);
void swap(int width, int x1, int y1, int block_w, int block_h, int x2, int y2, unsigned char *scr);
void compute(int width, int height, unsigned char *scr, bool anim_status, FILE *fd);

// main
int main(int argc, char **argv)
{
   
   // if there is less than 3 arguments we get Error (there is not input and output picture)
   if (argc < 3)
   {
      printf("ERROR: Invalid command\n");
      exit(-1);
   }

   // load input from given parameters
   char *inputpic = argv[1]; // in online tutorial ==> argc > 1 ? argv[1] : NULL
   char *instructions = argv[2];
   char *outputpic = argv[3];
   bool jpeg_status = false; // argc > 4 && strcmp(argv[4],"--anim") == 0;

   // if there is parameter outputpic enable saving jpg
   if (outputpic && (strstr(outputpic, ".jpg") || strstr(outputpic, ".jpeg")))
   {
      jpeg_status = true;
   }

   // if last parameter is given, enable animation
   bool anim_status = false;
   char *animval = "--anim";

   if (argc > 4 && strcmp(argv[4], animval) == 0) // argc > 4 && strcmp(argv[4],"--anim") == 0
   {
      anim_status = true;
   }

   // load input image
   int width, height;
   unsigned char *scr = xwin_load_image(inputpic, &width, &height);
   if (scr == NULL)
   {
      printf("ERROR: Invalid input picture name\n");
      exit(EXIT_FAILURE);
   }

   // open instructions
   FILE *fd = fopen(instructions, "r");
   if (fd == NULL)
   {
      printf("ERROR: Invalid instructions name\n");
      free(scr);
      exit(EXIT_FAILURE);
   }

   // initialize animation if enabled
   if (anim_status == true)
   {
      xwin_init(width, height);
   }

   // compute instructions
   compute(width, height, scr, anim_status, fd);

   // close animation
   if (anim_status == true)
   {
      xwin_close();
   }

   // save jpeg if enabled
   if (jpeg_status == true)
   {
      save_image_jpeg(width, height, scr, outputpic);
   }
   else
   {
      save_image_rgb(width, height, scr, outputpic);
   }

   // free and close files
   free(scr);
   fclose(fd);

   return EXIT_SUCCESS;
}

// copy
void copy(int width, int x1, int y1, int block_w, int block_h, int x2, int y2, unsigned char *scr)
{
   // get throught every selected pixel in picture
   for (int i = 0; i < block_h; i++)
   {
      for (int j = 0; j < block_w; j++)
      {
         // get throught rgb layer in pixel
         for (int p = 0; p < 3; p++)
         {
            // get position two to position one
            scr[y1 * width * 3 + x1 * 3 + i * width * 3 + j * 3 + p] = scr[y2 * width * 3 + x2 * 3 + i * width * 3 + j * 3 + p];
         }
      }
   }
}

// swap
void swap(int width, int x1, int y1, int block_w, int block_h, int x2, int y2, unsigned char *scr)
{
   // get throught every selected pixel in picture
   for (int i = 0; i < block_h; i++)
   {
      for (int j = 0; j < block_w; j++)
      {
         // get throught rgb layer in pixel
         for (int p = 0; p < 3; p++)
         {
            // save position one
            unsigned char save = scr[y1 * width * 3 + x1 * 3 + i * width * 3 + j * 3 + p];
            // get position two to position one
            scr[y1 * width * 3 + x1 * 3 + i * width * 3 + j * 3 + p] = scr[y2 * width * 3 + x2 * 3 + i * width * 3 + j * 3 + p];
            // get saved position one to position two
            scr[y2 * width * 3 + x2 * 3 + i * width * 3 + j * 3 + p] = save;
         }
      }
   }
}

// compute operation
void compute(int width, int height, unsigned char *scr, bool anim_status, FILE *fd)
{
   // initialize needed parameters for instructions
   char type[3];
   int x1, y1, block_w, block_h, x2, y2, check;

   // while no end of instructions file
   while (feof(fd) == 0)
   {
      // load line in instructions
      // sw/cp  x1  y1  w  h  x2  y2
      check = fscanf(fd, "%2s %i %i %i %i %i %i\n", &*type, &x1, &y1, &block_w, &block_h, &x2, &y2);

      // if there is not 7 instructions in line, print error
      if (check != 7)
      {
         printf("ERROR: Invalid text in the instruction file\n");
         free(scr);
         fclose(fd);
         exit(EXIT_FAILURE);
      }

      // control of negative numbers
      if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || block_h < 0 || block_w < 0)
      {
         printf("ERROR: Negative number in instructions\n");
         free(scr);
         fclose(fd);
         exit(EXIT_FAILURE);
      }

      // if we are out of bounds of picture print error
      if (x1 + block_w > width || x2 + block_w > width || y1 + block_h > height || y2 + block_h > height)
      {
         printf("ERROR: Invalid dimension of operation\n");
         free(scr);
         fclose(fd);
         exit(EXIT_FAILURE);
      }

      // if we get instruction swap
      if (strcmp(type, "sw") == 0)
      {
         swap(width, x1, y1, block_w, block_h, x2, y2, scr);
      }
      // if we get instruction copy
      else if (strcmp(type, "cp") == 0)
      {
         copy(width, x1, y1, block_w, block_h, x2, y2, scr);
      }
      // nothing else is possible
      else
      {
         printf("ERROR: Invalid text in the instruction file\n");
         free(scr);
         fclose(fd);
         exit(EXIT_FAILURE);
      }

      // if animation is enabled, animate operation
      if (anim_status == true)
      {
         // redraw (update picture)
         xwin_redraw(width, height, scr);
         // delay redrawing
         delay(500);
      }
   }
}
