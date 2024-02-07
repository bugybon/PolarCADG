// #define GLAD_GL_IMPLEMENTATION
// #include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <nuklear.h>
#include <GLFW/glfw3.h>
#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdexcept>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

std::vector<std::pair<GLdouble,GLdouble>> dots;

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
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        GLdouble xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        std::cout << "x: " << xpos << " y: " << ypos ;//<< std::endl;
        dots.push_back(std::make_pair(xpos - SCREEN_WIDTH/2, ypos - SCREEN_HEIGHT/2));
        std::cout << " & x: " << dots.back().first + SCREEN_WIDTH/2 << " y: " << dots.back().second + SCREEN_HEIGHT/2 << " lenght: " << dots.size() << std::endl;
    }
}

int main(void)
{   
    glfwInit();
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_SAMPLES, 4);


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Polar", NULL, NULL);
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



    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //   if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        //                  NK_WINDOW_BORDER |
        //                  NK_WINDOW_MOVABLE |
        //                  NK_WINDOW_SCALABLE |
        //                  NK_WINDOW_MINIMIZABLE |
        //                  NK_WINDOW_TITLE)) {
        //     // ..
        //     nk_layout_row_static(ctx, 30, 80, 1);
        //     if (nk_button_label(ctx, "button"))
        //         fprintf(stdout, "button pressed\n");
        //     }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        glEnable( GL_POINT_SMOOTH ); // make the point circular
        glPointSize( 10 ); // must be added before glDrawArrays is called
        glBegin( GL_POINTS);
        for(auto dot : dots)
        {   
            glVertex3d(dot.first / (SCREEN_WIDTH / 2), - dot.second / (SCREEN_HEIGHT / 2), 0);
        }
        glEnd();
        glDisable( GL_POINT_SMOOTH ); // stop the smoothing to make the points circular

        glLineWidth(3);
        glBegin( GL_LINES);
        for(int i = 0; i < (int)dots.size() - 1; ++i)
        {
            glVertex3d(dots[i].first / (SCREEN_WIDTH / 2), - dots[i].second / (SCREEN_HEIGHT / 2), 0);
            glVertex3d(dots[i + 1].first / (SCREEN_WIDTH / 2), - dots[i + 1].second / (SCREEN_HEIGHT / 2), 0);
        }
        glEnd();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}