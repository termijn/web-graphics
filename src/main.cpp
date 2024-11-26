#include <emscripten.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "objects/object.h"
#include "inputs/roll.h"
#include "io/loader.h"

#include "scheduler.h"
#include "viewport.h"
#include "animator.h"
#include "scene.h"
#include "cubemap.h"

using namespace glm;

class App
{
public:
    App()
    {
        camera.setPerspective(radians(45.0f), 0.1f, 1000.0f);

        floorRenderable.mesh.cube(1);
        floorRenderable.material.albedo      = vec3(1.0);
        floorRenderable.material.shaded      = false;
        floorRenderable.material.castsShadow = false;

        reflectionMap.positiveX = loadImage("/package/cubemap/px.jpg");
        reflectionMap.negativeX = loadImage("/package/cubemap/nx.jpg");
        reflectionMap.positiveY = loadImage("/package/cubemap/py.jpg");
        reflectionMap.negativeY = loadImage("/package/cubemap/ny.jpg");
        reflectionMap.positiveZ = loadImage("/package/cubemap/pz.jpg");
        reflectionMap.negativeZ = loadImage("/package/cubemap/nz.jpg");

        scene = loadModelObjects("/package/models/DamagedHelmet.glb", sceneParent);

        for (auto& renderable: scene->all())
           renderable->getRenderable().material.reflectionMap = &reflectionMap;

        sceneParent  .setTransform(translate(scale(mat4(1.0), vec3(5.0)), vec3(0.0, 0.0, 0.0)));
        floorObject  .setTransform(scale(translate(mat4(1.0), vec3(0, 0, -60)), vec3(500.0, 500.0, 1.0)));
        lightObject  .lookAt(vec3(0.5, 0.6, 1), vec3(0, 0, 0), vec3(0, 1, 0));

        Box box = scene->getBox();
        vec3 sceneCenter = Space::pos(box.center(), Space(), camera.getParentSpace());
        float diameter   = Space::dir(box.max - box.min, Space(), camera.getParentSpace()).length();

        camera.lookAt(sceneCenter + vec3(0, 0, diameter * 5), sceneCenter, vec3(0, 1, 0));
        //camera.lookAt(vec3(0, 0, -5), vec3(0,0,0), vec3(0, 1, 0));

        viewport.attachCamera       (camera);
        viewport.attachRenderable   (floorRenderable);
        viewport.attachLight        (lightObject);

        for (auto& renderableObject: scene->all())
            viewport.attachRenderable(renderableObject->getRenderable());
    }

    void run()
    {
        scheduler.run();
    }
private:

    Scheduler  scheduler;
    
    Cubemap reflectionMap;
    
    Object          root;
    CameraObject    camera          = CameraObject(root);
    Object          sceneParent     = Object(root);
    Object          floorObject     = Object(camera);
    Object          lightObject     = Object(camera);

    Renderable  floorRenderable = Renderable(floorObject);
    Viewport    viewport        = Viewport(scheduler);
    RollInput   cameraInput     = RollInput(viewport, camera);

    std::unique_ptr<Scene> scene;

};

std::unique_ptr<App> app;

int main(int argc, char** argv)
{
    std::cout << "Graphics engine starting" << std::endl;

    app = std::make_unique<App>();
    app->run();


    app = nullptr;
    return 0;
};
