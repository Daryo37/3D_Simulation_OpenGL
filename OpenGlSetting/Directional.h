#pragma once
#include "Light.h"
class Directional :
    public Light
{
public:
    Directional();
    Directional(GLuint shadowWidth, GLuint shadowHeight,
                     GLfloat red, GLfloat green, GLfloat blue,
                     GLfloat aIntensity, GLfloat dIntensity,
                     GLfloat xDir, GLfloat yDir, GLfloat zDir);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
        GLuint diffuseIntensityLocation, GLuint directionLocation);

    glm::mat4 CalculateLightTransform();


    ~Directional();
private:
    glm::vec3 direction;

};

