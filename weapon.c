/*
 *   This is a c file that contains all weapon related files
 */

#include "CSCIx229.h"
#include "igor.h"

// Weapon Globals
/*
    Textures:
    0: metal
    1: target
    2: wood
*/
unsigned int *weaponTexture;
struct bullet bullets[MAX_BULLETS];
double bulletLocations[MAX_BULLETS][BULLET_REPEAT][3];
struct enemy enemies[N_ENEMIES];
int currentFreeBullet = 0;
int noFreeBullet = 0;
double lastShootTime = 0.0;
int leftMouseDown = 0;
double lastEnemyCheckTime = 0.0;

// weapon data
// #define weaponOffsetX 0.02
// #define weaponOffsetY -CAMERA_HEIGHT / 5
// #define weaponOffsetZ -0.02
// #define weaponHeight 0.1
// #define weaponLen 0.7
// #define weaponWidth 0.05
// #define barrelRadius weaponWidth / 3
// #define barrelLen 0.3
// #define barrelStep 30
// #define gripWidth weaponWidth / 1.5
// #define gripHeight weaponHeight * 2
// #define gripLength weaponLen / 7
// #define gripZOffset weaponLen / 1.5
// #define gripXOffset (weaponWidth-gripWidth) / 2.0
double bulletStart[3] = {(weaponWidth/2)*WEAPON_SCALE + weaponOffsetX, (weaponHeight/1.5)*WEAPON_SCALE + weaponOffsetY, (-barrelLen-weaponLen)*WEAPON_SCALE + weaponOffsetZ};
double bulletDirLocal[3] = {WEAPON_FIRE_OFFSET_X,WEAPON_FIRE_OFFSET_Y,BULLET_SPEED};

// enemy data
double enemyHeight = 0.5*ENEMY_SCALE;
double enemyWidth = 0.4*ENEMY_SCALE;
double enemyHeadHeight = 0.21*ENEMY_SCALE;
double enemyHeadWidth = 0.2*ENEMY_SCALE;
double enemyThickness = 0.1*ENEMY_SCALE;
double enemySupportHeight = 0.5*ENEMY_SCALE;
double textureHeadHeight = 0.7;
double textureHeadStart = 0.25;
double textureHeadEnd = 0.75;
double woodThicknessTex = 0.05;

// bullet triangle data, from ex13
const int bulletN = 20;
const struct bulletTri bulletTriangles[] = {
    { 2, 1, 0}, { 3, 2, 0}, { 4, 3, 0}, { 5, 4, 0}, { 1, 5, 0},
    {11, 6, 7}, {11, 7, 8}, {11, 8, 9}, {11, 9,10}, {11,10, 6},
    { 1, 2, 6}, { 2, 3, 7}, { 3, 4, 8}, { 4, 5, 9}, { 5, 1,10},
    { 2, 7, 6}, { 3, 8, 7}, { 4, 9, 8}, { 5,10, 9}, { 1, 6,10}
};
const struct bulletVertex bulletVertices[] = {
    { 0.000, 0.000, 1.000}, { 0.894, 0.000, 0.447}, { 0.276, 0.851, 0.447},
    {-0.724, 0.526, 0.447}, {-0.724,-0.526, 0.447}, { 0.276,-0.851, 0.447},
    { 0.724, 0.526,-0.447}, {-0.276, 0.851,-0.447}, {-0.894, 0.000,-0.447},
    {-0.276,-0.851,-0.447}, { 0.724,-0.526,-0.447}, { 0.000, 0.000,-1.000}
};

void weaponIdle(double t, double dt, double fpPosVec[3], double h) {
    // Update bullet positions and check if they hit anything
    updateBullets(dt);
    checkHitboxes(t);

    // Every 1 second check status of all enemies
    if (t - lastEnemyCheckTime > 1) {
        checkAllEnemies(t);
        lastEnemyCheckTime = t;
    }

    // Shoot bullets, but limit to a certain fire rate
    if (leftMouseDown && t - lastShootTime > WEAPON_FIRE_RATE) {
        shootBullet(fpPosVec, h);
        lastShootTime = t;
    }
}

// draw all active bullets
void drawBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            drawBullet(bullets[i]);
        }
    }
}

// from example 13 of low poly representation of the sphere (icosasphere)
void drawBullet(struct bullet b) {
    // set color and material
    float black[]  = {0.0,0.0,0.0,1.0};
    float white[]  = {1.0,1.0,1.0,1.0};
    glColor3d(1,1,0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    
    // Set transforms
    glPushMatrix();
    glTranslatef(b.x, b.y, b.z);
    glScalef(BULLET_SCALE, BULLET_SCALE, BULLET_SCALE) ;

    // Since this is a sphere the vertex and normal values are the same
    glVertexPointer(3,GL_DOUBLE,0,bulletVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_DOUBLE,0,bulletVertices);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    // Draw icosahedron (3*N is number of vertices)
    glDrawElements(GL_TRIANGLES,3*bulletN,GL_UNSIGNED_INT,bulletTriangles);
    
    // Reset state
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glPopMatrix();
}

// draw the weapon
void drawWeapon(double x, double y, double z) {
    glPushMatrix();

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glTranslated(x, y, z);
    glScaled(WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE);
    glColor3d(1,1,1);

    // Main Part of Weapon
    glBindTexture(GL_TEXTURE_2D, weaponTexture[0]);
    glBegin(GL_QUADS);
    // left
    glNormal3d(-1,0,0);
    glTexCoord2d(0,1); glVertex3d(0.0, weaponHeight,    -weaponLen);
    glTexCoord2d(0,0); glVertex3d(0.0, 0.0,             -weaponLen);
    glTexCoord2d(5,0); glVertex3d(0.0, 0.0,             0.0);
    glTexCoord2d(5,1); glVertex3d(0.0, weaponHeight,    0.0);
    // right
    glNormal3d(1,0,0);
    glTexCoord2d(0,1); glVertex3d(weaponWidth, weaponHeight,    -weaponLen);
    glTexCoord2d(0,0); glVertex3d(weaponWidth, 0.0,             -weaponLen);
    glTexCoord2d(5,0); glVertex3d(weaponWidth, 0.0,             0.0);
    glTexCoord2d(5,1); glVertex3d(weaponWidth, weaponHeight,    0.0);
    // top
    glNormal3d(0,1,0);
    glTexCoord2d(0,1); glVertex3d(0.0,          weaponHeight, -weaponLen);
    glTexCoord2d(5,1); glVertex3d(weaponWidth,  weaponHeight, -weaponLen);
    glTexCoord2d(5,0); glVertex3d(weaponWidth,  weaponHeight, 0.0);
    glTexCoord2d(0,0); glVertex3d(0.0,          weaponHeight, 0.0);
    // bottom
    glNormal3d(0,-1,0);
    glTexCoord2d(0,1); glVertex3d(0.0,          0.0, -weaponLen);
    glTexCoord2d(5,1); glVertex3d(weaponWidth,  0.0, -weaponLen);
    glTexCoord2d(5,0); glVertex3d(weaponWidth,  0.0, 0.0);
    glTexCoord2d(0,0); glVertex3d(0.0,          0.0, 0.0);
    // front
    glNormal3d(0,0,-1);
    glVertex3d(0.0, 0.0, -weaponLen);
    glVertex3d(weaponWidth, 0.0, -weaponLen);
    glVertex3d(weaponWidth, weaponHeight, -weaponLen);
    glVertex3d(0.0, weaponHeight, -weaponLen);
    // back
    glNormal3d(0,0,1);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(weaponWidth, 0.0, 0.0);
    glVertex3d(weaponWidth, weaponHeight, 0.0);
    glVertex3d(0.0, weaponHeight, 0.0);
    glEnd();

    // Barrel
    double rx, ry, ra = 0;
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += barrelStep) {
        rx = Cos(th)*barrelRadius;
        ry = Sin(th)*barrelRadius;
        ra = th / 360.0;
        glNormal3d(rx, ry, 0);
        glTexCoord2d(ra, 0); glVertex3d((weaponWidth/2)+rx, (weaponHeight/1.5)+ry, -weaponLen);
        glTexCoord2d(ra, 1); glVertex3d((weaponWidth/2)+rx, (weaponHeight/1.5)+ry, -weaponLen-barrelLen);
    }
    glEnd();

    // Grip
    glBegin(GL_QUADS);
    // left
    glNormal3d(-1,0,0);
    glTexCoord2d(1,0); glVertex3d(gripXOffset, -gripHeight,    -gripLength-gripZOffset);
    glTexCoord2d(1,1); glVertex3d(gripXOffset, 0.0,            -gripLength-gripZOffset);
    glTexCoord2d(0,1); glVertex3d(gripXOffset, 0.0,            -gripZOffset);
    glTexCoord2d(0,0); glVertex3d(gripXOffset, -gripHeight,    -gripZOffset);
    // right
    glNormal3d(1,0,0);
    glTexCoord2d(1,0); glVertex3d(gripWidth+gripXOffset, -gripHeight,  -gripLength-gripZOffset);
    glTexCoord2d(1,1); glVertex3d(gripWidth+gripXOffset, 0.0,          -gripLength-gripZOffset);
    glTexCoord2d(0,1); glVertex3d(gripWidth+gripXOffset, 0.0,          -gripZOffset);
    glTexCoord2d(0,0); glVertex3d(gripWidth+gripXOffset, -gripHeight,  -gripZOffset);
    // bottom
    glNormal3d(0,-1,0);
    glTexCoord2d(0,1); glVertex3d(gripXOffset,             -gripHeight, -gripLength-gripZOffset);
    glTexCoord2d(1,1); glVertex3d(gripWidth+gripXOffset,   -gripHeight, -gripLength-gripZOffset);
    glTexCoord2d(1,0); glVertex3d(gripWidth+gripXOffset,   -gripHeight, -gripZOffset);
    glTexCoord2d(0,0); glVertex3d(gripXOffset,             -gripHeight, -gripZOffset);
    // front
    glNormal3d(0,0,-1);
    glTexCoord2d(0,1); glVertex3d(gripXOffset,             0.0,            -gripLength-gripZOffset);
    glTexCoord2d(1,1); glVertex3d(gripWidth+gripXOffset,   0.0,            -gripLength-gripZOffset);
    glTexCoord2d(1,0); glVertex3d(gripWidth+gripXOffset,   -gripHeight,    -gripLength-gripZOffset);
    glTexCoord2d(0,0); glVertex3d(gripXOffset,             -gripHeight,    -gripLength-gripZOffset);
    // back
    glNormal3d(0,0,1);
    glTexCoord2d(0,1); glVertex3d(gripXOffset,             0.0,            -gripZOffset);
    glTexCoord2d(1,1); glVertex3d(gripWidth+gripXOffset,   0.0,            -gripZOffset);
    glTexCoord2d(1,0); glVertex3d(gripWidth+gripXOffset,   -gripHeight,    -gripZOffset);
    glTexCoord2d(0,0); glVertex3d(gripXOffset,             -gripHeight,    -gripZOffset);
    glEnd();

    glPopMatrix();
}

// draw the lines in the middle of the screen
void drawHUD() {
    double bloomRadius = 0.1;
    double hitmarkWidth = 0.05;
    
    glColor3d(1,1,1);
    glPointSize(4);
    glLineWidth(1);

    // middle point
    glBegin(GL_POINTS);
    glVertex2d(0,0);
    glEnd();

    // lines around
    glBegin(GL_LINES);
    glVertex2d(bloomRadius,0);
    glVertex2d((bloomRadius+hitmarkWidth),0);
    glVertex2d(-bloomRadius,0);
    glVertex2d(-(bloomRadius+hitmarkWidth),0);
    glVertex2d(0,bloomRadius);
    glVertex2d(0,(bloomRadius+hitmarkWidth));
    glVertex2d(0,-bloomRadius);
    glVertex2d(0,-(bloomRadius+hitmarkWidth));
    glEnd();
}

// matrix transformations inspired by ex21, instead of ortho do perspective
void switchProjectionAndDrawWeapon(double fov, double asp) {
    //  Save transform attributes (Matrix Mode and Enabled Modes)
    glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
    //  Save projection matrix and set unit transform
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(fov, asp, Z_NEAR, Z_FAR);
    gluLookAt(0,0,0 , 0,0,-1 , 0,1,0);
    //  Save model view matrix and set to identity
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // draw the weapon alongside
    drawWeapon(weaponOffsetX, weaponOffsetY, weaponOffsetZ);

    //  Reset model view matrix
    glPopMatrix();
    //  Reset projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //  Pop transform attributes (Matrix Mode and Enabled Modes)
    glPopAttrib();
}

// similar to above, but ortho projection instead, also clearing depth buffer to avoid depth checking
void switchProjectionAndDrawHUD(double asp) {
    // do not do depth test or lighting on the HUD
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    //  Save transform attributes (Matrix Mode and Enabled Modes)
    glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
    //  Save projection matrix and set unit transform
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-asp,+asp,-1,1,-1,1);
    //  Save model view matrix and set to identity
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // clear z-buffer
    glClear(GL_DEPTH_BUFFER_BIT);

    // draw the HUD
    drawHUD();

    //  Reset model view matrix
    glPopMatrix();
    //  Reset projection matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    //  Pop transform attributes (Matrix Mode and Enabled Modes)
    glPopAttrib();

    // re-enable depth mask after done drawing HUD
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

// draw all enemies that are up
void drawEnemies() {
    for (int i = 0; i < N_ENEMIES; i++) {
        if (enemies[i].up) {
            drawEnemy(enemies[i]);
        }
    }
}

void drawEnemy(struct enemy e) {
    glPushMatrix();

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glTranslated(e.x, e.y, e.z);
    glRotated(90*e.xOrient, 0,1,0);
    // glScaled(ENEMY_SCALE, ENEMY_SCALE, ENEMY_SCALE);
    glColor3d(1,1,1);

    // Front and Back of Enemy
    glBindTexture(GL_TEXTURE_2D, weaponTexture[1]);
    glBegin(GL_QUADS);
    // Front Body
    glNormal3d(0,0,1);
    glTexCoord2d(0,0);                  glVertex3d(-enemyWidth/2, enemySupportHeight,             0.0);
    glTexCoord2d(1,0);                  glVertex3d(enemyWidth/2,  enemySupportHeight,             0.0);
    glTexCoord2d(1,textureHeadHeight);  glVertex3d(enemyWidth/2,  enemySupportHeight+enemyHeight, 0.0);
    glTexCoord2d(0,textureHeadHeight);  glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, 0.0);
    // Front Head
    glTexCoord2d(textureHeadStart,textureHeadHeight);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight,             0.0);
    glTexCoord2d(textureHeadEnd,textureHeadHeight);     glVertex3d(enemyHeadWidth/2,  enemySupportHeight+enemyHeight,             0.0);
    glTexCoord2d(textureHeadEnd,1);                     glVertex3d(enemyHeadWidth/2,  enemySupportHeight+enemyHeight+enemyHeadHeight,  0.0);
    glTexCoord2d(textureHeadStart,1);                   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight,  0.0);
    // Back Body
    glNormal3d(0,0,-1);
    glTexCoord2d(0,0);                  glVertex3d(-enemyWidth/2, enemySupportHeight,             -enemyThickness);
    glTexCoord2d(1,0);                  glVertex3d(enemyWidth/2,  enemySupportHeight,             -enemyThickness);
    glTexCoord2d(1,textureHeadHeight);  glVertex3d(enemyWidth/2,  enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(0,textureHeadHeight);  glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    // Back Head
    glTexCoord2d(textureHeadStart,textureHeadHeight);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight,             -enemyThickness);
    glTexCoord2d(textureHeadEnd,textureHeadHeight);     glVertex3d(enemyHeadWidth/2,  enemySupportHeight+enemyHeight,             -enemyThickness);
    glTexCoord2d(textureHeadEnd,1);                     glVertex3d(enemyHeadWidth/2,  enemySupportHeight+enemyHeight+enemyHeadHeight,  -enemyThickness);
    glTexCoord2d(textureHeadStart,1);                   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight,  -enemyThickness);
    glEnd();

    // Sides outlining enemy
    glBindTexture(GL_TEXTURE_2D, weaponTexture[2]);
    glBegin(GL_QUADS);
    // Left Body
    glNormal3d(-1,0,0);
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(-enemyWidth/2, enemySupportHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(-enemyWidth/2, enemySupportHeight, -enemyThickness);
    glTexCoord2d(0,1);                  glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, 0.0);
    // Left Head
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(0,1);                  glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, 0.0);
    // Right Body
    glNormal3d(1,0,0);
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(enemyWidth/2, enemySupportHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(enemyWidth/2, enemySupportHeight, -enemyThickness);
    glTexCoord2d(0,1);                  glVertex3d(enemyWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(enemyWidth/2, enemySupportHeight+enemyHeight, 0.0);
    // Right Head
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(0,1);                  glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, 0.0);
    // Top Body
    glNormal3d(0,1,0);
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(enemyWidth/2, enemySupportHeight+enemyHeight, 0.0);
    glTexCoord2d(0,1);                  glVertex3d(enemyWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(-enemyWidth/2, enemySupportHeight+enemyHeight, -enemyThickness);
    // Top Head
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, 0.0);
    glTexCoord2d(0,1);                  glVertex3d(enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(-enemyHeadWidth/2, enemySupportHeight+enemyHeight+enemyHeadHeight, -enemyThickness);
    // Bottom Body
    glNormal3d(0,-1,0);
    glTexCoord2d(woodThicknessTex,0);   glVertex3d(-enemyWidth/2, enemySupportHeight, 0.0);
    glTexCoord2d(0,0);                  glVertex3d(enemyWidth/2, enemySupportHeight, 0.0);
    glTexCoord2d(0,1);                  glVertex3d(enemyWidth/2, enemySupportHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,1);   glVertex3d(-enemyWidth/2, enemySupportHeight, -enemyThickness);
    glEnd();

    // Post holding up the enemy
    glBegin(GL_QUADS);
    // Left
    glNormal3d(-1,0,0);
    glTexCoord2d(0,0);    glVertex3d(-enemyThickness, 0.0, 0.0);
    glTexCoord2d(0,1);    glVertex3d(-enemyThickness, enemySupportHeight, 0.0);
    glTexCoord2d(woodThicknessTex,1); glVertex3d(-enemyThickness, enemySupportHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,0); glVertex3d(-enemyThickness, 0.0, -enemyThickness);
    // Right
    glNormal3d(1,0,0);
    glTexCoord2d(0,0);    glVertex3d(enemyThickness, 0.0, 0.0);
    glTexCoord2d(0,1);    glVertex3d(enemyThickness, enemySupportHeight, 0.0);
    glTexCoord2d(woodThicknessTex,1); glVertex3d(enemyThickness, enemySupportHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex,0); glVertex3d(enemyThickness, 0.0, -enemyThickness);
    // Front
    glNormal3d(0,0,1);
    glTexCoord2d(0,0);   glVertex3d(-enemyThickness, 0.0, 0.0);
    glTexCoord2d(0,1);   glVertex3d(-enemyThickness, enemySupportHeight, 0.0);
    glTexCoord2d(woodThicknessTex*2,1); glVertex3d(enemyThickness, enemySupportHeight, 0.0);
    glTexCoord2d(woodThicknessTex*2,0); glVertex3d(enemyThickness, 0.0, 0.0);
    // Back
    glNormal3d(0,0,-1);
    glTexCoord2d(0,0);   glVertex3d(-enemyThickness, 0.0, -enemyThickness);
    glTexCoord2d(0,1);   glVertex3d(-enemyThickness, enemySupportHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex*2,1); glVertex3d(enemyThickness, enemySupportHeight, -enemyThickness);
    glTexCoord2d(woodThicknessTex*2,0); glVertex3d(enemyThickness, 0.0, -enemyThickness);
    // Bottom
    glNormal3d(0,-1,0);
    glVertex3d(-enemyThickness, 0.0, 0.0);
    glVertex3d(-enemyThickness, 0.0, -enemyThickness);
    glVertex3d(enemyThickness, 0.0, -enemyThickness);
    glVertex3d(enemyThickness, 0.0, 0.0);
    glEnd();

    glPopMatrix();
}

// Helper to translate a point in local camera coords to a point relative to the absolute coords of the world
void translatePointFromLocalCoord(double result[3], double local[3], double fpPosVec[3], double fpVerTh, double fpLatTh, double h) {
    // Offset lat angle by 90
    double latTh = -(fpLatTh+90.0);
    double tmp[3];

    // init result
    result[0] = local[0]; result[1] = local[1]; result[2] = local[2];
    tmp[0] = result[0]; tmp[1] = result[1]; tmp[2] = result[2];
    
    // rotate about x
    result[1] = tmp[1]*Cos(fpVerTh) - tmp[2]*Sin(fpVerTh);
    result[2] = tmp[1]*Sin(fpVerTh) + tmp[2]*Cos(fpVerTh);
    tmp[0] = result[0]; tmp[1] = result[1]; tmp[2] = result[2];

    // rotate about y
    result[0] = tmp[0]*Cos(latTh) + tmp[2]*Sin(latTh);
    result[2] = -tmp[0]*Sin(latTh) + tmp[2]*Cos(latTh);

    // translate
    result[0] += fpPosVec[0];
    result[1] += h;
    result[2] += fpPosVec[2];
}

// callbacks for shooting
void weaponLeftMouseDown() {
    leftMouseDown = 1;
}
void weaponLeftMouseUp() {
    leftMouseDown = 0;
}

void shootBullet(double fpPosVec[3], double h) {
    // Check that there is a free bullet, if not attempt to find one, if that fails do not shoot
    if (noFreeBullet && updateFreeBulletIndex()) {
        return;
    }

    // Retrieve current look angles
    double verTh = getFpVerTh();
    double latTh = getFpLatTh();

    // Get current barrel position in world coords
    double barrelPos[3];
    translatePointFromLocalCoord(barrelPos, bulletStart, fpPosVec, verTh, latTh, h);

    // Apply bloom
    double bulletDirLocalWithBloom[3];
    bulletDirLocalWithBloom[0] = bulletDirLocal[0]; bulletDirLocalWithBloom[1] = bulletDirLocal[1]; bulletDirLocalWithBloom[2] = bulletDirLocal[2];
    bulletDirLocalWithBloom[0] += (((double)getRandomInRange(0, WEAPON_BLOOM) - (((double)WEAPON_BLOOM)/2.0)) / 1000.0);
    bulletDirLocalWithBloom[1] += (((double)getRandomInRange(0, WEAPON_BLOOM) - (((double)WEAPON_BLOOM)/2.0)) / 1000.0);

    // Get bullet direction by translating to world coords and subtracting barrel position
    double bulletDir[3];
    translatePointFromLocalCoord(bulletDir, bulletDirLocalWithBloom, fpPosVec, verTh, latTh, h);
    bulletDir[0] -= barrelPos[0]; bulletDir[1] -= barrelPos[1]; bulletDir[2] -= barrelPos[2];

    // Create bullet struct at current free index and set its active to 1
    bullets[currentFreeBullet].x = barrelPos[0];
    bullets[currentFreeBullet].y = barrelPos[1];
    bullets[currentFreeBullet].z = barrelPos[2];
    bullets[currentFreeBullet].dx = -bulletDir[0];
    bullets[currentFreeBullet].dy = -bulletDir[1];
    bullets[currentFreeBullet].dz = -bulletDir[2];
    bullets[currentFreeBullet].active = 1;

    // Update free index
    updateFreeBulletIndex();
}

// Update new free index by looping through until finding a free index (0 for success, 1 for no free index)
int updateFreeBulletIndex() {
    noFreeBullet = 1;
    for (int i = 0; i < MAX_BULLETS; i++) {
        currentFreeBullet++;
        if (currentFreeBullet >= MAX_BULLETS) {
            currentFreeBullet = 0;
        }
        if (bullets[currentFreeBullet].active == 0) {
            noFreeBullet = 0;
            return 0;
        }
    }
    return 1;
}

// helper to check status of enemies and bring them back up after a certain period
void checkAllEnemies(double t) {
    for (int i = 0; i < N_ENEMIES; i++) {
        updateEnemyStatus(i, t);
    }
}

void updateEnemyStatus(int i, double t) {
    if (enemies[i].health <= 0.0) {
        // below 0 and up, go down and set time down
        if (enemies[i].up) {
            enemies[i].up = 0;
            enemies[i].timeOfDown = t;
        }
        // below 0 and down, check if time down is more than 10 seconds ago and come up if so
        else if (t - enemies[i].timeOfDown > ENEMY_TIME_DOWN) {
            enemies[i].up = 1;
            enemies[i].health = 100;
        }
    }
}

// HITBOXES
double getEnemyHeadTop(int i) {
    return enemies[i].y + enemySupportHeight + enemyHeight + enemyHeadHeight;
}
double getEnemyHeadLeft(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].x - (enemyHeadWidth/2);
    } else {
        return enemies[i].x - enemyThickness;
    }
}
double getEnemyHeadRight(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].x + (enemyHeadWidth/2);
    } else {
        return enemies[i].x;
    }
}
double getEnemyHeadFront(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].z;
    } else {
        return enemies[i].z + (enemyHeadWidth/2);
    }
}
double getEnemyHeadBack(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].z - enemyThickness;
    } else {
        return enemies[i].z - (enemyHeadWidth/2);
    }
}
double getEnemyBodyTop(int i) {
    return enemies[i].y + enemySupportHeight + enemyHeight;
}
double getEnemyBodyBottom(int i) {
    return enemies[i].y + enemySupportHeight;
}
double getEnemyBodyLeft(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].x - (enemyWidth/2);
    } else {
        return enemies[i].x - enemyThickness;
    }
}
double getEnemyBodyRight(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].x + (enemyWidth/2);
    } else {
        return enemies[i].x;
    }
}
double getEnemyBodyFront(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].z;
    } else {
        return enemies[i].z + (enemyWidth/2);
    }
}
double getEnemyBodyBack(int i) {
    if (enemies[i].xOrient == 0) {
        return enemies[i].z - enemyThickness;
    } else {
        return enemies[i].z - (enemyWidth/2);
    }
}

int checkBullet(double x, double y, double z, double t, int enemyIdx) {
    // First check head hitboxes
    if (y-BULLET_SCALE < getEnemyHeadTop(enemyIdx) &&
        y+BULLET_SCALE > getEnemyBodyTop(enemyIdx) &&
        z-BULLET_SCALE < getEnemyHeadFront(enemyIdx) &&
        z+BULLET_SCALE > getEnemyHeadBack(enemyIdx) &&
        x-BULLET_SCALE < getEnemyHeadRight(enemyIdx) &&
        x+BULLET_SCALE > getEnemyHeadLeft(enemyIdx))
    {
        enemies[enemyIdx].health -= BULLET_DAMAGE*HEADSHOT_MULTIPLIER;
        updateEnemyStatus(enemyIdx, t);
        return 1;
    }
    // Then, check body hitboxes
    else if (
        y-BULLET_SCALE < getEnemyBodyTop(enemyIdx) &&
        y+BULLET_SCALE > getEnemyBodyBottom(enemyIdx) &&
        z-BULLET_SCALE < getEnemyBodyFront(enemyIdx) &&
        z+BULLET_SCALE > getEnemyBodyBack(enemyIdx) &&
        x-BULLET_SCALE < getEnemyBodyRight(enemyIdx) &&
        x+BULLET_SCALE > getEnemyBodyLeft(enemyIdx))
    {
        enemies[enemyIdx].health -= BULLET_DAMAGE;
        updateEnemyStatus(enemyIdx, t);
        return 1;
    }
    return 0;
}

void checkHitboxes(double t) {
    // loop over all active bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {

            // loop over all enemies that are up
            for (int j = 0; j < N_ENEMIES; j++) {
                if (enemies[j].up) {

                    // check against real bullet position
                    int hit = checkBullet(bullets[i].x, bullets[i].y, bullets[i].z, t, j);
                    if (hit) {
                        bullets[i].active = 0;
                    }
                    // if not hit, check against recorded bullet paths for more precision
                    else {
                        for (int x = 0; x < BULLET_REPEAT; x++) {
                            hit = checkBullet(bulletLocations[i][x][0], bulletLocations[i][x][1], bulletLocations[i][x][2], t, j);
                            if (hit) {
                                bullets[i].active = 0;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void updateBullets(double dt) {
    // update bullet positions
    double dtx, dty, dtz, repeatFactor;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // get bullet movements
            dtx = bullets[i].dx*dt;
            dty = bullets[i].dy*dt;
            dtz = bullets[i].dz*dt;

            // record the path of the bullet, otherwise bullet can "phase" through targets
            for (int j = 0; j < BULLET_REPEAT; j++) {
                repeatFactor = ((double)j+1.0)/((double)BULLET_REPEAT+1.0);
                bulletLocations[i][j][0] = bullets[i].x + (dtx*repeatFactor);
                bulletLocations[i][j][1] = bullets[i].y + (dty*repeatFactor);
                bulletLocations[i][j][2] = bullets[i].z + (dtz*repeatFactor);
            }

            // update real bullet position
            bullets[i].x += dtx;
            bullets[i].y += dty;
            bullets[i].z += dtz;
        }
    }

    // check if bullets out of bounds and deactivate if so
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            if (bullets[i].x < -WORLD_RADIUS ||
                bullets[i].x > WORLD_RADIUS ||
                bullets[i].z < -WORLD_RADIUS ||
                bullets[i].z > WORLD_RADIUS ||
                bullets[i].y < getElevation(bullets[i].x, bullets[i].z) ||
                bullets[i].y > WORLD_RADIUS) {
                    bullets[i].active = 0;
                }
        }
    }
}

void weaponInit(unsigned int *weaponTexturePtr) {
    weaponTexture = weaponTexturePtr;
    ReadEnemyDEM("dem/enemy.dem", enemies);
}
