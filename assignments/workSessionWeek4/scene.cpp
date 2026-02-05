#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct {
    float alpha = 128.0f;
    float diffuse = 0.5f;
    float specular = 0.5f;
    float ambient = 0.5f;
} debug;


Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toon = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");
    texturePlad = std::make_unique<ew::Texture>("assets/textures/PladColor.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f,1.0f,1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    palette = {
        .color1 =glm::vec3(1.0f,0.0f,0.0f),
        .color2 = glm::vec3(0.0f,0.0f,1.0f),
    };

    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    //advance opengl framebuffers
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbo_texture,0);
    

    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("framebuffer not complete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

auto matrix = glm::mat4(1.0f);

void Scene::Render(void)
{
    
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture->getID());

    
    toon->use();

    // scene matrices
    toon->setInt("texture0",0);
    toon->setInt("texture1",1);
    toon->setMat4("model", glm::mat4(1.0f));
    toon->setMat4("view_proj", view_proj);
    toon->setVec3("pal.color1", palette.color1);
    toon->setVec3("pal.color2",palette.color2);
    toon->setVec3("camera_position", camera.position);
    toon->setVec3("light.pos", light.position);
    toon->setVec3("light.color", light.color);
    toon->setFloat("material.shiny", debug.alpha);
    toon->setFloat("material.diffuse", debug.diffuse);
    toon->setFloat("material.specular", debug.specular);
    toon->setFloat("material.ambient", debug.ambient);

    // draw suzanne
    suzanne->draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    ImGui::ColorEdit3("Light Color", &light.color[0]);
    ImGui::DragFloat("Ambient", &debug.ambient, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Diffuse", &debug.diffuse, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Specular", &debug.specular, 0.01f, 0.0f,1.0f);
    ImGui::DragFloat("Shiny", &debug.alpha, 1.0f, 1.0f,128.0f);
    ImGui::SeparatorText("Palette");
    ImGui::ColorEdit3("Color1",&palette.color1[0]);
    ImGui::ColorEdit3("Color2",&palette.color2[0]);

    ImGui::Image(
        (void*)(intptr_t) fbo_texture,
        ImVec2(400,300),
        ImVec2(0,1), ImVec2(1,0)
    );

    /* build debug ui here */

    ImGui::End();
}