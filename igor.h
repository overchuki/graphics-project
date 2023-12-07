/*
*   This is my own archive full of elements / objects and the functions I've outlined to be implemented
*/
#ifndef igor
#define igor

#include <time.h>

// WINDOW GLOBALS
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 500
#define CAMERA_HEIGHT 0.07
#define FOV_NORMAL 55
#define FOV_SPRINT 53
#define DEFAULT_DIM 2.0
#define N_DISPLAY_LISTS 5

// HOUSE GLOBALS
#define N_HOUSES 42
#define N_HOUSE_THEMES 3
#define DEFAULT_HOUSE_SCALE 0.15
#define N_HOUSE_TEXTURES 7
#define WOOD_TEX 2

// HELICOPTER GLOBALS
#define N_HELICOPTERS 9
#define N_HELICOPTER_THEMES 2
#define DEFAULT_HELICOPTER_SCALE 0.3
#define DEFAULT_ROTOR_SPEED 1080.0
#define N_HELICOPTER_TEXTURES 4
#define MAX_FLYOVERS 3
#define MIN_FLYOVER_HEIGHT 2.0
#define MAX_FLYOVER_HEIGHT 7.0
#define FLYOVER_STEP 0.5
#define FLYOVER_TIME_FACTOR 3.0
#define MAX_HELIS_IN_FORMATION 3
#define MIN_FLYOVER_SPEED 1
#define MAX_FLYOVER_SPEED 5
#define FLYOVER_CHECK_SLEEP 1.0

// TREE GLOBALS
#define N_TREES 900
#define N_TREE_TEXTURES 2
#define N_LEAVES_LOD0 7
#define N_LEAF_SIDES_LOD0 6

// GROUND GLOBALS
#define N_GROUND_TEXTURES 3
#define WORLD_RADIUS 16
#define WORLD_WALK_RADIUS 8
#define WORLD_TOPO_WIDTH 161
#define N_ROADS 5
#define TOWN_OFFSET 1
#define ROAD_REPEAT_DIST 0.3
#define GROUND_REPEAT 16
#define GROUND_ANGLE_STEP 15

// MOVEMENT GLOBALS
#define MAX_VERTICAL_ANGLE 90.0
#define FB_MOVE_SPEED 0.5
#define LR_MOVE_SPEED 0.3
#define SPRINT_FACTOR 2.0
#define LATERAL_FACTOR 0.7
#define X_MOUSE_SENSITIVITY 0.15
#define Y_MOUSE_SENSITIVITY 0.1
#define JUMP_HEIGHT CAMERA_HEIGHT / 1.5
#define JUMP_DURATION 500
#define JUMP_COEFFICIENT -(4 * JUMP_HEIGHT) / (JUMP_DURATION * JUMP_DURATION)

// LIGHT GLOBALS
#define SUN_RADIUS_RATIO 3
#define SUN_RADIUS 0.2
#define DEFAULT_SUN_SPEED 5.0
#define SUN_MAX_SPEED 50.0
#define SUN_MIN_SPEED 1.0
#define SUN_SPEED_STEP 1.0
#define SUN_INCR 10
#define AMBIENT_FLOOR 10.0
#define AMBIENT_RANGE 40.0
#define DIFFUSE_FLOOR 0
#define DIFFUSE_RANGE 35.0
#define FLASHLIGHT_CUTOFF Cos(10.0)
#define FLASHLIGHT_OUTER_CUTOFF Cos(20.0)
#define FLASHLIGHT_EXP 1.0
#define FLASHLIGHT_CONSTANT_ATTENUATION 1.0
#define FLASHLIGHT_LINEAR_ATTENUATION 0.7
#define FLASHLIGHT_QUADRATIC_ATTENUATION 1.8
#define SKY_LIGHT_FLOOR 0.05

// WEAPON GLOBALS
#define N_ENEMIES 15
#define MAX_BULLETS 150
#define BULLET_SPEED 5.0
#define ENEMY_TIME_DOWN 10.0
#define ENEMY_SCALE 0.1
#define FIRE_DELAY 0.1
#define BULLET_DAMAGE 30
#define BULLET_SCALE 0.0015
#define HEADSHOT_MULTIPLIER 3
#define WEAPON_SCALE 0.05
#define N_WEAPON_TEXTURES 3
#define WEAPON_FIRE_RATE 0.1
#define WEAPON_BLOOM 50
#define WEAPON_FIRE_OFFSET_X 0.04
#define WEAPON_FIRE_OFFSET_Y -0.01
#define BULLET_REPEAT 2
#define weaponOffsetX 0.02
#define weaponOffsetY -CAMERA_HEIGHT / 5
#define weaponOffsetZ -0.02
#define weaponHeight 0.1
#define weaponLen 0.7
#define weaponWidth 0.05
#define barrelRadius weaponWidth / 3
#define barrelLen 0.3
#define barrelStep 30
#define gripWidth weaponWidth / 1.5
#define gripHeight weaponHeight * 2
#define gripLength weaponLen / 7
#define gripZOffset weaponLen / 1.5
#define gripXOffset (weaponWidth-gripWidth) / 2.0

// MIN/MAX functions
#define igormax(a,b) (((a) > (b)) ? (a) : (b))
#define igormin(a,b) (((a) < (b)) ? (a) : (b))

#ifdef __cplusplus
extern "C" {
#endif

// GLOBAL STRUCTS
struct rgb {
    double r;
    double g;
    double b;
};

// HOUSE STRUCTS
struct houseTheme {
    int wall;
    int roof;
    int door;
    int window;
};

struct house {
    double x;
    double y;
    double z;
    double rotateAngleY;
    double scale;
    int theme;
};

// TREE STRUCTS
struct treeTheme {
    int bark;
    int leaves;
};

struct tree {
    double x;
    double y;
    double z;
    double rotate;
    double scale;
    int theme;
};

// HELICOPTER STRUCTS
struct helicopterTheme {
    int body;
    int rotor;
    int window;
};
struct helicopter {
    double x;
    double y;
    double z;
    double yaw;
    double pitch;
    double roll;
    double scale;
    double mainRotorAngle;
    double mainRotorSpeed;
    double secondaryRotorAngle;
    double secondaryRotorSpeed;
    int inView;
    int theme;
};

struct flyover {
    double angle;
    double pos;
    double speed;
    double height;
    double x;
    double z;
    int n_helis;
    int active;
};

// GROUND STRUCTS
struct road {
    double left;
    double right;
    double top;
    double bottom;
};

// WEAPON STRUCTS
struct enemy {
    double x;
    double y;
    double z;
    double timeOfDown;
    int xOrient;
    int health;
    int up;
};

struct bullet {
    double x;
    double y;
    double z;
    double dx;
    double dy;
    double dz;
    int active;
};

struct bulletVertex {
    double x;
    double y;
    double z;
};

struct bulletTri {
    int A;
    int B;
    int C;
};

// HOUSE FUNCTIONS
void drawHouse(double x, double y, double z, double yth, double scale, struct houseTheme theme);
void drawHouses();
void houseInit(unsigned int *houseTexturePtr);

// HELICOPTER FUNCTIONS
void helicopterIdle(double t, double dt);
void drawRotor(double x, double y, double z, double xth, double yth, double scale, double rotorWidth, double shaftHeight);
void drawHelicopter(double x, double y, double z, double yaw, double pitch, double roll, double scale, double mainRotorAngle, double secondaryRotorAngle, struct helicopterTheme theme);
void drawHelicopters();
void helicopterInit(unsigned int *helicopterTexturePtr);
int getRandomInRange(int low, int high);
int timeForNewFlyover(double t);
int getFirstNonActiveFlyover();
int isInvalidFlyoverHeight(double height, int flyover);
void initFlyover(double t);
void setFormation(int flyover);
void updateFormation(int flyover, double dt);
void cleanFlyover(int flyover);

// GROUND FUNCTIONS
void drawGround();
void drawSky(double lightIntensity);
void groundInit(unsigned int *groundTexturePtr);
double getElevation(double x, double z);
void crossProduct(double a[3], double b[3], double res[3]);
void getGroundNormal(double resultNorm[3], int r, int c);

// TREE GLOBALS
void drawTrees();
void drawTree(double x, double y, double z, double rotate, double scale);
void treeInit(unsigned int *treeTexturePtr);

// MOVEMENT FUNCTIONS
double getFpLatTh();
double getFpVerTh();
void forwardKey(int down, int *fov);
void backKey(int down);
void leftKey(int down);
void rightKey(int down);
void sprintKey(int down, int *fov);
void jumpKey(int down);
void movementIdle(double t, double dt, double fpPosVec[3], double fpGroundDir[2], double mapBounds[4], double* jumpHeightPtr, double* elevationPtr);
void mouseMoved(int x, int y, int centerX, int centerY);
void setJumpHeight(double dt);
double magnitude(double vec[], int n);
void normalize(double vec[], int n, double scalar);
void calcFPAngle(double fpPosVec[3], double fpGroundDir[2], double fpViewVector[3], double fpUp[3]);

// LIGHTING FUNCTIONS
void lightingIdle(double dt, double *lightIntensity);
void lightingDisplay(double fpPosVec[3], double fpViewVector[3], double h);
void lightingPause();
void changeLightSpeed(int speedUp);
void sunVertex(double th,double ph);
void drawSun(double x,double y,double z,double r);

// WEAPON FUNCTIONS
void weaponIdle(double t, double dt, double fpPosVec[3], double h);
void weaponLeftMouseDown();
void weaponLeftMouseUp();
void drawBullets();
void drawBullet(struct bullet b);
void drawWeapon(double x, double y, double z);
void drawHUD();
void switchProjectionAndDrawWeapon(double fov, double asp);
void switchProjectionAndDrawHUD(double asp);
void drawEnemies();
void drawEnemy(struct enemy e);
void checkAllEnemies(double t);
void updateEnemyStatus(int i, double t);
void checkHitboxes(double t);
void updateBullets(double dt);
void weaponInit(unsigned int *weaponTexturePtr);
void translatePointFromLocalCoord(double result[3], double local[3], double fpPosVec[3], double fpVerTh, double fpLatTh, double h);
void shootBullet(double fpPosVec[3], double h);
int updateFreeBulletIndex();
double getEnemyHeadTop(int i);
double getEnemyHeadLeft(int i);
double getEnemyHeadRight(int i);
double getEnemyHeadFront(int i);
double getEnemyHeadBack(int i);
double getEnemyBodyTop(int i);
double getEnemyBodyBottom(int i);
double getEnemyBodyLeft(int i);
double getEnemyBodyRight(int i);
double getEnemyBodyFront(int i);
double getEnemyBodyBack(int i);
int checkBullet(double x, double y, double z, double t, int enemyIdx);

// DEM FUNCTIONS
void ReadTreeDEM(char* file, struct tree trees[N_TREES]);
void SaveTreeDEM(char* file, struct tree trees[N_TREES]);
void ReadHouseDEM(char* file, struct house houses[N_HOUSES]);
void SaveHouseDEM(char* file, struct house houses[N_HOUSES]);
void ReadMapDEM(char* file, double worldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH]);
void SaveMapDEM(char* file, double worldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH]);
void ReadEnemyDEM(char* file, struct enemy enemies[N_ENEMIES]);
void SaveEnemyDEM(char* file, struct enemy enemies[N_ENEMIES]);

// SHADER FUNCTIONS
void PrintShaderLog(int obj, char* file);
void PrintProgramLog(int obj);
int CreateShader(GLenum type, char* file);
int CreateShaderProg(char* VertFile, char* FragFile);
char* ReadText(char *file);

#ifdef __cplusplus
}
#endif

#endif
