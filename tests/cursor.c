//========================================================================
// Mouse cursor accuracy test
// Copyright (c) Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//
// System cursors and input modes tests.
//
//========================================================================

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

static int W = 640;
static int H = 480;
static int delay = 0;

static GLFWwindow* window = NULL;
static GLFWcursor* cursor = NULL;

static struct { int key; double time; } commands[] = {
    {GLFW_KEY_H, 0},
    {GLFW_KEY_C, 0},
    {GLFW_KEY_D, 0},
    {GLFW_KEY_S, 0},
    {GLFW_KEY_N, 0},
    {GLFW_KEY_1, 0},
    {GLFW_KEY_2, 0},
    {GLFW_KEY_3, 0}
};

static int CommandCount = sizeof(commands) / sizeof(commands[0]);

static void command_callback(int key)
{
    switch (key)
    {
        case GLFW_KEY_H:
        {
            printf("H: show this help\n");
            printf("C: call glfwCreateCursor()\n");
            printf("D: call glfwDestroyCursor()\n");
            printf("S: call glfwSetCursor()\n");
            printf("N: call glfwSetCursor() with NULL\n");
            printf("1: set GLFW_CURSOR_NORMAL\n");
            printf("2: set GLFW_CURSOR_HIDDEN\n");
            printf("3: set GLFW_CURSOR_DISABLED\n");
            printf("T: enable 3s delay for all previous commands\n");
        }
        break;

        case GLFW_KEY_C:
        {
            if (cursor == NULL)
            {
                int w = 32, h = 32;
                int x, y, i = 0;
                unsigned char image[4 * w * h];

                for (y = 0; y < h; y++)
                {
                    for (x = 0; x < w; x++)
                    {
                        image[i++] = 0xFF;
                        image[i++] = 0;
                        image[i++] = 255 * y / 32;
                        image[i++] = 255 * x / 32;
                    }
                }

                cursor = glfwCreateCursor(w, h, w / 2, h / 2, 0, image);
            }
        }
        break;

        case GLFW_KEY_D:
        {
            if (cursor != NULL)
            {
                glfwDestroyCursor(cursor);
                cursor = NULL;
            }
        }
        break;

        case GLFW_KEY_S:
        {
            if (cursor != NULL)
                glfwSetCursor(window, cursor);
            else
                printf("The cursor is not created\n");
        }
        break;

        case GLFW_KEY_N:
        {
            glfwSetCursor(window, NULL);
        }
        break;

        case GLFW_KEY_1:
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        break;

        case GLFW_KEY_2:
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        break;

        case GLFW_KEY_3:
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
    }
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    W = width;
    H = height;

    glViewport(0, 0, W, H);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        case GLFW_KEY_T:
            delay = !delay;
            printf("Delay %s.\n", delay ? "enabled" : "disabled");
            break;

        default:
        {
            if (delay)
            {
                int i = 0;

                while (i < CommandCount && commands[i].key != key)
                    i++;

                if (i < CommandCount)
                    commands[i].time = glfwGetTime();
            }
            else
            {
                command_callback(key);
            }
        }
        break;
    }
}

static void refresh_callback(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(W, H, "Cursor testing", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowRefreshCallback(window, refresh_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);

    key_callback(window, GLFW_KEY_H, 0, GLFW_PRESS, 0);

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
    {
        if (delay)
        {
            int i;
            double t = glfwGetTime();

            for (i = 0; i < CommandCount; i++)
            {
                if (commands[i].time != 0 && t - commands[i].time >= 3.0)
                {
                    command_callback(commands[i].key);
                    commands[i].time = 0;
                }
            }
        }

        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

