#include "WindowGL.h"

WindowGL::WindowGL()
{
	width = 800;
	height = 600;
	xChange = 0.0f;
    yChange = 0.0f;
	bufferWidth = 0;
	bufferWidth = 0;
	lastX = 0;
	lastY = 0;
	mainWindow = 0;
	xChange = 0;
	yChange = 0;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
WindowGL::WindowGL(GLint windowWidth, GLint windowHeight )
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}


int WindowGL::Initialise() 
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf( "GLFW Initialisation failed!" );
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	//OpenGL version
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	// Core profile = no Backwards compatibility
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	//Allow forward compatibility
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

    mainWindow = glfwCreateWindow(width,height, "Test window", NULL, NULL);
	if (!mainWindow)
	{
		printf( "GLFW Window creation failed!" );
		glfwTerminate();
		return 1;
	}
	//Get Buff side information
	glfwGetFramebufferSize(mainWindow,&bufferWidth,&bufferHeight);
	//Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	//Handle Key + Mouse Input
	createCallbacks();
	glfwSetInputMode(mainWindow,GLFW_CURSOR,GLFW_CURSOR_DISABLED); 
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error!= GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString( error ) );
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable( GL_DEPTH_TEST );
	//Setup Viewport Size
	glViewport( 0, 0, bufferWidth, bufferHeight );

	glfwSetWindowUserPointer(mainWindow,this);

	return 1;
}

void WindowGL::createCallbacks()
{
	glfwSetKeyCallback(mainWindow,handleKeys);
	glfwSetCursorPosCallback(mainWindow,handleMouse );
}
GLfloat WindowGL::getXChange() 
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}
GLfloat WindowGL::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

 void WindowGL::handleKeys(GLFWwindow* window, int key, int code, int action, int mode )
{
	WindowGL* theWindow = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,GL_TRUE);
	}
	if (key > -0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			
		}
	}
}

 void WindowGL::handleMouse(GLFWwindow* window, double xPos,double yPos)
 {
	 WindowGL* theWindow = static_cast<WindowGL*>(glfwGetWindowUserPointer(window));

	 if (theWindow->mousedFirstMoved)
	 {
		 theWindow->lastX = xPos;
		 theWindow->lastY = yPos;
		 theWindow->mousedFirstMoved = false;
	 }
	 theWindow->xChange = xPos - theWindow->lastX;
	 theWindow->yChange = theWindow->lastY - yPos;

	 theWindow->lastX = xPos;
	 theWindow->lastY = yPos;

	 //printf("x:%.6f,y:%6f\n", theWindow->xChange,theWindow->yChange);
 }


WindowGL::~WindowGL()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}