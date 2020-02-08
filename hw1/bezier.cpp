#include "bezier.h"

extern int g_wWidth, g_wHeight;

GLfloat points[NUM_POINT][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
GLfloat orig_point[NUM_POINT][3] = {
    {248, 428, 0},  // a
    {148, 388, 0},  // ctrl1
    {14, 270, 0},   // ctrl2
    {158, 262, 0},  // b
    {116, 25, 0},   // ctrl3
    {23, 327, 0},   // ctrl4
    {71, 431, 0}    // c
};

GLfloat ap[NUM_POINT][3], bp[NUM_POINT][3];
float orthoMax[3] = {1.0f, 1.0f, 1.0f}, orthoMin[3] = {-1.0f, -1.0f, -1.0f};
float draw_point[21][2];

bool g_showControlPoint = true, g_showControlLine = true;

int g_selectedPoint = -1;
float pointSize = 10.f;

// For 3D
int g_3dDrawMode = WIRE_MODE;
ImVec4 color_3d = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
float x_translate = 0.f, y_translate = 0.f, z_translate = 0.f;
float x_rot, y_rot, z_rot;
// For 3D ligiting
bool flag_3d_lighting;
// ref: http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
GLfloat light_position[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
// material
GLfloat obj_mat_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};  // 環境光
GLfloat obj_mat_diffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};  // 漫反射光
GLfloat obj_mat_specular[] = {0.0f, 0.0f, 0.0f, 1.0f}; // 鏡面反射光
GLfloat obj_mat_emission[] = {0.8f, 0.0f, 0.0f, 1.0f}; // 材質輻射光
GLfloat obj_mat_shininess = 0.0f;
// --------------------------------------

extern const char * const POINT_NAME[NUM_POINT] = {
    "A",
    "C1",
    "C2",
    "B",
    "C3",
    "C4",
    "C"
};

void real_point2ortho(GLfloat x, GLfloat y, GLfloat &fx, GLfloat &fy)
{
    fx = orthoMin[POS_X] + x / g_wWidth * (orthoMax[POS_X] - orthoMin[POS_X]);
    fy = orthoMin[POS_Y] + (g_wHeight - y) / g_wHeight * (orthoMax[POS_Y] - orthoMin[POS_Y]);
}

void draw_points()
{
    int ps;
    glGetIntegerv(GL_POINT_SIZE, &ps);
    glPointSize(pointSize);

    if(g_showControlPoint)
    {
        glBegin(GL_POINTS);
        {
            for (int i = 0; i < NUM_POINT; i++)
            {
                if (i == P_A || i == P_B || i == P_C) // end point
                    glColor3f(0.0, 1.0, 1.0);
                else
                    glColor3f(0.0, 1.0, 0.0);
                if (i == g_selectedPoint)
                    glColor3f(1.0, 0.0, 0.0);
                glVertex3f(points[i][0], points[i][1], points[i][2]);
            }
        }
        glEnd();
    }

    if(g_showControlLine)
    {
        glBegin(GL_LINE_STRIP);
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            for(int i = 0; i < NUM_POINT; i++)
                glVertex3f(points[i][0], points[i][1], points[i][2]);
        }
        glEnd();
    }
    glPointSize(ps);
}

void draw_circle_xz(float x, float y, float z, float r)
{
    int sections = 100; // split into sections (for draw lines)
    const float twopi = M_PI * 2;
    glColor4f(color_3d.x, color_3d.y, color_3d.z, color_3d.w);
    glBegin(GL_LINE_STRIP);
    {
        for(int s = 0; s <= sections; s++)
        {
            glVertex3f(
                x + r * cos(s * twopi / sections),
                y,
                z + r * sin(s * twopi / sections)
            );
        }
    }
    glEnd();
}

void draw_horizonal_circle()
{
    GLfloat P0_X = points[0][0], P0_Y = points[0][1];
    GLfloat P1_X = points[1][0], P1_Y = points[1][1];
    GLfloat P2_X = points[2][0], P2_Y = points[2][1];
    GLfloat P3_X = points[3][0], P3_Y = points[3][1];
    GLfloat P4_X = points[4][0], P4_Y = points[4][1];
    GLfloat P5_X = points[5][0], P5_Y = points[5][1];
    GLfloat P6_X = points[6][0], P6_Y = points[6][1];

    for (GLfloat t = 0; t <= 1.1; t += 0.1)
    {
        GLfloat x = P0_X * pow((1 - t), 3) + 3 * P1_X * t * pow((1 - t), 2) + 3 * P2_X * pow(t, 2) * (1 - t) + P3_X * pow(t, 3);
        GLfloat y = P0_Y * pow((1 - t), 3) + 3 * P1_Y * t * pow((1 - t), 2) + 3 * P2_Y * pow(t, 2) * (1 - t) + P3_Y * pow(t, 3);
        draw_point[(int)(t * 10)][0] = x;
        draw_point[(int)(t * 10)][1] = y;
        if (g_3dDrawMode == WIRE_MODE)
            draw_circle_xz(0, y, 0, x);
    }

    for (GLfloat t = 0; t <= 1.1; t += 0.1)
    {
        GLfloat x = P3_X * pow((1 - t), 3) + 3 * P4_X * t * pow((1 - t), 2) + 3 * P5_X * pow(t, 2) * (1 - t) + P6_X * pow(t, 3);
        GLfloat y = P3_Y * pow((1 - t), 3) + 3 * P4_Y * t * pow((1 - t), 2) + 3 * P5_Y * pow(t, 2) * (1 - t) + P6_Y * pow(t, 3);
        draw_point[10 + (int)(t * 10)][0] = x;
        draw_point[10 + (int)(t * 10)][1] = y;
        if (g_3dDrawMode == WIRE_MODE)
            draw_circle_xz(0, y, 0, x);
    }
}

// draw_point[i][0]???
void draw_surface_color()
{
    int sections = 10;
    const float TWOPI = M_PI * 2;
    //
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 20; i++)
    {
        for (int s = 0; s <= sections; s++)
        {
            float part = (float)s / (float)sections * TWOPI;
            glVertex3f(
                draw_point[i][0] * cos(part),
                draw_point[i][1],
                draw_point[i][0] * sin(part)
            );
            glVertex3f(
                draw_point[i+1][0] * cos(part),
                draw_point[i+1][1],
                draw_point[i+1][0] * sin(part));
        }
    }
    glEnd();
}

void draw_bezier()
{
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    for (int i = 0; i < 7; i++)
        real_point2ortho(orig_point[i][0], orig_point[i][1], points[i][0], points[i][1]);

    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 100.0f, 3, 4, &points[0][0]);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 100; i++)
    {
        glEvalCoord1f((GLfloat)i);
    }
    glEnd();

    glMap1f(GL_MAP1_VERTEX_3, 0.0f, 100.0f, 3, 4, &points[3][0]);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 100; i++)
    {
        glEvalCoord1f((GLfloat)i);
    }
    glEnd();
    glPopMatrix();
}

void draw_3d_bezier()
{
    // Initialize the Model-View matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Set ploygon draw mode
    if (g_3dDrawMode == WIRE_MODE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glScalef(0.5, 0.5, 0.5); // scale down to 0.5
    glRotatef(x_rot, 1, 0, 0);
    glRotatef(z_rot, 0, 0, 1);
    glTranslatef(x_translate, y_translate, z_translate);

    int deg = 3;
    for (int theta = 0; theta < 360; theta += deg)
    {
        glColor4f(color_3d.x, color_3d.y, color_3d.z, color_3d.w);
        glRotatef(y_rot + deg, 0, 1, 0);
        if (g_3dDrawMode == WIRE_MODE)
            draw_horizonal_circle();
        else
            draw_horizonal_circle(), draw_surface_color();

        glMap1f(GL_MAP1_VERTEX_3, 0.0f, 500.0f, 3, 4, &points[0][0]);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 500; i++)
            glEvalCoord1f((GLfloat)i);
        glEnd();

        glMap1f(GL_MAP1_VERTEX_3, 0.0f, 500.0f, 3, 4, &points[3][0]);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < 500; i++)
            glEvalCoord1f((GLfloat)i);
        glEnd();
    }

    glPopMatrix();
}

void render_bezier()
{
    draw_bezier();
    draw_points();
}

bool touch_point(float x, float y, float z, const float p[3], float dis)
{
    float d = pow(abs(x - p[POS_X]), 2) + pow(abs(y - p[POS_Y]), 2);
    return d <= dis*dis;
}

void update_bezier()
{
    auto &io = ImGui::GetIO();
    float x = io.MousePos.x, y = io.MousePos.y;
    bool leftPress = io.MouseDown[0];
    static bool prePress = false;

    // printf("mouse: %f %f %d %d ", x, y, leftPress, prePress);

    do
    {
        if (leftPress)
        {
            if(prePress) break;

            // For 2D
            for(int i = 0; i < NUM_POINT; i++)
            {
                if (touch_point(x, y, 0.f, orig_point[i], pointSize))
                {
                    g_selectedPoint = i;
                    break;
                }
            }

        }
        else
        {
            g_selectedPoint = -1;
        }
    }
    while(0);

    // printf("select=%d\n", g_selectedPoint);

    if(g_selectedPoint != -1)
    {
        orig_point[g_selectedPoint][POS_X] = x;
        orig_point[g_selectedPoint][POS_Y] = y;
    }
    prePress = leftPress;
}