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


class Sphere {
    public:
    Sphere(float r , int slices, int stacks) : r(r),slices(slices),stacks(stacks) {
        vert = new float[slices * stacks * 6 * 3]();
        for(int i = 0;i < stacks;i++) {
            for(int j = 0;j < slices;j++) {
                push_point(i,j);
                push_point(i+1,j+1);
                push_point(i,j+1);

                push_point(i,j);
                push_point(i+1,j);
                push_point(i+1,j+1);
            }
        }
    }
    void push_point(int i,int j) {
        vert[p++] =   r * sin(i * M_PI / stacks) * cos(2 * j * M_PI / slices);
        vert[p++] = - r * cos(i * M_PI / stacks);
        vert[p++] =   r * sin(i * M_PI / stacks) * sin(2 * j * M_PI / slices);
        
    }

    float * get_vert() { return vert; }
    int get_size() { return slices * stacks * 6 * 3; }
    int get_vnum() { return slices * stacks * 6; }

    private:
    float r;
    int slices;
    int stacks;
    int p = 0;
    float * vert;
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



unsigned int load_Shader(const char * vertexPath,const char * fragmentPath) {

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    

    unsigned int vertexShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    //check compile
    

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);


    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    return shaderProgram;
}

GLFWwindow* init() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    
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
    return window; 
}

int main() {
    
    auto window = init();

    auto shaderProgram = load_Shader("shader/vshader.vs","shader/fshader.fs");

    auto sphere = Sphere(1,20,20);
    unsigned int VBO;
    unsigned int VAO;
    glGenBuffers(1,&VBO);

    glGenVertexArrays(1,&VAO);

    glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.get_size() * sizeof(float), sphere.get_vert(), GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // 绑定节点串
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glUseProgram(shaderProgram);

    unsigned int transformLoc = glGetUniformLocation(shaderProgram,"transform");
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    unsigned int sphereIndexLoc = glGetUniformLocation(shaderProgram,"sphere_index");
    glm::mat4 view;

    const float radius = 10.0f;
    glClearColor(0.0f,0.3f,0.3f,1.0f);
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();// 鼠标/键盘事件处理函数入口
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        input_process(window);


        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glm::mat4 model = glm::mat4(1.0f);

        // draw ball 0 
        glUniform1f(sphereIndexLoc,0);
        model = glm::rotate(model,10.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(draw_mode, 0, sphere.get_vnum());


        //draw ball 1
        glUniform1f(sphereIndexLoc,1);
        model = glm::scale(model,glm::vec3(0.5,0.5,0.5));
        model = glm::translate(model, glm::vec3(
            6.0f * sin( 3.0f * (float)glfwGetTime()) ,
            0,
            6.0f * cos( 3.0f * (float)glfwGetTime()) 
        ));
        model = glm::rotate(model,100.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(draw_mode, 0, sphere.get_vnum());

        //draw ball 2
        glUniform1f(sphereIndexLoc,2);
        model = glm::scale(model,glm::vec3(0.5,0.5,0.5));
        model = glm::translate(model, glm::vec3(
            4.0f * sin( 2.0f * (float)glfwGetTime()) ,
            0,
            4.0f * cos( 2.0f * (float)glfwGetTime()) 
        ));
        model = glm::rotate(model,10.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(draw_mode, 0, sphere.get_vnum());
       

        glfwSwapBuffers(window);

    }
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    glfwTerminate();
    return 0;
}