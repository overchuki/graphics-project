/*
 *   This is a c file that contains all helicopter related files
 */

#include "CSCIx229.h"
#include "igor.h"

// Helicopter Globals Vars
/*
    Textures:
    0: black metal
    1: dark grey metal
    2: grey metal
    3: window pane
*/
unsigned int *helicopterTexture;

// Store all helicopter/flyover objects and themes
struct helicopter helicopters[N_HELICOPTERS];
struct helicopterTheme helicopterThemes[N_HELICOPTER_THEMES] = {
    {1, 0, 3},
    {2, 0, 3}
};
struct flyover flyovers[MAX_FLYOVERS] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

// init timing based on world clock
double lastFlyoverTime = 0.0;
double lastFlyoverCheckTime = 0.0;

// set helicopter offset defaults
double formationOffset = 1.0;
double yawOffset = 90.0;

// random number generator
int getRandomInRange(int low, int high) {
    return (rand() % (high - low + 1)) + low;
}

// randomly generate a new flyover based on time elapsed
int timeForNewFlyover(double t) {
    // Only check once a second
    if (t - lastFlyoverCheckTime > FLYOVER_CHECK_SLEEP) {
        lastFlyoverCheckTime = t;

        // time since last flyover increases, which increases the odds of a new flyover
        double ran = (double)getRandomInRange(0,100);
        double chance = (t - lastFlyoverTime) * FLYOVER_TIME_FACTOR;
        if (ran < chance) {
            return 1;
        }
    }
    return 0;
}

// loop to find a flyover struct that isn't active, if none, return -1
int getFirstNonActiveFlyover() {
    for (int i = 0; i < MAX_FLYOVERS; i++) {
        if (flyovers[i].active == 0) {
            return i;
        }
    }
    return -1;
}

// set formation for a certain flyover
void setFormation(int flyover) {
    // offset helicopter index and determine start position
    int heliOffset = flyover * 3;
    double leadPosX = flyovers[flyover].x * WORLD_RADIUS;
    double leadPosZ = flyovers[flyover].z * WORLD_RADIUS;

    // figure out position of lead helicopter
    helicopters[heliOffset].x = leadPosX;
    helicopters[heliOffset].y = flyovers[flyover].height;
    helicopters[heliOffset].z = leadPosZ;
    helicopters[heliOffset].yaw = -flyovers[flyover].angle + yawOffset;
    helicopters[heliOffset].pitch = 0;
    helicopters[heliOffset].roll = 0;
    helicopters[heliOffset].scale = DEFAULT_HELICOPTER_SCALE;
    helicopters[heliOffset].theme = 0;
    helicopters[heliOffset].mainRotorAngle = 0;
    helicopters[heliOffset].mainRotorSpeed = DEFAULT_ROTOR_SPEED;
    helicopters[heliOffset].secondaryRotorAngle = 0;
    helicopters[heliOffset].secondaryRotorSpeed = DEFAULT_ROTOR_SPEED*2;
    helicopters[heliOffset].inView = 1;

    // set positions of other helicopters based on the position of lead
    double tmpX, tmpZ;
    for (int i = 1; i < flyovers[flyover].n_helis; i++) {

        // x and z are diff as one is to the left and one is to the right
        tmpX = leadPosX + (formationOffset * flyovers[flyover].x);
        tmpZ = leadPosZ + (formationOffset * flyovers[flyover].z);
        if (i == 1) {
            tmpX += (formationOffset * flyovers[flyover].z);
            tmpZ -= (formationOffset * flyovers[flyover].x);
        } else if (i == 2) {
            tmpX -= (formationOffset * flyovers[flyover].z);
            tmpZ += (formationOffset * flyovers[flyover].x);
        }

        // rest is the same for 2nd and 3rd helis
        helicopters[heliOffset+i].x = tmpX;
        helicopters[heliOffset+i].z = tmpZ;
        helicopters[heliOffset+i].y = flyovers[flyover].height;
        helicopters[heliOffset+i].yaw = -flyovers[flyover].angle + yawOffset;
        helicopters[heliOffset+i].pitch = 0;
        helicopters[heliOffset+i].roll = 0;
        helicopters[heliOffset+i].scale = DEFAULT_HELICOPTER_SCALE;
        helicopters[heliOffset+i].theme = 0;
        helicopters[heliOffset+i].mainRotorAngle = 0;
        helicopters[heliOffset+i].mainRotorSpeed = DEFAULT_ROTOR_SPEED;
        helicopters[heliOffset+i].secondaryRotorAngle = 0;
        helicopters[heliOffset+i].secondaryRotorSpeed = DEFAULT_ROTOR_SPEED*2;
        helicopters[heliOffset+i].inView = 1;
    }
}

// check heights with other active flyovers to make sure they don't hit each other
int isInvalidFlyoverHeight(double height, int flyover) {
    for (int i = 0; i < MAX_FLYOVERS; i++) {
        if (flyovers[i].active == 0 || i == flyover) {
            continue;
        }
        if (abs(height - flyovers[i].height) < 0.8) {
            return 1;
        }
    }
    return 0;
}

// init a new flyover
void initFlyover(double t) {
    int i = getFirstNonActiveFlyover();
    if (i != -1) {

        // Get random height and make sure it isn't already in use
        double height = ((double)getRandomInRange(2*((int)MIN_FLYOVER_HEIGHT), 2*((int)MAX_FLYOVER_HEIGHT))) / 2.0;
        while(isInvalidFlyoverHeight(height, i)) {
            height = ((double)getRandomInRange(2*((int)MIN_FLYOVER_HEIGHT), 2*((int)MAX_FLYOVER_HEIGHT))) / 2.0;
        }

        // set all flyover values, randomizing some based on global bounds
        flyovers[i].active = 1;
        flyovers[i].angle = (double)getRandomInRange(0,359);
        flyovers[i].pos = 0.0;
        flyovers[i].speed = (double)getRandomInRange(MIN_FLYOVER_SPEED, MAX_FLYOVER_SPEED);
        flyovers[i].height = height;
        flyovers[i].n_helis = getRandomInRange(1, MAX_HELIS_IN_FORMATION);
        flyovers[i].x = Sin(flyovers[i].angle);
        flyovers[i].z = -Cos(flyovers[i].angle);

        // init heli structs of this flyover
        setFormation(i);

        lastFlyoverTime = t;
    }
}

// move all active helicopters of a flyover
void updateFormation(int flyover, double dt) {
    int heliOffset = flyover * 3;
    for (int i = 0; i < flyovers[flyover].n_helis; i++) {
        helicopters[heliOffset+i].x += (-flyovers[flyover].x * flyovers[flyover].speed * dt);
        helicopters[heliOffset+i].z += (-flyovers[flyover].z * flyovers[flyover].speed * dt);
    }
}

// cleanup flyover that is done, just set inView and active to 0
void cleanFlyover(int flyover) {
    int offset = flyover * 3;
    for (int i = 0; i < MAX_HELIS_IN_FORMATION; i++) {
        helicopters[offset+i].inView = 0;
    }
    flyovers[flyover].active = 0;
}

// helicopter's idle func
void helicopterIdle(double t, double dt) {
    // move blades
    for (int i = 0; i < N_HELICOPTERS; i++) {
        if (helicopters[i].inView) {
            helicopters[i].mainRotorAngle += helicopters[i].mainRotorSpeed * dt;
            helicopters[i].mainRotorAngle = fmod(helicopters[i].mainRotorAngle, 360.0);
            helicopters[i].secondaryRotorAngle += helicopters[i].mainRotorSpeed * dt;
            helicopters[i].secondaryRotorAngle = fmod(helicopters[i].secondaryRotorAngle, 360.0);
        }
    }

    // progress flyovers
    for (int i = 0; i < MAX_FLYOVERS; i++) {
        if (flyovers[i].active) {
            flyovers[i].pos += (flyovers[i].speed * dt);
            if (flyovers[i].pos > 2*WORLD_RADIUS) {
                cleanFlyover(i);
            } else {
                updateFormation(i, dt);
            }
        }
    }

    // initiate new flyover every once in a while
    if (timeForNewFlyover(t)) {
        initFlyover(t);
    }
}

// Draw a rotor
void drawRotor(double x, double y, double z, double xth, double yth, double scale, double rotorWidth, double shaftHeight) {
    double shaftWidth = 0.04;
    double rodMinorScale = 2;
    double rodMajorScale = 4;
    double rotorLength = 1;
    double rotorShaftStep = 30;
    double rotorThickness = 0.02;
    double rotorRepeat = 6;

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glPushMatrix();

    // allow rotor to be rotated around x-axis as well for the stabilizing rotor
    glTranslated(x, y, z);
    glRotated(xth, 1.0, 0, 0);
    glRotated(yth, 0, 1.0, 0);
    glScaled(scale, scale, scale);

    // ROTOR SHAFT
    double rx, rz, ra = 0;
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += rotorShaftStep) {
        rx = Cos(th)*shaftWidth;
        rz = Sin(th)*shaftWidth;
        ra = th / 360.0;
        glNormal3d(rx, 0, rz);
        glTexCoord2d(ra, 0); glVertex3d(rx, 0,           rz);
        glTexCoord2d(ra, 1); glVertex3d(rx, shaftHeight, rz);
    }
    glEnd();

    // MIDDLE HEXAGON
    glBegin(GL_POLYGON);
    // Bottom
    glNormal3d(0,-1,0);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight, shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight, -shaftWidth);
    glVertex3d(-shaftWidth,                 shaftHeight, -rodMinorScale*shaftWidth);
    glVertex3d(shaftWidth,                  shaftHeight, -rodMinorScale*shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight, -shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight, shaftWidth);
    glVertex3d(shaftWidth,                  shaftHeight, rodMinorScale*shaftWidth);
    glVertex3d(-shaftWidth,                 shaftHeight, rodMinorScale*shaftWidth);
    glEnd();

    glBegin(GL_POLYGON);
    // Top
    glNormal3d(0,1,0);
    glTexCoord2d(-rodMinorScale/2, 0.5);    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight+rotorThickness, shaftWidth);
    glTexCoord2d(-rodMinorScale/2, -0.5);   glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight+rotorThickness, -shaftWidth);
    glTexCoord2d(-0.5, -rodMinorScale/2);   glVertex3d(-shaftWidth,                 shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glTexCoord2d(0.5, -rodMinorScale/2);    glVertex3d(shaftWidth,                  shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glTexCoord2d(rodMinorScale/2,  -0.5);   glVertex3d(rodMinorScale*shaftWidth,    shaftHeight+rotorThickness, -shaftWidth);
    glTexCoord2d(rodMinorScale/2,  0.5);    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight+rotorThickness, shaftWidth);
    glTexCoord2d(0.5, rodMinorScale/2);     glVertex3d(shaftWidth,                  shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glTexCoord2d(-0.5, rodMinorScale/2);    glVertex3d(-shaftWidth,                 shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glEnd();

    // ROTOR CONNECTING RODS
    glBegin(GL_QUADS);
    // left bottom
    glNormal3d(0,-1,0);
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight, shaftWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight, rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight, -rotorWidth);
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight, -shaftWidth);
    // back bottom
    glVertex3d(-shaftWidth, shaftHeight, -rodMinorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight, -rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight, -rodMajorScale*shaftWidth);
    glVertex3d(shaftWidth,  shaftHeight, -rodMinorScale*shaftWidth);
    // right bottom
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight, -shaftWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight, -rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight, rotorWidth);
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight, shaftWidth);
    // front bottom
    glVertex3d(shaftWidth,  shaftHeight, rodMinorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight, rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight, rodMajorScale*shaftWidth);
    glVertex3d(-shaftWidth, shaftHeight, rodMinorScale*shaftWidth);
    // left top
    glNormal3d(0,1,0);
    glTexCoord2d(0,0); glVertex3d(-rodMinorScale*shaftWidth, shaftHeight+rotorThickness, -shaftWidth);
    glTexCoord2d(0,1); glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(1,1); glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness, rotorWidth);    
    glTexCoord2d(1,0); glVertex3d(-rodMinorScale*shaftWidth, shaftHeight+rotorThickness, shaftWidth);
    // back top
    glTexCoord2d(0,0); glVertex3d(-shaftWidth, shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glTexCoord2d(0,1); glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glTexCoord2d(1,1); glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glTexCoord2d(1,0); glVertex3d(shaftWidth,  shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    // right top
    glTexCoord2d(0,0); glVertex3d(rodMinorScale*shaftWidth, shaftHeight+rotorThickness, -shaftWidth);
    glTexCoord2d(0,1); glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(1,1); glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(1,0); glVertex3d(rodMinorScale*shaftWidth, shaftHeight+rotorThickness, shaftWidth);
    // front top
    glTexCoord2d(0,0); glVertex3d(shaftWidth,  shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glTexCoord2d(0,1); glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glTexCoord2d(1,1); glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glTexCoord2d(1,0); glVertex3d(-shaftWidth, shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glEnd();

    // CONNECTING RODS EDGES
    glBegin(GL_QUADS);
    // NE
    glNormal3d(1,0,-1);
    glVertex3d(shaftWidth,                  shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight+rotorThickness, -shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight,                -shaftWidth);
    glVertex3d(shaftWidth,                  shaftHeight,                -rodMinorScale*shaftWidth);
    // SE
    glNormal3d(1,0,1);
    glVertex3d(shaftWidth,                  shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight+rotorThickness, shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth,    shaftHeight,                shaftWidth);
    glVertex3d(shaftWidth,                  shaftHeight,                rodMinorScale*shaftWidth);
    // SW
    glNormal3d(-1,0,1);
    glVertex3d(-shaftWidth,                 shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight+rotorThickness, shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight,                shaftWidth);
    glVertex3d(-shaftWidth,                 shaftHeight,                rodMinorScale*shaftWidth);
    // NW
    glNormal3d(-1,0,-1);
    glVertex3d(-shaftWidth,                 shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight+rotorThickness, -shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth,   shaftHeight,                -shaftWidth);
    glVertex3d(-shaftWidth,                 shaftHeight,                -rodMinorScale*shaftWidth);
    // N - left
    glNormal3d(-1,0,0);
    glVertex3d(-shaftWidth, shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight,                -rodMajorScale*shaftWidth);
    glVertex3d(-shaftWidth, shaftHeight,                -rodMinorScale*shaftWidth);
    // S - right
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glVertex3d(-shaftWidth, shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glVertex3d(-shaftWidth, shaftHeight,                rodMinorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth);
    // W - far
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness,   rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness,   -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight,                  -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight,                  rotorWidth);
    // N - right
    glNormal3d(1,0,0);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glVertex3d(shaftWidth,  shaftHeight+rotorThickness, -rodMinorScale*shaftWidth);
    glVertex3d(shaftWidth,  shaftHeight,                -rodMinorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth);
    // E - far
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness,    -rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness,    rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight,                   rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight,                   -rotorWidth);
    // S - left
    glVertex3d(shaftWidth,  shaftHeight+rotorThickness, rodMinorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight,                rodMajorScale*shaftWidth);
    glVertex3d(shaftWidth,  shaftHeight,                rodMinorScale*shaftWidth);
    // N - far
    glNormal3d(0,0,-1);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight,                -rodMajorScale*shaftWidth);
    // E - left
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight+rotorThickness,    -shaftWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness,    -rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight,                   -rotorWidth);
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight,                   -shaftWidth);
    // W - right
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness,   -rotorWidth);
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight+rotorThickness,   -shaftWidth);
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight,                  -shaftWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight,                  -rotorWidth);
    // E - right
    glNormal3d(0,0,1);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight+rotorThickness,    rotorWidth);
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight+rotorThickness,    shaftWidth);
    glVertex3d(rodMinorScale*shaftWidth, shaftHeight,                   shaftWidth);
    glVertex3d(rodMajorScale*shaftWidth, shaftHeight,                   rotorWidth);
    // S - far
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight,                rodMajorScale*shaftWidth);
    // W - left
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight+rotorThickness,   shaftWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight+rotorThickness,   rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth, shaftHeight,                  rotorWidth);
    glVertex3d(-rodMinorScale*shaftWidth, shaftHeight,                  shaftWidth);
    glEnd();

    // MAIN ROTORS
    glBegin(GL_QUADS);
    // left top
    glNormal3d(0,1,0);
    glTexCoord2d(1,0);              glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(0,0);              glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, -rotorWidth);
    // back top
    glTexCoord2d(0,0);              glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glTexCoord2d(1,0);              glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    // right top
    glTexCoord2d(0,rotorRepeat);    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(0,0);              glVertex3d(rodMajorScale*shaftWidth,                shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(1,0);              glVertex3d(rodMajorScale*shaftWidth,                shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, rotorWidth);
    // front top
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    glTexCoord2d(1,0);              glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glTexCoord2d(0,0);              glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    // left bottom
    glNormal3d(0,-1,rotorThickness);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(1,0);              glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight+rotorThickness, rotorWidth);
    glTexCoord2d(0,0);              glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight,                -rotorWidth);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight,                -rotorWidth);
    // back bottom
    glNormal3d(-rotorThickness,-1,0);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    glTexCoord2d(0,0);              glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glTexCoord2d(1,0);              glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth-rotorLength);
    // right bottom
    glNormal3d(0,-1,-rotorThickness);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(0,0);              glVertex3d(rodMajorScale*shaftWidth,                shaftHeight+rotorThickness, -rotorWidth);
    glTexCoord2d(1,0);              glVertex3d(rodMajorScale*shaftWidth,                shaftHeight,                rotorWidth);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight,                rotorWidth);
    // front bottom
    glNormal3d(rotorThickness,-1,0);
    glTexCoord2d(1,rotorRepeat);    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    glTexCoord2d(1,0);              glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glTexCoord2d(0,0);              glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth);
    glTexCoord2d(0,rotorRepeat);    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth+rotorLength);
    glEnd();

    // ROTOR BACKS
    glBegin(GL_QUADS);
    // left
    glNormal3d(0,0,-1);
    glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight+rotorThickness, -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight,                -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth,               shaftHeight,                -rotorWidth);
    // back
    glNormal3d(1,0,0);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth-rotorLength);
    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth);
    // right
    glNormal3d(0,0,1);
    glVertex3d(rodMajorScale*shaftWidth,                shaftHeight+rotorThickness, rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight,                rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth,                shaftHeight,                rotorWidth);
    // front
    glNormal3d(-1,0,0);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth+rotorLength);
    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth);
    glEnd();

    // ROTOR TIPS
    glBegin(GL_TRIANGLES);
    // left
    glNormal3d(-1,0,0);
    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight+rotorThickness, -rotorWidth);
    glVertex3d(-rodMajorScale*shaftWidth-rotorLength,   shaftHeight,                -rotorWidth);
    // back
    glNormal3d(0,0,-1);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, -rodMajorScale*shaftWidth-rotorLength);
    glVertex3d(rotorWidth,  shaftHeight,                -rodMajorScale*shaftWidth-rotorLength);
    // right
    glNormal3d(1,0,0);
    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, -rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight+rotorThickness, rotorWidth);
    glVertex3d(rodMajorScale*shaftWidth+rotorLength,    shaftHeight,                rotorWidth);
    // front
    glNormal3d(0,0,1);
    glVertex3d(rotorWidth,  shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    glVertex3d(-rotorWidth, shaftHeight+rotorThickness, rodMajorScale*shaftWidth+rotorLength);
    glVertex3d(-rotorWidth, shaftHeight,                rodMajorScale*shaftWidth+rotorLength);
    glEnd();

    glPopMatrix();
}

// Draw a helicopter
void drawHelicopter(double x, double y, double z, double yaw, double pitch, double roll, double scale, double mainRotorAngle, double secondaryRotorAngle, struct helicopterTheme theme) {
    int angleStep = 60;     // define how many sides the fuselage will have

    double nose = -1.0;     // x of the tip of the nose

    double cone1 = -0.9;
    double wid1 = 0.05;     // dimensions of the end of the first section
    double yOff1 = -0.1;    // offset ensures the nose is lower than the body
    
    double cone2 = -0.3;    // dimensions of the end of the second section
    double wid2 = 0.3;
    
    double cone3 = 0.1;     // third section
    double wid3 = 0.3;
    
    double cone4 = 0.3;
    double wid4 = 0.18;     // fourth section
    double yOff4 = -0.02;   // slightly lower the tail section

    double tailTip = 1.2;   // end of the tail x

    double tailFinX1 = 0.9;     // dimensions to define the tail fin and its location
    double tailFinX2 = 1.1;
    double tailFinOffset = 0.2;
    double tailFinHeight = 0.3;
    double tailFinThickness = 0.02;

    double mainRotorInset = 0.05;   // dimensions and parameters for rotor placement
    double mainRotorScale = 1.0;
    double mainRotorWidth = 0.05;
    double mainRotorShaftHeight = 0.1; 
    double secondaryRotorInset = 0.015;
    double secondaryRotorScale = 0.2;
    double secondaryRotorWidth = 0.08;
    double secondaryRotorShaftHeight = 0.3;

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(yaw, 0, 1.0, 0);
    glRotated(pitch, 0, 0, 1.0);
    glRotated(roll, 1.0, 0, 0);
    glScaled(scale, scale, scale);
    glColor3d(1,1,1);

    // NOSE CONE
    glBindTexture(GL_TEXTURE_2D, helicopterTexture[theme.body]);
    glBegin(GL_TRIANGLES);
    for (int th = 0; th < 360; th += angleStep) {
        glNormal3d(-wid1, Cos(th+angleStep/2)*(cone1-nose), Sin(th+angleStep/2)*(cone1-nose));
        glTexCoord2d(0.5,1);    glVertex3d(nose,    yOff1,                          0.0);
        glTexCoord2d(0,0);      glVertex3d(cone1,   wid1*Cos(th)+yOff1,            wid1*Sin(th));
        glTexCoord2d(1,0);      glVertex3d(cone1,   wid1*Cos(th+angleStep)+yOff1,  wid1*Sin(th+angleStep));
    }
    glEnd();

    // FRONT section
    double widRatio = (wid1/wid2) / 2;
    // WINDOWS
    glBindTexture(GL_TEXTURE_2D, helicopterTexture[theme.window]);
    glBegin(GL_QUADS);
    for (int th = 0; th < 360; th += 360-angleStep) {
        glNormal3d(-(wid2-wid1), Cos(th+angleStep/2)*(cone2-cone1), Sin(th+angleStep/2)*(cone2-cone1));
        glTexCoord2d(0,0.5+widRatio);   glVertex3d(cone1, wid1*Cos(th)+yOff1,           wid1*Sin(th));
        glTexCoord2d(1,1);              glVertex3d(cone2, wid2*Cos(th),                 wid2*Sin(th));
        glTexCoord2d(1,0);              glVertex3d(cone2, wid2*Cos(th+angleStep),       wid2*Sin(th+angleStep));
        glTexCoord2d(0,0.5-widRatio);   glVertex3d(cone1, wid1*Cos(th+angleStep)+yOff1, wid1*Sin(th+angleStep));
    }
    glEnd();
    // BODY
    glBindTexture(GL_TEXTURE_2D, helicopterTexture[theme.body]);
    glBegin(GL_QUADS);
    for (int th = angleStep; th < 360-angleStep; th += angleStep) {
        glNormal3d(-(wid2-wid1), Cos(th+angleStep/2)*(cone2-cone1), Sin(th+angleStep/2)*(cone2-cone1));
        glTexCoord2d(0,0.5+widRatio);   glVertex3d(cone1, wid1*Cos(th)+yOff1,            wid1*Sin(th));
        glTexCoord2d(1,1);              glVertex3d(cone2, wid2*Cos(th),                  wid2*Sin(th));
        glTexCoord2d(1,0);              glVertex3d(cone2, wid2*Cos(th+angleStep),        wid2*Sin(th+angleStep));
        glTexCoord2d(0,0.5-widRatio);   glVertex3d(cone1, wid1*Cos(th+angleStep)+yOff1,  wid1*Sin(th+angleStep));
    }
    glEnd();

    // MIDDLE section
    glBegin(GL_QUADS);
    for (int th = 0; th < 360; th += angleStep) {
        glNormal3d(-(wid3-wid2), Cos(th+angleStep/2)*(cone3-cone2), Sin(th+angleStep/2)*(cone3-cone2));
        glTexCoord2d(0,1); glVertex3d(cone2, wid2*Cos(th),              wid2*Sin(th));
        glTexCoord2d(1,1); glVertex3d(cone3, wid3*Cos(th),              wid3*Sin(th));
        glTexCoord2d(1,0); glVertex3d(cone3, wid3*Cos(th+angleStep),    wid3*Sin(th+angleStep));
        glTexCoord2d(0,0); glVertex3d(cone2, wid2*Cos(th+angleStep),    wid2*Sin(th+angleStep));
    }
    glEnd();

    // END section
    widRatio = (wid4/wid3) / 2;
    glBegin(GL_QUADS);
    for (int th = 0; th < 360; th += angleStep) {
        glNormal3d(-(wid4-wid3), Cos(th+angleStep/2)*(cone4-cone3), Sin(th+angleStep/2)*(cone4-cone3));
        glTexCoord2d(0,1);              glVertex3d(cone3, wid3*Cos(th),                 wid3*Sin(th));
        glTexCoord2d(1,0.5+widRatio);   glVertex3d(cone4, wid4*Cos(th)+yOff4,           wid4*Sin(th));
        glTexCoord2d(1,0.5-widRatio);   glVertex3d(cone4, wid4*Cos(th+angleStep)+yOff4, wid4*Sin(th+angleStep));
        glTexCoord2d(0,0);              glVertex3d(cone3, wid3*Cos(th+angleStep),       wid3*Sin(th+angleStep));
    }
    glEnd();

    // TAIL section
    glBegin(GL_TRIANGLES);
    for (int th = 0; th < 360; th += angleStep) {
        glNormal3d(wid4, Cos(th+angleStep/2)*(tailTip-cone4), Sin(th+angleStep/2)*(tailTip-cone4));
        glTexCoord2d(3,0.5);    glVertex3d(tailTip, 0.0,                            0.0);
        glTexCoord2d(0,0);      glVertex3d(cone4,   wid4*Cos(th)+yOff4,             wid4*Sin(th));
        glTexCoord2d(0,1);      glVertex3d(cone4,   wid4*Cos(th+angleStep)+yOff4,   wid4*Sin(th+angleStep));
    }
    glEnd();

    // TAIL FIN
    glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glTexCoord2d(0,0); glVertex3d(tailFinX1, 0.0, tailFinThickness);
    glTexCoord2d(1,0); glVertex3d(tailFinX2, 0.0, tailFinThickness);
    glTexCoord2d(1,1); glVertex3d(tailFinX2+tailFinOffset/2, tailFinHeight, 0);
    glTexCoord2d(0,1); glVertex3d(tailFinX1+tailFinOffset, tailFinHeight, 0);
    glNormal3d(0,0,-1);
    glTexCoord2d(0,0); glVertex3d(tailFinX1, 0.0, -tailFinThickness);
    glTexCoord2d(1,0); glVertex3d(tailFinX2, 0.0, -tailFinThickness);
    glTexCoord2d(1,1); glVertex3d(tailFinX2+tailFinOffset/2, tailFinHeight, 0);
    glTexCoord2d(0,1); glVertex3d(tailFinX1+tailFinOffset, tailFinHeight, 0);
    glEnd();

    // TAIL FIN EDGES
    glBegin(GL_TRIANGLES);
    glNormal3d(1,0,0);
    glVertex3d(tailFinX2, 0.0, tailFinThickness);
    glVertex3d(tailFinX2, 0.0, -tailFinThickness);
    glVertex3d(tailFinX2+tailFinOffset/2, tailFinHeight, 0);
    glNormal3d(-1,0,0);
    glVertex3d(tailFinX1, 0.0, tailFinThickness);
    glVertex3d(tailFinX1, 0.0, -tailFinThickness);
    glVertex3d(tailFinX1+tailFinOffset, tailFinHeight, 0);
    glEnd();

    // ROTORS
    glBindTexture(GL_TEXTURE_2D, helicopterTexture[theme.rotor]);
    // main rotor
    drawRotor((cone2+cone3)/2, wid3-mainRotorInset/mainRotorScale, 0, 0, mainRotorAngle, mainRotorScale, mainRotorWidth, mainRotorShaftHeight);
    // tail rotor
    drawRotor((tailFinX1+tailFinX2)/2, 0, secondaryRotorInset, 90, secondaryRotorAngle, secondaryRotorScale, secondaryRotorWidth, secondaryRotorShaftHeight);

    glPopMatrix();
}

// draw all of the helicopters
void drawHelicopters() {
    for (int i = 0; i < N_HELICOPTERS; i++) {
        if (helicopters[i].inView) {
            drawHelicopter(
                helicopters[i].x,
                helicopters[i].y,
                helicopters[i].z,
                helicopters[i].yaw,
                helicopters[i].pitch,
                helicopters[i].roll,
                helicopters[i].scale,
                helicopters[i].mainRotorAngle,
                helicopters[i].secondaryRotorAngle,
                helicopterThemes[helicopters[i].theme]);
        }
    }
}

void helicopterInit(unsigned int *helicopterTexturePtr) {
    srand(time(0));
    helicopterTexture = helicopterTexturePtr;
}
