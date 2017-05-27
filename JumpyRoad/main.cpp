//
//  main.cpp
//  JumpyRoad
//
//  Created by Miles Luders on 5/9/15.
//  Copyright (c) 2015 Miles Luders. All rights reserved.
//

#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#include <random>

using namespace std;

vector<char> singleRow;
vector<vector<char> > listOfRows;

float gameZ = -120;
float gameX = 4;

float playerX = 0;
float playerY = -5;
float playerZ = 4;

int playerRelativeToGridX = 7;
int playerRelativeToGridZ = 31;

int playerDirection = 1;

bool canJump = true;
int jumpTimer = 0;
bool isCollisionUp = false, isCollisionDown = false, isCollisionLeft = false, isCollisionRight = false;

float finalX, finalZ;

bool playerIsDead = false;
bool playerHasWon = false;
float deathTimer = 0;
float winTimer = 0;

int score = 0;

GLuint textureGrass;

// Fog declarations
static GLint fogMode;
bool showFog = true;

// Car class
class Car {
    
public:
    Car(float inputX, float inputZ, float inputSpeed, int inputDirection, int inputLane);
    void Update();
    float x = -60.0, y = -5.0, z;
    int type, direction, lane;
    
private:
    float speed;
    
    
};

Car::Car(float inputX, float inputZ, float inputSpeed, int inputDirection, int inputLane) {
    x = inputX;
    z = inputZ;
    speed = inputSpeed;
    direction = inputDirection;
    lane = inputLane;
    
    type = (rand() % ((3 - 0) + 3)) + 0;
    
}

void Car::Update() {
    
    /*cout << "My lane is: " << lane << endl;
     cout << "Player lane is: " << playerRelativeToGridZ << endl;*/
    
    /*cout << "Player x is " << finalX << endl;
     cout << "my x is " << x << endl;*/
    
    
    if (direction == 0) {
        x+=speed;
        if (x > 60) {
            x = -60.0;
            type = (rand() % ((3 - 0) + 3)) + 0;
        }
    }
    else {
        x-=speed;
        if (x < -60) {
            x = 60;
            type = (rand() % ((3 - 0) + 3)) + 0;
        }
    }
    
    if (playerRelativeToGridX*4 > x+28 && playerRelativeToGridX*4 < x+32 && lane == playerRelativeToGridZ) {
        playerIsDead = true;
    }
}

vector<Car> vectorOfCars;

// Load a bitmap image to be used for texturing
GLuint LoadBMP(const char *fileName)
{
    FILE *file;
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int size;
    unsigned int width, height;
    unsigned char *data;
    
    
    file = fopen(fileName, "rb");
    
    if (file == NULL)
    {
        cout << "Invalid file 1" << endl;
        return false;
    }
    
    if (fread(header, 1, 54, file) != 54)
    {
        cout << "Invalid file 2" << endl;
        return false;
    }
    
    if (header[0] != 'B' || header[1] != 'M')
    {
        cout << "Invalid file 3" << endl;
        return false;
    }
    
    dataPos		= *(int*)&(header[0x0A]);
    size		= *(int*)&(header[0x22]);
    width		= *(int*)&(header[0x12]);
    height		= *(int*)&(header[0x16]);
    
    if (size == 0)
        size = width * height * 3;
    if (dataPos == 0)
        dataPos = 54;
    
    data = new unsigned char[size];
    
    fread(data, 1, size, file);
    
    fclose(file);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    return texture;
}

void InitDraw() {
    
    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    GLfloat light_position[] = { 0.0, 30.0, -15.0, 0.0 };
    GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
    glShadeModel (GL_SMOOTH);
    
    glMaterialfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_LIGHT0, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_COLOR_MATERIAL);
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    // Set the clear color
    glClearColor (38/256.0, 166/256.0, 91/256.0, 0.0);
    
    // Set the fill color
    glColor3f(1.0, 1.0, 1.0);
    
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(30, 1.333333, 0.1, 1000);
    
    // Fog color
    glClearColor(0.5, 0.5, 0.5, 1.0);
    
    
    
    // Enable and load textures
    //glEnable(GL_TEXTURE_2D);
    
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    
    textureGrass = LoadBMP("grass.bmp");
}

void LudersCube(float size, float posX, float posY, float posZ, float scaleX, float scaleY, float scaleZ, GLuint texture) {
    
    //glTranslatef(posX, (-7.0f+((size*scaleY)/2.0f)), posZ);
    glTranslatef(posX, posY, posZ);
    /*glScalef(scaleX, scaleY, scaleZ);
     glutSolidCube(size);*/
    
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glBegin(GL_QUADS);
    
    //glColor3f(0.0f,1.0f,0.0f);    // Color Blue
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Top)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Top)
    glVertex3f(-scaleX, scaleY, scaleZ);    // Bottom Left Of The Quad (Top)
    glVertex3f( scaleX, scaleY, scaleZ);    // Bottom Right Of The Quad (Top)
    
    //glColor3f(scaleX,0.5f,0.0f);    // Color Orange
    glVertex3f( scaleX,-1.0f, scaleZ);    // Top Right Of The Quad (Bottom)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Top Left Of The Quad (Bottom)
    glVertex3f(-scaleX,-1.0f,-scaleZ);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( scaleX,-1.0f,-scaleZ);    // Bottom Right Of The Quad (Bottom)
    
    //glColor3f(scaleX,0.0f,0.0f);    // Color Red
    glVertex3f( scaleX, scaleY, scaleZ);    // Top Right Of The Quad (Front)
    glVertex3f(-scaleX, scaleY, scaleZ);    // Top Left Of The Quad (Front)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Bottom Left Of The Quad (Front)
    glVertex3f( scaleX,-1.0f, scaleZ);    // Bottom Right Of The Quad (Front)
    
    //glColor3f(scaleX,1.0f,0.0f);    // Color Yellow
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Back)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Back)
    glVertex3f(-scaleX, -1.0f,-scaleZ);    // Bottom Left Of The Quad (Back)
    glVertex3f( scaleX, -1.0f,-scaleZ);    // Bottom Right Of The Quad (Back)
    
    //glColor3f(0.0f,0.0f,1.0f);    // Color Blue
    glVertex3f(-scaleX, scaleY, scaleZ);    // Top Right Of The Quad (Left)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Left)
    glVertex3f(-scaleX,-1.0f,-scaleZ);    // Bottom Left Of The Quad (Left)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Bottom Right Of The Quad (Left)
    
    // glColor3f(1.0f,0.0f,1.0f);    // Color Violet
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Right)
    glVertex3f( scaleX, scaleY, scaleZ);    // Top Left Of The Quad (Right)
    glVertex3f( scaleX,-1.0f, scaleZ);    // Bottom Left Of The Quad (Right)
    glVertex3f( scaleX,-1.0f,-scaleZ);    // Bottom Right Of The Quad (Right)
    
    glEnd();
}


void Reshape(int w, int h) {
    
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;
    float ratio = 1.0* w / h;
    
    // Use the Projection Matrix
    glMatrixMode(GL_PROJECTION);
    
    // Reset Matrix
    glLoadIdentity();
    
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    
    // Set the correct perspective.
    gluPerspective(30,ratio,1,1000);
    
    // Get Back to the Modelview
    glMatrixMode(GL_MODELVIEW);
}

void GenerateLevelFromFile() {
    
    ifstream ifs("level.txt");
    
    string line;
    
    while(getline(ifs, line))
    {
        singleRow.clear();
        
        for(int i = 0; i < line.length(); ++i)
        {
            singleRow.push_back(line[i]);
            //cout << line[i];
        }
        
        listOfRows.push_back(singleRow);
        //cout << endl;
    }
}

void CreateCar(float x, float y, float z, int type, int direction) {
    
    int facing;
    if (direction == 0)
        facing = 1;
    else
        facing = -1;
    
    switch(type) {
            
            //Small car
        case 0:
            // Body
            glPushMatrix();
            glColor3f(25.0/255.0, 222.0/255.0, 254.0/255.0);
            glTranslatef(x+gameX, y-1.25, z+120+gameZ);
            glScaled(1.2, .5, .8);
            glutSolidCube(3);
            glPopMatrix();
            
            // Upper body
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX, y+.25, z+120+gameZ);
            glScaled(.7, .4, .5);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window side
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX, y-.15, z+120+gameZ);
            glScaled(.5, .25, .51);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window front/back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX, y-.15, z+120+gameZ);
            glScaled(.71, .25, .4);
            glutSolidCube(3);
            glPopMatrix();
            
            // Side mirrors
            glPushMatrix();
            glColor3f(4.0/255.0, 187.0/255.0, 253.0/255.0);
            glTranslatef(x+gameX+(.25*facing), y-.75, z+120+gameZ);
            glScaled(.3, .15, 1);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels front
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX+(1*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, .815);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX+(1*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, .816);
            glutSolidCube(3);
            glPopMatrix();
            
            
            // Wheels back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(1*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, .815);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(1*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, .816);
            glutSolidCube(3);
            glPopMatrix();
            break;
            
            // Medium car - orange
        case 1:
            // Body
            glPushMatrix();
            glColor3f(255.0/255.0, 112.0/255.0, 54.0/255.0);
            glTranslatef(x+gameX, y-1.25, z+120+gameZ);
            glScaled(2.2, .5, 1);
            glutSolidCube(3);
            glPopMatrix();
            
            // Upper body
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(.8*facing), y+.25, z+120+gameZ);
            glScaled(1, .4, .75);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window side
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(.5, .25, .751);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window front/back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(1.1, .25, .5);
            glutSolidCube(3);
            glPopMatrix();
            
            // Side mirrors
            glPushMatrix();
            glColor3f(255.0/255.0, 76.0/255.0, 43.0/255.0);
            glTranslatef(x+gameX+(.25*facing), y-.75, z+120+gameZ);
            glScaled(.3, .15, 1.25);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels front
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            
            
            // Wheels back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            break;
            
            // Medium car purple
        case 2:
            // Body
            glPushMatrix();
            glColor3f(161.0/255.0, 141.0/255.0, 255.0/255.0);
            glTranslatef(x+gameX, y-1.25, z+120+gameZ);
            glScaled(2.2, .5, 1);
            glutSolidCube(3);
            glPopMatrix();
            
            // Upper body
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(.8*facing), y+.25, z+120+gameZ);
            glScaled(1, .4, .75);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window side
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(.5, .25, .751);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window front/back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(1.1, .25, .5);
            glutSolidCube(3);
            glPopMatrix();
            
            // Side mirrors
            glPushMatrix();
            glColor3f(147.0/255.0, 97.0/255.0, 255.0/255.0);
            glTranslatef(x+gameX+(.25*facing), y-.75, z+120+gameZ);
            glScaled(.3, .15, 1.25);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels front
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            
            
            // Wheels back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            break;
            
            // Taxi
        case 3:
            // Body
            glPushMatrix();
            glColor3f(255.0/255.0, 254.0/255.0, 101.0/255.0);
            glTranslatef(x+gameX, y-1.25, z+120+gameZ);
            glScaled(2.2, .5, 1);
            glutSolidCube(3);
            glPopMatrix();
            
            // Upper body
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(.8*facing), y+.25, z+120+gameZ);
            glScaled(1, .4, .75);
            glutSolidCube(3);
            glPopMatrix();
            
            // Light
            glPushMatrix();
            glColor3f(255.0/255.0, 254.0/255.0, 101.0/255.0);
            glTranslatef(x+gameX-(.8*facing), y+.4, z+120+gameZ);
            glScaled(.5, .4, .25);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window side
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(.5, .25, .751);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window front/back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(1.1, .25, .5);
            glutSolidCube(3);
            glPopMatrix();
            
            // Side mirrors
            glPushMatrix();
            glColor3f(255.0/255.0, 233.0/255.0, 95.0/255.0);
            glTranslatef(x+gameX+(.25*facing), y-.75, z+120+gameZ);
            glScaled(.3, .15, 1.25);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels front
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            break;
            
            // Medium car - green
        case 4:
            // Body
            glPushMatrix();
            glColor3f(186.0/255.0, 241.0/255.0, 111.0/255.0);
            glTranslatef(x+gameX, y-1.25, z+120+gameZ);
            glScaled(2.2, .5, 1);
            glutSolidCube(3);
            glPopMatrix();
            
            // Upper body
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(.8*facing), y+.25, z+120+gameZ);
            glScaled(1, .4, .75);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window side
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(.5, .25, .751);
            glutSolidCube(3);
            glPopMatrix();
            
            // Window front/back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(.8*facing), y-.15, z+120+gameZ);
            glScaled(1.1, .25, .5);
            glutSolidCube(3);
            glPopMatrix();
            
            // Side mirrors
            glPushMatrix();
            glColor3f(145.0/255.0, 215.0/255.0, 93.0/255.0);
            glTranslatef(x+gameX+(.25*facing), y-.75, z+120+gameZ);
            glScaled(.3, .15, 1.25);
            glutSolidCube(3);
            glPopMatrix();
            
            // Wheels front
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX+(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            
            
            // Wheels back
            glPushMatrix();
            glColor3f(0, 0, 0);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.3, .3, 1.1);
            glutSolidCube(3);
            glPopMatrix();
            
            glPushMatrix();
            glColor3f(1, 1, 1);
            glTranslatef(x+gameX-(2*facing), y-1.6, z+120+gameZ);
            glScaled(.125, .125, 1.11);
            glutSolidCube(3);
            glPopMatrix();
            break;
    }
}

void SpecialKeys(int key, int x, int y) {
    
    switch(key) {
            
        case GLUT_KEY_UP:
            
            if (canJump == true)
            {
                switch(listOfRows[playerRelativeToGridZ-1][playerRelativeToGridX])
                {
                    case '1':
                        break;
                    default:
                        finalZ = gameZ +4;
                        playerDirection = 1;
                        canJump = false;
                        playerRelativeToGridZ -= 1;
                        break;
                }
                
            }
            break;
            
        case GLUT_KEY_LEFT:
            
            if (canJump == true && playerRelativeToGridX > 0)
            {
                switch(listOfRows[playerRelativeToGridZ][playerRelativeToGridX-1]) {
                    case '1':
                        break;
                        
                    default:
                        finalX = gameX + 4;
                        playerDirection = 2;
                        canJump = false;
                        playerRelativeToGridX -= 1;
                        break;
                }
                
            }
            break;
            
        case GLUT_KEY_RIGHT:
            
            if (canJump == true && playerRelativeToGridX < 15)
            {
                switch(listOfRows[playerRelativeToGridZ][playerRelativeToGridX+1]) {
                    case '1':
                        break;
                        
                    default:
                        finalX = gameX - 4;
                        playerDirection = 0;
                        canJump = false;
                        playerRelativeToGridX += 1;
                        break;
                }
            }
            break;
            
        case GLUT_KEY_DOWN:
            
            if (canJump == true && playerRelativeToGridZ < 31)
            {
                switch(listOfRows[playerRelativeToGridZ+1][playerRelativeToGridX]) {
                    case '1':
                        break;
                        
                    default:
                        finalZ = gameZ - 4;
                        playerDirection = 3;
                        canJump = false;
                        playerRelativeToGridZ += 1;
                        break;
                }
            }
            break;
    }
    
    glutPostRedisplay();
}

void HandleDeath() {
    
    gameZ = -120;
    gameX = 4;
    
    playerX = 0;
    playerY = -5;
    playerZ = 4;
    
    playerRelativeToGridX = 7;
    playerRelativeToGridZ = 31;
    
    playerDirection = 1;
    
    canJump = true;
    jumpTimer = 0;
    isCollisionUp = false;
    isCollisionDown = false;
    isCollisionLeft = false;
    isCollisionRight = false;
    
    playerIsDead = false;
    deathTimer = 0;
    
    cout << "You died!" << endl;
    
}

void HandleWin() {
    
    gameZ = -120;
    gameX = 4;
    
    playerX = 0;
    playerY = -5;
    playerZ = 4;
    
    playerRelativeToGridX = 7;
    playerRelativeToGridZ = 31;
    
    playerDirection = 1;
    
    canJump = true;
    jumpTimer = 0;
    isCollisionUp = false;
    isCollisionDown = false;
    isCollisionLeft = false;
    isCollisionRight = false;
    
    playerIsDead = false;
    winTimer = 0;
    playerHasWon = false;
    deathTimer = 0;
    
    cout << "You won!" << endl;
    
    score ++;
    cout << "Score: " << score << endl;
    
}

void Update(void) {
    
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    gluLookAt(0,70,40,0,0,0,0,1,0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //    // Handle collisions
    //    switch(listOfRows[playerRelativeToGridZ][playerRelativeToGridX])
    //    {
    //        case '3':
    //            playerIsDead = true;
    //            break;
    //        case '5':
    //            playerHasWon = true;
    //        default:
    //            break;
    //    }
    //
    //    if (playerIsDead == true) {
    //        deathTimer += 5;
    //    }
    //
    //    if (deathTimer > 30) {
    //        HandleDeath();
    //    }
    //
    //    if (playerHasWon == true) {
    //        winTimer += 5;
    //    }
    //
    //    if (winTimer > 30) {
    //        HandleWin();
    //    }
    //
    // Handle player movement
    if (canJump == false) {
        
        switch(playerDirection) {
                
                // Right
            case 0:
                gameX -= 0.4;
                
                if (gameX > finalX + 2) {
                    
                    playerY += 0.7;
                }
                else {
                    playerY -= 0.7;
                }
                
                if (gameX < finalX) {
                    
                    gameX = finalX;
                    playerY = -5;
                    canJump = true;
                }
                break;
                
                // Up
            case 1:
                gameZ += 0.4;
                
                if (gameZ < finalZ -2) {
                    
                    playerY += 0.7;
                }
                else {
                    playerY -= 0.7;
                }
                
                if (gameZ > finalZ) {
                    
                    gameZ = finalZ;
                    playerY = -5;
                    canJump = true;
                }
                break;
                
                
                // Left
            case 2:
                gameX += 0.4;
                
                if (gameX < finalX -2) {
                    
                    playerY += 0.7;
                }
                else {
                    playerY -= 0.7;
                }
                
                if (gameX > finalX) {
                    
                    gameX = finalX;
                    playerY = -5;
                    canJump = true;
                }
                break;
                
                // Down
            case 3:
                gameZ -= 0.4;
                
                if (gameZ > finalZ + 2) {
                    
                    playerY += 0.7;
                }
                else {
                    playerY -= 0.7;
                }
                
                if (gameZ < finalZ) {
                    
                    gameZ = finalZ;
                    playerY = -5;
                    canJump = true;
                }
                break;
        }
        
    }
    
    // Draw the grid
    for (size_t i = 0; i < listOfRows.size(); i++) {
        
        for (size_t j = 0; j < 16; j++)
        {
            
            switch(listOfRows[i][j]) {
                    
                    // Grass
                case '0':
                    glPushMatrix();
                    glColor3f(190.0/255.0, 245.0/255.0, 102.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -6.8, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, .15, 1);
                    glRotated(90, 1, 0, 0);
                    glBegin(GL_QUADS);
                    glTexCoord2i(0, 0); glVertex2i(-2, -2);
                    glTexCoord2i(0, 1); glVertex2i(-2, 2);
                    glTexCoord2i(1, 1); glVertex2i(2, 2);
                    glTexCoord2i(1, 0); glVertex2i(2, -2);
                    glEnd();
                    glPopMatrix();
                    break;
                    
                    
                    // Tree - large
                case '1':
                    
                    glPushMatrix();
                    glColor3f(190.0/255.0, 245.0/255.0, 102.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -6.8, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, .15, 1);
                    glRotated(90, 1, 0, 0);
                    glBegin(GL_QUADS);
                    glTexCoord2i(0, 0); glVertex2i(-2, -2);
                    glTexCoord2i(0, 1); glVertex2i(-2, 2);
                    glTexCoord2i(1, 1); glVertex2i(2, 2);
                    glTexCoord2i(1, 0); glVertex2i(2, -2);
                    glEnd();
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(142.0/255.0, 84.0/255.0, 80.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -5, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, 1, 1);
                    glutSolidCube(2);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor4f(183.0/255.0, 214.0/255.0, 33.0/255.0,1.0f);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -1.8, (static_cast<int>(i)*4)+gameZ);
                    glScalef(.7, 1.25, .7);
                    glutSolidCube(4);
                    glPopMatrix();
                    
                    break;
                    
                    
                    // Road
                case '2':
                    glPushMatrix();
                    glColor3f(82.0/255.0, 88.0/255.0, 102.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -7, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, 0, 1);
                    glutSolidCube(4);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(82.0/255.0, 88.0/255.0, 102.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -7, (static_cast<int>(i)*4)+gameZ);
                    glScalef(.5, .1, .08);
                    glutSolidCube(4);
                    glPopMatrix();
                    break;
                    
                    // Water
                case '3':
                    glPushMatrix();
                    glColor3f(128.0/255.0, 245.0/255.0, 255.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -7, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, 0, 1);
                    glutSolidCube(4);
                    glPopMatrix();
                    
                    break;
                    
                    // Lilypad
                case '4':
                    glPushMatrix();
                    glColor3f(128.0/255.0, 245.0/255.0, 255.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -7, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, 0, 1);
                    glutSolidCube(4);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(11.0/255.0, 181.0/255.0, 93.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -6.8, (static_cast<int>(i)*4)+gameZ);
                    glRotated(45, 0, 1, 0);
                    glScalef(.5, .2, .8);
                    glutSolidCube(2);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(11.0/255.0, 181.0/255.0, 93.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX+.3, -6.8, (static_cast<int>(i)*4)+gameZ);
                    glRotated(45, 0, 1, 0);
                    glScalef(.8, .2, .5);
                    glutSolidCube(2);
                    glPopMatrix();
                    
                    break;
                    
                    // Flag
                case '5':
                    glPushMatrix();
                    glColor3f(190.0/255.0, 245.0/255.0, 102.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -6.8, (static_cast<int>(i)*4)+gameZ);
                    glScalef(1, .15, 1);
                    glutSolidCube(4);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(100.0/255.0, 100.0/255.0, 100.0/255.0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX, -2, (static_cast<int>(i)*4)+gameZ);
                    glScalef(.18, 4, .18);
                    glutSolidCube(2);
                    glPopMatrix();
                    
                    glPushMatrix();
                    glColor3f(1, 0, 0);
                    glTranslatef((static_cast<int>(j)*4)-32+gameX-2, -2, (static_cast<int>(i)*4)+gameZ);
                    glBegin(GL_QUADS);
                    glTexCoord2i(0, 0); glVertex2i(0, 0);
                    glTexCoord2i(0, 1); glVertex2i(2, 0);
                    glTexCoord2i(1, 1); glVertex2i(2, 4);
                    glTexCoord2i(1, 0); glVertex2i(4, 0);
                    glEnd();
                    glPopMatrix();
                    
                    break;
                    
            }
        }
    }
    
    // Draw player start -------------------------------------------------------------
    
    int directionSwitcher = 1;
    
    if (playerDirection == 0 || playerDirection == 3) {
        
        directionSwitcher = 1;
    }
    else {
        directionSwitcher = -1;
    }
    
    // Draw player
    glPushMatrix();
    LudersCube(4.0f, 0-gameX, 0.0f, 0-gameZ, 2.0f, 2.0f, 2.0f, textureGrass);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0-gameX+10, 0, 0-gameZ);
    glutSolidCube(4);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0-gameX-10, 0, 0-gameZ);
    
    glBegin(GL_QUADS);
    
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
    
    //glColor3f(0.0f,1.0f,0.0f);    // Color Blue
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Top)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Top)
    glVertex3f(-scaleX, scaleY, scaleZ);    // Bottom Left Of The Quad (Top)
    glVertex3f( scaleX, scaleY, scaleZ);    // Bottom Right Of The Quad (Top)
    
    //glColor3f(scaleX,0.5f,0.0f);    // Color Orange
    glVertex3f( scaleX,-1.0f, scaleZ);    // Top Right Of The Quad (Bottom)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Top Left Of The Quad (Bottom)
    glVertex3f(-scaleX,-1.0f,-scaleZ);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( scaleX,-1.0f,-scaleZ);    // Bottom Right Of The Quad (Bottom)
    
    //glColor3f(scaleX,0.0f,0.0f);    // Color Red
    glVertex3f( scaleX, scaleY, scaleZ);    // Top Right Of The Quad (Front)
    glVertex3f(-scaleX, scaleY, scaleZ);    // Top Left Of The Quad (Front)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Bottom Left Of The Quad (Front)
    glVertex3f( scaleX,-1.0f, scaleZ);    // Bottom Right Of The Quad (Front)
    
    //glColor3f(scaleX,1.0f,0.0f);    // Color Yellow
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Back)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Back)
    glVertex3f(-scaleX, -1.0f,-scaleZ);    // Bottom Left Of The Quad (Back)
    glVertex3f( scaleX, -1.0f,-scaleZ);    // Bottom Right Of The Quad (Back)
    
    //glColor3f(0.0f,0.0f,1.0f);    // Color Blue
    glVertex3f(-scaleX, scaleY, scaleZ);    // Top Right Of The Quad (Left)
    glVertex3f(-scaleX, scaleY,-scaleZ);    // Top Left Of The Quad (Left)
    glVertex3f(-scaleX,-1.0f,-scaleZ);    // Bottom Left Of The Quad (Left)
    glVertex3f(-scaleX,-1.0f, scaleZ);    // Bottom Right Of The Quad (Left)
    
    // glColor3f(1.0f,0.0f,1.0f);    // Color Violet
    glVertex3f( scaleX, scaleY,-scaleZ);    // Top Right Of The Quad (Right)
    glVertex3f( scaleX, scaleY, scaleZ);    // Top Left Of The Quad (Right)
    glVertex3f( scaleX,-1.0f, scaleZ);    // Bottom Left Of The Quad (Right)
    glVertex3f( scaleX,-1.0f,-scaleZ);    // Bottom Right Of The Quad (Right)
    
    glEnd();
    glPopMatrix();
    
    // Body lower
    glPushMatrix();
    glColor3f(256.0/256.0, 256.0/256.0, 256.0/256.0);
    glTranslatef(playerX, playerY, playerZ);
    glRotated(90*playerDirection, 0, 1, 0);
    glScaled(1.5, 1, 1);
    glutSolidCube(1.5);
    glPopMatrix();
    
    
    // Neck
    glPushMatrix();
    glColor3f(256.0/256.0, 256.0/256.0, 256.0/256.0);
    
    if (playerDirection == 0 || playerDirection == 2)
        glTranslatef(playerX+.3*directionSwitcher,playerY+2, playerZ);
    else
        glTranslatef(playerX,playerY+2,playerZ+.3*directionSwitcher);
    glRotated(90, 0, 1, 0);
    glScaled(1, 2, 1);
    glutSolidCube(1.5);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(256.0/256.0, 256.0/256.0, 256.0/256.0);
    
    if (playerDirection == 0 || playerDirection == 2) {
        glTranslatef(playerX-.1*directionSwitcher,playerY, playerZ);
        glRotated(90, 0, 1, 0);
    }
    else {
        glTranslatef(playerX,playerY, playerZ-.1*directionSwitcher);
        glRotated(180, 0, 1, 0);
    }
    glScaled(1.5, .75, .75);
    glutSolidCube(1.5);
    glPopMatrix();
    
    
    
    glPushMatrix();
    glColor3f(255.0/256.0, 128.0/256.0, 103.0/256.0);
    
    if (playerDirection == 0 || playerDirection == 2) {
        glTranslatef(playerX+1.25*directionSwitcher,playerY+3, playerZ);
        glRotated(90, 0, 1, 0);
    }
    else {
        glTranslatef(playerX,playerY+3, playerZ+1.25*directionSwitcher);
        glRotated(180, 0, 1, 0);
    }
    
    glRotated(90, 0, 1, 0);
    glScaled(.25, .5, .25);
    glutSolidCube(1.5);
    glPopMatrix();
    
    
    
    
    glPushMatrix();
    glColor3f(255.0/256.0, 110.0/256.0, 165.0/256.0);
    if (playerDirection == 0 || playerDirection == 2) {
        glTranslatef(playerX+1.25*directionSwitcher,playerY+2.5, playerZ);
        glRotated(90, 0, 1, 0);
    }
    else {
        glTranslatef(playerX,playerY+2.5, playerZ+1.25*directionSwitcher);
        glRotated(180, 0, 1, 0);
    }
    
    glRotated(90, 0, 1, 0);
    glScaled(.25, .5, .25);
    glutSolidCube(1.5);
    glPopMatrix();
    
    
    
    glPushMatrix();
    glColor3f(0.0/256.0, 0.0/256.0, 0.0/256.0);
    
    if (playerDirection == 0 || playerDirection == 2) {
        glTranslatef(playerX+.5*directionSwitcher,playerY+3, playerZ);
        glRotated(180, 0, 1, 0);
    }
    else {
        glTranslatef(playerX,playerY+3, playerZ+.5*directionSwitcher);
        glRotated(90, 0, 1, 0);
    }
    
    glRotated(90, 0, 1, 0);
    glScaled(1, .3, .2);
    glutSolidCube(1.5);
    glPopMatrix();
    
    
    
    glPushMatrix();
    glColor3f(255.0/256.0, 110.0/256.0, 165.0/256.0);
    
    if (playerDirection == 0 || playerDirection == 2) {
        glTranslatef(playerX+.25*directionSwitcher,playerY+4, playerZ);
        glRotated(180, 0, 1, 0);
    }
    else {
        glTranslatef(playerX,playerY+4, playerZ+.25*directionSwitcher);
        glRotated(90, 0, 1, 0);
    }
    
    glRotated(90, 0, 1, 0);
    glScaled(.25, .5, .5);
    glutSolidCube(1.5);
    glPopMatrix();
    
    // Draw player End -------------------------------------------------------------
    
    // Draw cars
    for (size_t i = 0; i < vectorOfCars.size(); i++) {
        
        vectorOfCars[i].Update();
        CreateCar(vectorOfCars[i].x,vectorOfCars[i].y,vectorOfCars[i].z, vectorOfCars[i].type, vectorOfCars[i].direction);
    }
    
    //    // Flush matrix
    glFlush();
    glutSwapBuffers();
    
}

int main(int argc, char** argv) {
    
    // Init GLUT and create window
    glutInit(&argc, argv);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(900,480);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("JumpyRoad");
    
    srand( time( NULL ) );
    int r1 = rand();
    
    glEnable(GL_DEPTH_TEST);
    
    // Register callbacks
    glutDisplayFunc(Update);
    glutIdleFunc(Update);
    glutSpecialFunc(SpecialKeys);
    glutReshapeFunc(Reshape);
    
    // Init
    InitDraw();
    
    GenerateLevelFromFile();
    
    /// Generate cars
    for (size_t i = 0; i < listOfRows.size(); i++) {
        
        if (listOfRows[i][0] == '2') {
            
            float speed = .25 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(.6-.25)));
            
            float distance1 = 15 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(40-15)));
            float distance2 = 15 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(40-15)));
            
            int direction = (rand() % ((1 - 0) + 1)) + 0;
            
            if (direction == 0) {
                Car car1(-60,(static_cast<int>(i)*4)+gameZ, speed, 0,static_cast<int>(i));
                vectorOfCars.push_back(car1);
                
                Car car2(-60-distance1,(static_cast<int>(i)*4)+gameZ, speed, 0,static_cast<int>(i));
                vectorOfCars.push_back(car2);
                
                Car car3(-60-distance1-distance2,(static_cast<int>(i)*4)+gameZ, speed, 0,static_cast<int>(i));
                vectorOfCars.push_back(car3);
            } else {
                
                Car car1(60,(static_cast<int>(i)*4)+gameZ, speed, 1,static_cast<int>(i));
                vectorOfCars.push_back(car1);
                
                Car car2(60+distance1,(static_cast<int>(i)*4)+gameZ, speed, 1,static_cast<int>(i));
                vectorOfCars.push_back(car2);
                
                Car car3(60+distance1+distance2,(static_cast<int>(i)*4)+gameZ, speed, 1,static_cast<int>(i));
                vectorOfCars.push_back(car3);
            }
        }
        
    }
    float speed = .25 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(.6-.25)));
    
    Car car1(-60,(static_cast<int>(29)*4)+gameZ, speed, 0, 29);
    vectorOfCars.push_back(car1);
    
    
    
    // Enter GLUT event processing cycle
    glutMainLoop();
    
    return 0;
}
