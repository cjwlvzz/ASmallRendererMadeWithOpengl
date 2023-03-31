#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//声明窗口注册回调函数
void framebuffer_size_callback(GLFWwindow* window,int width,int height);

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

	//设置渲染循环
	while(!glfwWindowShouldClose(window))
	{

		processInput(window);

		//清屏操作
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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