//Ritaj Suleman
//2D word shooter game
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include "util.h"
#include <cstdlib> // For system() -> background music

using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

string * dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27 // A

int remainingTime=150;

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int shooterX = 460; // Initial X-coordinate of the shooter
int shooterY = 20;  // Initial Y-coordinate of the shooter (near the bottom)

bool isMouseClicked=false;// to track if mouse is clicked

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset  /*-bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
int **board; // 2D-arrays for holding the data...
char **grid;// to store board in char 
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;

bool isAlphabetMoving = false; // Tracks if the alphabet is in motion
float alphabetX = shooterX; // X position of the moving alphabet
float alphabetY = shooterY; // Y position of the moving alphabet
float velocityX = 0.0f; // X velocity of the alphabet
float velocityY = 0.0f; // Y velocity of the alphabet

float targetX = 0.0f, targetY = 0.0f; // Declare target position variables

void playBackgroundMusic() {
    // Play the WAV file in the background using 'aplay'
    system("aplay background_music.wav &");
}

void stopBackgroundMusic() {
    // Stop the 'aplay' process when needed
    system("killall aplay");
}

enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...

int GetAlphabet() {
	return GetRandInRange(0, 26);
}
int alphashooter=GetAlphabet();

void Pixels2Cell(int px, int py, int & cx, int &cy) {
cx = px / (2 * bradius); 
cy = py / (2 * bradius); 
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
px = cx * (2 * bradius) + bradius; 
py = cy * (2 * bradius) + bradius; 
}

void addword2file(const string& word) {
    ofstream file("wordsformed.txt", ios::out|ios::app); 
    if (file.is_open()) {
        file << word << " (" << word.length() << " points)" << endl;
        file.close();
    } else {
        cout << "Error: Unable to open file!" << endl;
    }
}

char numToChar(int num) {
    return (num == -1) ? '-' : (char)(num + 'a');  // Assumes 0 = 'a', 1 = 'b', ...
}

bool checkWordInGrid(const string &word, char ** grid, int row, int col, int dirX, int dirY, bool removeWord) {
    int wordLen = word.length();
     // Check if the word fits in the grid from (x, y) in direction (dirx, diry)
    for (int i = 0; i < wordLen; ++i) {
        int nextX = row + i * dirX;
        int nextY = col + i * dirY;
          // Check if we are out of bounds
        if (nextX < 0 || nextX >= nycells || nextY < 0 || nextY >= nxcells) {
            return false;
        }
        // Check if the current letter matches the grid letter
        if (grid[nextX][nextY] != word[i]) {
            return false;
        }
    }
    // If removeWord is true, add score, remove it and replace with -1
    if (removeWord) {
    score+=wordLen;
        for (int i = 0; i < wordLen; ++i) {
            int nextX = row + i * dirX;
            int nextY = col + i * dirY;
            if ( nextX==8 || nextX==9)//if first two rows , replace with an alphabet
            {
            board[nextX][nextY] = GetAlphabet(); 
            grid[nextX][nextY]=numToChar(board[nextX][nextY]);//place an alphabet
            }
            else
            {
            board[nextX][nextY] = -1; 
            grid[nextX][nextY]='-';// Mark as empty
            } 
        }
    }
    return true;
}
bool isWordInGrid(const string &word, char ** grid) {
    for (int row = nycells-1; row >=0; --row) {
        for (int col = 0; col < nxcells; ++col) {
            // Check all directions
            if (checkWordInGrid(word, grid, row, col, 0, 1,true))
            return true;  // Horizontal
            if (checkWordInGrid(word, grid, row, col, -1, 0,true)) 
            return true;  // Vertical
            if (checkWordInGrid(word, grid, row, col, 1, 1,true)) 
            return true;  // Diagonal (up-left to right)
            if (checkWordInGrid(word, grid, row, col, -1, 1,true)) 
            return true; // Diagonal (down-left to right)
        }
    }
    return false;
}

void wordValidation()
{
for (int i=nycells-1; i>=0;i--)
{
  for (int j=0; j<nxcells; j++)
  {
  grid[i][j]=numToChar(board[i][j]);
  cout<<grid[i][j]<<"  ";
  }
  cout<<endl;
}

 for (int i = 0; i < dictionarysize; ++i) {
        if (isWordInGrid(dictionary[i], grid)) {
        cout<<dictionary[i]<<endl;
            addword2file(dictionary[i]);
        }
    }
}

void InitializeBoard() {
    board = new int*[nycells]; // Allocate rows
    for (int cy = 0; cy < nycells; cy++) {
        board[cy] = new int[nxcells]; // Allocate columns
        for (int cx = 0; cx < nxcells; cx++) {
            board[cy][cx] = -1; // Initialize all cells to empty (-1)
        }
    }


//grid =characters, board= integers values
    grid = new char*[nycells];  
    for (int i = 0; i < nycells; i++) {
        grid[i] = new char[nxcells];  
    }
    
    board[8][0] = GetAlphabet();  
    board[8][1] = GetAlphabet();  
    board[8][2] = GetAlphabet();  
    board[8][3] = GetAlphabet(); 
    board[8][4] = GetAlphabet();
    board[8][5] = GetAlphabet(); 
    board[8][6] = GetAlphabet(); 
    board[8][7] = GetAlphabet();
    board[8][8] = GetAlphabet(); 
    board[8][9] = GetAlphabet(); 
    board[8][10] = GetAlphabet(); 
    board[8][11] = GetAlphabet(); 
    board[8][12] = GetAlphabet();
    board[8][13] = GetAlphabet(); 
    board[8][14] = GetAlphabet();  
    board[9][0] = GetAlphabet();
    board[9][1] = GetAlphabet();
    board[9][2] = GetAlphabet();
    board[9][3] = GetAlphabet();
    board[9][4] = GetAlphabet();  
    board[9][5] = GetAlphabet(); 
    board[9][6] = GetAlphabet(); 
    board[9][7] = GetAlphabet();
    board[9][8] = GetAlphabet();
    board[9][9] = GetAlphabet();
    board[9][10] = GetAlphabet(); 
    board[9][11] = GetAlphabet(); 
    board[9][12] = GetAlphabet();
    board[9][13] = GetAlphabet();
    board[9][14] = GetAlphabet(); 
    
    wordValidation();
    

}

//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; i++) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}

void InitializeNewAlphabet() {
    // Reset the alphabet's position to the shooter
    alphabetX = shooterX;
    alphabetY = shooterY;

    // Generate a new random alphabet
    alphashooter = GetAlphabet();

    // Reset velocity 
    velocityX = 0.0f;
    velocityY = 0.0f;

    // Mark the new alphabet as not in motion
    isAlphabetMoving = false;
}

void UpdateAlphabetPosition() {
    if (isAlphabetMoving) {
        // Update the position of the alphabet
        alphabetX += velocityX; 
        alphabetY += velocityY;

        // Check if the alphabet is close enough to the mouse-clicked target
        float distanceToTarget = sqrt(pow(alphabetX - targetX, 2) + pow(alphabetY - targetY, 2));
        if (distanceToTarget < 1.0f) { // Threshold distance for stopping
            isAlphabetMoving = false; // Stop movement
            alphabetX = targetX; // Snap to target position
            alphabetY = targetY;
        }

        // Collision detection with walls
        if (alphabetX <= 0 || alphabetX >= width - (2 * bradius)) {
            velocityX = -velocityX; // Reflect horizontally
        }

        if (alphabetY >= height - bradius) { // Top boundary collision
            velocityY = 0.0f; // Stop movement 
            isAlphabetMoving = false; // Stop the alphabet
            InitializeNewAlphabet();
        }

        // Collision detection with other alphabets
        for (int cy = 0; cy < nycells; cy++) {
            for (int cx = 0; cx < nxcells; cx++) {
                if (board[cy][cx] != -1) { // If a cell is occupied
                    int cellX, cellY;
                    Cell2Pixels(cx, cy, cellX, cellY);
                    float distance = sqrt(pow(alphabetX - cellX, 2) + pow(alphabetY - cellY, 2));
                    if (distance <= bradius * 2) {
                        // Snap the alphabet into position
                        isAlphabetMoving = false;
                        velocityX = velocityY = 0;
                        Pixels2Cell(alphabetX, alphabetY, cx, cy);
                        board[cy][cx] = alphashooter; // Place the alphabet on the board
                        wordValidation();
                        InitializeNewAlphabet();
                        break;
                    }
                }
            }
        }
    }
}


void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
      glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(255.0f / 255.0f, 224.0f / 255.0f, 233.0f / 255.0f, 1.0f);


	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}


void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(255.0f / 255.0f, 224.0f / 255.0f, 233.0f / 255.0f, 1.0f);


	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */
void DrawBoard() {
    for (int row = 0; row < nycells; row++) {
        for (int col = 0; col < nxcells; col++) {
            if (board[row][col] != -1) { // If the cell is not empty
                int cellX, cellY;
                Cell2Pixels(col, row, cellX, cellY);
                DrawAlphabet((alphabets)board[row][col], cellX, cellY, awidth, aheight);
            }
        }
    }
}


void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(255.0f / 255.0f, 224.0f / 255.0f, 233.0f / 255.0f, 1.0f); // Background color
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...
	DrawBoard();
	UpdateAlphabetPosition();
	
        if (isAlphabetMoving) {
        DrawAlphabet((alphabets)alphashooter, alphabetX, alphabetY, awidth, aheight);
    } else {
        // If no alphabet is moving, place a new one at the shooter
        DrawAlphabet((alphabets)alphashooter, shooterX, shooterY, awidth, aheight);
    }
	
	DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[TEAL]);
	DrawString(width / 2 - 30, height - 25, width, height,
		"Time Left:" + Num2Str(remainingTime) + " secs", colors[DEEP_PINK]);
	DrawString(width-240 , height - 20, width, height + 5, "Ritaj Suleman 24i-0112", colors[DARK_MAGENTA]);

	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...
             if (shooterX-bradius>0)//check left boundary 
             {
             shooterX-=10;//move shooter left
             }
	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
        if (shooterX+bradius<width)//check right boundary 
             {
             shooterX+=10;//move shooter right
             }
	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	if (shooterY + aheight < height) { 
            shooterY += 10; // Move shooter up
        }
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	if (shooterY - aheight > 0) { 
            shooterY -= 10; // Move shooter down
        }
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	
	glutPostRedisplay();
	
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls
      //update shooterX based on mouse's X position
     // if (isMouseClicked) { 
      //if ( x > bradius && x < width - bradius){
     // shooterX=x;
     // }
    //  }
      glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */
void MouseClicked(int button, int state, int x, int y) {

if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !isAlphabetMoving) {
        // Adjust y-coordinate for OpenGL's bottom-left origin
       
       y = height - y; // Invert y-coordinate
      
        // Set the target position to the mouse click
        targetX = x;
        targetY = y;

        // Set the starting position of the alphabet to the shooter's position
        alphabetX = shooterX;
        alphabetY = shooterY;

        // Calculate the direction vector (deltaX, deltaY)
        float deltaX = targetX - shooterX;
        float deltaY = targetY - shooterY;

        // magnitude of vector distance
        float magnitude = sqrt(deltaX * deltaX + deltaY * deltaY);
        if (magnitude != 0) {
            // unit vector: normalized velocity 
            velocityX = (deltaX / magnitude) * 10.0f; // 10 is for to control the speed
            velocityY = (deltaY / magnitude) * 10.0f;
            isAlphabetMoving = true; // Start the alphabet's movement
        }
    }
    else if (state == GLUT_UP) {
            isMouseClicked = false; // Reset the flag when mouse button is released
        }
        glutPostRedisplay(); // Redraw the screen with updated shooter position
    }

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
}

/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {
if (remainingTime>0)
{
       remainingTime--;
      UpdateAlphabetPosition(); // Update the moving alphabet's position
	glutPostRedisplay();
	glutTimerFunc(1000.0/*/FPS*/, Timer, 0);//calls this function at every second.
}
else
{
   stopBackgroundMusic();
   cout<<"Game Over! Time's up."<<endl;
   cout<<"Score: "<<score<<endl;
   //dellocation of board array
   for (int i = 0; i < nycells; i++) {
    delete[] board[i]; 
   }
  delete[] board; 
   //dellocation of grid array
   for (int i = 0; i < nycells; i++) {
    delete[] grid[i]; 
   }
  delete[] grid; 
  
  board = nullptr;
  grid = nullptr;
   exit(0);
}
}

/*
* our gateway main function
* */
int main(int argc, char*argv[]) {
	InitRandomizer(); // seed the random number generator...
       
	//Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary
	for(int i=0; i < 5; ++i)
		cout<< " word "<< i << " =" << dictionary[i] <<endl;

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets



	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("Ritaj's Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	InitializeBoard(); // Initialize the game grid
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse
	
       playBackgroundMusic(); // Start the background music
	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* */
