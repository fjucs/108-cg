#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl2.h"
#include <GL/freeglut.h>

#include "bezier.h"

int g_wWidth = 1280, g_wHeight = 720;

static ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

static float x, y, z, r;
static ImVec4 circle_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

void gui_overlay()
{
    // static bool b = true;
    // ImGui::ShowDemoWindow(&b);
    //
    static bool panel_debug = true, panel_3d = true;
    ImGui::Begin("Control");
    {
        ImGui::Checkbox("Debug", &panel_debug);
        ImGui::Text("Avg. %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::End();

    if(panel_debug)
    {
        ImGui::Begin("Bezier Debug");
        ImGui::ColorEdit3("clear color", (float *)&clear_color);

        for (int i = 0; i < NUM_POINT; i++)
            ImGui::Text("%-3s= (%.2f, %.2f)", POINT_NAME[i], orig_point[i][0], orig_point[i][1]);

        ImGui::Checkbox("Show control points", &g_showControlPoint);
        ImGui::SameLine();
        ImGui::Checkbox("Show control lines", &g_showControlLine);

        ImGui::Text("Not select = %s", g_selectedPoint != -1 ? POINT_NAME[g_selectedPoint] : "None");
        ImGui::End();

        ImGui::Begin("3D Debug");
        ImGui::ColorEdit3("3D color", (float *)&color_3d);
        // for (int i = 0; i < NUM_POINT; i++)
        //     ImGui::Text("%-3s= (%.2f, %.2f)", POINT_NAME[i], ap[i][0], ap[i][1]);
        static float val_translate[3];
        ImGui::DragFloat3("Pos", val_translate, 0.01f, -1.f, 1.f);
        x_translate = val_translate[0], y_translate = val_translate[1], z_translate = val_translate[1];

        static bool wire_flag = true, fill_flag = false;
        if (ImGui::RadioButton("Wire mode", wire_flag))
            g_3dDrawMode = WIRE_MODE, wire_flag = true, fill_flag = false;;
        if (ImGui::RadioButton("Fill mode", fill_flag))
            g_3dDrawMode = FILL_MODE, fill_flag = true, wire_flag = false;

        ImGui::SliderFloat("x rotate", &x_rot, -360.f, 360.f);
        ImGui::SliderFloat("y rotate", &y_rot, -360.f, 360.f);
        ImGui::SliderFloat("z rotate", &z_rot, -360.f, 360.f);

        ImGui::Checkbox("3D Lighting", &flag_3d_lighting);
        ImGui::End();

        if (flag_3d_lighting)
        {
            ImGui::Begin("Lighting");

            ImGui::BeginGroup();
            ImGui::Text("Light (x, y, z, w)");
            ImGui::DragFloat4("Position", light_position, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Ambient", light_ambient, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Diffuse", light_diffuse, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Specular", light_specular, 0.01f, -1.f, 1.f);
            ImGui::EndGroup();

            ImGui::BeginGroup();
            ImGui::Text("Material (x, y, z, w)");
            ImGui::DragFloat4("Ambient", obj_mat_ambient, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Diffuse", obj_mat_diffuse, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Specular", obj_mat_specular, 0.01f, -1.f, 1.f);
            ImGui::DragFloat4("Emission", obj_mat_emission, 0.01f, -1.f, 1.f);
            ImGui::DragFloat("Shininess", &obj_mat_shininess, 0.01f, -1.f, 1.f);
            ImGui::EndGroup();

            ImGui::End();
        }
    }
}

void update()
{
    update_bezier();
}

void render()
{
    // Disable ligiting when drawing 2D
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    render_bezier();

    if(flag_3d_lighting)
    {
        // Set light attributes
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        // Enable lighting
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);
        // glEnable(GL_DEPTH_TEST); // https://blog.csdn.net/huutu/article/details/20872525
        glShadeModel(GL_SMOOTH);

        // Set material attributes
        glMaterialfv(GL_FRONT, GL_AMBIENT, obj_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, obj_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, obj_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, obj_mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, obj_mat_shininess);
        glEnable(GL_COLOR_MATERIAL);
    }

    draw_3d_bezier();
}

void glut_display_func()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    // Init
    glEnable(GL_MAP1_VERTEX_3);
    // Update
    // Rendering
    gui_overlay();
    ImGui::Render();
    ImGuiIO &io = ImGui::GetIO();
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.

    update();
    render();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

int main(int argc, char **argv)
{
    // Create GLUT window
    glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_wWidth, g_wHeight);
    glutCreateWindow("hw1");

    // Setup GLUT display function
    // We will also call ImGui_ImplGLUT_InstallFuncs() to get all the other functions installed for us,
    // otherwise it is possible to install our own functions and call the imgui_impl_glut.h functions ourselves.
    glutDisplayFunc(glut_display_func);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGLUT_Init();
    ImGui_ImplGLUT_InstallFuncs();
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    glutMainLoop();

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
