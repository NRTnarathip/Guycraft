#include "ClientEngine.h"
#include <stb/stb_image.h>
#include "Input.h"

ClientEngine* ClientEngine::instance = nullptr;

void framebuffer_size_callback(GLFWwindow* glWindow, int width, int height)
{
    glViewport(0, 0, width, height);
    Window::GetInstance()->onWindowResize(width, height);
}
int ClientEngine::setupWindow() {
    //Init opengl
    glfwInit();

    //opengl init version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = new Window("Guycraft");
    auto glWindow = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);
    window->window = glWindow;

    if (glWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(glWindow);
    window->setActiveVsync(true);
    glfwSetFramebufferSizeCallback(glWindow, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //gl options render
    window->setupRender3D();

    //init icon app
    GLFWimage imageIcon;
    int w, h, c;
    unsigned char* data = stbi_load("src/icon/icon_128.png", &w, &h, &c, 4);
    imageIcon.width = w;
    imageIcon.height = h;
    imageIcon.pixels = data;
    window->setIcon(imageIcon);
    return 0;
}
void ClientEngine::launch() {
    //---init window
    setupWindow();
    auto glfwWindow = window->window;

    m_gui = new GUI(glfwWindow);

    //init core engine
    m_input = new Input(glfwWindow);
    m_input->initKeyMapping();

    m_resouceManager = new ResourceManager();
    m_resouceManager->loadAllResouces();

    m_textRenderer = new TextRenderer(glfwWindow);
    m_textRenderer->setupGL();

    //init game
    graphicSetting.renderDistance = 4;

	game = new Game(window);
    game->init();

    while (!glfwWindowShouldClose(glfwWindow))
    { 
        //part render clear
        window->clearBuffer();
        //part Input
        m_input->update();
        m_gui->update();
        //part Client Logic Update
        game->counterTime();
        game->beforeUpdate();
        game->update();
        game->lastUpdate();
        //part Renderer 3D, 2D
        game->render();
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(glfwWindow);
    }
    exit();
}
void ClientEngine::exit() {
    glfwDestroyWindow(window->window);
    glfwTerminate();
}

