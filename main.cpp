// #define GLAD_GL_IMPLEMENTATION
// #include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include <nuklear.h>
#include <nuklear_glfw_gl2.h>


#include <cmath>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define POINT_RADIUS 6
#define MENU_WIDTH 230

typedef std::pair<GLdouble,GLdouble> coords;
typedef std::vector<coords> points;

points dots;
std::size_t gottenPointIndex;

coords deCasteljau(std::size_t r, std::size_t i, double t)
{
    if(r == 0)
        return dots[i];
    coords p1 = deCasteljau(r-1, i, t),
           p2 = deCasteljau(r-1, i+1,t);
    return std::make_pair((1 - t) * p1.first + t * p2.first, (1 - t) * p1.second + t * p2.second);
}

coords polar(std::size_t r, std::size_t i, double t, double p, std::size_t power, points& polarLines)
{
    if(r == 0)
        return dots[i];
    coords p1 = polar(r-1, i, t, p, power, polarLines),
           p2 = polar(r-1, i+1,t, p, power, polarLines);
    // if (std::find(dots.begin(), dots.end(), p1) == dots.end() && std::find(polarLines.begin(), polarLines.end(), p1) == polarLines.end())
    //     polarLines.push_back(p1);
    // if (std::find(dots.begin(), dots.end(), p2) == dots.end() && std::find(polarLines.begin(), polarLines.end(), p2) == polarLines.end())
    //     polarLines.push_back(p2);
    if(r <= power){
        return std::make_pair((1 - p) * p1.first + p * p2.first, (1 - p) * p1.second + p * p2.second);
    }
    return std::make_pair((1 - t) * p1.first + t * p2.first, (1 - t) * p1.second + t * p2.second);
}

points computePoints(double minT, double maxT, std::size_t segments)
{
    points pointlist;
    double t;
    for(std::size_t i = 0; i <= segments; ++i)
    {
        t = minT + i/(double)segments * (maxT - minT);
        pointlist.push_back(deCasteljau(dots.size() - 1, 0, t));
    }
    return pointlist;
}

points computePolarPoints(double minT, double maxT, std::size_t segments, double p, std::size_t power, points& polarLines)
{
    points pointlist;
    double t;
    for(std::size_t i = 0; i <= segments; ++i)
    {
        t = minT + i/(double)segments * (maxT - minT);
        pointlist.push_back(polar(dots.size() - 1, 0, t, p, power, polarLines));
    }
    return pointlist;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        GLdouble xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if(xpos <= MENU_WIDTH )
        {
            return;
        }
        dots.push_back(std::make_pair(xpos - SCREEN_WIDTH/2, ypos - SCREEN_HEIGHT/2));
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        GLdouble xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = xpos - SCREEN_WIDTH/2;
        ypos = ypos - SCREEN_HEIGHT/2;
        for(std::size_t i = 0; i < dots.size(); ++i)
        {
            if(std::pow(dots[i].first - xpos, 2) + std::pow(dots[i].second - ypos, 2) <= POINT_RADIUS * POINT_RADIUS)
            {   
                gottenPointIndex = i;
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT && action != GLFW_PRESS){
        gottenPointIndex = -1;
    }
}

int main(void)
{   
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 10);

    /* Platform */
    static GLFWwindow *window;
    int width = 0, height = 0;
    struct nk_context *ctx;
    struct nk_colorf bg;

    /* GLFW */
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stdout, "[GFLW] failed to init!\n");
        exit(1);
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Polar", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);

    /* GUI */
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    {
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        nk_glfw3_font_stash_end();
    }


    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor( 0.4f, 0.3f, 0.4f, 0.0f );

    glEnable(GL_MULTISAMPLE);  


    points bezierPoints, polarPoints, polarLines;
    int powerPolar = 2;
    gottenPointIndex = -1;
    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {   
        glfwPollEvents();

        /* Input */
        glfwPollEvents();
        nk_glfw3_new_frame();

        /* GUI */
        if (nk_begin(ctx, "Contorls", nk_rect(0, 0, MENU_WIDTH, SCREEN_HEIGHT),
            NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);


        /* Draw */
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);


        glEnable( GL_POINT_SMOOTH ); // make the point circular
        glPointSize( POINT_RADIUS * 2 - 2); // must be added before glDrawArrays is called
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin( GL_POINTS);
        for(auto dot : dots)
        {   
            glVertex3d(dot.first / (SCREEN_WIDTH / 2), - dot.second / (SCREEN_HEIGHT / 2), 0);
        }
        glEnd();
        glDisable( GL_POINT_SMOOTH ); // stop the smoothing to make the points circular

        glLineWidth(3);
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin( GL_LINES);
        for(std::size_t i = 1; i < dots.size(); ++i)
        {
            glVertex3d(dots[i - 1].first / (SCREEN_WIDTH / 2), - dots[i - 1].second / (SCREEN_HEIGHT / 2), 0);
            glVertex3d(dots[i].first / (SCREEN_WIDTH / 2), - dots[i].second / (SCREEN_HEIGHT / 2), 0);
        }
        glEnd();

        if(gottenPointIndex != -1){
            GLdouble xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            if(xpos < MENU_WIDTH + POINT_RADIUS ){
                xpos = MENU_WIDTH + POINT_RADIUS;
            }
            if(xpos > SCREEN_WIDTH - POINT_RADIUS){
                xpos = SCREEN_WIDTH - POINT_RADIUS;
            }
            if(ypos < POINT_RADIUS){
                ypos = POINT_RADIUS;
            }
            if(ypos > SCREEN_HEIGHT - POINT_RADIUS){
                ypos = SCREEN_HEIGHT - POINT_RADIUS;
            }
            xpos = xpos - SCREEN_WIDTH/2;
            ypos = ypos - SCREEN_HEIGHT/2;
            dots[gottenPointIndex].first = xpos;
            dots[gottenPointIndex].second = ypos;
        }

        if(dots.size() > 2)
        {
            bezierPoints = computePoints(0,1,10*dots.size());
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin( GL_LINES);
            for(std::size_t i = 1; i < bezierPoints.size(); ++i)
            {
                glVertex3d(bezierPoints[i - 1].first / (SCREEN_WIDTH / 2), - bezierPoints[i - 1].second / (SCREEN_HEIGHT / 2), 0);
                glVertex3d(bezierPoints[i].first / (SCREEN_WIDTH / 2), - bezierPoints[i].second / (SCREEN_HEIGHT / 2), 0);
            }
            glEnd();
            
        }

        if(dots.size() > 2 + powerPolar){
            polarPoints = computePolarPoints(0,1,10*dots.size(), 0.3, powerPolar, polarLines);
            glColor3f(1.0f, 1.0f, 0.0f);
            glBegin( GL_LINES);
            for(std::size_t i = 1; i < polarPoints.size(); ++i)
            {
                glVertex3d(polarPoints[i - 1].first / (SCREEN_WIDTH / 2), - polarPoints[i - 1].second / (SCREEN_HEIGHT / 2), 0);
                glVertex3d(polarPoints[i].first / (SCREEN_WIDTH / 2), - polarPoints[i].second / (SCREEN_HEIGHT / 2), 0);
            }
            glEnd();

            glColor3f(0.0f, 1.0f, 1.0f);

            glBegin(GL_POINTS);
            glEnd();
            glBegin( GL_LINES);
            for(std::size_t i = 1; i < polarLines.size(); ++i)
            {
                glVertex3d(polarLines[i - 1].first / (SCREEN_WIDTH / 2), - polarLines[i - 1].second / (SCREEN_HEIGHT / 2), 0);
                glVertex3d(polarLines[i].first / (SCREEN_WIDTH / 2), - polarLines[i].second / (SCREEN_HEIGHT / 2), 0);
            }
            glEnd();
        }
        /* Swap front and back buffers */
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(window);

        /* Poll for and process events */
        // glfwPollEvents();
    }
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}