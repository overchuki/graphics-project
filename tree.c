/*
 *   This is a c file that contains all tree related files
 */

#include "CSCIx229.h"
#include "igor.h"

// Tree globals
/*
    Textures:
    0: bark
    1: leaves
*/
unsigned int *treeTexture;
struct tree trees[N_TREES];
struct treeTheme treeTheme = {0, 1};

// draw all tree structs
void drawTrees() {
    for (int i = 0; i < N_TREES; i++) {
        drawTree(trees[i].x, trees[i].y, trees[i].z, trees[i].rotate, trees[i].scale);
    }
}

void drawTree(double x, double y, double z, double rotate, double scale) {
    double trunkStep = 30;
    double trunkWidth = 0.1;
    double trunkHeight = 1.8;

    double leafStep = 60;
    double leafNum = 7;
    double leafTrunkOffset = 0.2;
    double leafConeTopMultiplier = 0.16;
    double leafWidthMultiplier = 0.06;
    double leafHeightMultiplier = 0.05;

    // Set material
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(rotate, 0, 1.0, 0);
    glScaled(scale, scale, scale);
    glColor3d(1,1,1);

    // TRUNK
    glBindTexture(GL_TEXTURE_2D, treeTexture[treeTheme.bark]);
    double rx, rz, ra = 0;
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += trunkStep) {
        rx = Cos(th)*trunkWidth;
        rz = Sin(th)*trunkWidth;
        ra = th / 360.0;
        glNormal3d(rx, 0, rz);
        glTexCoord2d(ra, 0); glVertex3d(rx, 0,           rz);
        glTexCoord2d(ra, 8); glVertex3d(rx, trunkHeight, rz);
    }
    glEnd();

    // LOOP OVER CONES going down
    double trx, trz, lrx, lrz, rrx, rrz;
    glBindTexture(GL_TEXTURE_2D, treeTexture[treeTheme.leaves]);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < leafNum; i++) {
        double coneTop = trunkHeight + leafTrunkOffset - (i * leafConeTopMultiplier);
        double coneBottom = trunkHeight + leafTrunkOffset - ((i+1) * leafConeTopMultiplier);
        double coneHeight = 0.4 + (i * leafHeightMultiplier);
        double coneWidth = 0.3 + (i * leafWidthMultiplier);
        double coneHWRatio = coneHeight / coneWidth;

        for (int th = 0; th < 360; th += leafStep) {
            // normal calculations
            trx = Cos(th+(leafStep/2))*coneWidth;
            trz = Sin(th+(leafStep/2))*coneWidth;
            lrx = Cos(th)*coneWidth;
            lrz = Sin(th)*coneWidth;
            rrx = Cos(th+leafStep)*coneWidth;
            rrz = Sin(th+leafStep)*coneWidth;

            // draw top cone
            glNormal3d(trx*coneHWRatio, coneWidth, trz*coneHWRatio);
            glTexCoord2d(1, 2); glVertex3d(0, coneTop, 0);
            glNormal3d(lrx*coneHWRatio, coneWidth, lrz*coneHWRatio);
            glTexCoord2d(0, 0); glVertex3d(lrx, coneTop-coneHeight, lrz);
            glNormal3d(rrx*coneHWRatio, coneWidth, rrz*coneHWRatio);
            glTexCoord2d(2, 0); glVertex3d(rrx, coneTop-coneHeight, rrz);

            // draw bottom cone
            glNormal3d(trx*coneHWRatio, coneWidth, trz*coneHWRatio);
            glTexCoord2d(0.5, 1); glVertex3d(0, coneBottom, 0);
            glNormal3d(lrx*coneHWRatio, coneWidth, lrz*coneHWRatio);
            glTexCoord2d(0, 0); glVertex3d(lrx, coneTop-coneHeight, lrz);
            glNormal3d(rrx*coneHWRatio, coneWidth, rrz*coneHWRatio);
            glTexCoord2d(1, 0); glVertex3d(rrx, coneTop-coneHeight, rrz);
        }
    }
    glEnd();

    glPopMatrix();
}

void treeInit(unsigned int *treeTexturePtr) {
    treeTexture = treeTexturePtr;
    ReadTreeDEM("dem/tree.dem", trees);
}
