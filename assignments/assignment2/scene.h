#pragma once

// batteries
#include "batteries/scene.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"
#include "batteries/lights.h"
#include "ew/mesh.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void CreateFrameBuffer();
    void CreateDepthBuffer();
    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> toon;
    std::unique_ptr<ew::Shader> toonShadowMapping;
    std::unique_ptr<ew::Texture> texture;
    std::unique_ptr<ew::Texture> texturePlad;

    std::unique_ptr<ew::Shader> postProcess;
    std::unique_ptr<ew::Shader> depth;
    ew::Mesh plane;

    batteries::light_t light;

    struct 
    {
      glm::vec3 color1;
      glm::vec3 color2;
    }palette;
    
    //framebuffer
    unsigned int fbo;
    unsigned int fbo_texture;
    unsigned int fbo_depth;

    //depthbuffer
    unsigned int shadow_fbo;
    unsigned int shadow_depth;
};
