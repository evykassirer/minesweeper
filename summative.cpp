#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //for randomizing mines
#include <assert.h> //assert(assummption) for debuggin
#include <allegro.h>


/*const int RED = makecol32(255, 0, 0);
    const int ORANGE = makecol32(255, 69, 0);
    const int YELLOW = makecol32(255, 225, 0);
    const int LIME = makecol32(124, 252, 0);
    const int GREEN = makecol32(0, 100, 0);
    const int AQUA = makecol32(0, 225, 225);
    const int BLUE = makecol32(0, 0, 225);
    const int LILAC = makecol32(147, 112, 219);
    const int PURPLE = makecol32(148, 0, 211);
    const int WHITE = makecol32(255, 225, 225);
    
    int x = 0, y, w = 20, h = 20;
    y = 480;
    rectfill(screen, x, y, x+w, y+h, RED);
    y = 460;
    rectfill(screen, x, y, x+w, y+h, ORANGE);
    y = 440;
    rectfill(screen, x, y, x+w, y+h, YELLOW);
    y = 420;
    rectfill(screen, x, y, x+w, y+h, LIME);
    y = 400;
    rectfill(screen, x, y, x+w, y+h, GREEN);
    y = 380;
    rectfill(screen, x, y, x+w, y+h, AQUA);
    y = 360;
    rectfill(screen, x, y, x+w, y+h, BLUE);
    y = 340;
    rectfill(screen, x, y, x+w, y+h, LILAC);
    y = 320;
    rectfill(screen, x, y, x+w, y+h, PURPLE);
    y = 300;
    rectfill(screen, x, y, x+w, y+h, WHITE);
    y = 280;
    */




const int scrwidth = 480;
const int scrheight = 520;
const int side = 16;

//create mineboard
struct board {
    char display[100][100]; //opened mine is x, not opened is .
    int integer[100][100]; //empty is 0, mine is 9
    int minesLeftDisplay;
    int minesLeft;
};
board mineboard;

//button structure to check buttons
struct button {
    int top;
    int bottom;
    int left;
    int right;
};
//first menu buttons
const button instruct = {139, 201, 107, 373};
const button firstNewGame = {220, 283, 107, 373}; 
const button Open = {298, 360, 106, 372};
const button exitGame = {379, 443, 107, 373};

//board menu buttons
const button secondNewGame = {480, 520, 160, 320}; 
const button save = {480, 520, 0, 160};
const button back = {480, 520, 320, 480};

//instructions back button
const button back_instruct = {452, 494, 25, 133};
    
//functions
bool mainMenu();
void addMines(); //adds mines to the board
void displayboard(bool lose); //displays the board
bool guess(int x, int y); //takes in a guess and does whatever must be done to the board
int numMines(int x, int y); //finds the number of mines surrounding a square
void openSurround(int x, int y); //opens the squares surrounding a square (one with no mines around it)
void checkAndOpen(int x, int y); //used in openSurround to avoid repitition
void saveGame();
bool checkClick();
void NewGame();
void Instructions();
void openSaved();
bool checkButton(int x, int y, button option);
bool mainBoard();
bool lose();

                
// resources
BITMAP *Blank;
BITMAP *Square[9];
BITMAP *MinePicture;
BITMAP *Flag;
BITMAP *buffer;
BITMAP *BottomBar;
BITMAP *LoseBottomBar;

BITMAP *mainMenuScreen;
BITMAP *InstructionPage;
BITMAP *WinPage;


//MAIN
int main(int argc, char *argv[]) {  
    //initialize Allegro
    allegro_init();
    set_color_depth(32); 
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 480,520,0,0); 
    
    //install mouse
    install_mouse();
    enable_hardware_cursor();
    select_mouse_cursor(MOUSE_CURSOR_ARROW);
    show_mouse(screen);
    
    //load bitmaps
    Blank = load_bitmap("Blank.bmp", 0);
    BITMAP *numbers = load_bitmap("numbers.bmp", 0);
    for(int i = 0; i < 9; i++) {
        Square[i] = create_bitmap(30, 30);
        blit(numbers, Square[i], i*30, 0, 0, 0, 30, 30);
    }

    MinePicture = load_bitmap("Mine.bmp", 0);
    Flag = load_bitmap("Flag.bmp",0);
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    BottomBar = load_bitmap("Bottom Bar.bmp", 0);
    LoseBottomBar = load_bitmap("Lose Bottom Bar.bmp", 0);
    
    mainMenuScreen = load_bitmap("First menu.bmp", 0);
    InstructionPage = load_bitmap("Instructions.bmp",0);  
    WinPage = load_bitmap("Win.bmp",0);  
    
    while(1){
        if(mainMenu())
            return 0;
    
        if(mainBoard())
            return 0;
    }
}
END_OF_MAIN();

bool mainBoard() {
    //loop for user input
    displayboard(true);
    while(1) {
        bool mine = false;
        
        //check buttons
        if (mouse_b & 1 || mouse_b & 2) {
            if (checkButton(mouse_x, mouse_y, save)) {
                saveGame();
            }     
            else if (checkButton(mouse_x, mouse_y, secondNewGame)) {
                NewGame();
            }
            else if (checkButton(mouse_x, mouse_y, back)) {
                if(mainMenu())
                    return true;
            }
            //click and check guess
            else mine = checkClick();
            displayboard(true); //display normal board
        } 
        
        if (mine == true) {//if a mine was found in this loop
            if(lose()) //run loose and if the user wants to exit, true is returned to main
                return true;
            else return false; //otherwise, go back to the main function
        }
        else if (mineboard.minesLeft == 0){ //if the user won
            clear_bitmap(screen);
            blit(WinPage, screen, 0,0,0,0,480,520);
            while(1) {
                if (mouse_b & 1) {
                    if (checkButton(mouse_x, mouse_y, secondNewGame)){
                        NewGame();
                        return false;
                        break;
                    }
                    if (checkButton(mouse_x, mouse_y, back)){                    
                        return true;
                    }
                }
            }
        }   
    }
}

bool lose() {
    for(int i=0; i<side; i++){
        for(int j=0; j<side; j++) {
            if(mineboard.integer[i][j] == 9)
                mineboard.display[i][j] = 'x' ;
        }
    }
    displayboard(false);
    while(1) {
        if (mouse_b & 1) {
            if (checkButton(mouse_x, mouse_y, secondNewGame)){
                NewGame();
                return false;
                break;
            }
            if (checkButton(mouse_x, mouse_y, back)){                    
                return true;
            }
        }
    }
}
            
bool mainMenu(){
   
    //view menu
    blit(mainMenuScreen, screen, 0, 0, 0, 0, scrwidth, scrheight);
    
    do {
        if (mouse_b & 1){    
            //user chooses instructions
            if (checkButton(mouse_x, mouse_y, instruct)) {
                 Instructions();
            } 
            //user chooses new game
            else if (checkButton(mouse_x, mouse_y, firstNewGame)) {
                 NewGame();
                 return false;
            }
            //user chooses to open saved game
            else if (checkButton(mouse_x, mouse_y, Open)) {
                 openSaved();
                 return false;
            }
            else if (checkButton(mouse_x, mouse_y, exitGame)) {
                 return true;
            }
        }   
    }while(1);
}
END_OF_FUNCTION(mainMenu);

//open saved game
void openSaved() {
     char filename[30];
    printf("Enter your username\n\t");
    fflush(stdin);
    gets(filename);
    strcat(filename, ".txt");
    FILE *fptr;      
    if ((fptr = fopen(filename, "r"))) {  
        fscanf(fptr, "%d", &side); //read dimension 
        for (int i=0; i<side; i++) { //initalize values of the integer board
            for (int j=0; j<side; j++) {
                fscanf(fptr, "%d", &mineboard.integer[i][j]);
            }
        } 
        for (int i=0; i<side; i++) { //initalize values of the display board
            for (int j=0; j<side; j++) {
                fscanf(fptr, "%d", &mineboard.display[i][j]);
            }
        } 
    }
    else printf("\nThe file you entered cannot be found. Command Prompt will now close your program"); 
    fclose(fptr);
}

//makes the appropriate changes for a right or left click on the board
bool checkClick() {
     int i;
     int j;
     bool mine = false;
     bool click = true;
     do {
        i=mouse_x/30;
        j=mouse_y/30;     
        if (mouse_b & 1 && mineboard.display[i][j] != '#') {
            mine = guess(i,j);
            click = false;
            rest(100);
        }
        else if (mouse_b & 2){         
            if (mineboard.display[i][j] == '.'){
                mineboard.display[i][j] = '#';
                click = false;
                mineboard.minesLeftDisplay--;
                if (mineboard.display[i][j]) { //if the guess is right
                    mineboard.minesLeft--;
                }
                rest(200);
            }
            else if (mineboard.display[i][j] == '#') {
                mineboard.display[i][j] = '.';
                click = false;
                mineboard.minesLeftDisplay++;
                if (mineboard.display[i][j]) { //if the guess was right
                    mineboard.minesLeft++;
                }
                rest(200);
            }
        }
    }while(click);
    return mine; 
}

//processes a left click guess
bool guess(int x, int y) {
    bool mine = false;
    //if the guess is a mine
    if (mineboard.integer[x][y] == 9) {
        mineboard.display[x][y] = 'x';
        mine = true;
    }
    //if the guess isn't a mine
    else if (mineboard.integer[x][y] != 0) {
        mineboard.display[x][y] = mineboard.integer[x][y]+48;
    }
    //if the guess has no mines surrounding it
    else if(mineboard.integer[x][y] == 0){
        openSurround(x, y);
    }
    return mine;
}

//saves the game
void saveGame() {
    char username[30];
    
    system("cls");
    printf("SAVE GAME");
    printf("\nEnter your username: ");
    fflush(stdin);
    gets(username);
    strcat(username, ".txt");
    FILE *fptr;      
    fptr = fopen(username, "w");  
    fprintf(fptr, "%d ", side); //write dimension 
    for (int i=0; i<side; i++) { //save values of the integer board
        for (int j=0; j<side; j++) {
            fprintf(fptr, "%d ", mineboard.integer[i][j]);
        }
    } 
    for (int i=0; i<side; i++) { //save values of the display board
        for (int j=0; j<side; j++) {
            fprintf(fptr, "%d ", mineboard.display[i][j]);
        }
    } 
    fclose(fptr);
}

void NewGame(){
    //initalize display board to all closed
    for(int i=0; i<side; i++){
        for(int j=0; j<side; j++) {
            mineboard.display[i][j] = '.' ;
        }
    }
    //add mines the the integer board which has the information for the game
    addMines();   
    //integer board is finished by adding in the numbers for each square- how may mines surrounding it
    for(int i = 0; i<side; i++){
        for(int j = 0; j<side; j++) {
            if(mineboard.integer[i][j] !=9) {
                mineboard.integer[i][j] = numMines(i, j);
            }
        }
    }
    rest(100);
}

//opens instructions
void Instructions() {
    clear_bitmap(screen);
    blit(InstructionPage, screen, 0,0,0,0,480,520);
    while(1) {
        if (mouse_b & 1){
            if (checkButton(mouse_x, mouse_y, back_instruct)) {
                mainMenu();
                break;
            }
        }
    }
}    

//checks to see if button was clicked on
bool checkButton (int mouse_x, int mouse_y, button option) {
    if (mouse_y > option.top && mouse_y < option.bottom && mouse_x > option.left && mouse_x < option.right) return true;
    else return false;
}        
    
int numMines(int x, int y){
    int count = 0;   
    if(x != 0 || y != 0)
        if (mineboard.integer[x-1][y-1] == 9) count++;
    if(x != 0)
        if (mineboard.integer[x-1][y] == 9) count++;
    if(y != 0)
        if (mineboard.integer[x][y-1] == 9) count++;
    if(x+1 != side || y+1 != side)
        if (mineboard.integer[x+1][y+1] == 9) count++;
    if(x+1 != side)
        if (mineboard.integer[x+1][y] == 9) count++;
    if(y+1 != side)
        if (mineboard.integer[x][y+1] == 9) count++;
    if(x != 0 || y+1 != side)
        if (mineboard.integer[x-1][y+1] == 9) count++;
    if(y != 0 || x+1 != side)
        if (mineboard.integer[x+1][y-1] == 9) count++;
    return count;
}

void openSurround(int x, int y) {
    mineboard.display[x][y] = '0';        
    checkAndOpen(x-1, y-1);
    checkAndOpen(x-1, y);
    checkAndOpen(x, y-1);
    checkAndOpen(x+1, y+1);
    checkAndOpen(x+1, y);
    checkAndOpen(x, y+1);
    checkAndOpen(x-1, y+1);
    checkAndOpen(x+1, y-1);
}

void checkAndOpen(int x, int y) {
    //if the location of this if statement exists and hasn't already been opened
    if (x >= 0 && x < side && y >= 0 && y < side && mineboard.display[x][y] != '0' + mineboard.integer[x][y]) {
        mineboard.display[x][y] = mineboard.integer[x][y] + '0'; //open it
        if (mineboard.integer[x][y] == 0) openSurround(x, y); //if it's also 0, open around it
    }
}

void addMines() {
    mineboard.minesLeft = 0;
    mineboard.minesLeftDisplay = 0;
    
    //assign numbers 0-255 to the array
    for(int i=0; i<side; i++){
        for(int j=0; j<side; j++) {
            mineboard.integer[i][j] = side*i + j;
        }
    }
    
    //randomly switch two points until the order is completely random
    srand(time(0));
    int randX1, randX2, randY1, randY2, temp;
    for(int i = 0; i<10000; i++) {
        randX1 = rand()%side;
        randX2 = rand()%side;
        randY1 = rand()%side;
        randY2 = rand()%side;
        temp = mineboard.integer[randX1][randY1];
        mineboard.integer[randX1][randY1] = mineboard.integer[randX2][randY2];
        mineboard.integer[randX2][randY2] = temp;
    }
    
    for(int i=0; i<side; i++){
        for(int j=0; j<side; j++) {
            if(mineboard.integer[i][j] < (5*side)/2) {
                mineboard.integer[i][j] = 9;
                mineboard.minesLeft++;
                mineboard.minesLeftDisplay++;
            }
            else mineboard.integer[i][j] = 0;
        }
    }
}

//DISPLAY THE BOARD (after each guess)
void displayboard(bool lose) {
    int i = 0;
    int j = 0;
    int xcoordinate = 0;
    int ycoordinate = 0;

    //print board
    for (i = 0; i < side; i++) {
        xcoordinate = i*30;
        for (j = 0; j < side; j++){
            ycoordinate = j*30;
            switch (mineboard.display[i][j]) {
                case '.' : draw_sprite(buffer, Blank, xcoordinate, ycoordinate); break;
                case 'x' : draw_sprite(buffer, MinePicture, xcoordinate, ycoordinate); break;
                case '0' ... '9' : draw_sprite(buffer, Square[mineboard.integer[i][j]], xcoordinate, ycoordinate); break;
                case '#' : draw_sprite(buffer, Flag, xcoordinate, ycoordinate);
            }
        }
    }
    clear_bitmap(screen);
    if(lose) {
        blit(BottomBar, buffer, 0, 0, 0, scrwidth, scrheight-40, scrheight);
    }
    else {
        blit(LoseBottomBar, buffer, 0, 0, 0, scrwidth, scrheight-40, scrheight);
    }
    blit(buffer, screen, 0, 0, 0, 0, scrwidth, scrheight);
}
END_OF_FUNCTION(displayboard);

/*List of things to do:
       *save game
       *Configure back button of Page
       *Make page work
       *Configure Win Loss
       *Figure out why Allegro is so slow
       *Put destruction code in
*/
