/* Gatech ECE2035 2015 SPRING PAC MAN
 * Copyright (c) 2015 Gatech ECE2035
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include "mbed.h"
#include "globals.h"
#include "map_public.h"
#include "pacman.h"

// Following code will only draw the Pacman on the screen. You are expected to modify most of the functions here.
// All other necessary functions will be implemented by you. Ex: the movement of Pacman, calculate the score ... etc.

void pacman_init(int grid_x, int grid_y){

    map_eat_cookie(grid_x, grid_y);
    pacman_draw(grid_x,grid_y, grid_x, grid_y);

}

void pacman_draw(int grid_x, int grid_y, int old_x, int old_y){ //Added inputs to determine direction change.
    
    GRID grid_info = map_get_grid_status(grid_x,grid_y);
    // Calculate the actual position of the grid
    int screen_x = grid_info.x + GRID_RADIUS;
    int screen_y = grid_info.y + GRID_RADIUS;
    
    //Draw a circle
    uLCD.filled_circle(screen_x, screen_y, GRID_RADIUS, PACMAN_COLOR);
    //Draw a rectangle (the mouth) of Pacman. It faces right by default.
    //You should add code to let the Pacman face to the direction it moves toward.
    if ((grid_x > old_x) && (grid_y == old_y)){ //Added values to make motion of pacman change. Draws rectangle dependent on added inputs.
    uLCD.filled_rectangle(screen_x,screen_y-1,screen_x+GRID_RADIUS,screen_y+1, BACKGROUND_COLOR);
    }
    if ((grid_x < old_x) && (grid_y == old_y)){
    uLCD.filled_rectangle(screen_x,screen_y-1,screen_x-GRID_RADIUS,screen_y+1, BACKGROUND_COLOR);
    }
    if ((grid_x == old_x) && (grid_y > old_y)){
    uLCD.filled_rectangle(screen_x-1,screen_y,screen_x+1,screen_y+GRID_RADIUS, BACKGROUND_COLOR);
    }
    if ((grid_x == old_x) && (grid_y < old_y)){
    uLCD.filled_rectangle(screen_x-1,screen_y,screen_x+1,screen_y-GRID_RADIUS, BACKGROUND_COLOR);
    }
    if ((grid_x == old_x) && (grid_y == old_y)){
    uLCD.filled_rectangle(screen_x,screen_y-1,screen_x+GRID_RADIUS,screen_y+1, BACKGROUND_COLOR);
    }
}

void pacman_clear(int grid_x, int grid_y){
    
    GRID grid_info = map_get_grid_status(grid_x,grid_y);
    //Fill the grid (a rectangle) with BACKGROUND_COLOR to clear the pacman
    uLCD.filled_rectangle(grid_info.x, grid_info.y, grid_info.x+GRID_SIZE-1, grid_info.y+GRID_SIZE-1, BACKGROUND_COLOR);
    
}

bool pacman_check_blk_occupied(int blk_x, int blk_y)  // Add a accesible block checking function for pacman based off of the grid status used for ghosts.
{
    if (map_get_grid_status(blk_x, blk_y).status==GRID_WALL)
        return true;
    else
        return false;
}

void pacman_fruit(int grid_x, int grid_y){  //Added function to draw fruit on map, based on pacman_draw.
    
    GRID grid_info = map_get_grid_status(grid_x,grid_y);
    int screen_x = grid_info.x + GRID_RADIUS;
    int screen_y = grid_info.y + GRID_RADIUS;
    uLCD.filled_circle(screen_x, screen_y, 3, LGREY);
    }
