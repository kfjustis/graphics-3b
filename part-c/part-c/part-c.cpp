// part-a.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>		// include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <algorithm>
#include <vector>
#include <math.h>

#include "InitShader.h"
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

GLuint vao;
int Index = 0;
GLuint program;
const int NumVertices = 36;
glm::vec4 points[NumVertices];
glm::vec4 colors[NumVertices];
glm::vec2 vTexCoords[NumVertices];
//----------------------------------------------------------------------------

glm::vec4 vertices[8] = {
	glm::vec4(-0.5, -0.5, 0.5, 1.0),
	glm::vec4(-0.5, 0.5, 0.5, 1.0),
	glm::vec4(0.5, 0.5, 0.5, 1.0),
	glm::vec4(0.5, -0.5, 0.5, 1.0),
	glm::vec4(-0.5, -0.5, -0.5, 1.0),
	glm::vec4(-0.5, 0.5, -0.5, 1.0),
	glm::vec4(0.5, 0.5, -0.5, 1.0),
	glm::vec4(0.5, -0.5, -0.5, 1.0)
};

enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };


GLuint  mvpi;

void quad(int a, int b, int c, int d)
{
	points[Index] = vertices[a]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
	points[Index] = vertices[b]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
	points[Index] = vertices[c]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
	points[Index] = vertices[a]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
	points[Index] = vertices[c]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
	points[Index] = vertices[d]; vTexCoords[Index] = glm::vec2(0.0, 0.0); Index++;
}

void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void init(void)
{
	colorcube();

	// texture stuff
	int x = 256;
	int y = 256;
	int n = 1;
	unsigned char* image_data = stbi_load("checker.jpg", &x, &y, &n, 0);

	GLuint texName = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, 36 * 4 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	GLuint texcoords_vbo;
	glGenBuffers(1, &texcoords_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
	glBufferData(GL_ARRAY_BUFFER, 36 * 2 * sizeof(GLfloat), vTexCoords, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	// Load shaders and use the resulting shader program
	program = InitShader("vshader36.glsl", "fshader36.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vertex_position = glGetAttribLocation(program, "vertex_position");
	glEnableVertexAttribArray(vertex_position);
	glVertexAttribPointer(vertex_position, 4, GL_FLOAT, GL_FALSE, 0,
		NULL);

	GLuint vt = glGetAttribLocation(program, "vt");
	glEnableVertexAttribArray(vt);
	glVertexAttribPointer(vt, 4, GL_FLOAT, GL_FALSE, 0,
		NULL);

	mvpi = glGetUniformLocation(program, "mvp");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

// globals for interactive operations
float g_lightpos_z = 4.0f;
bool g_light_on = true;

// add lighting
glm::vec4 light_position(0.0, 0.0, g_lightpos_z, 0.0); // x y z w 
glm::vec4 light_ambient(0.2, 0.2, 0.2, 1.0); // R G B A
glm::vec4 light_diffuse(1.0, 1.0, 1.0, 1.0);
glm::vec4 light_specular(1.0, 1.0, 1.0, 1.0);

glm::vec4 material_ambient(1.0, 0.0, 1.0, 1.0);
glm::vec4 material_diffuse(1.0, 0.8, 0.0, 1.0);
glm::vec4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

void handle_keys(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(window, 1);
		}

		if (key == GLFW_KEY_R) {
			g_lightpos_z = 4.0f;
			//glUseProgram(shader_programme);
		}

		// handle light pos z-axis
		if (key == GLFW_KEY_UP) {
			g_lightpos_z -= 1.0f;
			std::cout << "lightpos +: " << g_lightpos_z << std::endl;
		}
		if (key == GLFW_KEY_DOWN) {
			g_lightpos_z += 1.0f;
			std::cout << "lightpos -: " << g_lightpos_z << std::endl;
		}
		if (key == GLFW_KEY_L) {
			if (g_light_on) {
				g_light_on = false;
				std::cout << "g_light_on: false" << std::endl;
			}
			else if (!g_light_on) {
				g_light_on = true;
				std::cout << "g_light_on: true" << std::endl;
			}
		}
	}
}
//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	GLFWwindow *window = NULL;
	const GLubyte *renderer;
	const GLubyte *version;
	/* start GL context and O/S window using the GLFW helper library */
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	/* We must specify 3.2 core if on Apple OS X -- other O/S can specify
	anything here. I defined 'APPLE' in the makefile for OS X
	Remove the #ifdef #endif and play around with this - you should be starting
	an explicit version anyway, and some non-Apple drivers will require this too.
	*/
#ifdef APPLE
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	window = glfwCreateWindow(640, 640, "simple - kfjmbb - part c", NULL, NULL);

	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	/* start GLEW extension handler */
	glewExperimental = GL_TRUE;
	glewInit();
	renderer = glGetString(GL_RENDERER); /* get renderer string */
	version = glGetString(GL_VERSION);	 /* version as a string */
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	init();

	glfwSetKeyCallback(window, handle_keys);
	//


	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

		glm::mat4 model = glm::mat4(1.0);
		glm::vec3 eye(0.0f, 0.0f, 2.0f);
		glm::vec3 at(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(eye, at, up);

		model = glm::rotate(model, 5.0f, glm::vec3(25.0f, 10.0f, 5.0f));

		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.f);
		glm::mat4 mvp = Projection * view * model;

		glUniformMatrix4fv(mvpi, 1, GL_FALSE, glm::value_ptr(mvp));

		glDrawArrays(GL_TRIANGLES, 0, NumVertices);    // draw the points
													   /* update other events like input handling */
		glfwPollEvents();
		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;

}

