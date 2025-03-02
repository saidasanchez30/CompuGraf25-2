//pr�ctica 3: Modelado Geom�trico y C�mara Sint�tica.
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
//clases para dar orden y limpieza al c�digo
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
vector<MeshColor*> meshColorList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks (entre mas resolucioon, mejor se ve la esfera)


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


// Pir�mide triangular regular
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


//Piramide triangular equilatera
void CrearPiramideTriangularNegra()
{
	unsigned int indices_piramide_triangular_negra[] = {
	   0, 1, 2, 
	   1, 3, 2,
	   3, 0, 2,  
	   1, 0, 3  
	};

	GLfloat vertices_piramide_triangular_negra[] = {
		// Base (Tri�ngulo equil�tero en el plano XZ)
		-0.5f,  -0.5f,   0.288f,  // V�rtice 0
		 0.5f,  -0.5f,   0.288f,  // V�rtice 1
		 0.0f,   0.366f,   0.0f,   // V�rtice 2
		 0.0f,  -0.5f,  -0.577f,  // V�rtice 3
	};

	Mesh* e_negra = new Mesh();
	e_negra->CreateMesh(vertices_piramide_triangular_negra, indices_piramide_triangular_negra, 12, 12);
	meshList.push_back(e_negra);

}

void CrearPiramideTriangularEquilatera()
{
	float separacion = 0.06f; // Ajusta este valor para cambiar el espacio entre caras

	GLfloat vertices_piramide_triangular_equilatera[] = {
		//Cara 1 - Rosa (Separada en Z+)
	   -0.5f,   -0.5f,     0.288f + separacion,   1.0f,  0.5f,  0.6f,
		0.5f,   -0.5f,     0.288f + separacion,   1.0f,  0.5f,  0.6f,
		0.0f,    0.366f,   0.0f + separacion,     1.0f,  0.5f,  0.6f,

		//Cara 2 - Azul (Separada en X+)
		0.5f + separacion,  -0.5f,     0.288,    0.0f,  0.5f,  1.0f,
		0.0f + separacion,  -0.5f,    -0.577f,   0.0f,  0.5f,  1.0f,
		0.0f + separacion,   0.366f,   0.0f,     0.0f,  0.5f,  1.0f,

		//Cara 3 - Amarillo (Separada en X- y Z-)
		 0.0f - separacion,  -0.5f,   -0.577f - separacion,   1.0f,  1.0f,  0.0f,
		-0.5f - separacion,  -0.5f,    0.288f - separacion,   1.0f,  1.0f,  0.0f,
		 0.0f - separacion,   0.366f,  0.0f - separacion,     1.0f,  1.0f,  0.0f,

		//Base - Verde
		 0.5f,   -0.5f,   0.288f,   0.0f,  1.0f,  0.0f,  
		-0.5f,   -0.5f,   0.288f,   0.0f,  1.0f,  0.0f,  
		 0.0f,   -0.5f,  -0.577f,   0.0f,  1.0f,  0.0f,  

	};
	MeshColor* equilatera = new MeshColor();
	equilatera->CreateMeshColor(vertices_piramide_triangular_equilatera, 72);
	meshColorList.push_back(equilatera);
}

/*
Crear cilindro, cono y esferas con arreglos din�micos vector creados en el Semestre 2023 - 1 : por S�nchez P�rez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//c�lculo del paso interno en la circunferencia y variables que almacenar�n cada coordenada de cada v�rtice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los v�rtices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el c�rculo
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

	//Se generan los indices de los v�rtices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}


//funci�n para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//c�lculo del paso interno en la circunferencia y variables que almacenar�n cada coordenada de cada v�rtice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	//esto dice que el centro ests en el origen
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los v�rtices de la circunferencia del cono
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
	//almacena en una lista de geometria todos los puntos de la geometria
	//meshGeometry gl_trnaguluos_trgunfam en una fiuncion que crea un eje y todos los triangulos comparten un vertice comun
	//el primer vertice que se pone, todos los triangulos lo comparten
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}


//funci�n para crear pir�mide cuadrangular unitaria
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
	//0
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	//1
	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}


int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resoluci�n (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//�ndice 0 en MeshList
	CrearPiramideTriangular();//�ndice 1 en MeshList
	CrearCilindro(5, 1.0f);//�ndice 2 en MeshList       //aqui esta la resolucion del cilindro, entre mas resolucion, mas cilindricop se ve
	CrearCono(25, 2.0f);//�ndice 3 en MeshList
	CrearPiramideCuadrangular();//�ndice 4 en MeshList
	CrearPiramideTriangularNegra(); //indice 5 en meshlist
	CrearPiramideTriangularEquilatera(); //Indice 0 en meshColorList
	CreateShaders();


	/*C�mara se usa el comando: glm::lookAt(vector de posici�n, vector de orientaci�n, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posici�n,
	glm::vec3 vector up,
	GlFloat yaw rotaci�n para girar hacia la derecha e izquierda
	GlFloat pitch rotaci�n para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posici�n inicial est� en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);


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

	glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f); //inicializar Color para enviar a variable Uniform;

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//C�mara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad

		
		//Piramide negra
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));
		//otras transformaciones para el objeto
		//model = glm::scale(model, glm::vec3(0.5f,0.5f,0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//la l�nea de proyecci�n solo se manda una vez a menos que en tiempo de ejecuci�n
		//se programe cambio entre proyecci�n ortogonal y perspectiva
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		color = glm::vec3(0.0f, 0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshList[5]->RenderMesh(); //dibuja cubo y pir�mide triangular

		//******Piramides de colores*******
		//Cara rosa
			//1
		shaderList[1].useShader();
		uniformModel = shaderList[1].getModelLocation();
		uniformProjection = shaderList[1].getProjectLocation();
		uniformView = shaderList[1].getViewLocation();
		uniformColor = shaderList[1].getColorLocation();
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, 0.24f, -4.0f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		//Rotar la figura 
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		//model = glm::rotate(model, glm::radians(34.0f), glm::vec3(1.0f, 0.0f, 0.0f));     //inclina la piramide hacia atras
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f)); 
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//2
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.16f, -0.05f, -3.899f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//3
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, -0.1f, -3.88f));
		model = glm::scale(model, glm::vec3(0.28f,0.28f,0.28f));
			//Rotar la figura 
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(36.0f), glm::vec3(1.0f, 0.0f, 0.0f));     //inclina la piramide hacia atras
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor(); 

			//4
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.167f, -0.05f, -3.905f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//5
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.33f, -0.3576f, -3.8f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//6
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.165f, -0.39f, -3.79f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
			//Rotar la figura 
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(36.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			//7
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, -0.3576f, -3.8f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//8
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.165f, -0.39f, -3.79f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
			//Rotar la figura 
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(36.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			//9
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.34f, -0.3576f, -3.8f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();


		//Cara amarilla
			//2
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.01f, -0.05f, -4.185f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		
			//3
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.095f, -0.099f, -4.065f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(115.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//5
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.005f, -0.3576f, -4.38f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//6
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.091f, -0.40f, -4.27f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(112.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide

		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//7
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.17f, -0.3576f, -4.1f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//8
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.26f, -0.395f, -3.969f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(115.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();


		//Cara azul
			//3
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.098f, -0.099f, -4.047f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(-115.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(-165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//6
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.275f, -0.395f, -3.9455f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(-115.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(-165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		
		
			//7
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.17f, -0.3576f, -4.1f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

			//8
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.11f, -0.39f, -4.22f));
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		//Rotar la figura 
		model = glm::rotate(model, glm::radians(-115.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //inclina la piramide hacia Y
		model = glm::rotate(model, glm::radians(-165.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));     //invierte la piramide
		model = glm::rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));      //inclina la piramide hacia X
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();



		//Cara verde 
			//3
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.0f, -0.3576f, -4.2f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //Mover figura en Y
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		
			//6
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(-0.17f, -0.3576f, -3.9f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //Mover figura en Y
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();
		
			//8
		model = glm::mat4(1.0);
		//Traslaci�n inicial para posicionar en -Z a los objetos
		model = glm::translate(model, glm::vec3(0.17f, -0.3576f, -3.9f));
		model = glm::scale(model, glm::vec3(0.285f, 0.285f, 0.285f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));      //Mover figura en Y
		model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));  //al presionar la tecla Y se rota sobre el eje y
		model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color)); //para cambiar el color del objetos
		meshColorList[0]->RenderMeshColor();

		


		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}


