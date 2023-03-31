#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/shader.h>
#include <iostream>

//声明窗口注册回调函数
void framebuffer_size_callback(GLFWwindow* window,int width,int height);

std::string Shader::dirName;

//处理输入
void processInput(GLFWwindow* window);

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

	Shader ourShader("./src/06_glsl_exercise/shader/vertex.glsl","./src/06_glsl_exercise/shader/fragment.glsl");

	//定义顶点数据
	float vertices[] = {
        // 位置              // 颜色
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    }; 


	/*创建VBO 顶点缓冲对象
	创建VAO 顶点数组对象*/
	unsigned int VBO,VAO;
	glGenBuffers(1,&VBO);
	glGenVertexArrays(1,&VAO);

	//绑定VAO
	glBindVertexArray(VAO);

	//将创建的VBO绑定到GL_ARRAY_BUFFER缓冲区中
	glBindBuffer(GL_ARRAY_BUFFER,VBO);

	//将定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	//设置顶点位置属性属性指针
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
	//启用顶点属性
	glEnableVertexAttribArray(0);
    //设置顶点颜色设置指针
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    //启用顶点属性
    glEnableVertexAttribArray(1);
	//解绑VAO
	glBindVertexArray(0);

	//设置线框绘制模式
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	float xOffset = 0.5f;

	//设置渲染循环
	while(!glfwWindowShouldClose(window))
	{

		processInput(window);

		//清屏操作
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		ourShader.setFloat("xOffset",xOffset);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES,0,3);

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