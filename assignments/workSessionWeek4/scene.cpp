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
    suzanne = std::make_unique<ew::Model>("assets/models/skull.obj");
    toon = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    //textuxe = std::make_unique<ew::Texture>("")

    light = {
        .brightness = 1.0f,
        .color = {1.0f,1.0f,1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };
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

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,textuxe->getID());
    

    toon->use();

    // scene matrices
    //toon->setInt("texture0",index);
    toon->setMat4("model", glm::mat4(1.0f));
    toon->setMat4("view_proj", view_proj);
    toon->setVec3("camera_position", camera.position);
    toon->setVec3("light.pos", light.position);
    toon->setVec3("light.color", light.color);
    toon->setFloat("material.shiny", debug.alpha);
    toon->setFloat("material.diffuse", debug.diffuse);
    toon->setFloat("material.specular", debug.specular);
    toon->setFloat("material.ambient", debug.ambient);

    // draw suzanne
    suzanne->draw();
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

    /* build debug ui here */

    ImGui::End();
}