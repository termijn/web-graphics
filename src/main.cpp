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
    Object cubeObject(root);
    Object lightObject(camera);

    knotObject   .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    sphereObject .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    cubeObject   .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    lightObject  .setTransform(glm::inverse(glm::lookAt(vec3(10,10,10), vec3(0,0,-2), vec3(0,1,0))));

    Renderable knotRenderable(knotObject);
    knotRenderable.mesh.knot(5, 2, 200, 100);
    knotRenderable.material.roughness = 0.3;
    knotRenderable.material.metallic = 0.8;

    Renderable sphereRenderable(sphereObject);
    //sphereRenderable.mesh.noisySphere(5, 100, 100, 1);
    sphereRenderable.mesh.sphere(5, 100, 100);
    sphereRenderable.material.roughness = 0.5;
    sphereRenderable.material.metallic = 0.6;

    Renderable cubeRenderable(lightObject);
    cubeRenderable.mesh.cube(1);
    cubeRenderable.material.roughness = 1.0;
    cubeRenderable.material.metallic = 0.3;
    
    glm::mat4 fromParent = glm::lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera.setTransform(glm::inverse(fromParent));

    Animator cameraRotation(scheduler, [&](double time) {
        glm::mat4 fromParent = glm::lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    
        fromParent = glm::rotate(fromParent, glm::radians((float)time * 60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        fromParent = glm::rotate(fromParent, glm::radians((float)time * 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        camera.setTransform(glm::inverse(fromParent));
    });

    Viewport viewport = Viewport(scheduler);
    viewport.attachCamera       (camera);
    viewport.attachRenderable   (knotRenderable);
    viewport.attachRenderable   (sphereRenderable);
    //viewport.attachRenderable   (cubeRenderable);
    viewport.attachLight        (lightObject);

    cameraRotation.start();

    scheduler.run();

    return 0;
};