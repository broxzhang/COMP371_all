#include <iostream>
#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <random>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "Shader.h"
#include "../include/OBJloader.h"
#include "../include/stb_image.h"
//#include <irrklang/include/ "anything" >

using namespace std;
using namespace glm;

const std::string shadowVertex = "shadow.vert.glsl";
const std::string shadowFragment = "shadow.frag.glsl";

const std::string textureVertex = "Texture.vert.glsl";
const std::string textureFragment = "Texture.frag.glsl";

Shader shadow;
Shader texture;

int compileAndLinkShadowShaders(){

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar*  vertexShaderSource = Shader::readFileContents(shadowVertex).c_str();

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSource = Shader::readFileContents(shadowFragment).c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int compileAndLinkSceneShaders()
{


    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar * vertexShaderSource = Shader::readFileContents(textureVertex).c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar * fragmentShaderSource = Shader::readFileContents(textureFragment).c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}




void SetUniformMat4(GLuint shader_id, const char* uniform_name, glm::mat4 uniform_value)
{
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

void SetUniformVec3(GLuint shader_id, const char* uniform_name, glm::vec3 uniform_value)
{
    glUseProgram(shader_id);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, &uniform_value[0]);
}



//***********NOTWORKING**************//
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
{
    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
}

//this method only takes positions as attirbutes
int createMashArrayObject(const glm::vec3* vertexArray, int arraySize)
{
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, arraySize, vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}

//this method takes positions and normals as attirbutes
int createVertexArrayObject(const glm::vec3* vertexArray, int arraySize)
{
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, arraySize, vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
                          3,                   // size
                          GL_FLOAT,            // type
                          GL_FALSE,            // normalized?
                          2 * sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
                          (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, arraySize, vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(1,                    // attribute 1 matches aNormal in Vertex Shader
                          3,                    // size
                          GL_FLOAT,             // type
                          GL_FALSE,             // normalized?
                          2 * sizeof(glm::vec3),     // stride - each vertex contain 1 vec3 (position)
                          (void*)(sizeof(glm::vec3)) // array buffer offset
    );

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}

//this method has a bind-in cube which takes positions, normals, and Texture UVs as attirbutes
int createTexturedCubeVertexArrayObject()
{
    // Cube model w/ position and texture
    float vertexArray[] = {
            // Position           Normal                Texture
            -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
            1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
            1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

            -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
            1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
            1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
            1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

            -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

            1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            1.0f,  1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
            1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

            -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
            1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
            1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
            1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
            -1.0f, -1.0f,  1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

            -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
            1.0f,  1.0f , 1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
            1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
            1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                            // attribute 0 matches aPos in Vertex Shader
                          3,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)0                      // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,                            // attribute 1 matches aNormal in Vertex Shader
                          3,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)(3 * sizeof(float))    // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aTexCoords in Vertex Shader
                          2,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)(6 * sizeof(float))    // array buffer offset
    );
    glEnableVertexAttribArray(2);



    return vertexArrayObject;
}

//this method has a bind-in surface (please rendered as polygon) which takes positions, normals, and Texture UVs as attirbutes
int createTexturedGroundVertexArrayObject()
{
    // Cube model w/ position and texture
    float vertexArray[] = {
            // Position           Normal                Texture
            -25.0f, 0.0f, -60.0f,   0.0f,  1.0f, 0.0f,   0.0f, 0.0f,
            -25.0f, 0.0f,  10.0f,   0.0f,  1.0f, 0.0f,   0.0f, 25.0f,
            25.0f, 0.0f,  10.0f,   0.0f,  1.0f, 0.0f,   25.0f, 25.0f,
            25.0f, 0.0f, -60.0f,   0.0f,  1.0f, 0.0f,   25.0f, 0.0f,
    };

    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);


    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                            // attribute 0 matches aPos in Vertex Shader
                          3,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)0                      // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,                            // attribute 1 matches aNormal in Vertex Shader
                          3,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)(3 * sizeof(float))    // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aTexCoords in Vertex Shader
                          2,                            // size
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride - each vertex contain 1 vec3 (position)
                          (void*)(6 * sizeof(float))    // array buffer offset
    );
    glEnableVertexAttribArray(2);

    return vertexArrayObject;
}


glm::vec3 mashArray[] = {
        glm::vec3(0.0f, 0.0f, -25.0f), // position 1
        glm::vec3(0.0f, 0.0f, 25.0f), // position 2
};

//to bind texture, use this method
GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


//preseted colors, normaly wont need for textures.
//set color to white means only use the texture to render otherwise is only the color
//usage:  SetUniformVec3(shaderProgram, "overrideColor", white);
glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 skyBlue = glm::vec3(0.643f, 0.820f, 0.894f);
glm::vec3 whiteishGray = glm::vec3(0.85f, 0.85f, 0.90f);
glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 n1f1e33 = glm::vec3(0.122f, 0.118f, 0.2f);
glm::vec3 carrot = glm::vec3(0.945f, 0.513f, 0.070f);

glm::mat4 worldMatrix;

//set matrices to idenity matrix
glm::mat4 set0() {
    return glm::mat4(1.0f);
}

//boolean and numerical switch flipper
bool flipper(bool toflip) {
    return !toflip;
}
int flipper(int toflip) {
    if (toflip == 1) {
        return 0;
    }
    else {
        return 1;
    }
}
float flipper(float toflip) {
    if (toflip == 1.0) {
        return 0.0;
    }
    else {
        return 1.0;
    }
}

//to bind model, use this method  CAUTION: cannot bind too complex model
GLuint bindModelVBO(string path, int& vertexCount) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    //read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}

float normalize(float a, float b) {
    return a / b;
}


vec3 linear(vec3 x, vec3 y, float t,float clipTime) {
    t = mod(t, clipTime);
    return x * (1.0f - normalize(t, clipTime)) + y * normalize(t,clipTime);
}

vec3 turnBack(vec3 x, vec3 y, float t, float clipTime) {
    t = mod(t, clipTime);
    return t > clipTime / 2 ?
           x * (1.0f - normalize(t, clipTime)) + y * normalize(t, clipTime) :
           x * (normalize(t, clipTime)) + y * (1 - normalize(t, clipTime));
}
float linear(float x, float y, float t,float clipTime) {
    t = mod(t, clipTime);
    return  x * (1.0f - normalize(t, clipTime)) + y * normalize(t, clipTime);
}


int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Comp371 - Project", NULL, NULL);

    glfwMakeContextCurrent(window);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    //backgroundcolor
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    float lastFrameTime = glfwGetTime();

    // Compile and link shaders here ...
    int shaderProgram = compileAndLinkSceneShaders();
    int shadowShaderProgram = compileAndLinkShadowShaders();


    //TODO 1
    // it is possible to load more objs
    //object path standerd format ./assets/<your file name>
    string path = "./assets/cone.obj";
    //vertices is requires for later glDrawArray() method
    int vertices;
    int coneVao = bindModelVBO(path, vertices);

    //other vaos
    int mashVao = createMashArrayObject(mashArray, sizeof(mashArray));
    int box0Vao = createTexturedCubeVertexArrayObject();
    int groundVao = createTexturedGroundVertexArrayObject();

    //texture path standerd format ./assets/<your file name>
    //it is also possible to load more textures
//    GLuint woodGroundTexture = loadTexture("./assets/woodGround.jpg");
    GLuint woodGroundTexture = loadTexture("./assets/woodGround.jpg");

    GLuint carrotTexture = loadTexture("./assets/carrottexture.jpg");
    GLuint tireTexture = loadTexture("./assets/rubberTire.jpg");
    GLuint copperTexture = loadTexture("./assets/copper.jpg");
    GLuint cobbleTexture = loadTexture("./assets/cobblestone.jpg");
    GLuint iceTexture = loadTexture("./assets/ice.jpg");
    GLuint skyTexture = loadTexture("./assets/skyTexture.jpg");
    GLuint dupplerTexture = loadTexture("./assets/doppler.jpg");
    GLuint marbleTexture = loadTexture("./assets/marble.jpg");
    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
    //trigger of shadow rendering
    float lightSwitch = 0.0f;
    float isShadow = 1.0f;



    float xDisplacement = 0.0f;
    float zDisplacement = 0.0f;
    float clicked = 0.0f;
    float cameraPan = 0.0f;
    float cameraZoom = 0.0f;
    float cameraTilt = 0.0f;
    float constRotationAngle = 20.0f;
    float rotationAngle = 0.0f;
    float cameraHorizontalAngle = 0.0f;
    float animationT = 0.0f;
    float currentRotation;

    glm::mat4 scaleMatrix;
    glm::mat4 basePosMatrix;
    glm::mat4 baseMatrix;
    glm::mat4 translationMatrix;
    glm::mat4 rotationMatrix;
    glm::mat4 baseScaleMatrix;
    mat4 cameraTranslationMatrix = mat4(1.0f);
    mat4 wholeRotationMatrix;
    mat4 reversedRotationMatrix;
    mat4 everythingBefore = scale(mat4(1.0f), vec3(0.5, 0.5, 0.5));
    vec3 cameraPosition = vec3(0.0, 3.0, 0.0);
    vec3 HorizontalPosition;
    vec3 VerticalPosition;
    mat4 animationHoriTranslationMatrix;
    mat4 animationVertTranslationMatrix;
    mat4 animationRotationMatrix;

    vec3 cameraLookAt = vec3(0.0, 0.0, 0.0);

    GLuint ambientLightLocation = glGetUniformLocation(shaderProgram, "lightSwitch");
    GLuint shadowLocation = glGetUniformLocation(shaderProgram, "isShadow");

    GLuint depth_map_texture;
    glGenTextures(1, &depth_map_texture);
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


    GLuint depth_map_fbo;
    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glm::mat4 scaleBox0Matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 posBox0Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 box0Matrix = posBox0Matrix * scaleBox0Matrix;


    float lightAngleOuter = 30.0;
    float lightAngleInner = 20.0;
    // Set light cutoff angles on scene shader
    SetUniform1Value(shaderProgram, "light_cutoff_inner", cos(glm::radians(lightAngleInner)));
    SetUniform1Value(shaderProgram, "light_cutoff_outer", cos(glm::radians(lightAngleOuter)));

    // Set light color on scene shader

    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    SetUniformVec3(shaderProgram, "light_color", lightColor);

    //set the projection type
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f),  // field of view in degrees
                                                  1024.0f / 768.0f,      // aspect ratio
                                                  1.0f, 100.0f);       // near and far (near > 0)
    SetUniformMat4(shaderProgram, "projectionMatrix", projectionMatrix);



    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    while (!glfwWindowShouldClose(window))
    {

        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        clicked += dt;

        animationT += dt;
        if (animationT > 60.0f) {
            animationT = 0.0f;
        }

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;


        scaleMatrix = set0();
        translationMatrix = set0();
        rotationMatrix = set0();
        basePosMatrix = set0();
        baseMatrix = set0();

        glm::vec3 lightPosition = glm::vec3(1.0f, 12.0f, 5.0f); // the location of the light in 3D space
        glm::vec3 lightFocus(0.0, 0.0, -1.0);      // the point in 3D space the light "looks" at
        glm::vec3 lightDirection = normalize(lightFocus - lightPosition);

        float lightNearPlane = 1.0f;
        float lightFarPlane = 180.0f;

        glm::mat4 lightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        glm::mat4 lightViewMatrix = glm::lookAt(lightPosition, lightFocus, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
        SetUniformMat4(shadowShaderProgram, "lightSpaceMatrix", lightSpaceMatrix);
        SetUniformMat4(shaderProgram, "lightSpaceMatrix", lightSpaceMatrix);
        SetUniform1Value(shaderProgram, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderProgram, "light_far_plane", lightFarPlane);
        SetUniformVec3(shaderProgram, "light_position", lightPosition);
        SetUniformVec3(shaderProgram, "light_direction", lightDirection);



        //TODO 2.1
        //animation process
        //                         initial position         destination       time to finish (default unit time is 1.0f)
        HorizontalPosition = linear(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -60.0f), animationT,20);
        animationHoriTranslationMatrix = translate(mat4(1.0f), HorizontalPosition);
        //                init angle  dest     time to finish
        /*currentRotation = linear(0.0, 90.0, animationT );
        animationRotationMatrix = rotate(mat4(1.0f),radians(currentRotation), vec3(0.0f,1.0f,0.0f));*/

        VerticalPosition = turnBack(vec3(0.0f, 0.25f, 0.0f), vec3(0.0f, 1.0f, 0.0f), animationT,2);
        animationVertTranslationMatrix = translate(mat4(1.0f), VerticalPosition);


        glm::vec3 cameraPos = glm::vec3(cameraPan, 4.0f, cameraZoom +6.0f+HorizontalPosition.z);
        mat4 viewMatrix;
        viewMatrix = glm::lookAt(cameraPos,
                                 glm::vec3(cameraPan, cameraTilt + 4.0f, cameraZoom+HorizontalPosition.z),
                                 glm::vec3(0.0f, 1.0f, 0.0f));



        SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);

        SetUniformVec3(shaderProgram, "view_position", cameraPos);

        glUniform1f(ambientLightLocation, lightSwitch);
        glUniform1f(shadowLocation, isShadow);


        //first pass --- render the shadow
        {
            glUseProgram(shadowShaderProgram);
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer

            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);


            glBindVertexArray(groundVao);
            worldMatrix = mat4(1.0f);
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_POLYGON, 0, 4);


            glBindVertexArray(box0Vao);
            translationMatrix = translate(mat4(1.0f), vec3(0, 0, 0));

            //TODO 2.2
            // with animated rotation
            //worldMatrix = translationMatrix * box0Matrix * animationTranslationMatrix * animationRotationMatrix;
            //without animated rotation
            worldMatrix = translationMatrix * box0Matrix * animationHoriTranslationMatrix * animationVertTranslationMatrix;
            baseMatrix = worldMatrix;
            worldMatrix *= scaleMatrix;

            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //draw the ear
            translationMatrix = translate(mat4(1.0f), vec3(-1.2, 0.9, 0));
            scaleMatrix = scale(mat4(1.0f), vec3(0.2, 0.2, 0.1));
            rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0, 1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            translationMatrix = translate(mat4(1.0f), vec3(1.2, 0.9, 0));
            rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0, -1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //draw the horn
            glBindVertexArray(coneVao);
            translationMatrix = translate(mat4(1.0f), vec3(-0.75, 0.8, 0));
            scaleMatrix = scale(mat4(1.0f), vec3(0.2, 0.5, 0.5));
            rotationMatrix = rotate(mat4(1.0f), radians(22.5f), vec3(0.0f, 0, -1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, vertices);
            translationMatrix = translate(mat4(1.0f), vec3(0.75, 0.8, 0));
            rotationMatrix = rotate(mat4(1.0f), radians(22.5f), vec3(0.0f, 0, 1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, vertices);



            glBindVertexArray(box0Vao);
            scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));
            translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 24.9f, 0.0f));
            worldMatrix = translationMatrix * scaleMatrix;
            SetUniformMat4(shadowShaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindVertexArray(0);
            glUseProgram(0);
        }





        //second pass --- render the scene
        {
            glUseProgram(shaderProgram);

            glViewport(0, 0, 1024, 768);

            // Each frame, reset color of each pixel to glClearColor
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //for special camera position
            //mat4 sceneMatrix = cameraTranslationMatrix * viewMatrix;
            //SetUniformMat4(shaderProgram, "viewMatrix", sceneMatrix);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depth_map_texture);
            glActiveTexture(GL_TEXTURE1);


            SetUniform1Value(shaderProgram, "textureSampler", 1);                // Set our Texture sampler to user Texture Unit 0



            glBindVertexArray(groundVao);
            worldMatrix = mat4(1.0f);
            glBindTexture(GL_TEXTURE_2D, cobbleTexture);
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_POLYGON, 0, 4);

            glBindVertexArray(box0Vao);
            translationMatrix = translate(mat4(1.0f), vec3(0, 0, 0));
            scaleMatrix = scale(mat4(1.0f), vec3(1.125, 0.775, 0.7));
            //TODO 2.2
            //with animatated rotation
            //worldMatrix = translationMatrix * box0Matrix * animationTranslationMatrix * animationRotationMatrix;
            //without animated rotation
            worldMatrix = translationMatrix * box0Matrix  * animationHoriTranslationMatrix * animationVertTranslationMatrix;
            baseMatrix = worldMatrix;
            worldMatrix *= scaleMatrix;
            //TODO 3 texture
            glBindTexture(GL_TEXTURE_2D, marbleTexture);
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            //draw the ear
            translationMatrix = translate(mat4(1.0f), vec3(-1.2, 0.9, 0));
            scaleMatrix = scale(mat4(1.0f), vec3(0.2, 0.2, 0.1));
            rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0, 1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            translationMatrix = translate(mat4(1.0f), vec3(1.2, 0.9, 0));
            rotationMatrix = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 0, -1.0f));
            worldMatrix = baseMatrix * translationMatrix *rotationMatrix *scaleMatrix ;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);


            //draw the horn
            glBindVertexArray(coneVao);
            translationMatrix = translate(mat4(1.0f), vec3(-0.75, 0.8, 0));
            scaleMatrix = scale(mat4(1.0f), vec3(0.2, 0.5, 0.5));
            rotationMatrix = rotate(mat4(1.0f), radians(22.5f), vec3(0.0f, 0, -1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, vertices);
            translationMatrix = translate(mat4(1.0f), vec3(0.75, 0.8, 0));
            rotationMatrix = rotate(mat4(1.0f), radians(22.5f), vec3(0.0f, 0, 1.0f));
            worldMatrix = baseMatrix * translationMatrix * rotationMatrix * scaleMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, vertices);


            glBindVertexArray(box0Vao);

            glCullFace(GL_FRONT);
            //TODO 4 skybox texture
            glBindTexture(GL_TEXTURE_2D, cobbleTexture);
            scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 120.0f));
            translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 24.9f, 0.0f));
            worldMatrix = translationMatrix * scaleMatrix;
            SetUniformMat4(shaderProgram, "worldMatrix", worldMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glCullFace(GL_BACK);

            glBindVertexArray(0);
        }



        glfwSwapBuffers(window);

        // Detect inputs
        glfwPollEvents();


        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            if (clicked > 0.5f) {
                lightSwitch = flipper(lightSwitch);
                clicked = 0;
            }

        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        {
            if (clicked > 1.0f) {
                isShadow = flipper(isShadow);
                clicked = 0.0f;
            }
        }
        //TODO 4 possible to change the way of changing camera pos in runtime
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            cameraZoom += dy * dt;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            cameraTilt += dy * dt;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            cameraPan += dx * dt;
        }



    }
    // Shutdown GLFW
    glfwTerminate();
    return 0;
}