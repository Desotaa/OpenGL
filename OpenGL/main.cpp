#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


//TODO Main function is too ugly. Need to encapsulate textures and models later.


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//My headers
#include "Utilities.h"
#include "Shader.h"
#include "Camera.h"

//Light
glm::vec3 lightPos(1.0f, 1.0f, 1.0f); //Position in world space

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//Deltatime as global because I got linker errors.
double deltaTime = 0.0;
double lastFrame = 0.0;

//Loads the texture from the disk and generates it
GLuint loadTexture(const char* path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	//Load the data from the disk
	//nrComponents stores the information about the format of the image.
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//Wrapping Params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//Filtering Params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

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

	glEnable(GL_DEPTH_TEST);

	Shader shader1("Shaders/LightingMaps/lightingMaps.vert", "Shaders/LightingMaps/lightingMaps.frag");
	Shader lightCubeShader("Shaders/LightCube/lightCube.vert", "Shaders/LightCube/lightCube.frag");
	


	//INIT THE RENDERING DATA
	//Cube vertices
	GLfloat vertices[] = { //vertices with normals, and textures
	// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	//Translation vectors for all the cubes
	glm::vec3 cubePositions[] = 
	{
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

	glm::vec3 pointLightPositions[] = 
	{
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	//Cube Object 
	//Generate the VAO and Bind it
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	//Generate the VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);

	//Bind the VBO we created to the GL_ARRAY_BUFFER Slot(target buffer type) of OpenGL.
	//Before specifying the data of the generated VBO, we need to bind it to the target buffer type of
	//OpenGL. From that point on any call on GL_ARRAY_BUFFER will effect the VBO we have bound.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Specify the data of the VBO.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Specify the vertex attributes of the currently bound VBO
	//Position Attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	//Normal Attrib
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
	//Texture Attrib
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));
	
	////Unbound the VAO, since we stored all the attributes inside it. Whenever we want to render that object we are going to rebind it.
	glBindVertexArray(0);

	GLuint diffuseMap = loadTexture("Resources/Textures/container2.png");
	GLuint specularMap = loadTexture("Resources/Textures/container2_specular.png");

	//Set the samplers of fragment shader (Will be done only once no need to do that in the render loop)
	//First activate the program to update uniforms.
	shader1.use();
	shader1.setInt("material.diffuse", 0);
	shader1.setInt("material.specular", 1);
	shader1.setFloat("material.shininess", 32.0f);
	//Diffuse Uniforms
	shader1.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader1.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	shader1.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader1.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	//Point Light Uniforms
	for (int i = 0; i < 4; ++i)
	{
		std::string num = std::to_string(i);
		shader1.setVec3("pointLights[" + num + "].position", pointLightPositions[i]);
		shader1.setVec3("pointLights[" + num + "].ambient", 0.05f, 0.05f, 0.05f);
		shader1.setVec3("pointLights[" + num + "].diffuse", 0.8f, 0.8f, 0.8f);
		shader1.setVec3("pointLights[" + num + "].specular", 1.0f, 1.0f, 1.0f);
		shader1.setFloat("pointLights[" + num + "].constant", 1.0f);
		shader1.setFloat("pointLights[" + num + "].linear", 0.09f);
		shader1.setFloat("pointLights[" + num + "].quadratic", 0.032f);
	}
	//Spotlight uniform
	shader1.setVec3("spotLight.position", camera.getPosition());
	shader1.setVec3("spotLight.direction", camera.getFront());
	shader1.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader1.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader1.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader1.setFloat("spotLight.constant", 1.0f);
	shader1.setFloat("spotLight.linear", 0.09);
	shader1.setFloat("spotLight.quadratic", 0.032);
	shader1.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader1.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


	//Light Cube 
	GLuint lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	
	//No need to create a new VBO since light cube will use the cube data as vertex data. Only attributes will differ.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, 8 * sizeof(GLfloat), (void*)0);


	
	glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

	//The Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//Update deltaTime
		updateDeltaTime();

		//First check the inputs
		processInput(window);
	

		//Rendering commands here
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Before rendering bind the shader program, and VAO.
		shader1.use();


		//Note that we dont need to bind VBO since VAO stores attribute pointer which points to the corresponding VBO's data
		//VAO also stores the EBO corresponding to the object to be drawn.
		glBindVertexArray(VAO);
		//Need to activate all texture units needed, and bind corresponding textures.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glm::mat4 view = camera.getViewMatrix();

		//Projection Matrix
		glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);


		for (int i = 0; i < 10; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)glfwGetTime(), cubePositions[9-i]);
			glm::mat4 PVM = projection * view * model;
			shader1.setMat4("PVM", PVM);
			shader1.setMat4("model", model);
			shader1.setMat3("normalTransformation", glm::transpose(glm::inverse(glm::mat3(model))));
			shader1.setVec3("cameraPos", camera.getPosition());
			//Update camerapos and dir for spotlight
			shader1.setVec3("spotLight.position", camera.getPosition());
			shader1.setVec3("spotLight.direction", camera.getFront());
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		glBindVertexArray(0);
		
		//Render the point lights
		for (int i = 0; i < 4; ++i)
		{
			lightCubeShader.use();
			glm::mat4 lightCubeModelMat = glm::mat4(1.0f);
			lightCubeModelMat = glm::translate(lightCubeModelMat, pointLightPositions[i]);
			lightCubeModelMat = glm::scale(lightCubeModelMat, glm::vec3(0.2f));
			lightCubeShader.setMat4("PVM", projection * view * lightCubeModelMat);
			glBindVertexArray(lightCubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}



		//Before moving on to the next rendering iteration, swap the buffers, and poll the events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}