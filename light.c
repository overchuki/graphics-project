/*
 *   This is a c file that contains all lighting related files
 *   I have copied the "ball" and "Vertex" functions from ex13 to draw my sun
 */

#include "CSCIx229.h"
#include "igor.h"

// Lighting Globals
struct rgb sunColor = {1.0, 1.0, 0.8};
double lightingTh = 0;
double lightingSpeed = DEFAULT_SUN_SPEED;
double lightingRadius = WORLD_RADIUS - (2*SUN_RADIUS);

int sunMove = 1;    // Sun is moving
int light = 1;      // Lighting
int local = 0;      // Local Viewer Model
int emission = 100; // Emission intensity (%)
int ambient = AMBIENT_FLOOR;   // Ambient intensity (%)
int diffuse = DIFFUSE_FLOOR;   // Diffuse intensity (%)
int specular = 0;   // Specular intensity (%)
int shininess = 0;  // Shininess (power of two)
int shiny = 1;      // Shiny sun

// TODO: tmp


// Track light strength and sun position
double sunPosition, lightStrength;

void lightingIdle(double dt, double *lightIntensity) {
    if (sunMove) {
        // Move sun faster at night to reduce nights
        double nightFactor = 1;

        // determine strength of lighting based on sun's position, highest at the top, 0 once it dips a little below horizon
        sunPosition = Sin(lightingTh);
        lightStrength = (sunPosition + SUN_RADIUS/lightingRadius + 0.2) / (1.2 + SUN_RADIUS/lightingRadius);
        if (lightStrength < 0.01) {
            nightFactor = 2;
            lightStrength = 0;
            *lightIntensity = 0.5;
            ambient = AMBIENT_FLOOR;
        } else {
            // use a logarithmic function to determine strength so most of the day is bright and darkness only comes in/out at sunset/sunrise
            lightStrength = (log10(lightStrength)+2) / 2;
            *lightIntensity = (lightStrength + 0.5) / 1.5;
            ambient = AMBIENT_FLOOR + (lightStrength*AMBIENT_RANGE);
        }

        // turn off diffuse right as sun sets, but delay ambient until a little after sunset
        if (sunPosition + SUN_RADIUS/lightingRadius <= 0) {
            diffuse = 0;
        } else {
            diffuse = DIFFUSE_FLOOR + ((sunPosition + (SUN_RADIUS/lightingRadius))*DIFFUSE_RANGE);
        }

        // Move sun around in a circle
        lightingTh += lightingSpeed * dt * nightFactor;
        lightingTh = fmod(lightingTh, 360.0);
    }
}

// This is largely taken from ex13, with a few small tweaks
void lightingDisplay(double lightIntensity) {
    // Smooth shading
    glShadeModel(GL_SMOOTH);

    // Light switch
    if (light) {
        // Translate intensity to color vectors
        float Ambient[] = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[] = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[] = {0.01*specular,0.01*specular,0.01*specular,1.0};
        
        // Light position
        float Position[] = {lightingRadius*Cos(lightingTh), lightingRadius*Sin(lightingTh), 0};
        
        // Draw light position as the sun
        drawSun(Position[0], Position[1], Position[2], SUN_RADIUS);
        
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
        glLightfv(GL_LIGHT0,GL_AMBIENT, Ambient);
        glLightfv(GL_LIGHT0,GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
        glLightfv(GL_LIGHT0,GL_POSITION,Position);
    } else {
        glDisable(GL_LIGHTING);
    }
}

void lightingToggle() {
    light = !light;
}

void lightingPause() {
    sunMove = !sunMove;
}

void changeLightSpeed(int speedUp) {
    lightingSpeed += speedUp;
    if (lightingSpeed > SUN_MAX_SPEED) {
        lightingSpeed = SUN_MAX_SPEED;
    } else if (lightingSpeed < SUN_MIN_SPEED) {
        lightingSpeed = SUN_MIN_SPEED;
    }
}

// COPY of 'Vertex' function from ex13 to draw sun
void sunVertex(double th,double ph) {
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

// COPY of 'ball' function from ex13
void drawSun(double x,double y,double z,double r) {
    // Save transformation
    glPushMatrix();
    // Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    // White ball with yellow specular
    float yellow[]   = {1.0,1.0,0.0,1.0};
    float Emission[] = {1.0,1.0,0.08,1.0};
    glMaterialf(GL_FRONT,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

    glColor3f(sunColor.r, sunColor.g, sunColor.b);
    // Bands of latitude
    for (int ph=-90;ph<90;ph+=SUN_INCR)
    {
        glBegin(GL_QUAD_STRIP);
        for (int th=0;th<=360;th+=2*SUN_INCR)
        {
            sunVertex(th,ph);
            sunVertex(th,ph+SUN_INCR);
        }
        glEnd();
    }
    // Undo transofrmations
    glPopMatrix();
}
