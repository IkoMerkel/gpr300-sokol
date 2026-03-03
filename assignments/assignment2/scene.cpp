#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"
#include "ew/procGen.h"

struct FullScreenQuad
{
    unsigned int vao;
    unsigned int vbo;


    void Initialize(){
        float vertices[] = 
        {
            //pos (x,y) texcoords (u,v)
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),&vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        glBindVertexArray(0);
    }

    
} fullscreen_quad;

struct {
    float alpha = 128.0f;
    float diffuse = 0.5f;
    float specular = 0.5f;
    float ambient = 0.5f;
    float slot1 = 16.0f;
    float slot2 = 0.25f;
    float slot3 = 0.0f;
    glm::vec3 slotV3 = glm::vec3(0.01f,0.0f,-0.01f);
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toon = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    toonShadowMapping = std::make_unique<ew::Shader>("assets/shaders/defaultshadowmap.vs", "assets/shaders/toonShadowMapping.fs");
    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");
    postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/blur.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");
    //texturePlad = std::make_unique<ew::Texture>("assets/textures/PladColor.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f,1.0f,1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    palette = {
        .color1 =glm::vec3(0.2f,0.0f,0.0f),
        .color2 = glm::vec3(0.0f,1.0f,1.0f),
    };

    plane.load(ew::createPlane(10, 10, 20));
    fullscreen_quad.Initialize();

    Scene::CreateFrameBuffer();
    Scene::CreateDepthBuffer();
    
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &shadow_fbo);
    glDeleteFramebuffers(0, &fbo);
}

void Scene::CreateFrameBuffer()
{
        //framebuffer
        glCreateFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        {
            glGenTextures(1, &fbo_texture);
            glBindTexture(GL_TEXTURE_2D, fbo_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE,NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbo_texture,0);
    
            glGenTextures(1, &fbo_depth);
            glBindTexture(GL_TEXTURE_2D, fbo_depth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,fbo_depth,0);
    
            glBindTexture(GL_TEXTURE_2D, 0);
    
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                printf("framebuffer not complete");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::CreateDepthBuffer()
{
    glCreateFramebuffers(1, &shadow_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
    {

        glGenTextures(1, &shadow_depth);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT,NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,shadow_depth,0);

        glDrawBuffers(0,nullptr);
        glReadBuffer(GL_NONE);

        //glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,shadow_depth,0);

        glBindTexture(GL_TEXTURE_2D, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("depthbuffer not complete");
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

auto matrix = glm::mat4(1.0f);
static const char* shaderMenu[]{"blur","grayscale","vignette","film grain","chromatic aberration", "lens distortion", "inverse", "line detection"};
static int selectedMenu = 0;

void Scene::Render(void)
{
    switch (selectedMenu)
    {
    case 0:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/blur.fs");
        break;
    case 1:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/grayscale.fs");
        break;
    case 2:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/vignette.fs");
        break;
    case 3:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/filmGrain.fs");
        break;
    case 4:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/chromeabs.fs");
        break;
    case 5:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/lensdistort.fs");
        break;
    case 6:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/inverse.fs");
        break;
    case 7:
        postProcess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/linedetect.fs");
        break;
    default:
        break;
    }
    const auto view_proj = camera.Projection() * camera.View();

    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f,10.0f, 0.1f, 100.0f);
    const auto light_view = glm::lookAt(light.position,glm::vec3(0.0f),glm::vec3(0.0f,1.0f,0.0f));
    const auto light_view_proj = light_proj * light_view;

    

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture->getID());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,shadow_depth);
        
        toonShadowMapping->use();

        // scene matrices
        toonShadowMapping->setFloat("time", (float)time.absolute);
        toonShadowMapping->setInt("texture0",0);
        toonShadowMapping->setInt("shadow_map",1);
        toonShadowMapping->setMat4("model", glm::mat4(1.0f));
        toonShadowMapping->setMat4("view_proj", view_proj);
        toonShadowMapping->setMat4("light_view_proj", light_view_proj);
        toonShadowMapping->setVec3("pal.color1", palette.color1);
        toonShadowMapping->setVec3("pal.color2",palette.color2);
        toonShadowMapping->setVec3("camera_position", camera.position);
        toonShadowMapping->setVec3("light.pos", light.position);
        toonShadowMapping->setVec3("light.color", light.color);
        toonShadowMapping->setFloat("material.shiny", debug.alpha);
        toonShadowMapping->setFloat("material.diffuse", debug.diffuse);
        //toonShadowMapping->setFloat("material.specular", debug.specular);
        toonShadowMapping->setFloat("material.ambient", debug.ambient);
        
        // draw suzanne
        suzanne->draw();

        const auto planeMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-1.0f,0.0f));
        toonShadowMapping->setMat4("model", planeMatrix);
        plane.draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER,shadow_fbo);
    {
        

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glViewport(0,0, 800, 600);

        glClear(GL_DEPTH_BUFFER_BIT);

        depth->use();

        depth->setMat4("model", glm::mat4(1.0f));
        depth->setMat4("light_view_proj", light_view_proj);

        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    postProcess->use();
    postProcess->setInt("screen",0);
    switch (selectedMenu)
    {
    case 0:
        postProcess->setFloat("strength", debug.slot1);
        break;
    case 1:
        
        break;
    case 2:
        postProcess->setFloat("intensity", debug.slot1);
        postProcess->setFloat("extent", debug.slot2);
        break;
    case 3:
        postProcess->setFloat("strength", debug.slot3);
        break;
    case 4:
        postProcess->setVec3("offset", debug.slotV3);
        break;
    case 5:
        postProcess->setFloat("strength", debug.slot3);
        break;
    default:
        break;
    }

    

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(fullscreen_quad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glDrawArrays(GL_TRIANGLES,0,6);
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

    auto light_matrix = glm::translate(glm::mat4(1.0f),light.position);
    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(light_matrix)
    );

    if(ImGuizmo::IsUsing())
    {
        light.position = glm::vec3(light_matrix[3]);
    }

    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);
    /*ImGui::ColorEdit3("Light Color", &light.color[0]);
    ImGui::DragFloat("Ambient", &debug.ambient, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Diffuse", &debug.diffuse, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Specular", &debug.specular, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Shiny", &debug.alpha, 1.0f, 1.0f,128.0f);*/
    ImGui::SeparatorText("Palette");
    ImGui::ColorEdit3("Color1",&palette.color1[0]);
    ImGui::ColorEdit3("Color2",&palette.color2[0]);
    switch (selectedMenu)
    {
    case 0:
        ImGui::SliderFloat("Kernal Blur", &debug.slot1, 10.0f, 300.0f);
        break;
    case 1:
        
        break;
    case 2:
        ImGui::SliderFloat("intensity", &debug.slot1, 10.0, 60.0f);
        ImGui::SliderFloat("extent", &debug.slot2, 0.1f, 0.8f);
        break;
    case 3:
        ImGui::SliderFloat("strength", &debug.slot3, 0.0f, 1.0f);
        break;
    case 4:
        ImGui::SliderFloat("red offset", &debug.slotV3.x, -0.02f, 0.02f);
        ImGui::SliderFloat("green offset", &debug.slotV3.y, -0.02f, 0.02f);
        ImGui::SliderFloat("blue offset", &debug.slotV3.z, -0.02f, 0.02f);
        break;
    case 5:
        ImGui::SliderFloat("strength", &debug.slot3, 0.0f, 1.0f);
        break;
    default:
        break;
    }
    ImGui::Combo("Current Shader", &selectedMenu, shaderMenu, IM_ARRAYSIZE(shaderMenu));

    ImGui::Image(
        (void*)(intptr_t) fbo_texture,
        ImVec2(400,300),
        ImVec2(0,1), ImVec2(1,0)
    );

    ImGui::Image(
        (void*)(intptr_t) fbo_depth,
        ImVec2(400,300),
        ImVec2(0,1), ImVec2(1,0)
    );

    ImGui::Image(
        (void*)(intptr_t) shadow_depth,
        ImVec2(400,300),
        ImVec2(0,1), ImVec2(1,0)
    );

    /* build debug ui here */

    ImGui::End();
}