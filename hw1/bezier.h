#pragma once

#include <iostream>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/freeglut.h>
#include "imgui.h"

#define NUM_POINT 7

enum POINT_NAMES
{
    P_A = 0,
    P_C1,
    P_C2,
    P_B,
    P_C3,
    P_C4,
    P_C
};

enum COORD_ENUM
{
    POS_X = 0,
    POS_Y,
    POS_Z
};

enum DRAW_MODE3D
{
    WIRE_MODE,
    FILL_MODE
};
extern int g_3dDrawMode;

extern bool g_showControlPoint;
extern bool g_showControlLine;
extern int g_selectedPoint;
extern const char * const POINT_NAME[NUM_POINT];
extern GLfloat points[NUM_POINT][3];
extern GLfloat orig_point[NUM_POINT][3];
extern GLfloat ap[NUM_POINT][3];
extern GLfloat bp[NUM_POINT][3];
extern float orthoMax[3], orthoMin[3];

void real_point2ortho(int x, int y, GLfloat &fx, GLfloat &fy);

void render_bezier();
void draw_points();
void draw_bezier();

// For 3d

/*
 * Draw a circle on the x-z plane
 */
void draw_circle_xz(float x, float y, float z, float r);
/*
 * Draw the horizonal circle of the benzier curve in 3D
 * rememober to call this function in FILL_MODE
 * because the points needs to be updated)
 */
void draw_horizonal_circle();
void draw_surface_color();

extern int g_3dDrawMode;
extern ImVec4 color_3d;
extern float x_translate, y_translate, z_translate;
extern float x_rot, y_rot, z_rot;
extern bool flag_3d_lighting;
extern GLfloat light_position[4], light_ambient[4], light_diffuse[4], light_specular[4];
extern GLfloat obj_mat_ambient[4], obj_mat_diffuse[4], obj_mat_specular[4], obj_mat_emission[4], obj_mat_shininess;

void draw_3d_bezier();

bool touch_point(float x, float y, float z, const float p[3], float dis);
void update_bezier();