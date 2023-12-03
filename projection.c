//  CSCIx229 library
//  Willem A. (Vlakkies) Schreuder
//  NOTE: I (Igor Overchuk) have modified this file to more accurately reflect the needs of my project
#include "CSCIx229.h"

//
//  Set projection
//
void Project(double fov, double asp, double dim, int mode)
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);

    //  Undo previous transformations
    glLoadIdentity();
    
    // Orthogonal projection
    if (mode == 0) {
        glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    }
    // Overview perspective transformation
    else if (mode == 1) {
        gluPerspective(fov, asp, Z_NEAR, Z_FAR);
    }
    // First person perspective transformation
    else if (mode == 2) {
        gluPerspective(fov, asp, Z_NEAR, Z_FAR);
    }
    
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    
    //  Undo previous transformations
    glLoadIdentity();
}
