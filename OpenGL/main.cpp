#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <map>

#include "stb_image.h"

//My headers
#include "Utilities.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

//Light
glm::vec3 lightPos(1.0f, 1.0f, 1.0f); //Position in world space

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//Deltatime as global because I got linker errors.
double deltaTime = 0.0;
double lastFrame = 0.0;

void updateDeltaTime()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

//Callback function in case of resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Function that will process the inputs, such as keyboard inputs
void processInput(GLFWwindow* window)
{
	//If pressed glfwGetKey return GLFW_PRESS, if not it returns GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	//Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);

}

//Callback function for mouse position inputs
void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	camera.processMouseMovement(xPos, yPos, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(yOffset);
}

GLuint loadCubemap(const std::vector<std::string>& faces, bool verticalFlip)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	stbi_set_flip_vertically_on_load(verticalFlip);

	int width, height, nrChannels;
	for (int i = 0; i < faces.size(); ++i)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


int main()
{
	glfwInit();
	//Specify the version and the OpenGL profile. We are using version 3.3
	//Note that these functions set features for the next call of glfwCreateWindow
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create the window object
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Before calling any OpenGL function we need to initialize GLAD since it manages the function pointers
	//for OpenGL, which are OS-Specific.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Specify the actual window rectangle for renderings.
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Register our size callback funtion to GLFW.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//GLFW will capture the mouse and will hide the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Configure Global OpenGL State
	glEnable(GL_DEPTH_TEST);
	

	//Create Shader Objects
	Shader backpackShader("Shaders/backpack/backpack.vert", "Shaders/backpack/backpack.frag");
	Shader normalVisualizerShader("Shaders/normal_visualizer/normal_visualizer.vert", "Shaders/normal_visualizer/normal_visualizer.frag", "Shaders/normal_visualizer/normal_visualizer.geom");

	Model backpack("Resources/Models/backpack/backpack.obj");


	// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
	GLfloat points[] = 
	{
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};
	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);





	glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

	//The Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//Update deltaTime
		updateDeltaTime();
		//First check the inputs
		processInput(window);
	

		//Rendering commands here
		//Bind our framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		////Render the objects first
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 PV = projection * view;
		//Render the container
		backpackShader.use();
		backpackShader.setMat4("PVM", PV * model);
		backpack.draw(backpackShader);
		//Visualize normals
		normalVisualizerShader.use();
		normalVisualizerShader.setMat4("modelView", view * model);
		normalVisualizerShader.setMat4("projection", projection);
		backpack.draw(normalVisualizerShader);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}