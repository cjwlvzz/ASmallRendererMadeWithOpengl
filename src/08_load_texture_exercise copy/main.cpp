#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/shader.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

//声明窗口注册回调函数
void framebuffer_size_callback(GLFWwindow* window,int width,int height);

std::string Shader::dirName;

//处理输入
void processInput(GLFWwindow* window);

using namespace std;

int main()
{
	//初始化glfw
	glfwInit();

	//设置主要和次要版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);

	//设置OPENGL模式，这里我们使用核心模式
	//关于立即模式和核心模式的区别:https://blog.csdn.net/weixin_43399489/article/details/126857721
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

	//创建窗口对象，存放所有和窗口相关的数据
	GLFWwindow* window = glfwCreateWindow(800,600,"LearnOpenGL",NULL,NULL);
	if(window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//将当前窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	//GLAD用于管理OpenGL的函数指针，所以在调用任何OpenGL的函数之前我们都需要初始化GLAD
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//设置视口,前两个参数用于控制窗口左下角的位置
	glViewport(0,0,800,600);

	//注册窗口改变大小监听
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

	Shader ourShader("./src/09_transform/shader/vertex.glsl","./src/09_transform/shader/fragment.glsl");

	//定义顶点数据
	float vertices[] = {
        //---- 位置 ----   ---- 颜色 ----   - 纹理坐标 -
      	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 右上
      	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 右下
      	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0, 0.0f, // 左下
      	-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // 左上
    }; 

	// 索引数据
  	unsigned int indices[] = {
      0, 1, 3, // 三角形一
      1, 2, 3  // 三角形二
  	};

	/*创建VBO 顶点缓冲对象
	创建VAO 顶点数组对象*/
	unsigned int VBO,VAO,EBO;
	glGenBuffers(1,&VBO);
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&EBO);

	//绑定VAO
	glBindVertexArray(VAO);

	//将创建的VBO绑定到GL_ARRAY_BUFFER缓冲区中
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	//将定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);


	//设置顶点位置属性属性指针
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
	//启用顶点属性
	glEnableVertexAttribArray(0);
    //设置顶点颜色设置指针
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    //启用顶点属性
    glEnableVertexAttribArray(1);
	// 设置顶点纹理坐标属性指针
  	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  	glEnableVertexAttribArray(2);

	//解绑VAO
	glBindVertexArray(0);

	//生成纹理
	unsigned int texture1,texture2;
	glGenTextures(1,&texture1);
	glBindTexture(GL_TEXTURE_2D,texture1);

	//设置环绕和过滤方式
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//加载图片
	stbi_set_flip_vertically_on_load(true);
	int width , height , nrChannels;
	unsigned char *data = stbi_load("./static/texture/container.jpg",&width,&height,&nrChannels,0);

	if(data)
	{
		//将图片数据写入纹理
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	glGenTextures(1,&texture2);
	glBindTexture(GL_TEXTURE_2D,texture2);

	//设置环绕和过滤方式
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//加载图片
	data = stbi_load("./static/texture/awesomeface.png",&width,&height,&nrChannels,0);

	if(data)
	{
		//将图片数据写入纹理
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	ourShader.use();
	ourShader.setInt("texture2",0);
	ourShader.setInt("texture2",1);

	//设置线框绘制模式
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	float factor = 0;
	
	//初始化单位矩阵
	glm::mat4 trans = glm::mat4(1.0f);

	

	// //将矩阵传递给顶点着色器
	// unsigned int transform = glGetUniformLocation(ourShader.ID,"transform");
	// glUniformMatrix4fv(transform,1,GL_FALSE,glm::value_ptr(trans));

	//设置渲染循环
	while(!glfwWindowShouldClose(window))
	{

		processInput(window);

		//清屏操作
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();

		factor = glfwGetTime();
		ourShader.setFloat("factor",factor);
		trans = glm::rotate(trans,glm::radians(factor),glm::vec3(0.0,0.0,1.0));
		trans = glm::scale(trans,glm::vec3(sin(factor),0.5,0.5));

		ourShader.setMat4("transform",trans);
		trans = glm::mat4(1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texture2);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

		trans =glm::translate(trans,glm::vec3(0.5,0.0,0.0));
		trans = glm::scale(trans,glm::vec3(sin(factor),0.5,0.5));
		ourShader.setMat4("transform",trans);
		trans = glm::mat4(1.0f);

		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

		glBindVertexArray(0);

		//交换双缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//循环结束释放资源
	glfwTerminate();

	return 0;

}

void framebuffer_size_callback(GLFWwindow *window,int width,int height)
{
	glViewport(0,0,800,600);
}

void processInput(GLFWwindow* window)
{
	if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,true);
	}
}