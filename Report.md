## Assignment 4 Report

------

> 陈子航 3200100515

### #1 生成球体三角面集合

```c++
class Sphere {
    public:
    Sphere(float r , int slices, int stacks) : r(r),slices(slices),stacks(stacks) {
        // slices为经线方向分割数量 , stacks为纬线方向分割数量
        vert = new float[slices * stacks * 6 * 3]();
        for(int i = 0;i < stacks;i++) {
            for(int j = 0;j < slices;j++) {
                // 将经纬球的四边形面分割为两个三角面存入数组
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
	...
    private:
	...
};
```

### #2 绑定buffer

```c++
// 将顶点数组绑定到buffer
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sphere.get_size() * sizeof(float), sphere.get_vert(), GL_STATIC_DRAW);
// 给定数组数据读取规则 
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

### #3 创建ShaderProgram

顶点着色器

```GLSL
#version 330 core
// 根据数据解释规则1获得的输入数据aPos , 对应顶点的坐标
layout (location = 0) in vec3 aPos;

out vec4 pos;
// model变换矩阵 , view变换矩阵 , 透视投影矩阵
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 变换矩阵乘法
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
    pos = gl_Position;
}
```

片段着色器

```GLSL
#version 330 core
out vec4 FragColor;

in vec4 pos;
// 根据球体编号选择不同的绘制颜色
uniform int sphere_index;
void main()
{
    switch (sphere_index){
        case 0 :
        FragColor = vec4( 1.0, 0.0 ,0.0,1.0);
        break;
        case 1 :
        FragColor = vec4( 0.0, 1.0 ,0.0,1.0);
        break;
        case 2 :
        FragColor = vec4( 0.0, 0.0 ,1.0,1.0);
        break;
        case 3 :
        FragColor = vec4( 0.0, 1.0 ,1.0,1.0);
        break;
        default:
        FragColor = vec4( 1.0, 1.0 ,1.0,1.0);
    }
}
```

使用glCreateShader创建着色器 , glShaderSource添加着色器代码 , glCompileShader编译着色器 , 最后使用glAttachShader和glLinkProgram构建一个着色程序.

### #4 绘制图形

```c++
// ----- draw sun ----------
glm::mat4 model = glm::mat4(1.0f);
// 向着色器写入球体编号
glUniform1i(sphereIndexLoc,0);
// 自转
model = glm::rotate(model,10.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
// 向着色器程序写入model变换矩阵
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
// 绘制buffer
glDrawArrays(draw_mode, 0, sphere.get_vnum());

// ----- draw planet 1 -----
glUniform1i(sphereIndexLoc,1);
auto model_p0 = glm::mat4(1.0f);
// 将星球1缩小至0.5倍
model_p0 = glm::scale(model_p0,glm::vec3(0.5,0.5,0.5));
// 星球1的平移变换 , 随时间在xz平面绕原点做圆周运动
model_p0 = glm::translate(model_p0, glm::vec3(
    6.0f * sin( 4.0f * (float)glfwGetTime()) ,
    0,
    6.0f * cos( 4.0f * (float)glfwGetTime()) 
));
// 自转
model_p0 = glm::rotate(model_p0,100.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_p0));
glDrawArrays(draw_mode, 0, sphere.get_vnum());

//draw planet 2
glUniform1i(sphereIndexLoc,2);
auto model_p1 = glm::mat4(1.0f);
// 将星球2缩小0.5倍
model_p1 = glm::scale(model_p1,glm::vec3(0.5,0.5,0.5));
// 绕z轴旋转30°
model_p1 = glm::rotate(model_p1, glm::radians(30.0f),glm::vec3(0,0,1));
// 圆周运动的平面偏转了30°
model_p1 = glm::translate(model_p1, glm::vec3(
    12.0f * sin( 2.0f * (float)glfwGetTime()) ,
    0,
    12.0f * cos( 2.0f * (float)glfwGetTime()) 
));
// 自转
model_p1 = glm::rotate(model_p1,100.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_p1));
glDrawArrays(draw_mode, 0, sphere.get_vnum());

//draw satellite 1
glUniform1i(sphereIndexLoc,3);
// 在星球2 model变换的基础上 , 再缩小0.5倍
auto model_s0 = glm::scale(model_p1,glm::vec3(0.5,0.5,0.5));
// 绕星球2的y轴做圆周运动
model_s0 = glm::translate(model_s0, glm::vec3(
    4.0f * sin( 2.0f * (float)glfwGetTime()) ,
    0,
    4.0f * cos( 2.0f * (float)glfwGetTime()) 
));
// 自转
model_s0 = glm::rotate(model_s0,10.0f * glm::radians((float)glfwGetTime()),glm::vec3(0,1,0));
glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_s0));
glDrawArrays(draw_mode, 0, sphere.get_vnum());
```

```c++
while(!glfwWindowShouldClose(window)) {
    // 清除缓冲区 , 由于涉及z轴 , 需要清除depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 处理注册的call back函数
    glfwPollEvents();
    // 获得前后帧的时间
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;  
	// 处理键盘输入
    input_process(window);
    // 获得view矩阵
    auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); 
    // 获得投影矩阵
    auto projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    // 向着色器程序写入矩阵值
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	/*  ---绘制部分见上---
		...
    */
    glfwSwapBuffers(window);
}
```

### #5 鼠标call back

```c++
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// 初始化位置
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    // 按住右键固定视角
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        lastX = xpos;
        lastY = ypos;
        return;
    }
    float xoffset = xpos - lastX ;
    float yoffset = lastY - ypos ;
    lastX = xpos;
    lastY = ypos;
	// 鼠标移动灵敏度
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    // 限制视角范围
    if(pitch > 89.0f) pitch =  89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    // 计算方向向量
    glm::vec3 direction;
    direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    // 正则化
    cameraFront = glm::normalize(direction);
    return;
}
```

### #6  操作

wasd控制视角移动 , space或up控制视角向上移动 , down控制向下移动 , 鼠标右键固定视角 , 左键切换绘图模式.

<img src="F:\learn_opengl\Report.assets\image-20221016162657920.png" alt="image-20221016162657920" style="zoom:50%;" />

<img src="F:\learn_opengl\Report.assets\image-20221016162730551.png" alt="image-20221016162730551" style="zoom:50%;" />
