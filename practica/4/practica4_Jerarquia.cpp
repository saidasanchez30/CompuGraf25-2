/*Práctica 4: Modelado Jerárquico.
Se implementa el uso de matrices adicionales para almacenar información de transformaciones geométricas que se quiere
heredar entre diversas instancias para que estén unidas
Teclas de la F y G para mover la colita
Teclas de la U a la P son para mover las llantas
*/
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
#include <gtc\random.hpp>
//clases para dar orden y limpieza al còdigo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z
using std::vector;
//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
Sphere sp = Sphere(0.1, 20, 20); //recibe radio, slices, stacks

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


// Pirámide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(obj1);

}


/*
Crear cilindro y cono con arreglos dinámicos vector creados en el Semestre 2023 - 1 : por Sánchez Pérez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los vértices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el círculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los vértices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}


//función para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//cálculo del paso interno en la circunferencia y variables que almacenarán cada coordenada de cada vértice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los vértices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}


//función para crear pirámide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	vector<unsigned int> piramidecuadrangular_indices = {
		0,3,4,
		3,2,4,
		2,1,4,
		1,0,4,
		0,1,2,
		0,2,4

	};
	vector<GLfloat> piramidecuadrangular_vertices = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramide = new Mesh();
	piramide->CreateMeshGeometry(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramide);
}



void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resolución (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//índice 0 en MeshList
	CrearPiramideTriangular();//índice 1 en MeshList
	CrearCilindro(100, 0.8f);//índice 2 en MeshList
	CrearCono(100, 1.0f);//índice 3 en MeshList
	CrearPiramideCuadrangular();//índice 4 en MeshList
	CreateShaders();


	/*Cámara se usa el comando: glm::lookAt(vector de posición, vector de orientación, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posición,
	glm::vec3 vector up,
	GlFloat yaw rotación para girar hacia la derecha e izquierda
	GlFloat pitch rotación para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posición inicial está en 0,0,1 y ve hacia 0,0,-1.
	*/
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.2f);
	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4
	glm::mat4 modelaux(1.0);//Inicializar matriz de Modelo 4x4 auxiliar para la jerarquía
	glm::mat4 modelaux2;
	glm::mat4 modelaux3;

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//Cámara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		model = glm::mat4(1.0);

		/*
		*
		*
		* Grua con cabina
		*
		*
		*/

		/*
		//Cabina
		modelaux3 = model;
		model = glm::translate(model, glm::vec3(2.0f, 5.0f, -4.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		model = modelaux;

		//Base
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 2.0f, 6.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.75f, 0.8f, 0.8f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMesh(); //dibuja piramide cuadrangular

		model = modelaux;

		//LLanta uno
		model = glm::translate(model, glm::vec3(-2.5f, -1.0f, 3.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla U
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla L todas las llantas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMesh(); //dibuja cilindro


		model = modelaux;
		//Llanta dos
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla I
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla L todas las llantas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMesh(); //dibuja cilindro

		model = modelaux;

		//LLanta tres
		model = glm::translate(model, glm::vec3(3.3f, 4.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));   //rotate de 135° en eje zmodelaux = model;
		modelaux = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 1.0f, 0.0f));   //Mueve con Tecla O
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla L todas las llantas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMesh(); //dibuja cilindro

		model = modelaux;
		//LLanta cuatro
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla P
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 1.0f, 0.0f));  //Mueve con Tecla L todas las llantas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMesh(); //dibuja cilindro

		// Creando el brazo de una grúa
		
		//articulacion1 hasta articulación5 sólo son puntos de rotación o articulación, en este caso no dibujaremos esferas que los representen
		// SE EMPIEZA EL DIBUJO DEL BRAZO
		//articulación 1
		//rotación alrededor de la articulación que une con la cabina
		model = modelaux3;
		model = glm::translate(model, glm::vec3(0.0f, 6.0f, -4.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;                             		//matriz secundario auxiliar temporal para que no se herede la escala
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();
		model = modelaux;


		//primer brazo que conecta con la cabina
		// //Traslación inicial para posicionar en -Z a los objetos
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		//otras transformaciones para el objeto
		model = glm::rotate(model, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo, pirámide triangular, pirámide base cuadrangular
		//meshList[2]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro y cono

		//para descartar la escala que no quiero heredar se carga la información de la matrix auxiliar
		model = modelaux;
		//articulación 2
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();


		model = modelaux;
		//segundo brazo
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		model = modelaux;

		//articulación 3 extremo derecho del segundo brazo
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		// Crear instancias para completar el brazo y  la cabina. Imporante considerar que la cabina es el nodo padre. 
		//La cabina y el brazo deben de estar unidos a la cabina 

		model = modelaux;
		//Brazo 3
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 1.0f, 1.5f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular

		model = modelaux;
		//articulacion 4
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;
		//Canasta
		//model = glm::rotate(model, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.8f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo y pirámide triangular	*/

		/*
		* 
		* 
		* 
		* Jirafa robótica
		* F y G mueven la cola
		* H, J, K, y L mueven las patas delanteras
		* U, I, O y P mueven las patas traseras
		* 
		* 
		* 
		*/
		
		//Cuerpo
		modelaux3 = model;

		model = glm::translate(model, glm::vec3(2.0f, 6.0f, -4.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.5f, 3.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la línea de proyección solo se manda una vez a menos que en tiempo de ejecución
		//se programe cambio entre proyección ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = glm::rotate(model, glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

		model = glm::translate(model, glm::vec3(-2.0f, 0.3f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(19.0f, 21.0f, 16.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.2f, -0.03f, 0.0f));
		//modelaux = model;
		//model = glm::scale(model, glm::vec3(19.0f, 21.0f, 16.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;

		//Creando el cuello que conecta con el cuerpo
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(110.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::translate(model, glm::vec3(2.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(8.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[0]->RenderMesh(); //dibuja cubo
		//meshList[2]->RenderMeshGeometry(); //dibuja las figuras geométricas cilindro y cono
		model = modelaux;

		//Creando cabeza
		model = glm::translate(model, glm::vec3(4.5f, 0.0f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(8.5f, 10.5f, 8.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();
		
		model = modelaux;

		model = glm::translate(model, glm::vec3(0.2f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.6f, 1.5f, 0.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.08f, 0.7f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;

		//Orejas 
		model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::rotate(model, glm::radians(-100.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::translate(model, glm::vec3(1.8f, 1.3f, -0.3f));
		model = glm::scale(model, glm::vec3(0.5f, 1.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry(); //dibuja cono

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry(); //dibuja cono

		model = modelaux;
		model = glm::translate(model, glm::vec3(1.75f, 1.2f, -0.3f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 1.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry(); //dibuja cono

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[3]->RenderMeshGeometry(); //dibuja cono

		model = modelaux;

		//Cuernitos
		model = glm::translate(model, glm::vec3(-0.5f, -0.1f, 0.7f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -7.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.53f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;

		//Pelo
		model = glm::translate(model, glm::vec3(0.87f, -2.5f, -0.4f));
		model = glm::rotate(model, glm::radians(-91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.3f, 0.6f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMeshGeometry(); //dibuja piramide cuadrangular

		model = glm::translate(model, glm::vec3(1.0f, 0.025f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMeshGeometry(); //dibuja piramide cuadrangular

		model = glm::translate(model, glm::vec3(1.0f, 0.028f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMeshGeometry(); //dibuja piramide cuadrangular

		model = glm::translate(model, glm::vec3(1.0f, 0.029f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//color = glm::vec3(1.0f, 0.812f, 0.584f);
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[4]->RenderMeshGeometry(); //dibuja piramide cuadrangular

		model = modelaux;

		//Colita
			//Articulacion de cola
		model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		//modelaux = model;
		//dibujar una pequeña esfera
		//model = glm::scale(model, glm::vec3(2.0f, 2.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		//model = modelaux;

			//Cilindro
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.3f, 4.0f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

			//pelito
		model = glm::translate(model, glm::vec3(-0.0f, -2.2f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.5f, 0.2f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(2.0f, 3.5f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		//Base (más cuerpo para unir las patas)
		model = modelaux3;
		model = glm::translate(model, glm::vec3(2.0f, 6.0f, -4.0f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(3.5f, 3.0f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

		//Patas delanteras
			//Articulacion 1 de pata 1
		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 1.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		modelaux2 = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f)); //Mueve con H
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();
		
		model = modelaux;
		
		model = glm::translate(model, glm::vec3(-0.8f, -1.4f, 0.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 3.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

			//Articulacion 2 de pata 1
		model = glm::translate(model, glm::vec3(0.0f, -1.6f, 0.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.6f, -0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;
		model = modelaux2;

			//Articulacion 1 de pata 2
		model = glm::translate(model, glm::vec3(-0.4f, 0.0f, -2.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		modelaux2 = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion5()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.5f, -1.4f, 0.0f));
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 3.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

			//Articulacion 2 de pata 2
		model = glm::translate(model, glm::vec3(0.0f, -1.6f, 0.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion6()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.6f, -0.8f, 0.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;
		model = modelaux2;


		//Patas traseras
			//Articulacion 1 de pata 1
		model = glm::translate(model, glm::vec3(5.0f, 0.0f, 2.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		modelaux2 = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion7()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.8f, -1.4f, 0.0f));
		model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 3.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

			//Articulacion 2 de pata 1
		model = glm::translate(model, glm::vec3(0.0f, -1.6f, 0.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion8()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.2f, -0.9f, 0.0f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 1.8f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;
		model = modelaux2;

			//Articulacion 1 de pata 2
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -2.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		modelaux2 = model;
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion9()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 1.0f, 0.0f);    //color Amarillo
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.2f, -1.4f, 0.0f));
		model = glm::rotate(model, glm::radians(-5.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 3.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro

		model = modelaux;

			//Articulacion 2 de pata 2
		model = glm::translate(model, glm::vec3(0.0f, -1.6f, 0.0f));
		//model = glm::rotate(model, glm::radians(91.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		//model = glm::translate(model, glm::vec3(3.8f, -8.7f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion10()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		//dibujar una pequeña esfera
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.2f, -0.9f, 0.0f));
		model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));   //rotate de 135° en eje z
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.4f, 2.0f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(1.0f, 0.812f, 0.584f);
		//color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshList[2]->RenderMeshGeometry(); //dibuja cilindro
		

		//(EXTRA) Diseño de manchas y ojos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, 14.2f, -3.5));
		modelaux = model;
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.7));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.55f, -0.8f, -0.7));
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.35f, -1.0f, -0.2));
		model = glm::scale(model, glm::vec3(0.3f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(1.75f, -2.0f, 1.9));
		model = glm::scale(model, glm::vec3(5.4f, 5.4f, 5.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.35f));
		//model = glm::scale(model, glm::vec3(5.4f, 5.4f, 5.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.5f, -1.0f, -0.06f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.9f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 0.5f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(1.0f, 0.5f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.7f, -0.8f, -0.3f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-3.2f, 0.5f, -0.3f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(1.2f, 0.5f, -0.65f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(2.0f,-0.5f, 0.1f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.8f, -0.2f, -0.2f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.6f, 0.0f, 2.5f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.8f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.4f, 0.2f, 0.3f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.3f, -0.2f, -2.8f));
		//model = glm::scale(model, glm::vec3(2.5f, 2.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.05f, 0.2f, 2.4f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 3.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.2f, 0.045f, -0.58f));
		//model = glm::scale(model, glm::vec3(1.2f, 1.2f, 3.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(0.4f, -0.04f, 0.42f));
		//model = glm::scale(model, glm::vec3(1.2f, 1.2f, 3.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.2f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(1.2f, 1.2f, 3.8f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.37f, 0.0f, 0.05f));
		model = glm::scale(model, glm::vec3(1.2f, 1.2f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.25f, 0.02f, 0.035f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();

		model = glm::translate(model, glm::vec3(-0.01f, -0.025f, -0.395f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		color = glm::vec3(0.749f, 0.443f, 0.071f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		sp.render();


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}


