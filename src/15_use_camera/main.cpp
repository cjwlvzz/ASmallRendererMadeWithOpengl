#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/shader.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/gui.h>

//Window resize callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
//all the callback function below are not used in this project
//mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//mouse_button callback function 
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
std::string Shader::dirName;

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

//camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

using namespace std;

//calculate delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

int main()
{
  glfwInit();
  // 设置主要和次要版本
  const char* glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // 创建窗口对象
  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  //--------------------
  // 创建imgui上下文
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  //设置imgui样式
  ImGui::StyleColorsDark();
  //设置平台和渲染器
  ImGui_ImplGlfw_InitForOpenGL(window,true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // -------------------------


  // 设置视口
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);

  // Mouse and keyboard event listener
  // 注册窗口变化监听
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  Shader ourShader("./src/15_use_camera/shader/vertex.glsl", "./src/15_use_camera/shader/fragment.glsl");

  PlaneGeometry planeGeometry(0.5,0.5,1.0,1.0);
  BoxGeometry boxGeometry(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);

  // generate texture
  unsigned int texture1, texture2;
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  // 设置环绕和过滤方式
  float borderColor[] = {0.3f, 0.1f, 0.7f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // 图像y轴翻转
  stbi_set_flip_vertically_on_load(true);

  // 加载图片
  int width, height, nrChannels;
  unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(data);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  // 设置环绕和过滤方式
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // 加载图片
  data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  stbi_image_free(data);
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);

  float factor = 0.0;

  // rotate matrix
  glm::mat4 ex = glm::eulerAngleX(45.0f);
  glm::mat4 ey = glm::eulerAngleY(45.0f);
  glm::mat4 ez = glm::eulerAngleZ(45.0f);

  glm::mat4 qularXYZ = glm::eulerAngleXYX(45.0f,45.0f,45.0f);


  float fov = 45.0f;//angle of the frustum
  glm::vec3 view_translate = glm::vec3(0.0,0.0,1.0);
  ImVec4 clear_color = ImVec4(0.1,0.1,0.1,1.0);

  float radius = 10.0f;
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    
    ImGui::Begin("Hello World");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::SliderFloat("float",&fov,0.0f,180.0f);
    ImGui::SliderFloat3("translate",(float*)&view_translate,0.0,10.0);
    ImGui::ColorEdit3("clear color",(float*)&clear_color);
    ImGui::End();


    // render cammand
    // ... make run dir=13_model_view_projection
    glClearColor(clear_color.x,clear_color.y,clear_color.z,clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    factor = glfwGetTime();
    ourShader.setFloat("factor", -factor * 0.3);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    float radius = 5.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    
    glm::mat4 view = glm::mat4(1.0f);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov),(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,0.1f,100.0f);
    glm::mat4 model = glm::mat4(1.0f);

    glm::qua<float> qu = glm::qua<float>(glm::vec3(glfwGetTime(), glfwGetTime(), glfwGetTime()));
    model = glm::mat4_cast(qu);

    ourShader.setMat4("view",view);
    ourShader.setMat4("projection",projection);

    ourShader.setMat4("model",model);
    glBindVertexArray(boxGeometry.VAO);
    glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

    // render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  planeGeometry.dispose();
  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }

  //camera movement control
  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    cameraPos += cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    cameraPos -= cameraSpeed * cameraFront;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  }
}

// mouse movement linster
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

}

// mouse button linster
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      break;
    }
  }
}

// cursor position callback function
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
  float xpos = float((x-SCREEN_WIDTH/2)/SCREEN_WIDTH) * 2;
  float ypos = float((SCREEN_HEIGHT/2-y)/SCREEN_HEIGHT) * 2;

  return;
}

// scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  return;
}