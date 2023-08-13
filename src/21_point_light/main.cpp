#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tool/shader.h>
#include <tool/camera.h>
#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

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

unsigned int loadTexture(char const * path);

std::string Shader::dirName;

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

using namespace std;

//calculate delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(glm::vec3(0.0,1.0,5.0));

//the mouse position in the last frame
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;

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

  Shader ourShader("./src/21_point_light/shader/vertex.glsl", "./src/21_point_light/shader/fragment.glsl");
  Shader lightShader("./src/21_point_light/shader/light_vertex.glsl", "./src/21_point_light/shader/light_fragment.glsl");

  PlaneGeometry planeGeometry(0.5,0.5,1.0,1.0);
  BoxGeometry boxGeometry(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
  SphereGeometry sphereGeometry(0.1, 10.0, 10.0);
  SphereGeometry sphereGeometry1(1.0, 100.0, 100.0);

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

  unsigned int diffuseMap = loadTexture("static/texture/container2.png");
  unsigned int specularMap = loadTexture("static/texture/container2_specular.png");
  unsigned int specularColorMap = loadTexture("static/texture/lighting_maps_specular_color.png");
  ourShader.use();
  ourShader.setInt("material.diffuse", 0);
  ourShader.setInt("material.specular", 1);
  ourShader.setInt("material.specularColor", 2);

  float factor = 0.0;

  // rotate matrix
  glm::mat4 ex = glm::eulerAngleX(45.0f);
  glm::mat4 ey = glm::eulerAngleY(45.0f);
  glm::mat4 ez = glm::eulerAngleZ(45.0f);

  glm::mat4 qularXYZ = glm::eulerAngleXYX(45.0f,45.0f,45.0f);


  float fov = 45.0f;//angle of the frustum
  glm::vec3 view_translate = glm::vec3(0.0,0.0,1.0);
  ImVec4 clear_color = ImVec4(0.1,0.1,0.1,1.0);

  //light settings
  glm::vec3 lightPosition = glm::vec3(1.0f, 1.5f, 0.0f);

  // Set material properties
  ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
  ourShader.setFloat("material.shininess", 64.0f);

  // Set light properties
  ourShader.setVec3("pointLight.ambient", 0.2f, 0.2f, 0.2f);
  ourShader.setVec3("pointLight.diffuse", 0.5f, 0.5f, 0.5f);
  ourShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);

  //set attenuation coefficient
  ourShader.setFloat("pointLight.constant", 1.0f);
  ourShader.setFloat("pointLight.linear", 0.09f);
  ourShader.setFloat("pointLight.quadratic", 0.032f);

  //define the position of the boxes
  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),
      glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),
      glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),
      glm::vec3(-1.3f, 1.0f, -1.5f)};

  //render loop
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastTime;
    lastTime = currentFrame;

    // showing the frame rate in the window title

    int fps_value = (int)round(ImGui::GetIO().Framerate);
    int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);

    std::string FPS = std::to_string(fps_value);
    std::string MS = std::to_string(ms_value);
    std::string TITLE = "LearnOpenGL - " + FPS + " FPS (" + MS + " ms)";

    glfwSetWindowTitle(window, TITLE.c_str());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // render cammand
    // ... make run dir=13_model_view_projection
    glClearColor(clear_color.x,clear_color.y,clear_color.z,clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    //modify light color
    glm::vec3 lightColor = glm::vec3(1.0f);
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularColorMap);
    
    glm::mat4 view = glm::mat4(1.0f);

    view = camera.GetViewMatrix();

    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov),(float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,0.1f,100.0f);
    glm::mat4 model = glm::mat4(1.0f);

    float rotate = glfwGetTime() * 0.2f;

    glm::vec3 lightPos = glm::vec3(glm::vec3(lightPosition.x * glm::sin(glfwGetTime()),lightPosition.y, lightPosition.z));

    //Drawing the box object
    ourShader.setMat4("view",view);

    ourShader.setMat4("projection",projection);

    ourShader.setVec3("pointLight.position",lightPos);

    ourShader.setVec3("viewPos",camera.Position);

    for (unsigned int i = 0; i < 10; i++)
    {
      model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);

      float angle = 10.0f * i;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

      ourShader.setMat4("model", model);
      glBindVertexArray(boxGeometry.VAO);
      glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);
    }

    //Drawing the light object
    lightShader.use();
    model = glm::mat4(1.0f);
    lightShader.setVec3("lightColor", lightColor);
    model = glm::translate(model, lightPos);

    lightShader.setMat4("view",view);
    lightShader.setMat4("projection",projection);
    lightShader.setMat4("model",model);
    glBindVertexArray(sphereGeometry.VAO);
    glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

    // render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  //release resources
  planeGeometry.dispose();
  boxGeometry.dispose();
  sphereGeometry.dispose();
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
    camera.ProcessKeyboard(FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    camera.ProcessKeyboard(LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    camera.ProcessKeyboard(RIGHT, deltaTime);
  }
}

// mouse movement linster
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);

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

unsigned int loadTexture(char const *path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}