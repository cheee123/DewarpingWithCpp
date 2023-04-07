#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

const unsigned int width = 1024;
const unsigned int height = 1024;

int main()
{
	// Square object
	GLfloat vertices[4*5] = { //4 points, 3 coord + 2 tex coord
		-0.5,  0.5, 0., 0., 1.,
		-0.5, -0.5, 0., 0., 0.,
		0.5 , -0.5, 0., 1., 0.,
		0.5 ,  0.5, 0., 1., 1.
	};
	GLuint  indices[6] = {
		0, 1, 3,
		1, 2, 3
	};

	// Initialize glfw
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(width, height, "Dewarping", NULL, NULL);
    if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
    glfwMakeContextCurrent(window);

	gladLoadGL();

    glViewport(0, 0, width, height);

	// Generates Shader
	Shader shaderProgram("./resources/default.vert", "./resources/default.frag");

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(vertices, sizeof(vertices));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(indices, sizeof(indices));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();	
	
	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 1.0f));

	// Texture
	Texture brickTex0("./resources/F8.jpg", "GL_TEXTURE_2D", 0);
    brickTex0.texUnit(shaderProgram, "tex0" , 0);

	// Width, Height of source image
	cv::Mat inimg = cv::imread("./resources/F8.jpg");
	int ws = inimg.cols, hs = inimg.rows;

	// outimg (same size as FBO)
	cv::Mat outimg(height, width, CV_8UC3);

	// Set up uniform
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "Wd"), width);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "Hd"), height);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "Ws"), ws);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "Hs"), hs);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "rotation"), 0.0);
	glUniform1f(glGetUniformLocation(shaderProgram.ID, "fov_angle"), 180.0);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "line"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "isPerspective"), 0);

	// Keeps track of the amount of frames in timeDiff
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate FPS
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 2.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "Dewarping " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();
		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.Matrix(60.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");
		
		// Binds texture so that is appears in rendering
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brickTex0.ID);

		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		
		// Read pixels from current frame
		glReadPixels(0, 0 , height, width, GL_RGB, GL_UNSIGNED_BYTE, outimg.data);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Write outimg
	cv::flip(outimg, outimg, 0);
	cv::cvtColor(outimg, outimg, cv::COLOR_BGR2RGB);
	cv::imwrite("./outimg.jpg", outimg);

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickTex0.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
