#include "Directional.h"

Directional::Directional() : Light()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	
}

Directional::Directional(GLuint shadowWidth, GLuint shadowHeight,
	                     GLfloat red, GLfloat green, GLfloat blue, 
	                     GLfloat aIntensity, GLfloat dIntensity,
	                     GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(shadowWidth,shadowHeight,red,green,blue,aIntensity,dIntensity)
{

	direction = glm::vec3(xDir, yDir, zDir);

	lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f);
}

void Directional::UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
	GLuint diffuseIntensityLocation, GLuint directionLocation)
{
	glUniform3f(ambientColourLocation, colour.x, colour.y, colour.z);
	glUniform1f(ambientIntensityLocation, ambientIntensity);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
}

glm::mat4 Directional::CalculateLightTransform()
{
	return lightProj * glm::lookAt(-direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}




Directional::~Directional()
{

}
