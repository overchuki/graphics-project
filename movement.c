/*
 *   This is a c file that contains all player movement related files
 */

#include "CSCIx229.h"
#include "igor.h"

double fpLatTh = 90;    // 0 is along +x-axis
double fpVerTh = 0;     // 0 is in the x-z plane

// track current player movement
int moveForward = 0;
int moveBackward = 0;
int moveLeft = 0;
int moveRight = 0;
int moveSprint = 0;

// track jump values
double jumpHeight = 0;
double jumpTime = 0;
int isJumping = 0;
int spaceDown = 0;

// getters for camera angles
double getFpLatTh() {
    return fpLatTh;
}
double getFpVerTh() {
    return fpVerTh;
}

// key callbacks to set their respective movements
void forwardKey(int down, int *fov) {
    moveForward = down;
    if (!down) {
        *fov = FOV_NORMAL;
    } else if (moveSprint) {
        *fov = FOV_SPRINT;
    }
}
void backKey(int down) {
    moveBackward = down;
}
void leftKey(int down) {
    moveLeft = down;
}
void rightKey(int down) {
    moveRight = down;
}
void sprintKey(int down, int *fov) {
    moveSprint = down;
    // narrower fov when sprinting for additional effect
    if (!down) {
        *fov = FOV_NORMAL;
    } else if (moveForward) {
        *fov = FOV_SPRINT;
    }
}
void jumpKey(int down) {
    spaceDown = down;
    if (down) {
        // Jump if not jumping and space is clicked
        if (!isJumping) {
            isJumping = 1;
            jumpTime = glutGet(GLUT_ELAPSED_TIME);
        }
    }
}

void movementIdle(double t, double dt, double fpPosVec[3], double fpGroundDir[2], double mapBounds[4], double* jumpHeightPtr, double* elevationPtr) {
    // flag to check for movement
    int moved = 0;

    // calculate 
    int fbMove = moveForward - moveBackward;
    int lrMove = moveRight - moveLeft;

    // forward/backward
    if (fbMove != 0) {
        moved = 1;
        double sprintFactor = 1.0;
        if (moveSprint && fbMove > 0) { sprintFactor = SPRINT_FACTOR; }
        if (lrMove != 0) { sprintFactor *= LATERAL_FACTOR; }

        fpPosVec[0] += (fpGroundDir[0] * dt * FB_MOVE_SPEED * fbMove * sprintFactor);
        fpPosVec[2] += (fpGroundDir[1] * dt * FB_MOVE_SPEED * fbMove * sprintFactor);
    }
    // left/right
    if (lrMove != 0) {
        moved = 1;
        fpPosVec[0] -= (fpGroundDir[1] * dt * LR_MOVE_SPEED * lrMove);
        fpPosVec[2] += (fpGroundDir[0] * dt * LR_MOVE_SPEED * lrMove);
    }

    // keep character from moving out of bounds
    if (moved) {
        if (fpPosVec[0] < mapBounds[0]) { fpPosVec[0] = mapBounds[0]; }
        if (fpPosVec[0] > mapBounds[1]) { fpPosVec[0] = mapBounds[1]; }
        if (fpPosVec[2] < mapBounds[2]) { fpPosVec[2] = mapBounds[2]; }
        if (fpPosVec[2] > mapBounds[3]) { fpPosVec[2] = mapBounds[3]; }
    }

    // SET JUMP HEIGHT if currently jumping
    if (isJumping) {
        setJumpHeight(t*1000 - jumpTime);
    }
    *jumpHeightPtr = jumpHeight;

    // Set character height based on position on elevation map
    *elevationPtr = getElevation(fpPosVec[0], fpPosVec[2]);
}

// Movement mouse callback
void mouseMoved(int x, int y, int centerX, int centerY) {
    // change camera angles based on movement
    double lateralDiff = (x - centerX) * X_MOUSE_SENSITIVITY;
    double verticalDiff = (centerY - y) * Y_MOUSE_SENSITIVITY;

    // cap lateral to 360
    fpLatTh = fmod(fpLatTh + lateralDiff, 360.0);
    fpVerTh = fpVerTh + verticalDiff;
    
    // cap vertical angle just before vertical to avoid division by 0
    if (fpVerTh >= MAX_VERTICAL_ANGLE) { fpVerTh = MAX_VERTICAL_ANGLE; }
    if (fpVerTh <= -MAX_VERTICAL_ANGLE) { fpVerTh = -MAX_VERTICAL_ANGLE; }
}

// convenience function to get magnitude of a vector
double magnitude(double vec[], int n) {
    double total = 0;
    for (int i = 0; i < n; i++) {
        total += pow(vec[i], 2);
    }
    return sqrt(total);
}

// normalize a vector with an additional scalar
void normalize(double vec[], int n, double scalar) {
    double mag = magnitude(vec, n) / scalar;
    for (int i = 0; i < n; i++) {
        vec[i] /= mag;
    }
}

// calculate jumpHeight
void setJumpHeight(double dt) {
    if (dt > JUMP_DURATION) {
        // reset all jump values if jump is complete
        isJumping = 0;
        jumpHeight = 0;
        jumpTime = 0;
        if (spaceDown) {
            isJumping = 1;
            jumpTime = glutGet(GLUT_ELAPSED_TIME);
        }
    } else {
        // y = ax(x-t)      : parabola function used to calculate jump position based on time elapsed
        jumpHeight = JUMP_COEFFICIENT * dt * (dt - JUMP_DURATION);
    }
}

void calcFPAngle(double fpPosVec[3], double fpGroundDir[2], double fpViewVector[3], double fpUp[3]) {
    // calculate ground direction based on the lateral angle
    fpGroundDir[0] = Cos(fpLatTh);
    fpGroundDir[1] = Sin(fpLatTh);

    // calculate view vector and up vector based on ground direction and vertical angle
    // looking straight ahead
    if (fpVerTh == 0) {
        fpViewVector[0] = fpGroundDir[0];
        fpViewVector[1] = 0;
        fpViewVector[2] = fpGroundDir[1];
        fpUp[0] = 0;
        fpUp[1] = 1;
        fpUp[2] = 0;
    }
    // looking straight up/down
    else if (fpVerTh >= 90.0 || fpVerTh <= -90.0) {
        // factor defines straight up(-1)/down(1)
        int factor = -1;
        if (fpVerTh < 0.0) { factor = 1; }

        fpViewVector[0] = 0;
        fpViewVector[1] = -factor;
        fpViewVector[2] = 0;

        fpUp[0] = factor*fpGroundDir[0];
        fpUp[1] = 0;
        fpUp[2] = factor*fpGroundDir[1];
    }
    // vertical between 0 and 90
    else {
        // normalize ground vector to length of cosine of vertical angle to have correct view angle
        double tmpGround[2] = {fpGroundDir[0], fpGroundDir[1]};
        double groundMag = Cos(fpVerTh);
        normalize(tmpGround, 2, groundMag);

        // calculate y value based on the normalized ground vector
        fpViewVector[0] = tmpGround[0];
        fpViewVector[1] = Sin(fpVerTh);
        fpViewVector[2] = tmpGround[1];

        // can avoid additional calculation for up vector because it is not going to parallel to view vector here
        fpUp[0] = 0;
        fpUp[1] = 1;
        fpUp[2] = 0;
    }
}
