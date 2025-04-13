/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

//una super clase es cuando las variables son publicas 
//sirve para herencia 

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h" 

     //Luz del escenario, la que ilumina todo el ambiente y es blanca porque deja ver toda la gama de colores
#include "DirectionalLight.h"    //Siempre debe de existir, La numero 0

	 //Superclase tambien
     //Luz de bulbo(foco), ilumina a su alrededor por medio de esfera con cierto alcance
     //Atenuacion lineal (que tan lejos llega a iluminar nuestro foco)
#include "PointLight.h"

	 //Clase hija de 
     //Luz que ilumnia por medio de un cono y le damos una direccion     
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture paredTexture;
Texture AgaveTexture;
Texture octaedroTexturaAnimal;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;
Model Carro;
Model LlantaDelanteraIzq;
Model LlantaDelanteraDer;
Model LlantaTraseraIzq;
Model LlantaTraseraDer;
Model Cofre;
Model Parabrisas;
Model Techo;
Model Parrilla;
Model Lampara;
Model Luciernaga;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
PointLight pointLights1[MAX_POINT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
SpotLight spotLights1[MAX_SPOT_LIGHTS];
SpotLight spotLights2[MAX_SPOT_LIGHTS];


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
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


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz      
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f, 
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	//Dice que las luces apuntan a la direccion contraria a la nomral del plano
	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


//función para crear pirámide cuadrangular unitaria
void CrearOctaedro()
{
	unsigned int octaedro_indices[] = {
		// Cara superior 1
		0, 1, 2,
		// Cara superior 2
		3, 4, 5,
		// Cara superior 3
		6, 7, 8,
		// Cara superior 4
		9, 10, 11,

		// Cara inferior 1
		12, 13, 14,
		// Cara inferior 2
		15, 16, 17,
		// Cara inferior 3
		18, 19, 20,
		// Cara inferior 4
		21, 22, 23,
	};

	GLfloat octaedro_vertices[] = {
		// Vértices         |   Coordenadas     |  Normales
		//x		y		z		S		T			NX		NY		NZ
		// Mitad superior
		0.5f,  0.0f,  0.5f,    0.25f,   0.5f,      0.0f,   -1.0f,   -1.0f,// 0   enfrente jirafaa
		-0.5f, 0.0f,  0.5f,    0.75f,   0.5f,      0.0f,   -1.0f,   -1.0f,// 3
		0.0f,  0.6f,  0.0f,    0.5f,    0.75f,     0.0f,   -1.0f,   -1.0f,// 4  

		-0.5f, 0.0f,  0.5f,    0.99f,   0.74f,      1.0f,  -1.0f,    0.0f,// 3   derecha mapache  
		-0.5f, 0.0f, -0.5f,    0.52f,    0.74f,     1.0f,  -1.0f,    0.0f,// 2
		0.0f,  0.6f,  0.0f,    0.75f,   0.5f,       1.0f,  -1.0f,    0.0f,// 4 

		-0.5f, 0.0f, -0.5f,    0.01f,   0.75f,     0.0f,   -1.0f,    1.0f,// 2   atras  panda
		0.5f,  0.0f, -0.5f,    0.5f,    0.75f,     0.0f,   -1.0f,    1.0f,// 1 
		0.0f,  0.6f,  0.0f,    0.25f,   0.99f,     0.0f,   -1.0f,    1.0f,// 4 

		0.5f,  0.0f, -0.5f,    0.48f,    0.74f,     -1.0f,   -1.0f,    0.0f,// 1   izquierda oso 
		0.5f,  0.0f,  0.5f,    0.01f,   0.74f,      -1.0f,   -1.0f,    0.0f,// 0   
		0.0f,  0.6f,  0.0f,    0.25f,    0.5f,      -1.0f,   -1.0f,    0.0f,// 4 

		// Mitad inferior
		0.5f,  0.0f,  0.5f,    0.27f,   0.49f,     0.0f,   1.0f,    -1.0f,// 0   enfrente  pinguino
		-0.5f, 0.0f,  0.5f,    0.74f,   0.49f,     0.0f,   1.0f,    -1.0f,// 3
		0.0f, -0.6f,  0.0f,    0.5f,    0.25f,     0.0f,   1.0f,    -1.0f,// 5

		-0.5f, 0.0f,  0.5f,    0.98f,   0.26f,     1.0f,  1.0f,     0.0f,// 3   derecha leon
		-0.5f, 0.0f, -0.5f,    0.51f,   0.26f,     1.0f,  1.0f,     0.0f,// 2
		0.0f, -0.6f,  0.0f,    0.75f,   0.5f,      1.0f,  1.0f,     0.0f,// 5

		-0.5f, 0.0f, -0.5f,    0.01f,   0.25f,     0.0f,   1.0f,    1.0f,// 2   atras tigre
		0.5f,  0.0f, -0.5f,    0.5f,    0.25f,     0.0f,   1.0f,    1.0f,// 1 
		0.0f, -0.6f,  0.0f,    0.25f,   0.01f,     0.0f,   1.0f,    1.0f,// 5

		0.5f,  0.0f, -0.5f,    0.5f,    0.25f,     -1.0f,   1.0f,    0.0f,// 1   izquierda Koala  
		0.5f,  0.0f,  0.5f,    0.01f,   0.25f,     -1.0f,   1.0f,    0.0f,// 0   
		0.0f, -0.6f,  0.0f,    0.25f,   0.5f,      -1.0f,   1.0f,    0.0f,// 5


	};

	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices, 192, 24);
	meshList.push_back(octaedro);
}


//Funcion para crear cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20
	};

	GLfloat cubo_vertices[] = {
		// Cara frontal (Z-)
	   -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,    0.0f,  0.0f,  -1.0f,
		0.5f, -0.5f,  0.5f,   1.0f, 0.0f,    0.0f,  0.0f,  -1.0f,
		0.5f,  0.5f,  0.5f,   1.0f, 1.0f,    0.0f,  0.0f,  -1.0f,
	   -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,    0.0f,  0.0f,  -1.0f,

	   // Cara trasera (Z+)
		0.5f, -0.5f, -0.5f,   0.0f, 0.0f,    0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,    0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,   0.0f, 1.0f,    0.0f,  0.0f,  1.0f,

		// Cara derecha (X-)
		 0.5f, -0.5f,  0.5f,   0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,   1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,   1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,   0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,

		 // Cara izquierda (X+)
		 -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		 -0.5f, -0.5f,  0.5f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f,  0.5f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,

		 // Cara superior (Y-)
		 -0.5f,  0.5f,  0.5f,   0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f,  0.5f,   1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
		  0.5f,  0.5f, -0.5f,   1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
		 -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,    0.0f, -1.0f,  0.0f,

		 // Cara inferior (Y+)
		 -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,    0.0f, 1.0f,  0.0f,
		  0.5f, -0.5f, -0.5f,   1.0f, 0.0f,    0.0f, 1.0f,  0.0f,
		  0.5f, -0.5f,  0.5f,   1.0f, 1.0f,    0.0f, 1.0f,  0.0f,
		 -0.5f, -0.5f,  0.5f,   0.0f, 1.0f,    0.0f, 1.0f,  0.0f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CreateShaders();
	CrearOctaedro();//índice 4 en MeshList
	CrearCubo(); //índice 5 de MesjList


	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	paredTexture = Texture("Textures/brick.png");
	paredTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	octaedroTexturaAnimal = Texture("Textures/octaedro.png");
	octaedroTexturaAnimal.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Carro = Model();
	Carro.LoadModel("Models/CarroTexturizado.obj");
	LlantaDelanteraIzq = Model();
	LlantaDelanteraIzq.LoadModel("Models/LlantaDelanteraIzq.obj");
	LlantaDelanteraDer = Model();
	LlantaDelanteraDer.LoadModel("Models/LlantaDelanteraDer.obj");
	LlantaTraseraIzq = Model();
	LlantaTraseraIzq.LoadModel("Models/LlantaTraseraIzq.obj");
	LlantaTraseraDer = Model();
	LlantaTraseraDer.LoadModel("Models/LlantaTraseraDer.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Cofre = Model();
	Cofre.LoadModel("Models/Cofre.obj");
	Parabrisas = Model();
	Parabrisas.LoadModel("Models/Parabrisas.obj");
	Techo = Model();
	Techo.LoadModel("Models/Techo.obj");
	Parrilla = Model();
	Parrilla.LoadModel("Models/Parrilla.obj");
	Lampara = Model();
	Lampara.LoadModel("Models/lampara.obj");
	Luciernaga = Model();
	Luciernaga.LoadModel("Models/luciernaga.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	//Ahorita esta en direccion de -y
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,       //Color de la luz
		0.3f, 0.3f,                         //intensidad ambiental e intensidad(tonalidad) de blanco
		0.0f, 0.0f, 1.0f);                 //Hacia donde apunta el sol
	
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	unsigned int pointLightCount1 = 0;

	//Declaración de primer luz puntual

	//luz puntual (farola)
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,    //Luz blanca
		0.5f, 1.0f,
		-5.2f, 5.0f, -12.3f,
		0.5f, 0.09f, 0.01f);
	pointLightCount++;

	//Luz amarilla (Luciernaga)
	pointLights1[0] = PointLight(1.0f, 1.0f, 0.0f,    //Luz amarilla
		0.5f, 1.0f,                        //Radiacion de la luz y saturacion del color(En cero es menos)
		-3.3f, 10.0f, -5.0,                 //posicion en el escenario
		0.5f, 0.02f, 0.02f);                 //Ecuacion de segundo grado
	pointLightCount1++;

	/*
	pointLights1[0] = pointLights[1];
	pointLightCount1++;
	
	/*
	//Creando arreglo de luces
	pointLights2[0] = PointLight(0.0f, 1.0f, 0.0f,    //Luz Verde
		0.0f, 1.0f,
		-14.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights2[1] = PointLight(0.0f, 0.0f, 1.0f,    //Luz Azul
		0.0f, 1.0f,
		-12.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights2[2] = PointLight(1.0f, 0.0f, 0.0f,    //Luz roja
		0.0f, 1.0f,
		-10.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights2[3] = PointLight(1.0f, 1.0f, 1.0f,    //Luz blanca
		0.0f, 1.0f,
		-8.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	

	//Creando el arreglo 
	pointLights3[0] = pointLights2[3];
	pointLights3[1] = pointLights2[0];
	pointLights3[2] = pointLights2[1];
	pointLights3[3] = pointLights2[2];
	*/
	

	unsigned int spotLightCount = 0;
	unsigned int spotLightCount1 = 0;
	unsigned int spotLightCount2 = 0;

	/*
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,     
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,                  //Valor en el origen
		0.0f, -1.0f, 0.0f,                 //mira hacia abajo
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	
	//luz fija
	spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,    //Luz verde
		1.0f, 2.0f,                         //Radiacion e intencias. Mientras el valor sea pequeño, la luz solo ulimina con mucha fuerza en el centro de ella
		                                          //EL color del centro a su distancia maxima que tan fuerte va a ser
		5.0f, 10.0f, 0.0f,                  //posicion del foco invisible
		0.0f, -5.0f, 0.0f,                  //Hacia donde apunta
		1.0f, 0.0f, 0.0f,                   //Ecuacion de segundo grado que ahorita hace que sea infinita la luz
		15.0f);                             //Angulo, amplitud del cono
	spotLightCount++;                       //El contador de luz se incrementa
	*/
	
	//luz cofre
	spotLights2[0] = SpotLight(1.0f, 0.0f, 1.0f,
		4.0f, 5.0f,
		-5.0f, 10.0f, -10.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount2++;

	//luz frontal
	spotLights[0] = SpotLight(1.0f, 1.0f, 0.0f,
		4.0f, 5.0f,
		-5.0f, 10.0f, -10.0f,
		-1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//luz trasera
	spotLights1[0] = SpotLight(1.0f, 1.0f, 0.0f,
		4.0f, 5.0f,
		40.0f, 10.0f, -10.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount1++;

	
	/*
	//luz fija (Helicoptero)
	spotLights[3] = SpotLight(1.0f, 1.0f, 0.0f,    //Luz amarilla
		1.0f, 2.0f,                         
		0.0f, 5.0f, 6.0f,			
		0.0f, -1.0f, 0.0f,                 
		1.0f, 0.0f, 0.0f,                   
		25.0f);                             
	spotLightCount++;             
	*/

	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	//glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	
	
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		/*
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		
		//Ligada a la camara para (posiciion de la camara y direccion de la camara)
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		*/

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		//shaderList[0].SetPointLights(pointLights, pointLightCount);
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);
		
		
		if (mainWindow.getLampara() == true) {
			shaderList[0].SetPointLights(pointLights, pointLightCount); //Se apaga la lampara 
		}
		else {
			shaderList[0].SetPointLights(pointLights, pointLightCount - 1);
		}
		

		if (mainWindow.getLuciernaga() == true) {
			shaderList[0].SetPointLights(pointLights1, pointLightCount1); //Se apaga la luciernaga 
		}
		else {
			shaderList[0].SetPointLights(pointLights1, pointLightCount1 - 1);
		}


		if (mainWindow.getarticulacion10() == 0.0f) {
			if (mainWindow.getcofre() > 0.0) {
				shaderList[0].SetSpotLights(spotLights2, spotLightCount2);
			}
			else {
				shaderList[0].SetSpotLights(spotLights2, spotLightCount2 - 1);
			}
		}
		else {
			if (mainWindow.getarticulacion10() == 1.0)
				shaderList[0].SetSpotLights(spotLights, spotLightCount);
			else {
				if (mainWindow.getarticulacion10() == 2.0f) {
					shaderList[0].SetSpotLights(spotLights1, spotLightCount1);
				}
			}
		}


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		/*
		//Octaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		octaedroTexturaAnimal.UseTexture();
		meshList[4]->RenderMesh();  //Octaedro
		*/

		//Pared
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, 20.0f, -9.0f));
		model = glm::scale(model, glm::vec3(1.0f, 40.0f, 40.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		paredTexture.UseTexture();
		meshList[5]->RenderMesh();  //Cubo

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, -0.9f, -5.0f));
		//spotLights[2].SetPos(glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -8.0f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -3.0f));
		spotLights[0].SetPos(glm::vec3(0.0f + mainWindow.getmuevex(), 7.0f, -8.0f));
		spotLights2[0].SetPos(glm::vec3(0.0f + mainWindow.getmuevex(), 7.0f, -8.0f));
		spotLights1[0].SetPos(glm::vec3(37.0f + mainWindow.getmuevex(), 7.0f, -8.0f));
		modelaux = model;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro.RenderModel();

		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(-8.4f, 8.1f, 0.5f));
		//spotLights[2].SetPos(glm::vec3(5.0f + mainWindow.getmuevex(), 4.0f, -8.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getcofre()), glm::vec3(-1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Cofre.RenderModel();

		//Parabrisas
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Parabrisas.RenderModel();

		//Techo
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Techo.RenderModel();

		//Parrilla
		//model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.5f, -3.0f));
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Parrilla.RenderModel();

		//Llanta delantera Derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(-11.15f, 3.8f, 7.6f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDelanteraIzq.RenderModel();

		//Llanta Delantera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(-11.15f, 3.8f, -7.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(-1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaDelanteraDer.RenderModel();

		//Llanta Trasera Izquierda
		model = modelaux;
		model = glm::translate(model, glm::vec3(13.0f, 3.8f, -7.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTraseraIzq.RenderModel();

		//Llanta trasera derecha
		model = modelaux;
		model = glm::translate(model, glm::vec3(13.0f, 3.8f, 7.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(-1.0f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		LlantaTraseraDer.RenderModel();
		/*
		//Helicoptero
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 6.0));
		spotLights[3].SetPos(glm::vec3(0.0f + mainWindow.getmuevex(), 5.0f, 6.0));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();
		*/

		//Farola
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, -1.0f, -15.0));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		//Luciernaga
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 10.0f, -5.0));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luciernaga.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
