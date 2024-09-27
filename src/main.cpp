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

    camera.setPerspective(radians(45.0f), 0.1f, 100.0f);

    knotObject   .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    sphereObject .setTransform(translate(mat4(1.0), vec3(0.0, 0, 0)));
    cubeObject   .setTransform(scale(translate(mat4(1.0), vec3(0, 0, 0)), vec3(20.0, 20, 0.1)));
    lightObject  .lookAt(vec3(10,10,10), vec3(0,0,-2), vec3(0,1,0));

    Renderable knotRenderable(knotObject);
    knotRenderable.mesh.knot(5, 2, 200, 100);
    knotRenderable.material.roughness   = 0.3;
    knotRenderable.material.metallic    = 0.7;

    Renderable sphereRenderable(sphereObject);
    sphereRenderable.mesh.noisySphere(5, 100, 100, 2);
    //sphereRenderable.mesh.sphere(5, 100, 100);
    sphereRenderable.material.roughness     = 0.8;
    sphereRenderable.material.metallic      = 0.3;
    sphereRenderable.material.albedo  = vec3(0.7,0,0);

    Renderable cubeRenderable(cubeObject);
    cubeRenderable.mesh.cube(1);
    cubeRenderable.material.roughness   = 1.0;
    cubeRenderable.material.metallic    = 0.3;
    
    camera.lookAt(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    Animator cameraRotation(scheduler, [&](double time) {
        mat4 fromParent = lookAt(vec3(0, 0, 50), vec3(0, 0, 0), vec3(0, 1, 0));
        fromParent = rotate(fromParent, radians((float)time * 30.0f), vec3(0.0f, 0.0f, 1.0f));
        fromParent = rotate(fromParent, radians((float)time * 40.0f), vec3(1.0f, 0.0f, 0.0f));
        camera.setTransform(inverse(fromParent));
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