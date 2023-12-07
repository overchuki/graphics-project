/*
 *   This is a c file that contains all ground related files
 */

#include "CSCIx229.h"
#include "igor.h"

// Ground Globals
/*
    Textures:
    0: asphalt
    1: ground
    2: sky surround
*/
unsigned int *groundTexture;

// Define roads as 4 bounds: left, right, top, bottom
struct road roads[N_ROADS] = {
    {-WORLD_RADIUS, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.8},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, WORLD_RADIUS, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+0.8},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, -WORLD_RADIUS, WORLD_RADIUS},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.5, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6},
    {-WORLD_WALK_RADIUS+TOWN_OFFSET+2.8, WORLD_WALK_RADIUS-2.5, WORLD_WALK_RADIUS-3.0, WORLD_WALK_RADIUS-2.7}
};

// World elevation map, as well as texture repeats and step size for each row/col
double worldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH];
double topoStep = ((double)WORLD_RADIUS*2) / ((double)WORLD_TOPO_WIDTH-1);
double groundRepeat = ((double)WORLD_TOPO_WIDTH-1) / (double)GROUND_REPEAT;

// cross product formula to find normals
void crossProduct(double a[3], double b[3], double res[3]) {
    res[0] = (a[1] * b[2]) - (a[2] * b[1]);
    res[1] = (a[2] * b[0]) - (a[0] * b[2]);
    res[2] = (a[0] * b[1]) - (a[1] * b[0]);
}

// Get elevation at certain position, somewhat overcomplicated equation I came up with to find elevation of a point based on the "square" it's in
// Isn't fully accurate since there can't always be a perfectly straight plane defined by 4 points, but it does the job
// Averages each of the four points' elevation by evaluating the proximity of the coords to it
double getElevation(double x, double z) {
    double topoX = ((x+WORLD_RADIUS) / (2*(double)WORLD_RADIUS)) * ((double)WORLD_TOPO_WIDTH-1);
    double topoZ = ((z+WORLD_RADIUS) / (2*(double)WORLD_RADIUS)) * ((double)WORLD_TOPO_WIDTH-1);
    int px = floor(topoX);
    int pz = floor(topoZ);
    double diffX = topoX - px;
    double diffZ = topoZ - pz;
    return (((diffX * worldTopo[pz][px+1]) + ((1-diffX) * worldTopo[pz][px])) * (1-diffZ) +
            ((diffX * worldTopo[pz+1][px+1]) + ((1-diffX) * worldTopo[pz+1][px])) * diffZ +
            ((diffZ * worldTopo[pz+1][px]) + ((1-diffZ) * worldTopo[pz][px])) * (1-diffX) +
            ((diffZ * worldTopo[pz+1][px+1]) + ((1-diffZ) * worldTopo[pz][px+1])) * diffX) / 2;
}

// Get a normal for a ground vertex by averaging all 4 cross products around it
void getGroundNormal(double resultNorm[3], int r, int c) {
    // Init averages to 0
    double a[3];
    double b[3];
    double norms[4][3] = {
        {0,0,0},
        {0,0,0},
        {0,0,0},
        {0,0,0}
    };
    int tot = 0;

    // bottom and right
    if (r < WORLD_TOPO_WIDTH-1 && c < WORLD_TOPO_WIDTH-1) {
        a[0] = 0; a[1] = worldTopo[r+1][c] - worldTopo[r][c]; a[2] = 1;
        b[0] = 1; b[1] = worldTopo[r][c+1] - worldTopo[r][c]; b[2] = 0;
        crossProduct(a, b, norms[0]);
        tot++;
    }
    
    // left and bottom
    if (r < WORLD_TOPO_WIDTH-1 && c > 0) {
        a[0] = -1; a[1] = worldTopo[r][c-1] - worldTopo[r][c]; a[2] = 0;
        b[0] = 0; b[1] = worldTopo[r+1][c] - worldTopo[r][c]; b[2] = 1;
        crossProduct(a, b, norms[1]);
        tot++;
    }
    
    // top and left
    if (r > 0 && c > 0) {
        a[0] = 0; a[1] = worldTopo[r-1][c] - worldTopo[r][c]; a[2] = -1;
        b[0] = -1; b[1] = worldTopo[r][c-1] - worldTopo[r][c]; b[2] = 0;
        crossProduct(a, b, norms[2]);
        tot++;
    }
    
    // right and top
    if (r > 0 && c < WORLD_TOPO_WIDTH-1) {
        a[0] = 1; a[1] = worldTopo[r][c+1] - worldTopo[r][c]; a[2] = 0;
        b[0] = 0; b[1] = worldTopo[r-1][c] - worldTopo[r][c]; b[2] = -1;
        crossProduct(a, b, norms[3]);
        tot++;
    }

    // take mean of the cross products to get true ground normal, guaranteed tot will > 0
    resultNorm[0] = (norms[0][0]+norms[1][0]+norms[2][0]+norms[3][0]) / tot;
    resultNorm[1] = (norms[0][1]+norms[1][1]+norms[2][1]+norms[3][1]) / tot;
    resultNorm[2] = (norms[0][2]+norms[1][2]+norms[2][2]+norms[3][2]) / tot;
}

// Draw the ground
void drawGround() {

    // color behind texture all white
    glColor3d(1,1,1);

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,black);

    // enable polygon offset for roads
    glEnable(GL_POLYGON_OFFSET_FILL);

    // offset of 2 for the ground
    glPolygonOffset(2,2);

    // draw ground as grid of quads based on the elevation map, using previously set values to calculate position and texture repeat
    glBindTexture(GL_TEXTURE_2D, groundTexture[1]);
    glBegin(GL_QUADS);
    double norm[3];
    double wx, wz = 0;
    for (int r = 0; r < WORLD_TOPO_WIDTH-1; r++) {
        for (int c = 0; c < WORLD_TOPO_WIDTH-1; c++) {
            wx = -WORLD_RADIUS + topoStep*c;
            wz = -WORLD_RADIUS + topoStep*r;

            getGroundNormal(norm, r, c);
            glNormal3d(norm[0], norm[1], norm[2]);
            glTexCoord2d(r/groundRepeat, c/groundRepeat);
            glVertex3d(wx, worldTopo[r][c], wz);
            
            getGroundNormal(norm, r, c+1);
            glNormal3d(norm[0], norm[1], norm[2]);
            glTexCoord2d(r/groundRepeat, (c+1)/groundRepeat);
            glVertex3d(wx+topoStep, worldTopo[r][c+1], wz);
            
            getGroundNormal(norm, r+1, c+1);
            glNormal3d(norm[0], norm[1], norm[2]);
            glTexCoord2d((r+1)/groundRepeat,(c+1)/groundRepeat);
            glVertex3d(wx+topoStep, worldTopo[r+1][c+1], wz+topoStep);
            
            getGroundNormal(norm, r+1, c);
            glNormal3d(norm[0], norm[1], norm[2]);
            glTexCoord2d((r+1)/groundRepeat,c/groundRepeat);
            glVertex3d(wx, worldTopo[r+1][c], wz+topoStep);
        }
    }
    glEnd();

    // ROADS
    glPolygonOffset(1,1);
    glBindTexture(GL_TEXTURE_2D, groundTexture[0]);
    glBegin(GL_QUADS);
    glNormal3d(0,1,0);
    int roadRepeatX, roadRepeatZ;
    double l,r,b,t;
    for(int i = 0; i < N_ROADS; i++) {
        roadRepeatX = ceil((roads[i].right - roads[i].left) / ROAD_REPEAT_DIST);
        roadRepeatZ = ceil(((roads[i].bottom - roads[i].top) / ROAD_REPEAT_DIST));
        for(int x = 0; x < roadRepeatX; x++) {
            for(int z = 0; z < roadRepeatZ; z++) {
                l = roads[i].left + (ROAD_REPEAT_DIST * x);
                r = igormin(roads[i].right, roads[i].left + (ROAD_REPEAT_DIST * (x+1)));
                t = roads[i].top + (ROAD_REPEAT_DIST * z);
                b = igormin(roads[i].bottom, roads[i].top + (ROAD_REPEAT_DIST * (z+1)));
                glTexCoord2d(0,0); glVertex3d(l, 0, b);
                glTexCoord2d(0,2); glVertex3d(l, 0, t);
                glTexCoord2d(1,2); glVertex3d(r, 0, t);
                glTexCoord2d(1,0); glVertex3d(r, 0, b);
            }
        }
    }
    glEnd();

    // culdesac (somewhat hardcoded)
    glBegin(GL_POLYGON);
    glNormal3d(0,1,0);
    glTexCoord2d(3,2); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6);
    glTexCoord2d(3,4); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.5, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3);
    glTexCoord2d(2,6); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.2, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.0);
    glTexCoord2d(1,6); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.9, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.0);
    glTexCoord2d(0,4); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.6, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.3);
    glTexCoord2d(0,3); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.6, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.6);
    glTexCoord2d(1,0); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+0.9, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.9);
    glTexCoord2d(2,0); glVertex3d(-WORLD_WALK_RADIUS+TOWN_OFFSET+1.2, 0, -WORLD_WALK_RADIUS+TOWN_OFFSET+2.9);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
}

// draw the sky
void drawSky(double lightIntensity) {

    // Save transformation
    glPushMatrix();

    // background color white with adjusted intensity
    glColor3d(lightIntensity,lightIntensity,lightIntensity);

    // sky surroundings drawn as a dome, overlaying a spherical sky texture as if it was a basic rectangle
    double skyIncrement = 10.0;
    double skyX, skyY, skyZ = 0;
    glBindTexture(GL_TEXTURE_2D, groundTexture[2]);
    for (double sky_ph=0;sky_ph<90;sky_ph+=skyIncrement) {
        glBegin(GL_QUAD_STRIP);
        for (double sky_th=0;sky_th<=360;sky_th+=2*skyIncrement) {
            skyX = Cos(sky_th)*Cos(sky_ph);
            skyY = Sin(sky_ph);
            skyZ = Sin(sky_th)*Cos(sky_ph);
            glTexCoord2d(sky_th/360.0, sky_ph/90.0); glVertex3d(skyX*WORLD_RADIUS, skyY*WORLD_RADIUS, skyZ*WORLD_RADIUS);
            skyX = Cos(sky_th)*Cos(sky_ph+skyIncrement);
            skyY = Sin(sky_ph+skyIncrement);
            skyZ = Sin(sky_th)*Cos(sky_ph+skyIncrement);
            glTexCoord2d(sky_th/360.0, (sky_ph+skyIncrement)/90.0); glVertex3d(skyX*WORLD_RADIUS, skyY*WORLD_RADIUS, skyZ*WORLD_RADIUS);
        }
        glEnd();
    }

    glPopMatrix();
}

// Init ground
void groundInit(unsigned int *groundTexturePtr) {
    groundTexture = groundTexturePtr;

    ReadMapDEM("dem/map.dem", worldTopo);
}
