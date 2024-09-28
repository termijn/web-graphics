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
    CameraObject    camera(root);
    Object          knotObject(root);
    Object          sphereObject(root);
    Object          cubeObject(root);
    Object          lightObject(camera);

    camera.setPerspective(radians(45.0f), 0.1f, 1000.0f);

    knotObject   .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    sphereObject .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    cubeObject   .setTransform(scale(translate(mat4(1.0), vec3(0, -20, 0)), vec3(500.0, 0.1, 500)));
    lightObject  .lookAt(vec3(10,10,10), vec3(0,-10,-2), vec3(0,1,0));

    Renderable knotRenderable(knotObject);
    knotRenderable.mesh.knot(5, 2, 200, 100);
    knotRenderable.material.roughness   = 0.5;
    knotRenderable.material.metallic    = 0.9;

    Renderable sphereRenderable(sphereObject);
    sphereRenderable.mesh.sphere(5, 100, 100);
    sphereRenderable.material.roughness     = 0.3;
    sphereRenderable.material.metallic      = 0.1;
    sphereRenderable.material.albedo  = vec3(0.7,0,0);

    Renderable cubeRenderable(cubeObject);
    cubeRenderable.mesh.cube(1);
    cubeRenderable.material.albedo      = vec3(1,1,1);
    cubeRenderable.material.roughness   = 0.1;
    cubeRenderable.material.metallic    = 0.3;
    
    camera.lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    Animator cameraRotation(scheduler, [&](double time) {
        // mat4 fromParent = lookAt(vec3(0, 0, 50), vec3(0, 0, 0), vec3(0, 1, 0));
        // fromParent = rotate(fromParent, radians((float)time * 40.0f), vec3(0.0f, 0.0f, 1.0f));
        // fromParent = rotate(fromParent, radians((float)time * 50.0f), vec3(1.0f, 0.0f, 0.0f));
        // camera.setTransform(inverse(fromParent));

        mat4 m = rotate(mat4(1.0), radians((float)time * 40.0f), vec3(0.0f, 0.0f, 1.0f));
        m = rotate(m, radians((float)time * 50.0f), vec3(1.0f, 0.0f, 0.0f));
        knotObject.setTransform(m);
    });

     Animator sphereRotation(scheduler, [&](double time) {
        mat4 toParent = rotate(mat4(1.0), radians((float)time * 120.0f), vec3(0.0f, 1.0f, 0.0f));
        sphereObject.setTransform(toParent);
    });

    Viewport viewport = Viewport(scheduler);
    viewport.attachCamera       (camera);
    viewport.attachRenderable   (knotRenderable);
    viewport.attachRenderable   (sphereRenderable);
    viewport.attachRenderable   (cubeRenderable);
    viewport.attachLight        (lightObject);

    cameraRotation.start();
    sphereRotation.start();

    scheduler.run();

    return 0;
};