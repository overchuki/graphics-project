/*
 * Map Generator
 * 
 * m            cycle modes (map, tree, house, enemy)
 * c            cycle iterations of "selected" object
 * wasd/WASD    move brush around (in map), move object around (in tree, house, enemy)
 * click/drag   move around the map by clicking left mouse and dragging it around
 * arrows       change view angle
 * ESC          exit and save (will overwrite current dem files)
 * b/B          increase/decrease brush size in map editing mode
 * e/E          increase/decrease elevation/move step in map/objects editing modes
 * ,/<          zoom out
 * ./>          zoom in
 * u            increase elevation at selected point/s in map edit mode
 * j            decrease elevation at selected point/s in map edit mode
 * r            rotate objects
 * Z            randomize tree heights
 */
#include "CSCIx229.h"
#include "igor.h"

// Globals
int th = 0;                 // Azimuth of view angle
int ph = 45;                // Elevation of view angle
int angleStep = 5;          // Angle step for change in angle
int mode = 1;               // 0: orthogonal, 1: perspective overview, 2: perspective first person
double asp = 0;             // aspect ratio of window
double dim = 5;             // dimensions of window
int fov = 55;               // fov
int move=0;                 // Move mode
int X=0,Y=0;                // Last mouse location
double TX=0,TY=0;           // translate
int brushSize=0;            // size of "brush"
int editMode=0;             // 0: mapBuilding, 1: tree editing, 2: house editing, 3: target editing

// trees
struct tree trees[N_TREES];
int selectedTree = 0;
double treeMoveStep = 0.1;

// houses
struct house houses[N_HOUSES];
int selectedHouse = 0;
double houseMoveStep = 0.1;

// enemies
struct enemy enemies[N_ENEMIES];
int selectedEnemy = 0;
double enemyMoveStep = 0.1;

// World elevation map
double mgworldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH];
double mgtopoStep = ((double)WORLD_RADIUS*2) / ((double)WORLD_TOPO_WIDTH-1);
struct road mgroads[N_ROADS] = {
    {-WORLD_RADIUS, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.8},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, WORLD_RADIUS, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.8},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, WORLD_RADIUS, -WORLD_RADIUS},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, WORLD_WALK_RADIUS-2.5, WORLD_WALK_RADIUS-3.0, WORLD_WALK_RADIUS-2.7}
};

// track selected cells
int selectedX, selectedZ = 0;
double elevStep = 0.01;

// Get elevation at certain position
double getElevation(double x, double z) {
    double topoX = ((x+WORLD_RADIUS) / (2*(double)WORLD_RADIUS)) * (double)WORLD_TOPO_WIDTH;
    double topoZ = ((z+WORLD_RADIUS) / (2*(double)WORLD_RADIUS)) * (double)WORLD_TOPO_WIDTH;
    int px = floor(topoX);
    int pz = floor(topoZ);
    double diffX = topoX - px;
    double diffZ = topoZ - pz;
    return (((diffX * mgworldTopo[pz][px+1]) + ((1-diffX) * mgworldTopo[pz][px])) * (1-diffZ) +
            ((diffX * mgworldTopo[pz+1][px+1]) + ((1-diffX) * mgworldTopo[pz+1][px])) * diffZ +
            ((diffZ * mgworldTopo[pz+1][px]) + ((1-diffZ) * mgworldTopo[pz][px])) * (1-diffX) +
            ((diffZ * mgworldTopo[pz+1][px+1]) + ((1-diffZ) * mgworldTopo[pz][px+1])) * diffX) / 2;
}

// helper to move a tree and save its new location
void moveTree(double x, double z) {
    trees[selectedTree].x += x;
    trees[selectedTree].z += z;
    trees[selectedTree].y = getElevation(trees[selectedTree].x, trees[selectedTree].z) - 0.03; // sink into ground a little bit
}

// helper to move an enemy and save its new location
void moveEnemy(double x, double z) {
    enemies[selectedEnemy].x += x;
    enemies[selectedEnemy].z += z;
    enemies[selectedEnemy].y = getElevation(enemies[selectedEnemy].x, enemies[selectedEnemy].z) - 0.01; // sink into ground a little bit
}

// make sure trees and enemies all have proper y
void setHeights() {
    for (int i=0;i<N_TREES;i++) {
        selectedTree = i;
        moveTree(0,0);
    }
    for (int i=0;i<N_ENEMIES;i++) {
        selectedEnemy = i;
        moveEnemy(0,0);
    }
}

// randomize tree heights
void randomizeTreeScale() {
    double scaleBase = 0.05;
    double scaleRange = 0.4;
    printf("Start randomize\n");
    for(int i = 0; i < N_TREES; i++) {
        trees[i].scale = scaleBase + (scaleRange * ((double)getRandomInRange(0, 100) / 100.0));
    }
    printf("Scales randomized\n");
}

// save all dem files
void saveAll() {
    setHeights();

    printf("Saving to map.dem\n");
    SaveMapDEM("dem/map.dem", mgworldTopo);

    printf("Saving to tree.dem\n");
    SaveTreeDEM("dem/tree.dem", trees);
    
    printf("Saving to house.dem\n");
    SaveHouseDEM("dem/house.dem", houses);

    printf("Saving to enemy.dem\n");
    SaveEnemyDEM("dem/enemy.dem", enemies);

    printf("Saved!\n");
}

// Draw the ground
void drawWireframeGround() {

    // wireframe
    glColor3d(1,1,1);
    double wx, wz = 0;
    for (int r = 0; r < WORLD_TOPO_WIDTH-1; r++) {
        for (int c = 0; c < WORLD_TOPO_WIDTH-1; c++) {
            wx = -WORLD_RADIUS + mgtopoStep*c;
            wz = -WORLD_RADIUS + mgtopoStep*r;
            glBegin(GL_LINE_LOOP);
            glVertex3d(wx,          mgworldTopo[r][c],        wz);
            glVertex3d(wx+mgtopoStep, mgworldTopo[r][c+1],      wz);
            glVertex3d(wx+mgtopoStep, mgworldTopo[r+1][c+1],    wz+mgtopoStep);
            glVertex3d(wx,          mgworldTopo[r+1][c],      wz+mgtopoStep);
            glEnd();
        }
    }

    // points
    if (editMode == 0) {
        glColor3d(1,0,0);
        glPointSize(8);
        glBegin(GL_POINTS);
        for (int i = selectedZ-brushSize; i <= selectedZ+brushSize; i++) {
            if(i < 0 || i >= WORLD_TOPO_WIDTH) continue;
            for (int j = selectedX-brushSize; j <= selectedX+brushSize; j++) {
                if(j < 0 || j >= WORLD_TOPO_WIDTH) continue;
                glVertex3d(-WORLD_RADIUS + mgtopoStep*j, mgworldTopo[i][j], -WORLD_RADIUS + mgtopoStep*i);
            }
        }
        glEnd();
    } else {
        // TREES
        glPointSize(8);
        glBegin(GL_POINTS);
        for (int i = 0; i < N_TREES; i++) {
            if (i == selectedTree) {
                glColor3d(1,0,0);
            } else {
                glColor3d(0,0,1);
            }
            glVertex3d(trees[i].x, trees[i].y, trees[i].z);
        }
        glEnd();
        
        // HOUSES
        double width;
        for (int i = 0; i < N_HOUSES; i++) {
            width = 1.0 * houses[i].scale;
            glPushMatrix();
            glTranslated(houses[i].x, houses[i].y, houses[i].z);
            glRotated(houses[i].rotateAngleY, 0, 1.0, 0);
            glBegin(GL_QUADS);
            if (i == selectedHouse) {
                glColor3d(1,0,0);
            } else {
                glColor3d(0,0,1);
            }
            glVertex3d(-width, 0.0, -width);
            glVertex3d(+width, 0.0, -width);
            glColor3d(0,1,0);
            glVertex3d(+width, 0.0, +width);
            glVertex3d(-width, 0.0, +width);
            glEnd();
            glPopMatrix();
        }

        // ENEMIES
        width = 0.1;
        for (int i = 0; i < N_ENEMIES; i++) {
            glPushMatrix();
            glTranslated(enemies[i].x, enemies[i].y, enemies[i].z);
            glRotated(90*enemies[i].xOrient, 0, 1.0, 0);
            glBegin(GL_QUADS);
            if (i == selectedEnemy) {
                glColor3d(1,0,1);
            } else {
                glColor3d(0,0,1);
            }
            glVertex3d(-width, 0.0, -width);
            glVertex3d(+width, 0.0, -width);
            glColor3d(1,1,0);
            glVertex3d(+width, 0.0, +width);
            glVertex3d(-width, 0.0, +width);
            glEnd();
            glPopMatrix();
        }
    }

    // roads
    glColor3d(0,0,1);
    glBegin(GL_QUADS);
    for(int i = 0; i < N_ROADS; i++) {
        glVertex3d(mgroads[i].left,  0, mgroads[i].bottom);
        glVertex3d(mgroads[i].left,  0, mgroads[i].top);
        glVertex3d(mgroads[i].right, 0, mgroads[i].top);
        glVertex3d(mgroads[i].right, 0, mgroads[i].bottom);
    }
    glEnd();

    // culdesac
    glBegin(GL_POLYGON);
    glTexCoord2d(3,2); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6);
    glTexCoord2d(3,4); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3);
    glTexCoord2d(2,6); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.2, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.0);
    glTexCoord2d(1,6); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.9, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.0);
    glTexCoord2d(0,4); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.6, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3);
    glTexCoord2d(0,3); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.6, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6);
    glTexCoord2d(1,0); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.9, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.9);
    glTexCoord2d(2,0); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.2, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.9);
    glEnd();

    // map radius
    double mapRadiusHeight = 0.1;
    glColor3d(0,1,0);
    glBegin(GL_LINE_LOOP);
    glVertex3d(-WORLD_WALK_RADIUS,mapRadiusHeight,-WORLD_WALK_RADIUS);
    glVertex3d(WORLD_WALK_RADIUS,mapRadiusHeight,-WORLD_WALK_RADIUS);
    glVertex3d(WORLD_WALK_RADIUS,mapRadiusHeight,WORLD_WALK_RADIUS);
    glVertex3d(-WORLD_WALK_RADIUS,mapRadiusHeight,WORLD_WALK_RADIUS);
    glEnd();

    // sky dome radius
    glColor3d(1,0,0);
    glBegin(GL_LINE_LOOP);
    for (int th = 0; th <= 360; th += GROUND_ANGLE_STEP) {
        glVertex3d(Cos(th)*WORLD_RADIUS, 0, Sin(th)*WORLD_RADIUS);
    }
    glEnd();
}

// Display the scene
void display() {

    // Clear the image
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // Reset previous transforms
    glLoadIdentity();

    // always in perspective
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

    // draw wireframe
    glPushMatrix();
    glTranslated(TX, 0, TY);
    drawWireframeGround();
    glPopMatrix();

    // Display parameters
    glWindowPos2i(5,5);
    glColor3d(1,1,1);
    if (editMode == 0) {
        Print("MAP. Brush (b/B): %d, Elev (e/E): %.2f", brushSize, elevStep);
    } else if (editMode == 1) {
        Print("TREE. Selected: [%d] => x: %.2lf, z: %.2lf, step: %.2lf", selectedTree, trees[selectedTree].x, trees[selectedTree].z, treeMoveStep);
    } else if (editMode == 2) {
        Print("HOUSE. Selected: [%d] => x: %.2lf, z: %.2lf, th: %.2lf, step: %.2lf", selectedHouse, houses[selectedHouse].x, houses[selectedHouse].z, houses[selectedHouse].rotateAngleY, houseMoveStep);
    } else if (editMode == 3) {
        Print("ENEMY. Selected: [%d] => x: %.2lf, z: %.2lf, xOrient: %d, step: %.2lf", selectedEnemy, enemies[selectedEnemy].x, enemies[selectedEnemy].z, enemies[selectedEnemy].xOrient, enemyMoveStep);
    }

    // Flush and swap
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

// helper to adjust height of map elevation
void addHeight(double h) {
    for (int i = selectedZ-brushSize; i <= selectedZ+brushSize; i++) {
        if(i < 0 || i >= WORLD_TOPO_WIDTH) continue;
        for (int j = selectedX-brushSize; j <= selectedX+brushSize; j++) {
            if(j < 0 || j >= WORLD_TOPO_WIDTH) continue;
            mgworldTopo[i][j] += h;
            if (mgworldTopo[i][j] < 0.0) {
                mgworldTopo[i][j] = +0.0;
            }
        }
    }
}

// helper to increment/decrement elevStep
void incrElevStep(int i) {
    if (i > 0) {
        elevStep += 0.01;
    } else if (i < 0 && elevStep > 0.01) {
        elevStep -= 0.01;
    }
}

// GLUT key press callback
void key(unsigned char ch,int x,int y) {

    // Exit on ESC
    if (ch == 27) {
        saveAll();
        exit(0);
    }
    // Reset view angle
    else if (ch == '0') {
        th = ph = 0;
    }
    // move forwards
    else if (ch == 'w' || ch == 'W') {
        if (editMode == 0 && selectedZ > 0) {
            selectedZ -= 1;
        } else if (editMode == 1) {
            moveTree(0, -treeMoveStep);
        } else if (editMode == 2) {
            houses[selectedHouse].z -= houseMoveStep;
        } else if (editMode == 3) {
            moveEnemy(0, -enemyMoveStep);
        }
    }
    // move backwards
    else if (ch == 's' || ch == 'S') {
        if (editMode == 0 && selectedZ < WORLD_TOPO_WIDTH-1) {
            selectedZ += 1;
        } else if (editMode == 1) {
            moveTree(0, treeMoveStep);
        } else if (editMode == 2) {
            houses[selectedHouse].z += houseMoveStep;
        } else if (editMode == 3) {
            moveEnemy(0, enemyMoveStep);
        }
    }
    // move left
    else if (ch == 'a' || ch == 'A') {
        if (editMode == 0 && selectedX > 0) {
            selectedX -= 1;
        } else if (editMode == 1) {
            moveTree(-treeMoveStep, 0);
        } else if (editMode == 2) {
            houses[selectedHouse].x -= houseMoveStep;
        } else if (editMode == 3) {
            moveEnemy(-enemyMoveStep, 0);
        }
    }
    // move right
    else if (ch == 'd' || ch == 'D') {
        if (editMode == 0 && selectedX < WORLD_TOPO_WIDTH-1) {
            selectedX += 1;
        } else if (editMode == 1) {
            moveTree(treeMoveStep, 0);
        } else if (editMode == 2) {
            houses[selectedHouse].x += houseMoveStep;
        } else if (editMode == 3) {
            moveEnemy(enemyMoveStep, 0);
        }
    }
    // zoom out on "<"
    else if(ch == ',' || ch == '<') {
        dim += 0.1;
    }
    // zoom in on ">"
    else if((ch == '.' || ch == '>') && dim > 0.1) {
        dim -= 0.1;
    }
    // increase elev
    else if(ch == 'u' && editMode == 0) {
        addHeight(elevStep);
    }
    // decrease elev
    else if(ch == 'j' && editMode == 0) {
        addHeight(-elevStep);
    }
    // increment elevStep
    else if(ch == 'e') {
        if (editMode == 0) {
            incrElevStep(1);
        } else if (editMode == 1) {
            treeMoveStep += 0.01;
        } else if (editMode == 2) {
            houseMoveStep += 0.01;
        } else if (editMode == 3) {
            enemyMoveStep += 0.01;
        }
    }
    // decrement elevStep
    else if(ch == 'E') {
        if (editMode == 0) {
            incrElevStep(-1);
        } else if (editMode == 1) {
            treeMoveStep -= 0.01;
        } else if (editMode == 2) {
            houseMoveStep -= 0.01;
        } else if (editMode == 3) {
            enemyMoveStep -= 0.01;
        }
    }
    // increase brush width
    else if(ch == 'b' && editMode == 0) {
        brushSize++;
    }
    // decrease brush width
    else if(ch == 'B' && brushSize > 0 && editMode == 0) {
        brushSize--;
    }
    // change edit mode
    else if(ch == 'm') {
        if (editMode == 0) {
            editMode = 1;
            brushSize = 0;
        } else if (editMode == 1) {
            editMode = 2;
        } else if (editMode == 2) {
            editMode = 3;
        } else if (editMode == 3) {
            editMode = 0;
        }
    }
    // cycle forward through trees
    else if(ch == 'c') {
        if (editMode == 1) {
            selectedTree++;
            if (selectedTree >= N_TREES) {selectedTree = 0;}
        } else if (editMode == 2) {
            selectedHouse++;
            if (selectedHouse >= N_HOUSES) {selectedHouse = 0;}
        } else if (editMode == 3) {
            selectedEnemy++;
            if (selectedEnemy >= N_ENEMIES) {selectedEnemy = 0;}
        }
    }
    // cycle backward through trees
    else if(ch == 'C') {
        if (editMode == 1) {
            selectedTree--;
            if (selectedTree < 0) {selectedTree = N_TREES-1;}
        } else if (editMode == 2) {
            selectedHouse--;
            if (selectedHouse < 0) {selectedHouse = N_HOUSES-1;}
        } else if (editMode == 3) {
            selectedEnemy--;
            if (selectedEnemy < 0) {selectedEnemy = N_ENEMIES-1;}
        }
    }
    // rotate houses around
    else if(ch == 'r' || ch == 'R') {
        if (editMode == 2) {
            houses[selectedHouse].rotateAngleY = fmod(houses[selectedHouse].rotateAngleY + 15.0, 360.0);
        } else if (editMode == 3) {
            if (enemies[selectedEnemy].xOrient == 0) {
                enemies[selectedEnemy].xOrient = 1;
            } else {
                enemies[selectedEnemy].xOrient = 0;
            }
        }
    }
    // randomize tree scales
    else if(ch == 'Z' && editMode == 1) {
        randomizeTreeScale();
    }

    // Update projection
    Project(fov, asp, dim, mode);

    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

// GLUT window resize routine
void reshape(int width,int height) {

    // Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);
    
    // Set aspect ratio of the window
    asp = (height>0) ? (double)width/height : 1;

    // Update projection
    Project(fov, asp, dim, mode);
}

// GLUT special key callback (arrows and left shift)
void special(int key,int x,int y) {
    int lateralDiff = 0;
    int verticalDiff = 0;
    // Right arrow key - increase azimuth by 5 degrees
    if (key == GLUT_KEY_RIGHT) {
        lateralDiff += angleStep;
    }
    // Left arrow key - decrease azimuth by 5 degrees
    else if (key == GLUT_KEY_LEFT) {
        lateralDiff -= angleStep;
    }
    // Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP) {
        verticalDiff += angleStep;
    }
    // Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN) {
        verticalDiff -= angleStep;
    }
    
    // Keep angles to +/-360 degrees
    th = (th + lateralDiff) % 360;
    ph = (ph + verticalDiff) % 360;

    // Update projection
    Project(fov, asp, dim, mode);

    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

// from ex18
void mouse(int key,int status,int x,int y) {
    //  On button down, set 'move' and remember location
    if (status==GLUT_DOWN) {
        move = 1;
        X = x;
        Y = y;
        glutPostRedisplay();
    }
    //  On button up, unset move
    else if (status==GLUT_UP) {
        move = 0;
    }
}

// from ex18
void motion(int x,int y) {
    //  Do only when move is set
    if (move) {
        double factor = 20.0;
        TX -= ((Cos(th)*(X-x) - Sin(th)*(Y-y)) / factor);
        TY -= ((Sin(th)*(X-x) + Cos(th)*(Y-y)) / factor);
        //  Remember location
        X = x;
        Y = y;
        glutPostRedisplay();
    }
}

// Start up GLUT and tell it what to do
int main(int argc,char* argv[]) {

    // Init random generator
    srand(time(0));

    // Initialize GLUT and process user parameters
    glutInit(&argc,argv);
    
    // Request double buffered, true color window, z-buffering
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    // Create the window
    glutInitWindowSize(1400, 800);
    glutCreateWindow("Ground Generator - Igor Overchuk");

    // Initialize GLEW if needed
    #ifdef USEGLEW
    if (glewInit()!=GLEW_OK) {
        Fatal("Error initializing GLEW\n");
    }
    #endif

    // Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    
    // Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    
    // Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);

    // Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);

    // mouse move func
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    // read in map values
    ReadMapDEM("dem/map.dem", mgworldTopo);
    ReadTreeDEM("dem/tree.dem", trees);
    ReadHouseDEM("dem/house.dem", houses);
    ReadEnemyDEM("dem/enemy.dem", enemies);

    // Pass control to GLUT so it can interact with the user
    glutMainLoop();
    
    // Return code
    return 0;
}
