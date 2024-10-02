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

int main(int argc, char** argv)
{
    std::cout << "Graphics engine starting" << std::endl;

    Scheduler  scheduler;

    Object root;
    CameraObject    camera(root);
    Object          knotObject(root);
    Object          couchObject(root);
    Object          sphereObject(root);
    Object          cubeObject(root);
    Object          lightObject(camera);

    camera.setPerspective(radians(45.0f), 0.1f, 1000.0f);

    couchObject  .setTransform(translate(scale(mat4(1.0), vec3(30)), vec3(0,-0.46,0)));
    sphereObject .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    cubeObject   .setTransform(scale(translate(mat4(1.0), vec3(0, -15, 0)), vec3(500.0, 0.1, 500)));
    lightObject  .lookAt(vec3(10,20,10), vec3(0,-10,-2), vec3(0,1,0));

    std::unique_ptr<Scene> scene = loadModelObjects("/package/models/couch.glb", couchObject);

    Renderable sphereRenderable(sphereObject);
    sphereRenderable.mesh.sphere(5, 100, 100);
    sphereRenderable.material.roughness     = 0.3;
    sphereRenderable.material.metallic      = 0.1;
    sphereRenderable.material.albedo        = vec3(0.7,0,0);

    Renderable cubeRenderable(cubeObject);
    cubeRenderable.mesh.cube(1);
    cubeRenderable.material.albedo      = vec3(1,1,1);
    cubeRenderable.material.roughness   = 0.1;
    cubeRenderable.material.shaded      = false;
    cubeRenderable.material.castsShadow = false;
    cubeRenderable.material.metallic    = 0.3;
    
    camera.lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    Animator cameraRotation(scheduler, [&](double time) {
        mat4    m = lookAt(glm::vec3(0, 10, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
                m = rotate(m, radians((float)time * 40.0f), vec3(0.0f, 1.0f, 0.0f));
        camera.setTransform(inverse(m));
    });

    Viewport viewport = Viewport(scheduler);
    viewport.attachCamera       (camera);
    viewport.attachRenderable   (cubeRenderable);
    viewport.attachLight        (lightObject);

    for (auto& renderableObject: *scene)
        viewport.attachRenderable(renderableObject->getRenderable());

    cameraRotation.start();

    scheduler.run();

    return 0;
};