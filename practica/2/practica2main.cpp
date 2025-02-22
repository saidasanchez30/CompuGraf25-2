//Práctica 2: índices, mesh, proyecciones, transformaciones geométricas
#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
//clases para dar orden y limpieza al código
#include"Mesh.h"
#include"Shader.h"
#include"Window.h"

//variable exterm: se utilizan para ejercicios, no para reporte
//Dimensiones de la ventana

//Son listas y puedes extraer elementos de donde sea su ubicación con llamar al indice del dato.
const float toRadians = 3.14159265f / 180.0; //grados a radianes
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<MeshColor*> meshColorList;
std::vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";
static const char* vShaderVerde = "shaders/shaderverde.vert";
static const char* fShaderVerde = "shaders/shaderverde.frag";
static const char* vShaderVerdeOscuro = "shaders/shaderverdeoscuro.vert";
static const char* vShaderCafe = "shaders/shadercafe.vert";
static const char* vShaderRojo = "shaders/shaderrojo.vert";
static const char* vShaderAzul = "shaders/shaderazul.vert";
//shaders nuevos se crearían acá
//shader recibe dos variables de entrada 
//shader color solo recibe una variable de entrada
//frag recibe color y manda color
//Vertex uno recibe 2 in, agarra uno y lo manda. El otro recibe 1 in, lo convierte en dato de color y lo manda para la interpolación

float angulo = 0.0f;

//color café en RGB : 0.478, 0.255, 0.067

//Pirámide triangular regular
//Crearla a partir de índices. 
void CreaPiramide()
{
	//primero se declaran las caras y luego los indices
	unsigned int indices[] = {
		0,1,2,
		1,3,2,
		3,0,2,
		1,0,3

	};
	GLfloat vertices[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	//12 vertices, 12 indices
	obj1->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj1);
}

//Vértices de un cubo
void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f

	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}


void CrearLetrasyFiguras()
{
	//triangulo que se manda a meshcolor 3x6=18 triangulo azul
	//0
	GLfloat vertices_letras[] = {
		//X			Y			Z			R		G		B
		-0.7f,	-0.3f,		0.0f,			0.0f,	0.0f,	1.0f,
		0.7f,	-0.3f,		0.0f,			0.0f,	0.0f,	1.0f,
		0.0f,	0.3f,		0.0f,			0.0f,	0.0f,	1.0f,
		/*1.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		-1.0f,  1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,*/

	};
	MeshColor* letras = new MeshColor();
	letras->CreateMeshColor(vertices_letras, 18);
	meshColorList.push_back(letras);

	//1
	GLfloat vertices_triangulorojo[] = {
		//X			Y			Z			R		G		B
		-1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		1.0f,	-1.0f,		0.5f,			1.0f,	0.0f,	0.0f,
		0.0f,	1.0f,		0.5f,			1.0f,	0.0f,	0.0f,

	};
	MeshColor* triangulorojo = new MeshColor();
	triangulorojo->CreateMeshColor(vertices_triangulorojo, 18);
	meshColorList.push_back(triangulorojo);

	//2
	GLfloat vertices_trianguloverde[] = {
		//X			Y			Z			R		G		B
		-0.15f,	-0.35f,		0.0f,			0.0f,	0.5f,	0.0f,
		0.15f,	-0.35f,		0.0f,			0.0f,	0.5f,	0.0f,
		0.0f,	0.0f,		0.0f,			0.0f,	0.5f,	0.0f,

	};
	MeshColor* trianguloverde = new MeshColor();
	trianguloverde->CreateMeshColor(vertices_trianguloverde, 18);
	meshColorList.push_back(trianguloverde);

	//3
	GLfloat vertices_cuadradorojo[] = {
		//X			Y			Z			R		G		B
		-0.55f,	-0.55f,		0.0f,			1.0f,	0.0f,	0.0f,
		0.55f,	-0.55f,		0.0f,			1.0f,	0.0f,	0.0f,
		0.55f,	0.55f,		0.0f,			1.0f,	0.0f,	0.0f,
		-0.55f,	-0.55f,		0.0f,			1.0f,	0.0f,	0.0f,
		0.55f,	0.55f,		0.0f,			1.0f,	0.0f,	0.0f,
		-0.55f,	0.55f,		0.0f,			1.0f,	0.0f,	0.0f,

	};
	MeshColor* cuadradorojo = new MeshColor();
	cuadradorojo->CreateMeshColor(vertices_cuadradorojo, 36);
	meshColorList.push_back(cuadradorojo);

	//4
	GLfloat vertices_cuadradoverde[] = {
		//X			Y			Z			R		G		B
		-0.2f,	-0.2f,		0.0f,			0.0f,	1.0f,	0.0f,
		0.2f,	-0.2f,		0.0f,			0.0f,	1.0f,	0.0f,
		0.2f,	0.2f,		0.0f,			0.0f,	1.0f,	0.0f,
		-0.2f,	-0.2f,		0.0f,			0.0f,	1.0f,	0.0f,
		0.2f,	0.2f,		0.0f,			0.0f,	1.0f,	0.0f,
		-0.2f,	0.2f,		0.0f,			0.0f,	1.0f,	0.0f,

	};
	MeshColor* cuadradoverde = new MeshColor();
	cuadradoverde->CreateMeshColor(vertices_cuadradoverde, 36);
	meshColorList.push_back(cuadradoverde);

	//5
	GLfloat vertices_cuadradocafe[] = {
		//X			Y			Z			R		G		B
		-0.1f,	-0.1f,		0.5f,			0.478f, 0.255f, 0.067f,
		0.1f,	-0.1f,		0.5f,			0.478f, 0.255f, 0.067f,
		0.1f,	0.1f,		0.5f,			0.478f, 0.255f, 0.067f,
		-0.1f,	-0.1f,		0.5f,			0.478f, 0.255f, 0.067f,
		0.1f,	0.1f,		0.5f,			0.478f, 0.255f, 0.067f,
		-0.1f,	0.1f,		0.5f,			0.478f, 0.255f, 0.067f,

	};
	MeshColor* cuadradocafe = new MeshColor();
	cuadradocafe->CreateMeshColor(vertices_cuadradocafe, 36);
	meshColorList.push_back(cuadradocafe);

	//6
	GLfloat vertices_s[] = {
		//X			Y		   Z  		 R		  G		    B
		-0.833f,  0.332f,    0.0f,      1.0f,   0.655f,   0.937f, // S
		- 0.415f,  0.5f,      0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.5f,      0.0f,		1.0f,   0.655f,   0.937f,

		- 0.833f,  0.332f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f,  0.332f,    0.0f,     1.0f,   0.655f,   0.937f,
		- 0.415f,  0.5f,      0.0f,		1.0f,   0.655f,   0.937f,

		- 0.833f,  0.166f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.332f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.833f,  0.332f,    0.0f,		1.0f,   0.655f,   0.937f,

		- 0.833f,  0.166f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.166f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.332f,    0.0f,		1.0f,   0.655f,   0.937f,
		
		- 0.583f,  0.0f,      0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.166f,    0.0f,		1.0f,   0.655f,   0.937f,
		- 0.833f,  0.166f,    0.0f,		1.0f,   0.655f,   0.937f,

		- 0.583f,  0.0f,     0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f,  0.0f,     0.0f,		1.0f,   0.655f,   0.937f,
		- 0.666f,  0.166f,   0.0f,		1.0f,   0.655f,   0.937f,

		- 0.583f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f,  0.0f,     0.0f,		1.0f,   0.655f,   0.937f,
		- 0.583f,  0.0f,     0.0f,		1.0f,   0.655f,   0.937f,

		- 0.583f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f,  0.0f,     0.0f,		1.0f,   0.655f,   0.937f,

		- 0.833f, -0.332f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.833f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,

		- 0.833f, -0.332f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.583f, -0.332f,   0.0f,		1.0f,   0.655f,   0.937f,
		- 0.415f, -0.166f,   0.0f,		1.0f,   0.655f,   0.937f,
	};
	MeshColor* saida = new MeshColor();
	saida->CreateMeshColor(vertices_s, 180);
	meshColorList.push_back(saida);


	//7
	GLfloat vertices_m[] = {
		//X			Y		   Z  		 R		   G	     B
		-0.332f,  -0.332f,    0.0f,    0.761f,   0.878f,   0.961f,    //M
		-0.166f,   0.5f,      0.0f,	   0.761f,   0.878f,   0.961f,
		-0.332f,   0.5f,      0.0f,    0.761f,   0.878f,   0.961f,

		-0.332f,  -0.332f,    0.0f,    0.761f,   0.878f,   0.961f,
		-0.166f,  -0.332f,    0.0f,	   0.761f,   0.878f,   0.961f,
		-0.166f,   0.5f,      0.0f,    0.761f,   0.878f,   0.961f,

		-0.166f,   0.249f,    0.0f,    0.761f,   0.878f,   0.961f,
		-0.0f,     0.249f,    0.0f,	   0.761f,   0.878f,   0.961f,
		-0.166f,   0.5f,      0.0f,    0.761f,   0.878f,   0.961f,

		-0.166f,   0.249f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.0f,      0.0f,      0.0f,    0.761f,   0.878f,   0.961f, 
		0.0f,      0.249f,    0.0f,    0.761f,   0.878f,   0.961f,

		0.0f,      0.0f,      0.0f,    0.761f,   0.878f,   0.961f,
		0.166f,    0.249f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.0f,      0.249f,    0.0f,    0.761f,   0.878f,   0.961f,

		0.0f,      0.249f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.166f,    0.249f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.166f,    0.5f,      0.0f,    0.761f,   0.878f,   0.961f,

		0.166f,   -0.332f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.332f,    0.5f,      0.0f,    0.761f,   0.878f,   0.961f,
		0.166f,    0.5f,      0.0f,    0.761f,   0.878f,   0.961f,

		0.166f,   -0.332f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.332f,   -0.332f,    0.0f,    0.761f,   0.878f,   0.961f,
		0.332f,    0.5f,      0.0f,    0.761f,   0.878f,   0.961f,
	};
	MeshColor* mayela = new MeshColor();
	mayela->CreateMeshColor(vertices_m, 144);
	meshColorList.push_back(mayela);

	//8
	GLfloat vertices_c[] = {
		//X			Y		  Z     	 R		   G	    B
		0.415f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f, // C
		0.833f,   0.5f,      0.0f,     0.702f,   0.91f,   0.694f,
		0.583f,   0.5f,      0.0f,     0.702f,   0.91f,   0.694f,

		0.415f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.833f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.833f,   0.5f,      0.0f,     0.702f,   0.91f,   0.694f,

		0.415f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.583f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.415f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f,

		0.415f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.583f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.583f,   0.332f,    0.0f,     0.702f,   0.91f,   0.694f,

		0.583f,  -0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.833f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.415f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,

		0.583f,  -0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.833f,  -0.332f,    0.0f,     0.702f,   0.91f,   0.694f,
		0.833f,  -0.166f,    0.0f,     0.702f,   0.91f,   0.694f,
	};
	MeshColor* calvillo = new MeshColor();
	calvillo->CreateMeshColor(vertices_c, 108);
	meshColorList.push_back(calvillo);
}

//Recibe los archivos, los compila y con el push los agrega en la lista
void CreateShaders()
{
	//0
	Shader* shader1 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	//1
	Shader* shader2 = new Shader();//shader para usar color como parte del VAO: triangulo azul
	shader2->CreateFromFiles(vShaderColor, fShaderColor);
	shaderList.push_back(*shader2);

	//2
	Shader* shader3 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide color verde
	shader3->CreateFromFiles(vShaderVerde, fShader);
	shaderList.push_back(*shader3);

	//3
	Shader* shader4 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide color verde
	shader4->CreateFromFiles(vShaderVerdeOscuro, fShader);
	shaderList.push_back(*shader4);

	//4
	Shader* shader5 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide color verde
	shader5->CreateFromFiles(vShaderCafe, fShader);
	shaderList.push_back(*shader5);

	//5
	Shader* shader6 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide color verde
	shader6->CreateFromFiles(vShaderRojo, fShader);
	shaderList.push_back(*shader6);

	//6
	Shader* shader7 = new Shader(); //shader para usar índices: objetos: cubo y  pirámide color verde
	shader7->CreateFromFiles(vShaderAzul, fShader);
	shaderList.push_back(*shader7);
}


int main()
{
	mainWindow = Window(800, 800);
	mainWindow.Initialise();
	CreaPiramide(); //índice 0 en MeshList
	CrearCubo();//índice 1 en MeshList
	CrearLetrasyFiguras(); //usa MeshColor, índices en MeshColorList
	CreateShaders();
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

	/*Projection: Matriz de Dimensión 4x4 para indicar si vemos en 2D(orthogonal) o en 3D) perspectiva
	proyeccion centrada en el origen
	near(mayor a 0.1) y far (arriba de 100.0) mientras los Vertices en z este en ese rango, y lo vas a poder ver*/
	//glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//Model: Matriz de Dimensión 4x4 en la cual se almacena la multiplicación de las transformaciones geométricas.
	glm::mat4 model(1.0); //fuera del while se usa para inicializar la matriz con una identidad

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		//Recibir eventos del usuario
		glfwPollEvents();
		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad

		//Para las letras hay que usar el segundo set de shaders con índice 1 en ShaderList 
		/*//S
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//Considerar qiue dentro de meshColorList ya se encontraban los colores de la casa
		meshColorList[6]->RenderMeshColor();

		//M
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[7]->RenderMeshColor();

		//C 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshColorList[8]->RenderMeshColor();*/


		///////////////////////////////////////////////////
		//Para el cubo y la pirámide se usa el primer set de shaders con índice 0 en ShaderList
		//Cubo rojo
		shaderList[5].useShader();
		uniformModel = shaderList[5].getModelLocation();
		uniformProjection = shaderList[5].getProjectLocation();
		angulo += 0.02; //Cambia la velocidad de la rotacion
		//Inicializar matriz de dimensión 4x4 que servirá como matriz de modelo para almacenar las transformaciones geométricas
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		//Primero se traslada y luego rota para que el objeto se mueva en su propio centro.
		model = glm::translate(model, glm::vec3(0.0f, -0.3f, -4.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		//Piramide azul
		shaderList[6].useShader();
		uniformModel = shaderList[6].getModelLocation();
		uniformProjection = shaderList[6].getProjectLocation();
		model = glm::mat4(1.0);
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 0.7f, -4.0f)); //piramide
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f)); //rotar en el eje y
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		//Cubo verde
		shaderList[2].useShader();
		uniformModel = shaderList[2].getModelLocation();
		uniformProjection = shaderList[2].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		//Primero se traslada y luego rota para que el objeto se mueva en su propio centro.
		model = glm::translate(model, glm::vec3(0.25f, -0.1f, -3.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.25f, -0.1f, -3.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -0.56f, -3.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		//Piramide Verde Oscuro
		shaderList[3].useShader();
		uniformModel = shaderList[3].getModelLocation();
		uniformProjection = shaderList[3].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.95f, -0.35f, -4.0f)); 
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.95f, -0.35f, -4.0f)); //piramide
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[0]->RenderMesh();

		//Cubo cafe
		shaderList[4].useShader();
		uniformModel = shaderList[4].getModelLocation();
		uniformProjection = shaderList[4].getProjectLocation();
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.95f, -0.70f, -4.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.95f, -0.7f, -4.0f));
		model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		meshList[1]->RenderMesh();

		glUseProgram(0);
		mainWindow.swapBuffers();

	}
	return 0;
}


// inicializar matriz: glm::mat4 model(1.0);
// reestablecer matriz: model = glm::mat4(1.0);
//Traslación
//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
//////////////// ROTACIÓN //////////////////
//model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
////////////////  ESCALA ////////////////
//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
///////////////////// T+R////////////////
/*model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 1.0f, 0.0f));
*/
/////////////R+T//////////
/*model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
model = glm::translate(model, glm::vec3(valor, 0.0f, 0.0f));
*/