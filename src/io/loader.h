#pragma once

#include "mesh.h"
#include "objects/object.h"
#include "renderable.h"

#include <memory>
#include <string>
#include <map>

class RenderableObject
{
public:
    RenderableObject(const Object& parentObject);

    Object&     getObject();
    Renderable& getRenderable();

private:
    Object      object;
    Renderable  renderable;
};

using Scene = std::vector<std::unique_ptr<RenderableObject>>;

// Loads an entire gltf model in a single mesh
Mesh loadModel(const std::string& filePath);

// Loads the gltf model in a scene graph under parent, respecting the individual components in the model
std::unique_ptr<Scene> loadModelObjects(const std::string &filePath, Object &parent);