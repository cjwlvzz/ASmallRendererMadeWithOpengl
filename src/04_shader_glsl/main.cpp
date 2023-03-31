#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

//声明窗口注册回调函数
void framebuffer_size_callback(GLFWwindow* window,int width,int height);

//处理输入
void processInput(GLFWwindow* window);

//定义顶点着色器
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource1 = "#version 330 core\n"
	"out vec4 fragColor;\n"
	"void main()\n"
	"{\n"
	"fragColor = vec4(1.0f,0.5f,0.2f,1.0f);"
	"}\n\0";

const char *fragmentShaderSource2 = "#version 330 core\n"
	"out vec4 fragColor;\n"
	"uniform vec4 ourColor;"
	"void main()\n"
	"{\n"
	"fragColor = ourColor;"
	"}\n\0";

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

	//定义顶点数据
	float vertices[] = {
        //第一个三角形
		-0.5f, 0.5f, 0.0f,
		-0.75f, -0.5, 0.0f,
		-0.25f, -0.5f, 0.0f,

		//第二个三角形
		0.5f, 0.5f, 0.0f,
		0.75f, -0.5f, 0.0f,
		0.25f, -0.5f, 0.0f,
    }; 

	/*创建VBO 顶点缓冲对象
	创建VAO 顶点数组对象
	创建EBO 元素缓冲对象*/
	unsigned int VBOS[2],VAOS[2];
	glGenBuffers(2,VBOS);
	glGenVertexArrays(2,VAOS);

	//绑定VAO
	glBindVertexArray(VAOS[0]);

	//将创建的VBO绑定到GL_ARRAY_BUFFER缓冲区中
	glBindBuffer(GL_ARRAY_BUFFER,VBOS[0]);

	//将定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	//启用顶点属性
	glEnableVertexAttribArray(0);

	//绑定VAO
	glBindVertexArray(VAOS[1]);

	//将创建的VBO绑定到GL_ARRAY_BUFFER缓冲区中
	glBindBuffer(GL_ARRAY_BUFFER,VBOS[1]);

	//将定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

	//设置顶点属性指针
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)(9*sizeof(float)));
	//启用顶点属性
	glEnableVertexAttribArray(0);

	//解绑VAO
	glBindVertexArray(0);

	//创建着色器
	unsigned int vertexShader,fragmentShader1,fragmentShader2;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);

	//动态编译着色器代码
	glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
	glCompileShader(vertexShader);

	glShaderSource(fragmentShader1,1,&fragmentShaderSource1,NULL);
	glShaderSource(fragmentShader2,1,&fragmentShaderSource2,NULL);
	glCompileShader(fragmentShader1);
	glCompileShader(fragmentShader2);

	//检测是否编译成功
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);

	if(!success)
	{
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		std::cout << "ERROR:SHADER::VERTEX::COMPILE_FAILED\n" << infoLog << std::endl;
	}

	glGetShaderiv(fragmentShader1,GL_COMPILE_STATUS,&success);
	if(!success)
	{
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		std::cout << "ERROR:SHADER::FRAGMENT::COMPILE_FAILED\n" << infoLog << std::endl;
	}

	//创建着色器程序对象
	unsigned int shaderProgram1,shaderProgram2;
	shaderProgram1 = glCreateProgram();
	shaderProgram2 = glCreateProgram();
	//将着色器附加到程序对象上
	glAttachShader(shaderProgram1,vertexShader);
	glAttachShader(shaderProgram1,fragmentShader1);
	glAttachShader(shaderProgram2,vertexShader);
	glAttachShader(shaderProgram2,fragmentShader2);

	//连接两个shader
	glLinkProgram(shaderProgram1);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram1,GL_LINK_STATUS,&success);
	if(!success)
	{
		glGetProgramInfoLog(shaderProgram1,512,NULL,infoLog);
		std::cout << "ERROR:SHADER_PROGRAM_LINK_FAILED\n" << infoLog << std::endl;
	}

	//删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);

	//设置渲染循环
	while(!glfwWindowShouldClose(window))
	{

		processInput(window);

		//清屏操作
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram1);
		glBindVertexArray(VAOS[0]);
		glDrawArrays(GL_TRIANGLES,0,3);

		glBindVertexArray(0);

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue)/2.0f)+0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram2,"ourColor");


		glUseProgram(shaderProgram2);
		glUniform4f(vertexColorLocation,0.0f,greenValue,0.0f,1.0f);
		glBindVertexArray(VAOS[1]);
		glDrawArrays(GL_TRIANGLES,0,3);

		//glDrawElements(GL_LINE_LOOP,6,GL_UNSIGNED_INT,0);

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