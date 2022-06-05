#include "ClientEngine.h"
#include <stb/stb_image.h>
#include "Input.h"
#include "ImGUILoader.h"
#include "PlayerController.h"

ClientEngine* ClientEngine::instance = nullptr;

void framebuffer_size_callback(GLFWwindow* glWindow, int width, int height)
{
    Window::GetInstance()->resize(width, height);
}
int ClientEngine::setupWindow() {
    //Init opengl
    glfwInit();

    //opengl init version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* glWindow = nullptr;

    if (window->isFullscreenMode()) {
        auto monitor = glfwGetPrimaryMonitor();
        auto videoMode = glfwGetVideoMode(monitor);
        glWindow = glfwCreateWindow(videoMode->width, videoMode->height, window->title,
            monitor, NULL);
    }
    else {
        glWindow = glfwCreateWindow(window->width, window->height, window->title,
            NULL, NULL);
    }
    if (glWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        std::cerr << "Error Fialed to create GLFW window";
        glfwTerminate();
        return -1;
    }
    window->window = glWindow;
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
int ClientEngine::launch() {
    window = new Window("Guycraft");
    //---init window
    setupWindow();
    printf("end setup window\n");

    //resize window
    window->resize(1920, 1080);
    window->setActiveVsync(false);
    auto glfwWindow = window->window;

    //part core engine
    m_input = new Input(glfwWindow);
    m_input->initKeyMapping();
    m_resouceManager = new ResourceManager();
    m_resouceManager->loadAllResouces();
    m_sceneManager = new SceneManager();
    m_sceneManager->init();
    m_gui = new GUI(glfwWindow);
    m_textRenderer = new TextRenderer(glfwWindow);
    m_textRenderer->setupGL();
    m_blockDatabase = new BlockDatabase();
    m_blockDatabase->init();

    //part game logic
	game = new Game(window);
    game->init();

    //graphic setting
    graphicSetting.renderDistance = 14;

    //// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    while (not glfwWindowShouldClose(glfwWindow))
    { 
        // Take care of all GLFW events
        glfwPollEvents();
        //part render clear
        window->clearBuffer();
        //part Input
        m_input->update();
        //part GUI
        m_gui->update();
        //part Client Logic Update
        game->counterTime();
        game->beforeUpdate();
        game->update();
        game->lastUpdate();

       
        game->render();

        //// Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            auto atlas = m_resouceManager->m_textureAtlas["chunk"];
            ImGui::Begin("Hello, world!");                         
            ImGui::SliderInt("mipmap level", &atlas->m_mipmalLevel, 0, 4);
            if (ImGui::Button("reload atlas")) {
                atlas->reload();
            }                          
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        bool isNeedExitGame = false;
        {
            auto pc = PlayerController::GetInstance();
            if (pc != nullptr) {
                ImGui::Begin("Player Controller");
                ImGui::SliderFloat("mipmap level", &pc->speedRun, 0, 500);
                if (ImGui::Button("Exit Game")) {
                    isNeedExitGame = true;
                    auto sc = m_sceneManager->getCurrent();
                    sc->exit();
                }
                ImGui::End();
            };
        }
        {
            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        // Swap the back buffer with the front buffer
        glfwSwapBuffers(glfwWindow);
    }
    return exit();
}
int ClientEngine::exit() {
    glfwDestroyWindow(window->window);
    glfwTerminate();
    return 0;
}

