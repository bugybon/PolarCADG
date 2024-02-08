// #define GLAD_GL_IMPLEMENTATION
// #include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <nuklear.h>
#include <nuklear_glfw_gl2.h>
#include "linmath.h"

#include <cmath>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define POINT_RADIUS 6

typedef std::pair<GLdouble,GLdouble> coords;
typedef std::vector<coords> points;

points dots;
bool movePoint;
std::size_t gottenPoint;

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
        t = minT + i/(double)segments * (maxT + minT);
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
        t = minT + i/(double)segments * (maxT + minT);
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
        // std::cout << "x: " << xpos << " y: " << ypos ;//<< std::endl;
        dots.push_back(std::make_pair(xpos - SCREEN_WIDTH/2, ypos - SCREEN_HEIGHT/2));
        // std::cout << " & x: " << dots.back().first + SCREEN_WIDTH/2 << " y: " << dots.back().second + SCREEN_HEIGHT/2 << std::endl;
        std::cout << " lenght: " << dots.size() << std::endl;
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
                movePoint = true;
                gottenPoint = i;
                std::cout << "gotten" << std::endl;
            }
        }
    }

    if(button == GLFW_MOUSE_BUTTON_RIGHT && action != GLFW_PRESS){
        movePoint = false;
        gottenPoint = -1;
    }
}

int main(void)
{   
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);

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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Polar", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor( 0.4f, 0.3f, 0.4f, 0.0f );

    glEnable(GL_MULTISAMPLE);  


    points bezierPoints, polarPoints, polarLines;
    int powerPolar = 2;
    gottenPoint = -1;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


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

        if(gottenPoint != -1){
            GLdouble xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            xpos = xpos - SCREEN_WIDTH/2;
            ypos = ypos - SCREEN_HEIGHT/2;
            dots[gottenPoint].first = xpos;
            dots[gottenPoint].second = ypos;
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
            glBegin( GL_LINES);
            for(std::size_t i = 1; i < polarLines.size(); ++i)
            {
                glVertex3d(polarLines[i - 1].first / (SCREEN_WIDTH / 2), - polarLines[i - 1].second / (SCREEN_HEIGHT / 2), 0);
                glVertex3d(polarLines[i].first / (SCREEN_WIDTH / 2), - polarLines[i].second / (SCREEN_HEIGHT / 2), 0);
            }
            glEnd();
        }
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}