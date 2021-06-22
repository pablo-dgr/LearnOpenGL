#include <iostream>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Input.h"
#include "FpsCamera.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Buffers.h"
#include "VertexArray.h"

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static bool g_wireframeModeEnabled = false;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F8 && action == GLFW_PRESS)
	{
		if (g_wireframeModeEnabled)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		g_wireframeModeEnabled = !g_wireframeModeEnabled;
	}
	else if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void processInput(GLFWwindow* window, Input& input)
{
	input.moveForward = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	input.moveBackward = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	input.moveLeft = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	input.moveRight = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);

	double xPos = 0.0, yPos = 0.0;
	glfwGetCursorPos(window, &xPos, &yPos);
	if(!input.mousePosInitialized)
	{
		input.lastMouseX = (float)xPos;
		input.lastMouseY = (float)yPos;
		input.mousePosInitialized = true;
	}

	input.mouseMoveX = (float)xPos - input.lastMouseX;
	input.mouseMoveY = input.lastMouseY - (float)yPos; // inverse
	input.lastMouseX = (float)xPos;
	input.lastMouseY = (float)yPos;

	input.scrollDiff = input.currentScroll - input.lastScroll;
	input.lastScroll = input.currentScroll;
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->currentScroll += (float)yOffset;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to create window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
		std::cout << "failed to load OpenGL functions\n";
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, scrollCallback);

	float vertexData[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	VertexAttribute vertexAttributes[] = {
		VertexAttribute(3, 0),
		VertexAttribute(2, 3)
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	VertexBuffer vertexBuffer(vertexData, 180, GL_STATIC_DRAW, 5, vertexAttributes, 2);
	VertexArray vertexArray(GL_TRIANGLES, 36, false);
	vertexArray.bind();
	vertexBuffer.bind();
	vertexArray.RegisterVertexAttributes(vertexBuffer.attributes, vertexBuffer.nrOfAttributes, 
		vertexBuffer.attributeStride);

	ShaderProgram shaderProgram("Res/Shaders/Vertex.glsl", "Res/Shaders/Fragment.glsl");

	Texture containerTexture("Res/Textures/container.jpg", GL_TEXTURE_2D);
	Texture faceTexture("Res/Textures/awesomeface.png", GL_TEXTURE_2D);

	shaderProgram.use();
	// make sure to use shaderprogram first, before setting any uniforms!
	shaderProgram.setInt("texture1", 0);
	shaderProgram.setInt("texture2", 1);

	glEnable(GL_DEPTH_TEST);

	// camera & input stuff
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	Input input = {0};
	glfwSetWindowUserPointer(window, (void*)&input);
	FpsCamera camera(glm::vec3(0.0f, 0.0f, 3.0f), 45.0f);
	float lastTime = (float)glfwGetTime();
	float deltaTime = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		float currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glfwPollEvents();
		processInput(window, input);
		camera.processInput(input, deltaTime);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vertexArray.bind();
		shaderProgram.use();

		containerTexture.bind(GL_TEXTURE0);
		faceTexture.bind(GL_TEXTURE1);
		
		camera.updateViewMat();
		shaderProgram.setMat4("view", camera.viewMat);
		camera.updateProjMat();
		shaderProgram.setMat4("projection", camera.projMat);

		for (int i = 0; i < 10; ++i) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));

			shaderProgram.setMat4("model", model);
			glDrawArrays(vertexArray.primitiveType, 0, vertexArray.nrOfElements);
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}