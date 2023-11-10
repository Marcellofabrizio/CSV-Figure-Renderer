#include <Shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <shader_utils.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

unsigned int VBO, VAO;
unsigned int texture1, texture2;

const int MIN_VERTICES = 0;
const int MAX_VERTICES = 45;
const float MIN_VELOCIDADE = 1.0f;
const float MAX_VELOCIDADE = 10.0f;

int velocidade =1;
int quantosVertices = MAX_VERTICES;
int preenchimento = 1;
int numberOfLines = 0;

int getNumberOfLinesInFile(const std::string &filename)
{
  std::cout << "Reading " << filename << std::endl;

  std::ifstream file(filename);
  std::string _line;
  int lineCount = 0;

  while (std::getline(file, _line))
  {
    lineCount++;
  }

  file.close();
  return lineCount;
}

std::vector<float> readVerticesFromFile(const std::string &filename)
{

  std::cout << "Reading " << filename << std::endl;
  std::vector<float> points;

  std::ifstream file(filename);
  std::string line;

  numberOfLines = getNumberOfLinesInFile(filename);

  if (!file.is_open())
  {
    std::cerr << "Failed to open " << filename << std::endl;
    exit(1);
  }

  int count = 0;
  while (std::getline(file, line))
  {
    float x, y, z, r, g, b, t1, t2;
    std::istringstream iss(line);
    if (count + 1 == numberOfLines)
    {
      std::cout << "Reached end of file" << std::endl;
    }
    else if (sscanf(line.c_str(), "%f %f %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b, &t1, &t2) == 8)
    {
      points.push_back(x);
      points.push_back(y);
      points.push_back(z);
      points.push_back(r);
      points.push_back(g);
      points.push_back(b);
      points.push_back(t1);
      points.push_back(t2);

    }
    else
    {
      std::cerr << "Error parsing line " << line << std::endl;
    }

    count++;
  }

  file.close();

  return points;
}

std::string readTextureLocationFromFile(const std::string &filename)
{

  std::cout << "Reading " << filename << std::endl;

  std::ifstream file(filename);
  std::string line;
  std::string textureFileLocation;

  int numberOfLines = getNumberOfLinesInFile(filename);

  if (!file.is_open())
  {
    std::cerr << "Failed to open " << filename << std::endl;
    exit(1);
  }

  int count = 0;
  while (std::getline(file, line))
  {
    if (count + 1 == numberOfLines)
    {
      std::cout << "Reached end of file" << std::endl;
      break;
    }

    count++;
  }

  file.close();

  return line;
}

int load_texture(const std::string &texture_filename)
{
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // load image, create texture and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(
      true); // tell stb_image.h to flip loaded texture's on the y-axis.
  unsigned char *data =
      stbi_load(texture_filename.c_str(), &width, &height, &nrChannels, 0);
  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  return 1;
}

int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    std::cerr << "Usage: csv_shader <csv-file>";
    return -1;
  }

  std::string fileName = argv[1];

  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(
      SCR_WIDTH, SCR_HEIGHT, "Coordenadas OpenGL : Cubo", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glew: load all OpenGL function pointers
  // ---------------------------------------
  if (glewInit() != GLEW_OK)
  {
    std::cout << "Ocorreu um erro iniciando GLEW!" << std::endl;
  }
  else
  {
    std::cout << "GLEW OK!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  std::vector<float> points = readVerticesFromFile(fileName);
  std::string textureFile = readTextureLocationFromFile(fileName);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("/home/marcello/Repositories/CSV-Shadder/shaders/vertex.glsl", "/home/marcello/Repositories/CSV-Shadder/shaders/fragment.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // float vertices[] = {

    //     // Eixos

    //     // x
    //     -0.95f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    //      0.95f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    //     -0.95f,  0.0f,  0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,

    //     // y
    //      0.0f, -0.95f,  0.0f,   0.0f, 0.5f, 0.5f,   0.0f, 1.0f,
    //      0.0f,  0.95f,  0.0f,   0.0f, 0.5f, 0.5f,   0.0f, 1.0f,
    //      0.0f, -0.95f,  0.0f,   0.0f, 0.5f, 0.5f,   0.0f, 1.0f,

    //     // z
    //      0.0f,  0.0f, -0.95f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    //      0.0f,  0.0f,  0.95f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    //      0.0f,  0.0f, -0.95f,   1.0f, 0.0f, 1.0f,   0.0f, 1.0f,

    //      // Faces
    //     -0.5f, -0.5f, -0.5f,   0.7f, 0.25f, 0.0f,   0.0f, 0.0f,
    //      0.5f, -0.5f, -0.5f,   0.7f, 0.25f, 0.0f,   1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,   0.7f, 0.25f, 0.0f,   1.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,   0.0f, 0.25f, 0.7f,   1.0f, 1.0f,
    //     -0.5f,  0.5f, -0.5f,   0.0f, 0.25f, 0.7f,   0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,   0.0f, 0.25f, 0.7f,   0.0f, 0.0f,

    //     -0.5f, -0.5f,  0.5f,   0.0f, 0.7f, 0.0f,   0.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,   0.0f, 0.7f, 0.0f,   1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,   0.0f, 0.7f, 0.0f,   1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 0.7f,   1.0f, 1.0f,
    //     -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 0.7f,   0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 0.7f,   0.0f, 0.0f,

    //     -0.5f,  0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 1.0f,
    //     -0.5f, -0.5f, -0.5f,   0.7f, 0.0f, 0.7f,   0.0f, 1.0f,
    //     -0.5f, -0.5f,  0.5f,   0.7f, 0.0f, 0.7f,   0.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,   0.7f, 0.0f, 0.7f,   1.0f, 0.0f,

    //      0.5f,  0.5f,  0.5f,   0.7f, 0.0f, 0.7f,   1.0f, 0.0f,
    //      0.5f,  0.5f, -0.5f,   0.7f, 0.0f, 0.7f,   1.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,   0.7f, 0.0f, 0.7f,   0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 0.0f,

    //     -0.5f, -0.5f, -0.5f,   0.7f, 0.7f, 0.7f,   0.0f, 1.0f,
    //      0.5f, -0.5f, -0.5f,   0.7f, 0.7f, 0.7f,   1.0f, 1.0f,
    //      0.5f, -0.5f,  0.5f,   0.7f, 0.7f, 0.7f,   1.0f, 0.0f,
    //      0.5f, -0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 0.0f,
    //     -0.5f, -0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 0.0f,
    //     -0.5f, -0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 1.0f,

    //     -0.5f,  0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   0.0f, 1.0f,
    //      0.5f,  0.5f, -0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 1.0f,
    //      0.5f,  0.5f,  0.5f,   0.0f, 0.7f, 0.7f,   1.0f, 0.0f,
    //      0.5f,  0.5f,  0.5f,   0.7f, 0.0f, 0.7f,   1.0f, 0.0f,
    //     -0.5f,  0.5f,  0.5f,   0.7f, 0.0f, 0.7f,   0.0f, 0.0f,
    //     -0.5f,  0.5f, -0.5f,   0.7f, 0.0f, 0.7f,   0.0f, 1.0f,
    // };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), (const void *)&points[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Manda desenhar somente wireframe
    if(!preenchimento) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);

        // movimenta o modelo 0.8 unidades no eixo y
        model = glm::translate(model,glm::vec3(0.0f,0.8f,0.0f));

        // Gira o modelo ao redor do eixo y
        model = glm::rotate(model, (float)glfwGetTime() / velocidade, glm::vec3(0.0f, -0.5f, 0.0f));

        // afasta o objetoo do observador e o coloca um pouco abaixo dele,
        // para um ponto de vista mais elevado do objeto
        view  = glm::translate(view, glm::vec3(0.0f, -0.7f, -3.0f));

        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("projection", projection);

        // render box
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 420);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        //glfwWaitEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        if(quantosVertices>MIN_VERTICES) {
            quantosVertices--;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        if(quantosVertices<MAX_VERTICES) {
            quantosVertices++;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_KP_MULTIPLY) == GLFW_PRESS) {
        if(velocidade>MIN_VELOCIDADE) {
            velocidade--;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_KP_DIVIDE) == GLFW_PRESS) {
        if(velocidade<MAX_VELOCIDADE) {
            velocidade++;
        }
    }


     if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if(preenchimento) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        preenchimento = !preenchimento;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

