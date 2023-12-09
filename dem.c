/*
 *   This is a c file that contains all dem read and write functions. Inspired by function in ex18 and modified to fit various purposes
 */
#include "CSCIx229.h"
#include "igor.h"

void LocalFatal(const char* format , ...) {
    va_list args;
    va_start(args,format);
    vfprintf(stderr,format,args);
    va_end(args);
    exit(1);
}

// -------------------
// TREES
// -------------------
void ReadTreeDEM(char* file, struct tree trees[N_TREES]) {
    int i,j;
    double treeData[6];
    FILE* f = fopen(file,"r");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_TREES;i++) {
        for (j=0;j<6;j++) {
            if (fscanf(f,"%lf",&treeData[j])!=1) LocalFatal("Error reading dem file\n");
        }
        trees[i].x = treeData[0];
        trees[i].y = treeData[1];
        trees[i].z = treeData[2];
        trees[i].rotate = treeData[3];
        trees[i].scale = treeData[4];
        trees[i].theme = (int)treeData[5];
    }
    fclose(f);
}

void SaveTreeDEM(char* file, struct tree trees[N_TREES]) {
    int i;
    FILE* f = fopen(file,"w");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_TREES;i++) {
        if (trees[i].scale >= 1.0) {
            trees[i].scale = 0.25;
        }
        if (fprintf(f, " %.2lf %.2lf %.2lf %.2lf %.3lf %.2lf\n", trees[i].x, trees[i].y, trees[i].z, trees[i].rotate, trees[i].scale, (double)trees[i].theme) < 2) {
            LocalFatal("Error saving to dem file\n");
        }
    }
    fclose(f);
}

// -------------------
// HOUSES
// -------------------
void ReadHouseDEM(char* file, struct house houses[N_HOUSES]) {
    int i,j;
    double houseData[6];
    FILE* f = fopen(file,"r");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_HOUSES;i++) {
        for (j=0;j<6;j++) {
            if (fscanf(f,"%lf",&houseData[j])!=1) LocalFatal("Error reading dem file\n");
        }
        houses[i].x = houseData[0];
        houses[i].y = houseData[1];
        houses[i].z = houseData[2];
        houses[i].rotateAngleY = houseData[3];
        houses[i].scale = houseData[4];
        houses[i].theme = (int)houseData[5];
    }
    fclose(f);
}

void SaveHouseDEM(char* file, struct house houses[N_HOUSES]) {
    int i;
    FILE* f = fopen(file,"w");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_HOUSES;i++) {
        if (houses[i].scale >= 1.0) {
            houses[i].scale = 0.25;
        }
        if (fprintf(f, " %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n", houses[i].x, houses[i].y, houses[i].z, houses[i].rotateAngleY, houses[i].scale, (double)houses[i].theme) < 2) {
            LocalFatal("Error saving to dem file\n");
        }
    }
    fclose(f);
}

// -------------------
// MAP
// -------------------
void ReadMapDEM(char* file, double worldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH]) {
    int i,j;
    FILE* f = fopen(file,"r");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (j=0;j<WORLD_TOPO_WIDTH;j++) {
        for (i=0;i<WORLD_TOPO_WIDTH;i++) {
            if (fscanf(f,"%lf",&worldTopo[j][i])!=1) LocalFatal("Error reading dem file\n");
        }
    }
    fclose(f);
}

void SaveMapDEM(char* file, double worldTopo[WORLD_TOPO_WIDTH][WORLD_TOPO_WIDTH]) {
    int i,j;
    FILE* f = fopen(file,"w");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (j=0;j<WORLD_TOPO_WIDTH;j++) {
        for (i=0;i<WORLD_TOPO_WIDTH;i++) {
            if (worldTopo[j][i] < 0) {
                worldTopo[j][i] = +0.0;
            }
            if (fprintf(f, " %.2lf", worldTopo[j][i]) < 2) {
                LocalFatal("Error saving to dem file\n");
            }
            if (i >= WORLD_TOPO_WIDTH-1) {
                fprintf(f, "\n");
            }
        }
    }
    fclose(f);
}

// -------------------
// ENEMIES
// -------------------
void ReadEnemyDEM(char* file, struct enemy enemies[N_ENEMIES]) {
    int i,j;
    double enemyData[7];
    FILE* f = fopen(file,"r");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_ENEMIES;i++) {
        for (j=0;j<7;j++) {
            if (fscanf(f,"%lf",&enemyData[j])!=1) LocalFatal("Error reading dem file\n");
        }
        enemies[i].x = enemyData[0];
        enemies[i].y = enemyData[1];
        enemies[i].z = enemyData[2];
        enemies[i].timeOfDown = enemyData[3];
        enemies[i].xOrient = (int)enemyData[4];
        enemies[i].health = (int)enemyData[5];
        enemies[i].up = (int)enemyData[6];
    }
    fclose(f);
}

void SaveEnemyDEM(char* file, struct enemy enemies[N_ENEMIES]) {
    int i;
    FILE* f = fopen(file,"w");
    if (!f) LocalFatal("Cannot open file %s\n",file);
    for (i=0;i<N_ENEMIES;i++) {
        if (fprintf(f, " %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf\n", enemies[i].x, enemies[i].y, enemies[i].z, enemies[i].timeOfDown, (double)enemies[i].xOrient, (double)enemies[i].health, (double)enemies[i].up) < 2) {
            LocalFatal("Error saving to dem file\n");
        }
    }
    fclose(f);
}
