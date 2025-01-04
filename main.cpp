#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>


#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "WindowGL.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Directional.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Model.h"

#include "Skybox.h"
#include "ShadowMap.h"

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform =0,uniformOmniLightPos = 0,uniformFarPlane = 0;
unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;
// Window dimensions
const float toRadians = 3.14159265f / 180.0f;
WindowGL mainWindow;
std::vector<Mesh*> meshList;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

Camera camera;
GLfloat animegirlAngle = 0.0f;
Directional mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

Texture brickTexture;
Texture dirtTexture;
Texture muskTexture;
Texture michelottiTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model animegirl;
Model circus;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//Vertex Shader
static const char* vShader = "Shaders/shader.vert";                                             
//Fragment Shader                                     
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;   
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects(){
	unsigned int indices[] = {
		0,3,1,
		1,3,2,
		2,3,0,
		0,1,2
	};
	GLfloat vertices[]  {
		// x      y     z         u     v      nx    ny    nz
		  -1.0f, -1.0f, -0.6f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		   0.0f,-1.0f,1.0f,     0.5f, 0.0f,   0.0f, 0.0f, 0.0f,
		   1.0f, -1.0f,-0.6f,    1.0f, 0.0f,   0.0f, 0.0f, 0.0f,
		   0.0f, 1.0f, 0.0f,    0.5f, 1.0f,   0.0f, 0.0f, 0.0f
	};


	GLfloat  floorVertices[] = {
		-10.0f,0.0f,-10.0f,   0.0f,0.0f,    0.0f,-1.0f,0.0f,
		 10.0f,0.0f,-10.0f,   10.0f,0.0f,   0.0f,-1.0f,0.0f,
		 -10.0f,0.0f,10.0f,   0.0f,10.0f,   0.0f,-1.0f,0.0f,
		 10.0f,0.0f,10.0f,    10.0f,10.0f,  0.0f, -1.0f,0.0f
	};
	unsigned floorIndices[] = {
		0,2,1,
		1,2,3
	};
	 
	 calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices,indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh ();
	obj2->CreateMesh (vertices, indices, 32, 12);
	meshList.push_back (obj2);
	
	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj5);




}


void CreateShaders() 
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader,fShader);
	shaderList.push_back(*shader1);

	//directionalShadowShader =  Shader();
	directionalShadowShader.CreateFromFiles("Shaders/directional_shadow_map.vert","Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFiles("Shaders/omni_shadow_map.vert","Shaders/omni_shadow_map.geom", "Shaders/directional_shadow_map.frag");
}

void RenderScene()
{

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
	//model = glm::scale(model, glm::vec3(0.4, 0.4, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[0]->RenderMesh();


	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 2.0f, -2.5f));
	//model = glm::scale (model, glm::vec3 (0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dirtTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[1]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, 0.0f, -2.5f));
	//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	muskTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[2]->RenderMesh();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(10.0f, 0.0f, -0.5f));
	model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	michelottiTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[3]->RenderMesh();



	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, -0.0f));
	//model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[4]->RenderMesh();
	
	animegirlAngle += 0.1f;
	if (animegirlAngle > 360.0f)
	{
		animegirlAngle = 0.1f;
	}
	model = glm::mat4(1.0f);
	model = glm::rotate(model, -animegirlAngle * toRadians,glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
	model = glm::rotate(model, 20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	animegirl.RenderModel();


	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, -0.415f, 2.0f));
	model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
	circus.RenderModel();



}

void DirectionalShadowMapPass(Directional* light)
{
	directionalShadowShader.UseShader();
	glm::mat4 lightTransform = light->CalculateLightTransform();
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	directionalShadowShader.SetDirectionalLightTransform(&lightTransform);

	directionalShadowShader.Validate();
	RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light)
{
	omniShadowShader.UseShader();
	//glm::mat4 lightTransform = light->CalculateLightTransform();
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();
	RenderScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass(glm::mat4 viewMatrix,glm::mat4 projectionMatrix)
{

	glViewport(0, 0, 1920, 1080);

	//Clear Window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox.DrawSkybox(viewMatrix, projectionMatrix);


	shaderList[0].UseShader();

	glm::mat4 lightTransform = mainLight.CalculateLightTransform();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();

	

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount,3,0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount,3 + pointLightCount,pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&lightTransform);

	mainLight.GetShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shaderList[0].Validate();
	RenderScene();
}

int main() 
{

	
	mainWindow = WindowGL(1920,1080);
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();
	//Shadow Uv + light color + intensity =Shadow Width Shadow Height R     G    B    + ambientIntensity+ diffuseIntensity X     Y     Z 
	mainLight = Directional(2048, 2048,
		1.0f, 0.53f, 0.3f,
		0.1f, 0.9f,
		-10.0f, -12.0f, 18.5f);


	// ShadowWidth,ShadowHeigth,near value,far value light color + intensity = R     G    B    + ambientIntensity+ diffuseIntensity X     Y     Z  constant,linear,exponent
	pointLights[0] = PointLight(
		1024,1024,
		0.1f,100.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.1f,
		1.0f, 2.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(
		1024,1024,
		0.1f,100.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.4f,
		-4.0f, 3.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
    pointLightCount++;

	// light color + intensity = R     G    B    + ambientIntensity+ diffuseIntensity,position(XYZ),direction(XYZ), constant,linear,exponent,edge
	spotLights[0] = SpotLight(
		1024,1024,
		0.01f,100.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(
		1024,1024,
		0.01f,100.0f,
	    1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, -1.5f, 0.0f,
		-100.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	//Remember to put in order the textures following the vertices you choose in the skyMesh
	/*skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");*/
	skyboxFaces.push_back("Textures/Skybox/yokohama_rt.jpg");
	skyboxFaces.push_back("Textures/Skybox/yokohama_lf.jpg");
	skyboxFaces.push_back("Textures/Skybox/yokohama_up.jpg");
	skyboxFaces.push_back("Textures/Skybox/yokohama_dn.jpg");
	skyboxFaces.push_back("Textures/Skybox/yokohama_bk.jpg");
	skyboxFaces.push_back("Textures/Skybox/yokohama_ft.jpg");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);
	brickTexture = Texture((char*)("Textures/brick.png"));
	brickTexture.LoadTextureA();
	dirtTexture = Texture((char*)("Textures/dirt.png"));
	dirtTexture.LoadTextureA();
	muskTexture = Texture(((char*)"Textures/Hi_Long_Musk.png"));
	muskTexture.LoadTextureA();
	michelottiTexture = Texture(((char*)"Textures/michelotti.png"));
	michelottiTexture.LoadTextureA();
	plainTexture = Texture(((char*)"Textures/plain.png"));
	plainTexture.LoadTextureA();

	shinyMaterial = Material(1.0f, 32.0f);
	dullMaterial = Material(0.3f, 4.0f);

	animegirl = Model();
	animegirl.LoadModel("Models/Katsuragi.fbx");
	circus = Model();
	circus.LoadModel("Models/Katsuragi.fbx");
	
	

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), (GLfloat)mainWindow.getBufferWidth() /mainWindow.getBufferHeight(), 0.1f, 100.0f);

	

	//Loop until window closed
	while (!mainWindow.getShouldClose()) 
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		//Get + Handle user input events
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(),deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		if (mainWindow.getsKeys()[GLFW_KEY_L]) 
		{
			spotLights[0].Toggle();
			mainWindow.getsKeys()[GLFW_KEY_L] = false;

		}
		


		DirectionalShadowMapPass(&mainLight);
		for (size_t i = 0; i < pointLightCount; i++)
		{
			OmniShadowMapPass(&pointLights[i]);
		}
		for (size_t i = 0; i < spotLightCount; i++)
		{
			OmniShadowMapPass(&spotLights[i]);
		}
		RenderPass(camera.calculateViewMatrix(),projection);
		

		glUseProgram(0);

		mainWindow.swapBuffers();
	}




	return 0;
}