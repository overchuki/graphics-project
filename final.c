/*
 *  Final Project
 *
 *  Igor Overchuk
 * 
 *  Key bindings:
 *  WASD/wasd   navigate if in first person view
 *  left mouse  Shoot
 *  arrows      Change view angle in orthogonal and perspective overview
 *  0           Reset view angle
 *  ESC         Exit
 *  x/X         toggle axes
 *  m           cycle through view modes (orthogonal/perspective overview/perspective first person)
 *  mouse       move camera around in first person mode (move your mouse over any part of the window to initiate this)
 *  SPACE       jump in first person mode
 *  SHIFT       sprint in first person mode
 *  p           pause/resume lighting motion
 *  k/K         slow light cycle / speed up light cycle
 *  ./>         zoom in in perspective/orthogonal view
 *  ,/<         zoom out in perspective/orthogonal view
 *
 */
#include "CSCIx229.h"
#include "igor.h"

// View Globals
int th = 0;                 // Azimuth of view angle
int ph = 0;                 // Elevation of view angle
int angleStep = 5;          // Angle step for change in angle
int showAxes = 0;           // flag to show axes and angle information
int mode = 2;               // 1: perspective overview, 2: perspective first person
double asp = 0;             // aspect ratio of window
double dim = DEFAULT_DIM;   // dimensions of window
int fov = FOV_NORMAL;       // fov
int fullScreen = 0;         // full screen default to 0
int debugMode = 0;          // allow changing of the scene
int shader = 0;             // store compiled shader
int simpleShader = 0;       // shader with no lighting

// Window Globals
int windowWidth = DEFAULT_WINDOW_WIDTH;
int windowHeight = DEFAULT_WINDOW_HEIGHT;

// FPS Globals
static double fps = -1;
static int T0 = 0;
static int frames = 0;

// First Person View Globals
double mapBounds[4];                            // [-x,+x,-z,+z]
double fpPosVec[3] = {0, CAMERA_HEIGHT, 0};     // position vector
double fpGroundDir[2] = {0, 1};                 // ground direction vector [x,z]
double fpViewVector[3] = {0, 0, 1};             // camera "look" vector
double fpUp[3] = {0, 1, 0};                     // up vector, use y as up

// Movement Globals
int centerX = DEFAULT_WINDOW_WIDTH / 2;
int centerY = DEFAULT_WINDOW_HEIGHT / 2;
int mouseIn = 0;
double elevationGlobal = 0;

// Jump Globals
double jumpHeightGlobal = 0;

// Textures
unsigned int globalHouseTexture[N_HOUSE_TEXTURES];
unsigned int globalHelicopterTexture[N_HELICOPTER_TEXTURES];
unsigned int globalGroundTexture[N_GROUND_TEXTURES];
unsigned int globalTreeTexture[N_TREE_TEXTURES];
unsigned int globalWeaponTexture[N_WEAPON_TEXTURES];

// Light
double lightIntensity = 1;
int flashLightOn = 0;

// Display List
int displayList;

// idle function to manage animation and movement of the character
void idle() {

    // perform time elapsed calculation
    static double t0 = -1.;
    double dt, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    if (t0 < 0.0) {
        t0 = t;
    }
    dt = t - t0;
    t0 = t;

    // call all object idle functions
    lightingIdle(dt, &lightIntensity);
    helicopterIdle(t, dt);
    movementIdle(t, dt, fpPosVec, fpGroundDir, mapBounds, &jumpHeightGlobal, &elevationGlobal);
    weaponIdle(t, dt, fpPosVec, fpPosVec[1]+jumpHeightGlobal+elevationGlobal);

    // redisplay scene
    glutPostRedisplay();
}

// Display the scene
void display() {

    // Clear the image
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);
    
    // Reset previous transforms
    glLoadIdentity();

    // Perspective overview
    if (mode == 1) {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    }
    // Perspective first person
    else if (mode == 2) {
        gluLookAt(  fpPosVec[0], fpPosVec[1]+jumpHeightGlobal+elevationGlobal, fpPosVec[2],
                    fpPosVec[0]+fpViewVector[0], fpPosVec[1]+jumpHeightGlobal+elevationGlobal+fpViewVector[1], fpPosVec[2]+fpViewVector[2],
                    fpUp[0], fpUp[1], fpUp[2]);
    }

    // Lighting
    lightingDisplay(fpPosVec, fpViewVector, fpPosVec[1]+jumpHeightGlobal+elevationGlobal);

    // Textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

    // Use Custom Shader for main lighting
    glUseProgram(shader);
    int shaderId = glGetUniformLocation(shader,"OuterFlashlightCone");
    glUniform1f(shaderId, FLASHLIGHT_OUTER_CUTOFF);
    shaderId = glGetUniformLocation(shader,"FlashLightOn");
    glUniform1f(shaderId, (float)flashLightOn);

    // Draw scene
    glCallList(displayList);
    drawHelicopters();
    drawEnemies();
    // draw weapon last for the different projections if in first person
    if (mode == 2) {
        switchProjectionAndDrawWeapon(FOV_NORMAL, asp);
    }

    // Lighting/textures stop after objects drawn
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    // Use alternate shader to draw sky, bullets, text, and HUD
    glUseProgram(simpleShader);
    drawSky(lightIntensity);
    drawBullets();
    switchProjectionAndDrawHUD(asp);

    // track FPS
    frames++;
    int t = glutGet(GLUT_ELAPSED_TIME);
    if (t - T0 >= 1000) {
        double seconds = (t - T0) / 1000.0;
        fps = frames / seconds;
        T0 = t;
        frames = 0;
    }

    // only show axes and angle info if user toggles it on
    if (showAxes) {
        // Draw axes in white
        glLineWidth(2);
        glColor3d(1,1,1);
        glBegin(GL_LINES);
        glVertex3d(0,0,0);
        glVertex3d(1,0,0);
        glVertex3d(0,0,0);
        glVertex3d(0,1,0);
        glVertex3d(0,0,0);
        glVertex3d(0,0,1);
        glEnd();
        
        // Label axes
        glRasterPos3d(1,0,0);
        Print("X");
        glRasterPos3d(0,1,0);
        Print("Y");
        glRasterPos3d(0,0,1);
        Print("Z");

        // Display parameters
        glWindowPos2i(5,5);
        if (mode == 1) {
            Print("View Angle=%d,%d; fov=%d",th,ph,fov);
        } else if (mode == 2) {
            double fpLatTh = getFpLatTh();
            double fpVerTh = getFpVerTh();
            Print("View Angle=%.2f,%.2f; fov=%d",fpLatTh,fpVerTh,fov);
        }

        // print the fps
        glWindowPos2i(5,windowHeight-20);
        Print("FPS: %.0f", fps);
    }

    // Flush and swap
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

// compile display lists from ex19 modified, compiles and draws static objects
void compileDisplayLists(int delete) {
    if (delete) glDeleteLists(displayList,1);
    displayList = glGenLists(1);
    glNewList(displayList,GL_COMPILE);
    drawGround();
    drawTrees();
    drawHouses();
    glEndList();
}

// GLUT key press callback
void key(unsigned char ch,int x,int y) {

    // Exit on ESC
    if (ch == 27) {
        exit(0);
    }
    // Reset view angle
    else if (debugMode == 1 && ch == '0') {
        th = ph = 0;
    }
    // toggle axes
    else if (debugMode == 1 && (ch == 'x' || ch == 'X')) {
        if (showAxes == 1) {
            showAxes = 0;
        } else {
            showAxes = 1;
        }
    }
    // cycle through view modes
    else if (debugMode == 1 && (ch == 'm')) {
        mode += 1;
        if (mode > 2) {
            mode = 1;
        }
        if (mode == 2) {
            glutSetCursor(GLUT_CURSOR_NONE);
        } else {
            glutSetCursor(GLUT_CURSOR_INHERIT);
        }
    }
    // move forwards
    else if ((ch == 'w' || ch == 'W') && mode == 2) {
        forwardKey(1, &fov);
    }
    // move backwards
    else if ((ch == 's' || ch == 'S') && mode == 2) {
        backKey(1);
    }
    // move left
    else if ((ch == 'a' || ch == 'A') && mode == 2) {
        leftKey(1);
    }
    // move right
    else if ((ch == 'd' || ch == 'D') && mode == 2) {
        rightKey(1);
    }
    // jump
    else if(ch == ' ' && mode == 2) {
        jumpKey(1);
    }
    // toggle flashlight
    else if(ch == 'f' || ch == 'F') {
        if (flashLightOn) {
            flashLightOn = 0;
        } else {
            flashLightOn = 1.0;
        }
    }
    // slow down lighting cycle
    else if(debugMode == 1 && (ch == 'k')) {
        changeLightSpeed(-SUN_SPEED_STEP);
    }
    // speed up lighting cycle
    else if(debugMode == 1 && (ch == 'K')) {
        changeLightSpeed(SUN_SPEED_STEP);
    }
    // pause/resume lighting cycle
    else if(debugMode == 1 && (ch == 'p')) {
        lightingPause();
    }
    // zoom out on "<"
    else if(ch == ',' || ch == '<') {
        dim += 0.1;
    }
    // zoom in on ">"
    else if(debugMode == 1 && ((ch == '.' || ch == '>') && dim > 0.1)) {
        dim -= 0.1;
    }

    // Update projection
    Project(fov, asp, dim, mode);

    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

// define a key up function to release keys
void keyUp(unsigned char ch,int x,int y) {
    // forwards
    if ((ch == 'w' || ch == 'W') && mode == 2) {
        forwardKey(0, &fov);
    }
    // backwards
    else if ((ch == 's' || ch == 'S') && mode == 2) {
        backKey(0);
    }
    // left
    else if ((ch == 'a' || ch == 'A') && mode == 2) {
        leftKey(0);
    }
    // right
    else if ((ch == 'd' || ch == 'D') && mode == 2) {
        rightKey(0);
    }
    // jump
    else if(ch == ' ' && mode == 2) {
        jumpKey(0);
    }
}

// GLUT special key callback (arrows and left shift)
void special(int key,int x,int y) {
    if (mode == 2) {
        // Shift key - sprint
        if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
            sprintKey(1, &fov);
        }
        return;
    }

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

// GLUT special key callback for release
void specialUp(int key,int x,int y) {
    if (mode == 2 && !(glutGetModifiers() & GLUT_ACTIVE_SHIFT)) {
        sprintKey(0, &fov);
    
        // Update projection
        Project(fov, asp, dim, mode);
    }
}

// Handle mouse movements for the camera
void mouseMove(int x, int y) {
    if (mode != 2) {
        return;
    }

    // Ensure mouse gets captured in the center before adjusting movement
    if (mouseIn) {
        
        // Call mouseMoved in movement module
        mouseMoved(x, y, centerX, centerY);

        // Recalculate camera angle
        calcFPAngle(fpPosVec, fpGroundDir, fpViewVector, fpUp);

        // Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
    }

    mouseIn = 1;
    glutWarpPointer(centerX, centerY);
}

// Handle mouse clicks
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            weaponLeftMouseDown();
        } else if (state == GLUT_UP) {
            weaponLeftMouseUp();
        }
    }
}

// GLUT window resize routine
void reshape(int width,int height) {

    // set window size
    windowWidth = width;
    windowHeight = height;

    // reset mouse center
    centerX = width / 2;
    centerY = height / 2;

    // Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);
    
    // Set aspect ratio of the window
    asp = (height>0) ? (double)width/height : 1;

    // Update projection
    Project(fov, asp, dim, mode);
}

// Load textures
void loadTextures() {
    // house
    globalHouseTexture[0] = LoadTexBMP("textures/house/brick.bmp");
    globalHouseTexture[1] = LoadTexBMP("textures/house/clay.bmp");
    globalHouseTexture[2] = LoadTexBMP("textures/house/wood.bmp");
    globalHouseTexture[3] = LoadTexBMP("textures/house/roofRed.bmp");
    globalHouseTexture[4] = LoadTexBMP("textures/house/roofGrey.bmp");
    globalHouseTexture[5] = LoadTexBMP("textures/house/window.bmp");
    globalHouseTexture[6] = LoadTexBMP("textures/house/door.bmp");

    // helicopter
    globalHelicopterTexture[0] = LoadTexBMP("textures/helicopter/metalBlack.bmp");
    globalHelicopterTexture[1] = LoadTexBMP("textures/helicopter/metalDarkGrey.bmp");
    globalHelicopterTexture[2] = LoadTexBMP("textures/helicopter/metalGrey.bmp");
    globalHelicopterTexture[3] = LoadTexBMP("textures/helicopter/windowPane.bmp");

    // ground
    globalGroundTexture[0] = LoadTexBMP("textures/ground/asphalt.bmp");
    globalGroundTexture[1] = LoadTexBMP("textures/ground/ground.bmp");
    globalGroundTexture[2] = LoadTexBMP("textures/ground/skySphere.bmp");

    // tree
    globalTreeTexture[0] = LoadTexBMP("textures/tree/bark.bmp");
    globalTreeTexture[1] = LoadTexBMP("textures/tree/evergreen.bmp");

    // weapon
    globalWeaponTexture[0] = LoadTexBMP("textures/weapon/metalBlackConstant.bmp");
    globalWeaponTexture[1] = LoadTexBMP("textures/weapon/enemy.bmp");
    globalWeaponTexture[2] = globalHouseTexture[2];
}

// Init undefined globals
void initGlobals() {
    // mapBounds
    mapBounds[0] = -WORLD_WALK_RADIUS;
    mapBounds[1] = WORLD_WALK_RADIUS;
    mapBounds[2] = -WORLD_WALK_RADIUS;
    mapBounds[3] = WORLD_WALK_RADIUS;

    // Textures
    loadTextures();

    // ground
    groundInit(globalGroundTexture);

    // houses
    houseInit(globalHouseTexture);

    // helicopters
    helicopterInit(globalHelicopterTexture);

    // trees
    treeInit(globalTreeTexture);

    // weapon
    weaponInit(globalWeaponTexture);

    // Compile display lists
    compileDisplayLists(0);
}

// helper to go over and check for flags
void checkArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        // Check for full screen flag -f
        if (!strncmp("-f", argv[i], 2)) {
            fullScreen = 1;
        }
        // Check for debug flag -d
        if (!strncmp("-d", argv[i], 2)) {
            debugMode = 1;
        }
    }
}

// Start up GLUT and tell it what to do
int main(int argc, char* argv[]) {

    // Initialize GLUT and process user parameters
    glutInit(&argc,argv);

    // Process command line flags
    checkArgs(argc, argv);
    
    // Request double buffered, true color window, z-buffering
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    // Create the window
    if (!fullScreen) glutInitWindowSize(1400, 800);
    glutCreateWindow("Final Project - Igor Overchuk");

    // Go full screen if flag specified
    if (fullScreen) glutFullScreen();

    // Hide cursor by default
    glutSetCursor(GLUT_CURSOR_NONE);

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
    
    // Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);

    // Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);

    // GLUT mouse click callback
    glutMouseFunc(mouse);

    // GLUT mouse motion callbacks
    glutMotionFunc(mouseMove);
    glutPassiveMotionFunc(mouseMove);

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);

    // Register the idle function
    glutIdleFunc(idle);

    // Init undefined globals
    initGlobals();

    // Compile Shader
    shader = CreateShaderProg("shader/light.vert", "shader/light.frag");
    simpleShader = CreateShaderProg("shader/simple.vert", "shader/simple.frag");;
    
    // Pass control to GLUT so it can interact with the user
    glutMainLoop();
    
    // Return code
    return 0;
}
