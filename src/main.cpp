#include <emscripten.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include "scheduler.h"
#include "viewport.h"
#include "animator.h"
#include "objects/object.h"

using namespace glm;

int main(int argc, char** argv)
{
    std::cout << "Graphics engine starting" << std::endl;

    Scheduler  scheduler;

    Object root;
    Object camera(root);
    Object knotObject(root);
    Object sphereObject(root);

    knotObject  .setTransform(translate(mat4(1.0), vec3( 8.0, 0,0)));
    sphereObject.setTransform(translate(mat4(1.0), vec3(-8.0, 0,0)));

    Renderable knotRenderable(knotObject);
    knotRenderable.mesh.knot(5, 2, 100, 100);
    knotRenderable.material.roughness = 0.3;
    knotRenderable.material.metallic = 0.8;

    Renderable sphereRenderable(sphereObject);
    sphereRenderable.mesh.noisySphere(5, 100, 100, 1);
    sphereRenderable.material.roughness = 1.0;
    sphereRenderable.material.metallic = 0.3;

    Animator cameraRotation(scheduler, [&](double time) {
        glm::mat4 toParent = glm::lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        toParent = glm::rotate(toParent, glm::radians((float)time * 60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        toParent = glm::rotate(toParent, glm::radians((float)time * 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        camera.setTransform(toParent);
    });

    Viewport viewport = Viewport(scheduler);
    viewport.attachCamera(camera);
    viewport.attachRenderable(knotRenderable);
    viewport.attachRenderable(sphereRenderable);

    cameraRotation.start();

    scheduler.run();

    return 0;
};