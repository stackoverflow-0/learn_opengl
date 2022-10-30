#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <vector>


class Model
{
private:
    /* data */
    
public:
    unsigned int vertexBuffer;
    unsigned int VAO;

    unsigned int normalBuffer;
    std::vector< glm::vec3 > vertex;
    std::vector< glm::vec2 > texture;
    std::vector< glm::vec3 > norm;
    Model(const char * path);
    ~Model();
    int get_vertex_num() { return vertex.size(); }
};

Model::Model(const char * path)
{
    std::ifstream objfile(path);
    
    std::string info;
    std::getline(objfile,info);
    std::getline(objfile,info);
    std::getline(objfile,info);
    std::string flag;


    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    std::vector<unsigned int> temp_vertex_ind;
    std::vector<unsigned int> temp_uv_ind;
    std::vector<unsigned int> temp_normal_ind;
    while (true) {
        objfile >> flag;
        
        if(flag == "v") {
            glm::vec3 vec ;
            objfile >> vec.x >> vec.y >> vec.z;
            temp_vertices.push_back(vec);
        }
        else if(flag == "vt") {
            glm::vec2 vec ;
            objfile >> vec.x >> vec.y;
            temp_uvs.push_back(vec);
        }
        else if(flag == "vn") {
            glm::vec3 vec;
            objfile >> vec.x >> vec.y >> vec.z;
            temp_normals.push_back(vec);
        }
        else if (flag == "f") {
            unsigned int vertex_ind , texture_ind , norm_ind;
            char t;
            for (int i = 0;i < 3;i ++) {
                objfile >> vertex_ind >> t >> texture_ind >> t >> norm_ind;
                temp_vertex_ind.push_back(vertex_ind-1);
                temp_uv_ind.push_back(texture_ind-1);
                temp_normal_ind.push_back(norm_ind-1);
            }
        }
        if (objfile.eof()) break;
    }
    for(int i = 0;i < temp_vertex_ind.size();i++) {
        vertex.push_back(temp_vertices[temp_vertex_ind[i]]);
    }
    for(int i = 0;i < temp_uv_ind.size();i++) {
        texture.push_back(temp_uvs[temp_uv_ind[i]]);
    }
    for(int i = 0;i < temp_normal_ind.size();i++) {
        norm.push_back(temp_normals[temp_normal_ind[i]]);
    }

    glGenBuffers(1,&vertexBuffer);
    glGenBuffers(1,&normalBuffer);
    

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(glm::vec3), &norm[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0 , (void*)0);
    
}

Model::~Model()
{
    glDeleteBuffers(1,&vertexBuffer);
    glDeleteBuffers(1,&normalBuffer);
    glDeleteVertexArrays(1,&VAO);
}

class Wave {
    private:
    
    public:
    int d;
    std::vector<glm::vec3> vertex;
    unsigned int vertexBuffer;
    unsigned int VAO;
    Wave(int d) : d(d) {
        float delta = 1.0f / d;
        for(int i = 0;i < d ; i++) {
            for(int j = 0;j < d;j++) {
                vertex.push_back(glm::vec3(i * delta, j * delta, 0));
            }
        }
        glGenBuffers(1,&vertexBuffer);
        

        glGenVertexArrays(1,&VAO);
        glBindVertexArray(VAO);


        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 , (void*)0);

    }
    ~Wave() {

    }
};

glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void frame_size_callback(GLFWwindow* window, int w,int h) {
    glViewport(0,0,w,h);
}

float lastX = 0.0, lastY = 0.0;
float yaw = 0.0,pitch = 0.0;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        lastX = xpos;
        lastY = ypos;
        return;
    }
    
    float xoffset = xpos - lastX ;
    float yoffset = lastY - ypos ;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    if(pitch > 89.0f) pitch =  89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    glm::vec3 direction;
    direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
    return;
}


unsigned int draw_mode = GL_TRIANGLES;

void input_process(GLFWwindow* window) {
    static float cd = 0;
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window,true);
    }
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && cd >= 0.1f) {
        draw_mode = (draw_mode == GL_TRIANGLES) ? GL_LINE_LOOP : GL_TRIANGLES;
        cd = 0.0f;
    }
    cd += deltaTime;
    float cameraSpeed = 2.0f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window,GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window,GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
}



unsigned int load_Shader(const char * vertexPath,const char * fragmentPath,const char * geometryPath) {

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        gShaderStream << gShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
        geometryCode = gShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    unsigned int geometryShader;
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    glShaderSource(geometryShader,1,&gShaderCode,NULL);
    glCompileShader(geometryShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glAttachShader(shaderProgram,geometryShader);

    glLinkProgram(shaderProgram);
    return shaderProgram;
}

GLFWwindow* init() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "load model", NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }
    
    glfwMakeContextCurrent(window);
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window,frame_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); 
    return window; 
}

int main() {
    
    auto window = init();

    auto shaderProgram = load_Shader("shader/wave_vshader.vs","shader/fshader.fs","shader/gshader.gs");


    // Model m("model/chair.obj");
    Wave w(100);

    glUseProgram(shaderProgram);

    unsigned int transformLoc = glGetUniformLocation(shaderProgram,"transform");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int sphereIndexLoc = glGetUniformLocation(shaderProgram,"sphere_index");
    unsigned int tLoc = glGetUniformLocation(shaderProgram,"t");

    // unsigned int objectColorLoc = glGetUniformLocation(shaderProgram,"objectColor");
    // unsigned int lightColorLoc = glGetUniformLocation(shaderProgram,"lightColor");
    // unsigned int lightPosLoc = glGetUniformLocation(shaderProgram,"lightPos");
    // unsigned int viewPosLoc = glGetUniformLocation(shaderProgram,"viewPos");

    // glm::vec3 objectColor = glm::vec3(0.8,0.8,0.2);

    // glm::vec3 lightColor = glm::vec3(1.0,1.0,1.0);

    // glm::vec3 lightPos = glm::vec3(10.0,10.0,0.0);

    // glUniform3fv(objectColorLoc,1,&objectColor[0]);
    // glUniform3fv(lightColorLoc,1,&lightColor[0]);
    // glUniform3fv(lightPosLoc,1,&lightPos[0]);

    glm::mat4 view;

    const float radius = 10.0f;
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    
    // glDepthFunc(GL_NOTEQUAL);
    // glDepthFunc(GL_ALWAYS);

    
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  
        glUniform1f(tLoc,currentFrame);

        input_process(window);


        auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // glUniform3fv(viewPosLoc,1,&cameraPos[0]);
        
        
        
        glStencilFunc(GL_ALWAYS, 1, 0xFF); 
        glStencilMask(0xFF); 

        // draw chair
        draw_mode = GL_POINTS;
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::scale(model,glm::vec3(10,10,1));
        glUniform1i(sphereIndexLoc,0);
        // model = glm::rotate(model,10.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(draw_mode, 0, w.vertex.size());


        glfwSwapBuffers(window);
        
    }
    
    glfwTerminate();
    return 0;
}