#include <emscripten.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "scheduler.h"
#include "viewport.h"
#include "animator.h"
#include "objects/object.h"
#include "io/loader.h"

using namespace glm;

class App
{
public:
    App()
        : cameraRotation(Animator(scheduler, [&](double time) {
                mat4    m = lookAt(glm::vec3(0, 0.1, 20), glm::vec3(0, -6, 0), glm::vec3(0, 1, 0));
                        m = rotate(m, radians((float)time * 40.0f), vec3(0.0f, 1.0f, 0.0f));
                camera.setTransform(inverse(m));
            }))
    {
        camera.setPerspective(radians(35.0f), 0.1f, 1000.0f);

        floorRenderable.mesh.cube(1);
        floorRenderable.material.albedo      = vec3(1,1,1);
        floorRenderable.material.roughness   = 0.1;
        floorRenderable.material.shaded      = false;
        floorRenderable.material.castsShadow = false;
        floorRenderable.material.metallic    = 0.3;

        camera.lookAt(glm::vec3(0, 0.1, 20), glm::vec3(0, -6, 0), glm::vec3(0, 1, 0));

        scene = loadModelObjects("/package/models/DamagedHelmet.glb", sceneParent);

        sceneParent  .setTransform(translate(scale(mat4(1.0), vec3(5.0)), vec3(0.0, -1.0, 0.0)));
        floorObject  .setTransform(scale(translate(mat4(1.0), vec3(0, -15, 0)), vec3(500.0, 0.1, 500)));
        lightObject  .lookAt(vec3(5, 5, 5), vec3(0, 0, 0), vec3(0, 1, 0));

        viewport.attachCamera       (camera);
        viewport.attachRenderable   (floorRenderable);
        viewport.attachLight        (lightObject);

        for (auto& renderableObject: *scene)
            viewport.attachRenderable(renderableObject->getRenderable());

        cameraRotation.start();
    }

    void run()
    {
        scheduler.run();
    }
private:

    Scheduler  scheduler;

    Object root;
    CameraObject    camera          = CameraObject(root);
    Object          sceneParent     = Object(root);
    Object          floorObject     = Object(root);
    Object          lightObject     = Object(camera);

    Renderable floorRenderable  = Renderable(floorObject);
    Viewport viewport           = Viewport(scheduler);
    Animator cameraRotation;

    std::unique_ptr<Scene> scene;

};

std::unique_ptr<App> app;

int main(int argc, char** argv)
{
    std::cout << "Graphics engine starting" << std::endl;

    app = std::make_unique<App>();
    app->run();

    return 0;
};