/*
 *  Lighting and Textures
 *
 *  Adapted from ex13 to be the testing ground for my objects.
 *
 *  Key bindings:
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  o          Cycles through objects
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"
#include "igor.h"

int axes=1;       // Display axes
int mode=1;       // Projection mode
int move=1;       // Move light
int th=0;         // Azimuth of view angle
int ph=0;         // Elevation of view angle
int fov=55;       // Field of view (for perspective)
int obj=6;        // Scene/opbject selection
int maxObj=7;     // total objects
double asp=1;     // Aspect ratio
double dim=3;     // Size of world
// Light values
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
double zh     =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
typedef struct {float x,y,z;} vtx;
typedef struct {int A,B,C;} tri;
#define n 500
vtx is[n];

// Textures
unsigned int globalHouseTexture[N_HOUSE_TEXTURES];
unsigned int globalHelicopterTexture[N_HELICOPTER_TEXTURES];
unsigned int globalGroundTexture[N_GROUND_TEXTURES];
unsigned int globalTreeTexture[N_TREE_TEXTURES];
unsigned int globalWeaponTexture[N_WEAPON_TEXTURES];

// COPY of 'Vertex' function from ex13 to draw sun
void Vertex(double th,double ph) {
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    // For a sphere at the origin, the position
    // and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

// COPY of 'ball' function from ex13
void ball(double x,double y,double z,double r) {
    // Save transformation
    glPushMatrix();
    // Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    // White ball with yellow specular
    float yellow[]   = {1.0,1.0,0.0,1.0};
    float Emission[] = {0.0,0.0,0.01*emission,1.0};
    glMaterialf(GL_FRONT,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

    glColor3f(1,1,1);
    // Bands of latitude
    for (int ph=-90;ph<90;ph+=SUN_INCR)
    {
        glBegin(GL_QUAD_STRIP);
        for (int th=0;th<=360;th+=2*SUN_INCR)
        {
            Vertex(th,ph);
            Vertex(th,ph+SUN_INCR);
        }
        glEnd();
    }
    // Undo transofrmations
    glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
    // Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);

    // Undo previous transformations
    glLoadIdentity();
    // Perspective - set eye position
    if (mode)
    {
        double Ex = -2*dim*Sin(th)*Cos(ph);
        double Ey = +2*dim        *Sin(ph);
        double Ez = +2*dim*Cos(th)*Cos(ph);
        gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    }
    // Orthogonal - set world orientation
    else
    {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }

    // Flat or smooth shading
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    // Light switch
    if (light)
    {
        // Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        // Light position
        float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
        // Draw light position as ball (still no lighting here)
        glColor3f(1,1,1);
        ball(Position[0],Position[1],Position[2] , 0.1);
        // OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        // Enable lighting
        glEnable(GL_LIGHTING);
        // Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
        // glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        // Enable light 0
        glEnable(GL_LIGHT0);
        // Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    else {
        glDisable(GL_LIGHTING);
    }

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

    // Draw individual objects
    // HOUSE
    if (obj==0) {
        struct houseTheme thm = {2, 4, 6, 5};
        drawHouse(0,0,0,0,1,thm);
    }
    // HELICOPTER
    else if (obj==1) {
        struct helicopterTheme thm = {1, 0, 3};
        drawHelicopter(0,0,0,0,0,0,2,0,0,thm);
    }
    // HELICOPTER ROTOR
    else if (obj==2) {
        glColor3d(1,1,1);
        drawRotor(0,0,0,0,0,2,0.05,0.1);
    }
    // TREE
    else if (obj==3) {
        glColor3d(1,1,1);
        drawTree(0,0,0, 0, 1);
    }
    // WEAPON
    else if (obj==4) {
        glPushMatrix();
        glScaled(15,15,15);
        drawWeapon(0,0,0);
        glPopMatrix();
    }
    // BULLET
    else if (obj==5) {
        struct bullet b = {0,0,0,0,0,0,0};
        drawBullet(b);
    }
    // ENEMY
    else if (obj==6) {
        struct enemy e = {0,0,0,0,1,100,0};
        drawEnemy(e);
    }

    // Draw axes - no lighting from here on
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
    if (axes)
    {
        const double len=2.0;  // Length of axes
        glBegin(GL_LINES);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(len,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,len,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,len);
        glEnd();
        // Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }

    // Display parameters
    glWindowPos2i(5,5);
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
        th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
    if (light)
    {
        glWindowPos2i(5,45);
        Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
        glWindowPos2i(5,25);
        Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
    }

    // Render the scene and make it visible
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
    // perform time elapsed calculation
    static double t0 = -1.;
    double dt, t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    if (t0 < 0.0) {
        t0 = t;
    }
    dt = t - t0;
    t0 = t;

    if (move) {
        zh = fmod(zh + 90.0*dt,360.0);

        // Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
    }
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
    // Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
        th += 5;
    // Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT)
        th -= 5;
    // Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
        ph += 5;
    // Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
        ph -= 5;
    // PageUp key - increase dim
    else if (key == GLUT_KEY_PAGE_DOWN)
        dim += 0.1;
    // PageDown key - decrease dim
    else if (key == GLUT_KEY_PAGE_UP && dim>1)
        dim -= 0.1;
    // Smooth color model
    else if (key == GLUT_KEY_F1)
        smooth = 1-smooth;
    // Local Viewer
    else if (key == GLUT_KEY_F2)
        local = 1-local;
    else if (key == GLUT_KEY_F3)
        distance = (distance==1) ? 5 : 1;
    // Toggle ball increment
    else if (key == GLUT_KEY_F8)
        inc = (inc==10)?3:10;
    // Flip sign
    else if (key == GLUT_KEY_F9)
        one = -one;
    // Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    // Update projection
    Project(fov,asp,dim,mode);
    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
    // Exit on ESC
    if (ch == 27)
        exit(0);
    // Reset view angle
    else if (ch == '0')
        th = ph = 0;
    // Toggle axes
    else if (ch == 'x' || ch == 'X')
        axes = 1-axes;
    // Toggle lighting
    else if (ch == 'l' || ch == 'L')
        light = 1-light;
    // Switch projection mode
    else if (ch == 'p' || ch == 'P')
        mode = 1-mode;
    // Toggle light movement
    else if (ch == 'm' || ch == 'M')
        move = 1-move;
    // Move light
    else if (ch == '<')
        zh += 1;
    else if (ch == '>')
        zh -= 1;
    // Change field of view angle
    else if (ch == '-' && ch>1)
        fov--;
    else if (ch == '+' && ch<179)
        fov++;
    // Light elevation
    else if (ch=='[')
        ylight -= 0.1;
    else if (ch==']')
        ylight += 0.1;
    // Ambient level
    else if (ch=='a' && ambient>0)
        ambient -= 5;
    else if (ch=='A' && ambient<100)
        ambient += 5;
    // Diffuse level
    else if (ch=='d' && diffuse>0)
        diffuse -= 5;
    else if (ch=='D' && diffuse<100)
        diffuse += 5;
    // Specular level
    else if (ch=='s' && specular>0)
        specular -= 5;
    else if (ch=='S' && specular<100)
        specular += 5;
    // Emission level
    else if (ch=='e' && emission>0)
        emission -= 5;
    else if (ch=='E' && emission<100)
        emission += 5;
    // Shininess level
    else if (ch=='n' && shininess>-1)
        shininess -= 1;
    else if (ch=='N' && shininess<7)
        shininess += 1;
    // Switch scene/object
    else if (ch == 'o')
        obj = (obj+1)%maxObj;
    else if (ch == 'O')
        obj = (obj+maxObj-1)%maxObj;
    // Translate shininess power to value (-1 => 0)
    shiny = shininess<0 ? 0 : pow(2.0,shininess);
    // Reproject
    Project(fov,asp,dim,mode);
    // Animate if requested
    glutIdleFunc(idle);
    // Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    // Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    // Set the viewport to the entire window
    glViewport(0,0, RES*width,RES*height);
    // Set projection
    Project(fov,asp,dim,mode);
}

/*
 *  Random numbers from min to max to the power p
 */
static float frand(float min,float max,float p)
{
    return pow(rand()/(float)RAND_MAX,p)*(max-min)+min;
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
    globalGroundTexture[2] = LoadTexBMP("textures/ground/sky.bmp");

    // tree
    globalTreeTexture[0] = LoadTexBMP("textures/tree/bark.bmp");
    globalTreeTexture[1] = LoadTexBMP("textures/tree/evergreen.bmp");

    // weapon
    globalWeaponTexture[0] = LoadTexBMP("textures/weapon/metalBlackConstant.bmp");
    globalWeaponTexture[1] = LoadTexBMP("textures/weapon/enemy.bmp");
    globalWeaponTexture[2] = globalHouseTexture[2];
}

/*
 * Initialize icosasphere locations
 */
void Init()
{
    for (int i=0;i<n;i++)
    {
        float th = frand(0,360,1);
        float ph = frand(-90,+90,1);
        float r  = frand(0.1,0.7,3);
        is[i].x = r*Sin(th)*Cos(ph);
        is[i].y = r*Cos(th)*Cos(ph);
        is[i].z = r*Sin(ph) + 1.0;
    }

    loadTextures();
    houseInit(globalHouseTexture);
    helicopterInit(globalHelicopterTexture);
    groundInit(globalGroundTexture);
    treeInit(globalTreeTexture);
    weaponInit(globalWeaponTexture);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
    // Initialize GLUT
    glutInit(&argc,argv);
    // Request double buffered, true color window with Z buffering at 600x600
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(1000,600);
    glutCreateWindow("Debug View - Igor Overchuk");
    #ifdef USEGLEW
    // Initialize GLEW
    if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
    #endif
    // Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    // Initialize
    Init();
    // glClearColor(1,1,1,0);
    // Pass control to GLUT so it can interact with the user
    ErrCheck("init");
    glutMainLoop();
    return 0;
}
