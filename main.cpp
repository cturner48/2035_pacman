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
/** @file main.cpp */
// Include header files for platform
#include "mbed.h"
#include "wave_player.h"
#include "SDFileSystem.h"

// Include header files for pacman project
#include "globals.h"
#include "map_public.h"
#include "pacman.h"
#include "ghost.h"
#include "MMA8452.h"

// Platform initialization
DigitalIn left_pb(p21);  // push bottem
DigitalIn right_pb(p22); // push bottem
DigitalIn up_pb(p23);    // push bottem
DigitalIn down_pb(p24);  // push bottem
uLCD_4DGL uLCD(p9,p10,p11); // LCD (serial tx, serial rx, reset pin;)
Serial pc(USBTX,USBRX);     // used by Accelerometer
MMA8452 acc(p28, p27, 100000); // Accelerometer
AnalogOut DACout(p18);      // speaker
wave_player waver(&DACout); // wav player
SDFileSystem sd(p5, p6, p7, p8, "sd"); // SD card and filesystem (mosi, miso, sck, cs)
int nxtmovex, pacoldx, xpos;    //Variables for movement and fruit positioning.
int nxtmovey, pacoldy;
int paccurrx = 8, paccurry = 9; //Initiate points for pacman.
int lives = 3;                  // Variables for game events, score, lives, and other needed values.
bool game_on = true;
int score = 0;
double invcount = 0;
double level = 1;
int fruitcount = 0;
extern int remaining_cookie;
//bool game_over(void);
bool fruity = false;
int recent_scores[5];
int score_count;
void score_keeper(int new_score);
int new_score;
int temp;

// Example of the decleration of your implementation
void playSound(char * wav);


/** Main() is where you start your implementation
    @brief The hints of implementation are in the comments. <br>
    @brief You are expected to implement your code in main.cpp and pacman.cpp. But you could modify any code if you want to make the game work better.
*/
int main()
{   
    // Initialize the timer
    /// [Example of time control implementation]
        /// Here is a rough example to implement the timer control <br><br>
    int tick, pre_tick;
    double x,y,z;
    
    srand (time(NULL));
    Timer timer;
    timer.start();
    tick = timer.read_ms();
    pre_tick = tick;
    
    
    
    
    // Initialize the buttons        
    left_pb.mode(PullUp);  // The variable left_pb will be zero when the pushbutton for moving the player left is pressed    
    right_pb.mode(PullUp); // The variable rightt_pb will be zero when the pushbutton for moving the player right is pressed        
    up_pb.mode(PullUp);    //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    down_pb.mode(PullUp);  //the variable fire_pb will be zero when the pushbutton for firing a missile is pressed
    
    /// [Example of the game control implementation]
        /// Here is the example to initialize the game <br><br>
    uLCD.cls();
    map_init();
    pacman_init(8,9); // Center of the map
    
    // Add a new ghost with red color
    ghost_t ghost_red;
    ghost_init(&ghost_red,8,7,RED);
    ghost_show(&ghost_red);
    ghost_t ghost_dg;                       //Additional Ghost Init.
    ghost_init(&ghost_dg,7,7,DGREY);
    ghost_show(&ghost_dg);
    
    
    // [Demo of play sound file]
        playSound("/sd/wavfiles/BUZZER.wav");
        uLCD.locate(10,0);                              //Setup for displaying Lives counter.
        uLCD.printf("Lives:");
        for (x = lives; x > 0; x--){                        //For loop to draw number of pacman. X location of each based on number of lives.
        xpos = 128 - (x*10);                                //As lives decreases or increases, additional pacman are drawn or subtracted.
        uLCD.filled_circle(xpos, 10, GRID_RADIUS, PACMAN_COLOR);
        uLCD.filled_rectangle(xpos,9,xpos+GRID_RADIUS,11, BACKGROUND_COLOR);
        }
    /// 1. Begin the game loop
    while(game_on){                                     //Game On variable used to alternate between active game loop and score screen loop.
        if ((left_pb == 0) && (right_pb == 0)) {        //Fuction to force advance of level when both push buttons are pressed.
                    
                    //Code can be changed into level advance funtion and then called when needed
                    //Instead of repeating lines throughout code in future revisions.
                    
                    playSound("/sd/wavfiles/winning.wav");      //Folllowing lines initiate new map sequence including playing sounds, redrawing
                    lives = lives + 1;                          //map, adding new cookies, clearing ghosts and pacman and reseting locations.
                    uLCD.filled_circle((128-(lives*10)), 10, GRID_RADIUS, PACMAN_COLOR);
                    uLCD.filled_rectangle((128-(lives*10)),9,(128-(lives*10))+GRID_RADIUS,11, BACKGROUND_COLOR);
                    level = level + 1;                          //Level increase indicating move to next level of game.
                    map_init();
                    pacman_clear(paccurrx, paccurry);
                    pacman_init(8,9);
                    paccurrx = 8;
                    paccurry = 9;
                    pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                    ghost_init(&ghost_red,8,7,RED);
                    ghost_show(&ghost_red);
                    pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                    ghost_init(&ghost_dg,7,7,DGREY);
                    ghost_show(&ghost_dg);
                    game_on = true;
            
            }
        
        
        tick = timer.read_ms(); // Read current time
        
        /// 2. Implement the code to get user input and update the Pacman
            /// -[Hint] Implement the code to move Pacman. You could use either push-button or accelerometer. <br>

        // [Some hints for user-input detection]
        acc.readXYZGravity(&x,&y,&z); //read accelerometer
        uLCD.locate(1,0);
        uLCD.printf("Score:");      //Place score, and counter in upper left corner.
        uLCD.locate(1,1);
        uLCD.printf("%d", score);
        
        //uLCD.printf("sensor x%4.1f y%4.1f\n",x,y); //You could remove this code if you already make the accelerometer work.
            /// -[Hint] Here is a simple way to utilize the readings of accelerometer:
            ///         If x is larger than certain value (ex:0.3), then make the Pacman move right.
            ///         If x<-0.3, then make it move left. <br>
        if (abs(x) > abs(y)){       //Following lines are for motion. Function determines which tilt is the greatest difference
            if (x > .3){            //then creates variables for next move directions for both x and y coordinates.
                nxtmovex = 1;       //Values will then be added to current coordinates in future code.
                nxtmovey = 0;
            }
            if (x < -.3){
                nxtmovex = -1;
                nxtmovey = 0;
            }
        }
        if (abs(y) > abs(x)){
            if (y > .3){
                nxtmovex = 0;
                nxtmovey = -1;
            }
            if (y < -.3){
                nxtmovex = 0;
                nxtmovey = 1;
            }
        }
        
        if((tick-pre_tick)>500){ // Time step control
            pre_tick = tick; // update the previous tick
            if (invcount != 0) {        //Invincibility counter. If Pacman currently has invincibility, the counter decreases.
                invcount = invcount -1;
                }
            if (fruitcount == 30){      //If statements for fruit. Each tick counts up the fruit count value by 1, until the counter
                pacman_fruit(8,9);      //reaches 30, then fruit appears and the fruity var becomes true.
                fruity = true;
                fruitcount = fruitcount + 5;
                }
            else if (fruitcount > 30) { //If fruit exists, counter continues to increase at a faster rate.
                fruitcount = fruitcount + 5;
                }
            else if (fruitcount == 50) {//Once fruit has been around for a given time, the fruit dissappears and fruity is false.
                    fruity = false;     //Timer resets and begins sequence again.
                    pacman_clear(8,9);
                    fruitcount = 0;
                    }
                    
            else {
                fruity = false;
                fruitcount = fruitcount + 1;
                }
            if ((fruity) && (paccurrx == 8) && (paccurry == 9)){
                score = score + (level*25);     //If pacman enters location of fruit while fruity is active, gains 25 score per level.
                pacman_clear(8,9);              //Then fruit timer resets.
                pacman_draw(8,9,8,9);
                fruitcount = 0;
                fruity = false;
                }
            
        /// 3. Update the Pacman on the screen
            /// -[Hint] You could update the position of Pacman (draw it on the screen) here based on the user-input at step 2. <br>

            // [demo of the ghost movement]
            ghost_random_walk(&ghost_red);
            ghost_random_walk(&ghost_dg);
            
            // [demo of blinking Pacman]
            // It just a demonstration, you could remove this code.
            if (pacman_check_blk_occupied(paccurrx + nxtmovex, paccurry + nxtmovey)) {                                                                                                                  //If statement for pacman movement. Checks to see if path is blocked                 
                if (((paccurrx == int(ghost_red.ghost_blk_x)) && (paccurry == int(ghost_red.ghost_blk_y))) || ((paccurrx == int(ghost_dg.ghost_blk_x)) && (paccurry == int(ghost_dg.ghost_blk_y)))){   //using pacman function, then moves if the path is not blocked.
                    if ((invcount != 0) && ((paccurrx == int(ghost_red.ghost_blk_x)) && (paccurry == int(ghost_red.ghost_blk_y)))) {
                        pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                        ghost_init(&ghost_red,8,7,RED);         //Collision sequence for both ghosts if pacman has recently eaten super pellet.
                        ghost_show(&ghost_red);
                        score = score + 10; 
                    }
                    else if ((invcount != 0) && ((paccurrx == int(ghost_dg.ghost_blk_x)) && (paccurry == int(ghost_dg.ghost_blk_y)))) {
                        pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                        ghost_init(&ghost_dg,7,7,DGREY);
                        ghost_show(&ghost_dg);
                        score = score + 10; 
                    }
                    else {      //If collision is detected and pacman is not invincible.
                        uLCD.filled_circle((128 - (lives*10)), 10, GRID_RADIUS, BACKGROUND_COLOR);
                        lives = lives - 1; 
                        playSound("/sd/wavfiles/death.wav");                                                                    
                            if (lives == 0){ //Game over if lievs equal 0. Doesn't leave while loop, but enters internal loop score loop.
                                game_on = false;
                            }                                                                       
                        paccurrx = 8;   //Reseting pacman and ghosts. Can be setup as a function for future revisions to reduce repeat
                        paccurry = 9;   //code.
                        pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                        ghost_init(&ghost_red,8,7,RED);
                        ghost_show(&ghost_red);
                        pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                        ghost_init(&ghost_dg,7,7,DGREY);
                        ghost_show(&ghost_dg);
                    }
                    pacman_clear(paccurrx, paccurry);
                    pacman_draw(paccurrx, paccurry, paccurrx, paccurry); 
                }
                }
                else { //Second collision event to detect if pacman and ghost would 'pass through each other'
                    pacman_clear(paccurrx, paccurry);
                    if (((paccurrx == int(ghost_red.ghost_blk_x)) && (paccurry == int(ghost_red.ghost_blk_y))) || ((paccurrx == int(ghost_dg.ghost_blk_x)) && (paccurry == int(ghost_dg.ghost_blk_y)))){
                        if (invcount != 0) {
                            pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                            ghost_init(&ghost_red,8,7,RED);
                            ghost_show(&ghost_red);
                            score = score + 10; 
                        }
                        else if (invcount != 0) {
                            ghost_init(&ghost_dg,7,7,DGREY);
                            ghost_show(&ghost_dg);
                            score = score + 10; 
                        }
                        else {
                            uLCD.filled_circle((128 - (lives*10)), 10, GRID_RADIUS, BACKGROUND_COLOR);
                            lives = lives - 1;  
                            playSound("/sd/wavfiles/death.wav");                                                                        
                            if (lives == 0){
                                game_on = false;
                            }
                                                                                               
                            paccurrx = 8;
                            paccurry = 9;
                            pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                            ghost_init(&ghost_red,8,7,RED);
                            ghost_show(&ghost_red);
                            pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                            ghost_init(&ghost_dg,7,7,DGREY);
                            ghost_show(&ghost_dg);
                        }
                }
                    pacoldx = paccurrx;
                    pacoldy = paccurry;
                    paccurrx = paccurrx + nxtmovex;
                    paccurry = paccurry + nxtmovey;
                    if (((paccurrx == int(ghost_red.ghost_blk_x)) && (paccurry == int(ghost_red.ghost_blk_y))) || ((paccurrx == int(ghost_dg.ghost_blk_x)) && (paccurry == int(ghost_dg.ghost_blk_y)))){
                        if (invcount != 0) {
                            pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                            ghost_init(&ghost_red,8,7,RED);
                            ghost_show(&ghost_red);
                            score = score + 10; 
                        }
                        else if (invcount != 0) {
                            ghost_init(&ghost_dg,7,7,DGREY);
                            ghost_show(&ghost_dg);
                            score = score + 10; 
                        }
                    else {
                        uLCD.filled_circle((128 - (lives*10)), 10, GRID_RADIUS, BACKGROUND_COLOR);
                        lives = lives -1;
                        playSound("/sd/wavfiles/death.wav");
                            if (lives == 0){
                                game_on = false;
                            }
                        paccurrx = 8;
                        paccurry = 9;
                        pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                        ghost_init(&ghost_red,8,7,RED);
                        ghost_show(&ghost_red);
                        pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                        ghost_init(&ghost_dg,7,7,DGREY);
                        ghost_show(&ghost_dg);
                        }
                        }
                    if (paccurrx < 0) { //Movement rules when pacman approaches edge of map.
                        paccurrx = 17;
                        }
                    if (paccurrx > NUM_GRID_X-1) {
                        paccurrx = 0;
                        }
                    if (paccurry < 0) {
                        paccurry = NUM_GRID_Y-1;
                        }
                    if (paccurry == NUM_GRID_Y) {
                            paccurry = 0;
                        }
                    pacman_draw(paccurrx,paccurry, pacoldx, pacoldy );
                    }
            if (map_eat_cookie(paccurrx, paccurry)){    //Eat cookies function to count score for each cookie eaten.
                score = score + 1;
                }
            if (map_eat_supcookie(paccurrx, paccurry)){ //Super Cookie function to increase score and give pacman invincibility for
                score = score + 5;                      //for an amount of time, which decreases as level increases to a min of 1.
                invcount = ceil(10/level);
            }
                
                if (remaining_cookie == 0){ //End of map caused by eating all cookies. Pauses game, reloads map, pacman, and ghosts and continues.
                    game_on = false;
                    playSound("/sd/wavfiles/winning.wav");
                    lives = lives + 1;
                    uLCD.filled_circle((128-(lives*10)), 10, GRID_RADIUS, PACMAN_COLOR);
                    uLCD.filled_rectangle((128-(lives*10)),9,(128-(lives*10))+GRID_RADIUS,11, BACKGROUND_COLOR);
                    level = level + 1;
                    map_init();
                    pacman_clear(paccurrx, paccurry);
                    pacman_init(8,9);
                    paccurrx = 8;
                    paccurry = 9;
                    pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
                    ghost_init(&ghost_red,8,7,RED);
                    ghost_show(&ghost_red);
                    pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
                    ghost_init(&ghost_dg,7,7,DGREY);
                    ghost_show(&ghost_dg);
                    game_on = true;
                    
                    }      
        }
        
        /// 4. Implement the code to check the end of game.
            /// -[Hint] Check whether the ghost catch the Pacman. Make sure you could always detect that the ghost and Pacman meet on the screen.
            /// One tricky scenario is that: Pacman is at grid (3,3) and is moving to (3,4), while the ghost is at grid (3,4) and is moving to (3,3).
            /// Either at time t or t+1, you will not see the Pacman and the ghost on the same grid.
            /// No mater what, the Pacman should be caught by ghost in above scenario.
            /// Make sure your code could deal with above scenario correctly.
            /// -[Hint] Check whether Pacman win the game <br>
            
   
    if (game_on == false){          //Internal loop to pause game on score/game over screen. While the game_on function is on, game plays
        score_keeper(score);        //while off, game score screen. Score keeper saves current score into the next available slot in
        uLCD.locate(2,5);           //recent_scores array, and reloops when array is full.
        uLCD.printf("YOU'RE GHOST NOM");        //Game over message.
        for (temp = 0; temp < 5; temp++) {      //For loop to print recent scores array onto screen under game over message.
            uLCD.locate(5, 6+temp);             //These items outside of loop, but conditioned on game over so that they are only written once.
            uLCD.printf("%d", recent_scores[temp]);
        }
    while(game_on == false) {   //Stays in loop until button is pressed to continue.
        uLCD.locate(5, 13);
        uLCD.printf("Press Button To Play");
        uLCD.locate(5, 13);
        uLCD.printf("Press        To Play");    //Blinks message to push button while waiting, Old school pause screen!
        if (((left_pb == 0) && (right_pb == 1)) || ((left_pb == 1) && (right_pb == 0))) {   //Statement to check for button press. Begins init.
            game_on = true;
            uLCD.cls();
            lives = 3;
            uLCD.locate(10,0);
            uLCD.printf("Lives:");
            for (x = lives; x > 0; x--){
                xpos = 128 - (x*10);
                uLCD.filled_circle(xpos, 10, GRID_RADIUS, PACMAN_COLOR);
                uLCD.filled_rectangle(xpos,9,xpos+GRID_RADIUS,11, BACKGROUND_COLOR);
            }
            level = 1;
            map_init();
            pacman_clear(paccurrx, paccurry);
            pacman_init(8,9);
            paccurrx = 8;
            paccurry = 9;
            pacman_clear(int(ghost_red.ghost_blk_x), int(ghost_red.ghost_blk_y));
            ghost_init(&ghost_red,8,7,RED);
            ghost_show(&ghost_red);
            pacman_clear(int(ghost_dg.ghost_blk_x), int(ghost_dg.ghost_blk_y));
            ghost_init(&ghost_dg,7,7,DGREY);
            ghost_show(&ghost_dg);
            
            
            }
        }
        }         
    }
}

    
// Example of implementation of your functions
void playSound(char * wav)
{
    // open wav file
    FILE *wave_file;
    wave_file=fopen(wav,"r");

    if(wave_file == NULL){
        uLCD.locate(9,0);
        uLCD.printf("ERROR_SD");
        return;
    }
    
    // play wav file
    waver.play(wave_file);

    // close wav file
    fclose(wave_file);
}

void score_keeper(int new_score) {  //Score keeper function to handle scores vecotr.
    recent_scores[score_count] = new_score;
    if (score_count > 4){
        score_count = 1;
        }
        else {
        score_count = score_count + 1;
        }
}


