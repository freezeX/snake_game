// This is to get access to the non-portable (or at least not defined by the
// C99 standard) nanosleep function
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>


#include "screen.h"
#include "game.h"

#include <stdlib.h>
#include <stdio.h>


#include <sys/types.h>
#include <unistd.h>

int main (int argc, char ** args)
{
   // Initialize random generator
   srand (getpid()+getppid());

   screen_t * scr = screen_init ();

   game_t * game = game_init (scr);

   // Main event loop
   while (true)
   {
      unsigned long delay = game_tick (game);
      if (!delay)
         break;

      while (delay)
      {
         --delay;
         // Wait for delay (we sleep .5ms in between)
         struct timespec ts;
         ts.tv_sec=0;
         ts.tv_nsec = 500000lu;
         nanosleep (&ts, NULL);

         // Check for characters
         int key;
         while ((key = getch()) != ERR)
         {
            int hor = 0;
            int ver = 0;
            switch (key)
            {
               case KEY_UP:
                  ver = -1;
                  break;
               case KEY_DOWN:
                  ver = 1;
                  break;
               case KEY_LEFT:
                  hor = -1;
                  break;
               case KEY_RIGHT:
                  hor = 1;
                  break;
               default:
                  break;
            }
            if (hor || ver)
            {
               unsigned long newdelay = game_key (game, hor, ver);
               if (newdelay)
                  delay = newdelay;
            }
         }
      }
   }

   screen_done (scr);

   game_done (game);

   return EXIT_SUCCESS;
}
