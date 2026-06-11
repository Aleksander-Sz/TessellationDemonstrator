#define GLAD_GL_IMPLEMENTATION
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Shader.h"
#include "../Camera.h"
#include "../Light.h"
#include "../Model.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "../Bezier.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMovement = true;
Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool ChangeMeshSignal = false;
bool ChangeShading = false;
bool DisplayControlNet = false;
void processInput(GLFWwindow* window)
{
	static bool OnePressed = false;
	static bool TwoPressed = false;
	static bool ThreePressed = false;
	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.cameraPos += cameraSpeed * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.cameraPos -= cameraSpeed * glm::normalize(camera.cameraFront);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.cameraPos -= cameraSpeed * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.cameraPos += cameraSpeed * glm::normalize(cross(camera.cameraFront, camera.cameraUp));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.cameraPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.cameraPos.y -= cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		if (!OnePressed)
		{
			OnePressed = true;
			ChangeMeshSignal = true;
		}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
		OnePressed = false;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		if (!TwoPressed)
		{
			TwoPressed = true;
			ChangeShading = true;
		}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
		TwoPressed = false;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		if (!ThreePressed)
		{
			ThreePressed = true;
			DisplayControlNet = !DisplayControlNet;
		}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
		ThreePressed = false;

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMovement)
	{
		firstMovement = false;
		lastX = xpos;
		lastY = ypos;
	}
	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	camera.yaw += xOffset;
	camera.pitch -= yOffset;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.zoom -= (float)yoffset;
	if (camera.zoom < 1.0f)
		camera.zoom = 1.0f;
	if (camera.zoom > 45.0f)
		camera.zoom = 45.0f;
}

GLuint LoadTexture(const char* path)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (!data)
	{
		std::cout << "Failed to load: " << path << "\n";
		return 0;
	}

	GLenum format =
		(nrChannels == 4) ? GL_RGBA :
		(nrChannels == 3) ? GL_RGB :
		GL_RED;

	glTexImage2D(GL_TEXTURE_2D, 0, format,
		width, height, 0,
		format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return tex;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// 2. Center the window
	int monitorX, monitorY;
	glfwGetMonitorPos(monitor, &monitorX, &monitorY);
	int windowWidth = 800; // Your desired width
	int windowHeight = 600; // Your desired height

	glfwSetWindowPos(
		window,
		monitorX + (mode->width - windowWidth) / 2,
		monitorY + (mode->height - windowHeight) / 2
	);

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	glViewport(0, 0, fbWidth, fbHeight);
	camera = Camera(fbWidth, fbHeight);

	// Rendering commands here

	Shader ourShader("Shaders/VertexShader.glsl","Shaders/FragmentShaderFlat.glsl");
	Shader tessShader("Shaders/BezierVertexShader.glsl", "Shaders/SurfaceTessellationControlShader.glsl", "Shaders/SurfaceTessellationEvaluationShader.glsl", "Shaders/FragmentShaderFlat.glsl");
	Shader tessShaderPhong("Shaders/BezierVertexShader.glsl", "Shaders/SurfaceTessellationControlShader.glsl", "Shaders/SurfaceTessellationEvaluationShader.glsl", "Shaders/FragmentShader.glsl");
	Shader* chosenTessShader = &tessShader;
	ourShader.use();

	glEnable(GL_DEPTH_TEST);

	// Light properties
	std::vector<Light> lights;
	lights.push_back(Light::DirectionalLight(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f), glm::vec3(1.0f, -0.5f, 0.0f)));
	
	lights.push_back(Light::PointLight(glm::vec3(0.0f, 0.0f, 0.3f), glm::vec3(0.3f), glm::vec3(2.0f, 1.0f, 0.0f)));
	lights.push_back(Light::PointLight(glm::vec3(0.0f, 0.3f, 0.0f), glm::vec3(0.3f), glm::vec3(-2.0f, 1.0f, 0.0f)));
	lights.push_back(Light::SpotLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.7f), camera.cameraPos, camera.cameraFront, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f))));

	BezierSystem bezierSystem;

	// Textures
	unsigned int textures[3];
	glGenTextures(3, textures);
	textures[0] = LoadTexture("Textures/diffuse.png");
	textures[1] = LoadTexture("Textures/height.png");
	textures[2] = LoadTexture("Textures/normals.png");


	glCullFace(GL_NONE);

	int frame = 0;
	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		if (ChangeMeshSignal)
		{
			ChangeMeshSignal = false;
			bezierSystem.ChangeMesh();
		}
		if (ChangeShading)
		{
			ChangeShading = false;
			chosenTessShader = (chosenTessShader == &tessShader) ? &tessShaderPhong : &tessShader;
		}

		//rendering commands here
		ourShader.use();
		glClearColor(0.0f, 0.1f, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.setMat4("view", camera.view());
		ourShader.setMat4("projection", camera.projection());
		chosenTessShader->use();
		chosenTessShader->setInt("numLights", lights.size());
		chosenTessShader->setMat4("view", camera.view());
		chosenTessShader->setMat4("projection", camera.projection());
		lights[lights.size() - 1].UpdateFlashlight(camera); // Update spotlight to follow the camera
		for (int i = 0; i < lights.size(); i++)
		{
			chosenTessShader->setVec3( "lights[" + std::to_string(i) + "].ambient",     lights[i].ambient);
			chosenTessShader->setVec3( "lights[" + std::to_string(i) + "].diffuse",     lights[i].diffuse);
			chosenTessShader->setVec3( "lights[" + std::to_string(i) + "].specular",    lights[i].specular);
			chosenTessShader->setVec3( "lights[" + std::to_string(i) + "].position",    lights[i].position);
			chosenTessShader->setVec3( "lights[" + std::to_string(i) + "].direction",   lights[i].direction);
			chosenTessShader->setFloat("lights[" + std::to_string(i) + "].cutOff",      lights[i].cutOff);
			chosenTessShader->setFloat("lights[" + std::to_string(i) + "].outerCutOff", lights[i].outerCutOff);
			chosenTessShader->setInt(  "lights[" + std::to_string(i) + "].type",        lights[i].type); // 0: directional light, 1: point light, 2: spotlight
			chosenTessShader->setFloat("lights[" + std::to_string(i) + "].constant",    lights[i].constant);
			chosenTessShader->setFloat("lights[" + std::to_string(i) + "].linear",      lights[i].linear);
			chosenTessShader->setFloat("lights[" + std::to_string(i) + "].quadratic",   lights[i].quadratic);
		}
		chosenTessShader->setVec3("viewPos", camera.cameraPos);
		//material properties
		chosenTessShader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		chosenTessShader->setFloat("material.shininess", 32.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);

		chosenTessShader->setInt("material.diffuse", 0);
		chosenTessShader->setInt("material.height", 1);
		chosenTessShader->setInt("material.normal", 2);

		//
		
		if (chosenTessShader == &tessShaderPhong)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		bezierSystem.Draw(*chosenTessShader, camera.cameraPos);
		if (DisplayControlNet)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			bezierSystem.DrawControlNet(ourShader, camera.cameraPos);

		}
		
		glBindVertexArray(0);
		
		//lightColor = glm::vec3(fmod(lastFrame, 5.0f) / 5.0f);
		frame++;
		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}