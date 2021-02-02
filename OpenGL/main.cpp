#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

static constexpr unsigned int SCR_WIDTH = 800;
static constexpr unsigned int SCR_HEIGHT = 600;


//Callback function in case of resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//glViewport(0, 0, width, height);
}

//Function that will process the inputs, such as keyboard inputs
void processInput(GLFWwindow* window)
{
	//If pressed glfwGetKey return GLFW_PRESS, if not it returns GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
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
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
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

	Shader shader1("Shaders/simp_triangle/vertex.vert", "Shaders/simp_triangle/fragment.frag");



	//INIT THE RENDERING DATA
	//Triangle vertices
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};

	unsigned int indices[] =
	{
		0, 3, 1,
		2, 1, 3
	};


	//Generate the VAO and Bind it
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate the VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//Bind the VBO we created to the GL_ARRAY_BUFFER Slot(target buffer type) of OpenGL.
	//Before specifying the data of the generated VBO, we need to bind it to the target buffer type of
	//OpenGL. From that point on any call on GL_ARRAY_BUFFER will effect the VBO we have bound.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Specify the data of the VBO.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Generate the EBO. Note that A VAO stores the glBindBuffer calls when the target is GL_ELEMENT_ARRAY_BUFFER.
	//Meaning that you can store EBO inside the currently bound VAO.
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//Specify the vertex attributes of the currently bound VBO
	//Position Attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//Color Attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//Unbound the VAO, since we stored all the attributes inside it. Whenever we want to render that object we are going to rebind it.
	glBindVertexArray(0);



	//The Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//First check the inputs
		processInput(window);
		
		//Rendering commands here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Before rendering bind the shader program, and VAO.
		shader1.use();
		//Note that we dont need to bind VBO since VAO stores attribute pointer which points to the corresponding VBO's data
		//VAO also stores the EBO corresponding to the object to be drawn.
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		//Before moving on to the next rendering iteration, swap the buffers, and poll the events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	

	glfwTerminate();

	return 0;
}