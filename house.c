/*
 *   This is a c file that contains all house related files
 */

#include "CSCIx229.h"
#include "igor.h"

// House globals
/*
    Textures:
    0: brick
    1: clay
    2: wood
    3: roofRed
    4: roofGrey
    5: window
    6: door
*/
unsigned int *houseTexture;
struct house houses[N_HOUSES];
struct houseTheme houseThemes[N_HOUSE_THEMES] = {
    {0, 3, 6, 5},
    {1, 3, 6, 5},
    {2, 4, 6, 5}
};

// Draw a house, only able to rotate it around y-axis since it shouldn't face any other way
void drawHouse(double x, double y, double z, double yth, double scale, struct houseTheme theme) {
    // House dimensions
    double width = 2.0;
    double length = 2.0;
    double height = 1.0;

    // Roof dimensions
    double roofHeightRatio = 0.5;
    double roofSideOverhangRatio = 0.3;
    double roofFrontOverhangRatio = 0.2;
    double roofHeight = width/2 * roofHeightRatio;
    double roofSideOverhang = width/2 * roofSideOverhangRatio;
    double roofSideOverhangVer = roofSideOverhang * roofHeightRatio;
    double roofFrontOverhang = width/2 * roofFrontOverhangRatio;
    double roofThickness = 0.03;

    // Door Dimensions
    double doorWidthRatio = 0.4;
    double doorHeightRatio = 0.7;
    double doorWidth = width/2 * doorWidthRatio;
    double doorHeight = height * doorHeightRatio;
    double doorOffset = 0.2;

    // Window Dimensions
    double windowWidth = 0.4;
    double windowHeight = 0.5;
    double windowBaseHeight = 0.25;
    double windowOffsets[4] = {-0.6, -0.6, 0.2, 0.2};

    // Texture repeats
    double wallRepeat = 2.0;
    double windowRepeat = 1.0;
    double doorRepeat = 1.0;
    double roofRepeat = 2.0;

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(yth, 0, 1.0, 0);
    glScaled(scale, scale, scale);
    glColor3d(1,1,1);

    // WALL ROOF EDGES
    glBindTexture(GL_TEXTURE_2D, houseTexture[theme.wall]);
    glBegin(GL_TRIANGLES);
    // Back edge
    glNormal3d(0,0,-1);
    glTexCoord2d(0,0);                                      glVertex3d(-width/2,    height,             -length/2);
    glTexCoord2d(wallRepeat/2,wallRepeat*roofHeightRatio);  glVertex3d(0,           height+roofHeight,  -length/2);
    glTexCoord2d(wallRepeat,0);                             glVertex3d(width/2,     height,             -length/2);
    // Front Edge
    glNormal3d(0,0,1);
    glTexCoord2d(0,0);                                      glVertex3d(-width/2,    height,             length/2);
    glTexCoord2d(wallRepeat/2,wallRepeat*roofHeightRatio);  glVertex3d(0,           height+roofHeight,  length/2);
    glTexCoord2d(wallRepeat,0);                             glVertex3d(width/2,     height,             length/2);
    glEnd();

    // TOP ROOF
    glBindTexture(GL_TEXTURE_2D, houseTexture[theme.roof]);
    glBegin(GL_QUADS);
    // Left Top
    glNormal3d(-1*roofHeightRatio,1,0);
    glTexCoord2d(0,0);                      glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glTexCoord2d(roofRepeat,0);             glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    glTexCoord2d(roofRepeat,roofRepeat);    glVertex3d(0,                               height+roofHeight+roofThickness,            length/2 + roofFrontOverhang);
    glTexCoord2d(0,roofRepeat);             glVertex3d(0,                               height+roofHeight+roofThickness,            -(length/2 + roofFrontOverhang));
    // Right Top
    glNormal3d(1*roofHeightRatio,1,0);
    glTexCoord2d(roofRepeat,roofRepeat);    glVertex3d(0,                               height+roofHeight+roofThickness,            length/2 + roofFrontOverhang);
    glTexCoord2d(0,roofRepeat);             glVertex3d(0,                               height+roofHeight+roofThickness,            -(length/2 + roofFrontOverhang));
    glTexCoord2d(0,0);                      glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glTexCoord2d(roofRepeat,0);             glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    glEnd();

    // BOTTOM ROOF
    glBindTexture(GL_TEXTURE_2D, houseTexture[WOOD_TEX]);
    glBegin(GL_QUADS);
    // Left Bottom
    glNormal3d(1*roofHeightRatio,-1,0);
    glTexCoord2d(0,0); glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    glTexCoord2d(1,0); glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glTexCoord2d(1,1); glVertex3d(0,                               height+roofHeight,                          length/2 + roofFrontOverhang);
    glTexCoord2d(0,1); glVertex3d(0,                               height+roofHeight,                          -(length/2 + roofFrontOverhang));
    // Right Bottom
    glNormal3d(-1*roofHeightRatio,-1,0);
    glTexCoord2d(1,1); glVertex3d(0,                               height+roofHeight,                          length/2 + roofFrontOverhang);
    glTexCoord2d(0,1); glVertex3d(0,                               height+roofHeight,                          -(length/2 + roofFrontOverhang));
    glTexCoord2d(0,0); glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    glTexCoord2d(1,0); glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glEnd();

    // ROOF EDGES
    glBegin(GL_QUADS);
    // Left Front Edge
    glNormal3d(0,0,1);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    glVertex3d(0,                               height+roofHeight+roofThickness,            length/2 + roofFrontOverhang);
    glVertex3d(0,                               height+roofHeight,                          length/2 + roofFrontOverhang);
    // Right Front Edge
    glVertex3d(0,                               height+roofHeight+roofThickness,            length/2 + roofFrontOverhang);
    glVertex3d(0,                               height+roofHeight,                          length/2 + roofFrontOverhang);
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    // Left Edge
    glNormal3d(-1,0,0);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    // Right Edge
    glNormal3d(1,0,0);
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 length/2 + roofFrontOverhang);
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   length/2 + roofFrontOverhang);
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    // Left Back Edge
    glNormal3d(0,0,-1);
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    glVertex3d(-(width/2 + roofSideOverhang),   height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glVertex3d(0,                               height+roofHeight+roofThickness,            -(length/2 + roofFrontOverhang));
    glVertex3d(0,                               height+roofHeight,                          -(length/2 + roofFrontOverhang));
    // Right Back Edge
    glVertex3d(0,                               height+roofHeight+roofThickness,            -(length/2 + roofFrontOverhang));
    glVertex3d(0,                               height+roofHeight,                          -(length/2 + roofFrontOverhang));
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer,                 -(length/2 + roofFrontOverhang));
    glVertex3d(width/2 + roofSideOverhang,      height-roofSideOverhangVer+roofThickness,   -(length/2 + roofFrontOverhang));
    glEnd();

    // enable polygon offset to draw windows/doors/road
    glEnable(GL_POLYGON_OFFSET_FILL);

    // offset of 2 for walls and windows/door
    glPolygonOffset(2,2);

    // WALLS
    glBindTexture(GL_TEXTURE_2D, houseTexture[theme.wall]);
    glBegin(GL_QUADS);
    // Front
    glNormal3d(0,0,1);
    glTexCoord2d(0,0);                      glVertex3d(-width/2,    0,      length/2);
    glTexCoord2d(0,wallRepeat);             glVertex3d(-width/2,    height, length/2);
    glTexCoord2d(wallRepeat,wallRepeat);    glVertex3d(width/2,     height, length/2);
    glTexCoord2d(wallRepeat,0);             glVertex3d(width/2,     0,      length/2);
    // Right
    glNormal3d(1,0,0);
    glTexCoord2d(wallRepeat,0);             glVertex3d(width/2,     0,      length/2);
    glTexCoord2d(wallRepeat,wallRepeat);    glVertex3d(width/2,     height, length/2);
    glTexCoord2d(0,wallRepeat);             glVertex3d(width/2,     height, -length/2);
    glTexCoord2d(0,0);                      glVertex3d(width/2,     0,      -length/2);
    // Back
    glNormal3d(0,0,-1);
    glTexCoord2d(0,0);                      glVertex3d(-width/2,    0,      -length/2);
    glTexCoord2d(0,wallRepeat);             glVertex3d(-width/2,    height, -length/2);
    glTexCoord2d(wallRepeat,wallRepeat);    glVertex3d(width/2,     height, -length/2);
    glTexCoord2d(wallRepeat,0);             glVertex3d(width/2,     0,      -length/2);
    // Left
    glNormal3d(-1,0,0);
    glTexCoord2d(wallRepeat,0);             glVertex3d(-width/2,    0,      length/2);
    glTexCoord2d(wallRepeat,wallRepeat);    glVertex3d(-width/2,    height, length/2);
    glTexCoord2d(0,wallRepeat);             glVertex3d(-width/2,    height, -length/2);
    glTexCoord2d(0,0);                      glVertex3d(-width/2,    0,      -length/2);
    glEnd();

    // offset of 1 for door, windows
    glPolygonOffset(1,1);
    
    // DOOR
    glBindTexture(GL_TEXTURE_2D, houseTexture[theme.door]);
    glBegin(GL_QUADS);
    glNormal3d(0,0,1);
    glTexCoord2d(0,0);                      glVertex3d(doorOffset,              0,          length/2);
    glTexCoord2d(doorRepeat,0);             glVertex3d(doorOffset+doorWidth,    0,          length/2);
    glTexCoord2d(doorRepeat,doorRepeat);    glVertex3d(doorOffset+doorWidth,    doorHeight, length/2);
    glTexCoord2d(0,doorRepeat);             glVertex3d(doorOffset,              doorHeight, length/2);
    glEnd();

    // WINDOWS
    glBindTexture(GL_TEXTURE_2D, houseTexture[theme.window]);
    glBegin(GL_QUADS);
    // Front
    glNormal3d(0,0,1);
    glTexCoord2d(windowRepeat,0);               glVertex3d(windowOffsets[0]+windowWidth,    windowBaseHeight,               length/2);
    glTexCoord2d(0,0);                          glVertex3d(windowOffsets[0],                windowBaseHeight,               length/2);
    glTexCoord2d(0,windowRepeat);               glVertex3d(windowOffsets[0],                windowBaseHeight+windowHeight,  length/2);
    glTexCoord2d(windowRepeat,windowRepeat);    glVertex3d(windowOffsets[0]+windowWidth,    windowBaseHeight+windowHeight,  length/2);
    // Left 1
    glNormal3d(-1,0,0);
    glTexCoord2d(0,0);                          glVertex3d(-width/2, windowBaseHeight,              windowOffsets[1]);
    glTexCoord2d(windowRepeat,0);               glVertex3d(-width/2, windowBaseHeight,              windowOffsets[1]+windowWidth);
    glTexCoord2d(windowRepeat,windowRepeat);    glVertex3d(-width/2, windowBaseHeight+windowHeight, windowOffsets[1]+windowWidth);
    glTexCoord2d(0,windowRepeat);               glVertex3d(-width/2, windowBaseHeight+windowHeight, windowOffsets[1]);
    // Left 2
    glTexCoord2d(0,0);                          glVertex3d(-width/2, windowBaseHeight,              windowOffsets[2]);
    glTexCoord2d(windowRepeat,0);               glVertex3d(-width/2, windowBaseHeight,              windowOffsets[2]+windowWidth);
    glTexCoord2d(windowRepeat,windowRepeat);    glVertex3d(-width/2, windowBaseHeight+windowHeight, windowOffsets[2]+windowWidth);
    glTexCoord2d(0,windowRepeat);               glVertex3d(-width/2, windowBaseHeight+windowHeight, windowOffsets[2]);
    // Right
    glNormal3d(1,0,0);
    glTexCoord2d(0,0);                          glVertex3d(width/2,  windowBaseHeight,              windowOffsets[3]);
    glTexCoord2d(windowRepeat,0);               glVertex3d(width/2,  windowBaseHeight,              windowOffsets[3]+windowWidth);
    glTexCoord2d(windowRepeat,windowRepeat);    glVertex3d(width/2,  windowBaseHeight+windowHeight, windowOffsets[3]+windowWidth);
    glTexCoord2d(0,windowRepeat);               glVertex3d(width/2,  windowBaseHeight+windowHeight, windowOffsets[3]);
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);

    glPopMatrix();
}

// draw all house structs
void drawHouses() {
    for (int i = 0; i < N_HOUSES; i++) {
        drawHouse(houses[i].x, houses[i].y, houses[i].z, houses[i].rotateAngleY, houses[i].scale, houseThemes[houses[i].theme]);
    }
}

void houseInit(unsigned int *houseTexturePtr) {
    houseTexture = houseTexturePtr;
    ReadHouseDEM("dem/house.dem", houses);
}
