#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "mesh.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <tinygltf-2.9.3/stb_image.h>
#include <tinygltf-2.9.3/stb_image_write.h>
#include <tinygltf-2.9.3/json.hpp>

#define TINYGLTF_HEADER_ONLY
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include <tinygltf-2.9.3/tiny_gltf.h>

using namespace glm;

glm::mat4 make_mat4(const double* matrixArray) {
    return glm::mat4(
        (float)matrixArray[0], (float)matrixArray[1], (float)matrixArray[2], (float)matrixArray[3],
        (float)matrixArray[4], (float)matrixArray[5], (float)matrixArray[6], (float)matrixArray[7],
        (float)matrixArray[8], (float)matrixArray[9], (float)matrixArray[10], (float)matrixArray[11],
        (float)matrixArray[12], (float)matrixArray[13], (float)matrixArray[14], (float)matrixArray[15]
    );
}

glm::vec3 make_vec3(const double* vecArray) 
{
    return glm::vec3(vecArray[0], vecArray[1], vecArray[2]);
}


glm::quat make_quat(const double* quatArray) {
    // GLM expects quaternions in the (w, x, y, z) format
    return glm::quat(static_cast<float>(quatArray[3]), static_cast<float>(quatArray[0]), 
                     static_cast<float>(quatArray[1]), static_cast<float>(quatArray[2]));
}

glm::mat4 getNodeTransformation(const tinygltf::Node& node) 
{
    glm::mat4 transform = glm::mat4(1.0);

    // If the node has a matrix, use it
    if (node.matrix.size() == 16) 
    {
        transform = make_mat4(node.matrix.data());
    } 
    else 
    {
        // Otherwise, apply translation, rotation, and scale (TRS)
        if (node.translation.size() == 3) {
            glm::vec3 translation = make_vec3(node.translation.data());
            transform = glm::translate(transform, translation);
        }

        if (node.rotation.size() == 4) {
            glm::quat rotation = make_quat(node.rotation.data());
            transform *= glm::toMat4(rotation);  // Apply rotation
        }

        if (node.scale.size() == 3) {
            glm::vec3 scale = make_vec3(node.scale.data());
            transform = glm::scale(transform, scale);

            std::cout <<  node.name << " has scale: " << scale.x << "," << scale.y << "," << scale.z << std::endl;
        }
    }

    return transform;
}


void fillVertices(const tinygltf::Model& model, tinygltf::Primitive& primitive, const tinygltf::Node& node, Mesh& mesh, glm::mat4 nodeTransform)
{
    const tinygltf::Accessor& accessor      = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView& bufferView  = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer          = model.buffers[bufferView.buffer];
    const unsigned char* dataPtr = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
    size_t vertexCount = accessor.count;
    size_t positionStride = accessor.ByteStride(bufferView);
    if (positionStride == 0) positionStride = 3 * sizeof(float); // Default to vec3 size

    vec4 max = vec4(-100000);
    vec4 min = vec4(100000);

    // Check if normals exist
    const tinygltf::Accessor* normalAccessor = nullptr;
    const tinygltf::BufferView* normalBufferView = nullptr;
    const tinygltf::Buffer* normalBuffer = nullptr;
    const unsigned char* normalData = nullptr;
    size_t normalStride = 0;
    
    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        int normalAccessorIndex = primitive.attributes.at("NORMAL");
        normalAccessor = &model.accessors[normalAccessorIndex];
        normalBufferView = &model.bufferViews[normalAccessor->bufferView];
        normalBuffer = &model.buffers[normalBufferView->buffer];
        normalData = &normalBuffer->data[normalBufferView->byteOffset + normalAccessor->byteOffset];
        normalStride = normalAccessor->ByteStride(*normalBufferView);
        if (normalStride == 0) normalStride = 3 * sizeof(float); // Default to vec3 size
    }

    for (size_t i = 0; i < vertexCount; ++i) 
    {
        // Vertex position
        const float* position = reinterpret_cast<const float*>(dataPtr + i * positionStride);
        glm::vec3 vertexPosition(position[0], position[1], position[2]);

        // Apply node transformation to vertex position
        glm::vec4 transformedPosition = nodeTransform * glm::vec4(vertexPosition, 1.0f);

        glm::vec3 vertexNormal(0.0f);
        if (normalAccessor) {
            const float* normal = reinterpret_cast<const float*>(normalData + i * normalStride);
            vertexNormal = glm::vec3(normal[0], normal[1], normal[2]);

            // Apply node transformation to normals (ignoring translation)
            glm::vec4 transformedNormal = transpose(inverse(nodeTransform)) * glm::vec4(vertexNormal, 0.0f);
            vertexNormal = glm::normalize(glm::vec3(transformedNormal));
        }

        mesh.vertices.emplace_back(Vertex{ glm::vec4(transformedPosition), glm::vec4(vertexNormal, 0.0f) });

        min = glm::min(min, transformedPosition);
        max = glm::max(max, transformedPosition);
    }

    std::cout << "min=" << min.x << "," << min.y << "," << min.z << " max=" << max.x << "," << max.y << "," << max.z << std::endl;
}

void fillIndices(const tinygltf::Model& model, tinygltf::Primitive& primitive, Mesh& mesh, int vertexOffset)
{
    if (primitive.indices >= 0) 
    {
        const tinygltf::Accessor&   indexAccessor   = model.accessors[primitive.indices];
        const tinygltf::BufferView& bufferView      = model.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer&     buffer          = model.buffers[bufferView.buffer];

        const uint8_t* dataPtr = &buffer.data[bufferView.byteOffset + indexAccessor.byteOffset];
        size_t indexCount = indexAccessor.count;
        size_t componentSize = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType);

        for (size_t i = 0; i < indexCount; i += 3) {
            // Assuming indices are stored as unsigned shorts (uint16)
            uint16_t index0, index1, index2;

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                index0 = *reinterpret_cast<const uint16_t*>(dataPtr + i * componentSize);
                index1 = *reinterpret_cast<const uint16_t*>(dataPtr + (i + 1) * componentSize);
                index2 = *reinterpret_cast<const uint16_t*>(dataPtr + (i + 2) * componentSize);
            } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                index0 = *reinterpret_cast<const uint32_t*>(dataPtr + i * componentSize);
                index1 = *reinterpret_cast<const uint32_t*>(dataPtr + (i + 1) * componentSize);
                index2 = *reinterpret_cast<const uint32_t*>(dataPtr + (i + 2) * componentSize);
            }

            mesh.indices.emplace_back(vertexOffset + index0, vertexOffset + index1, vertexOffset + index2);
        }
    } else {
        std::cout << "Primitive has no indices; might be a non-indexed model." << std::endl;
    }
}

void traverseNodes(tinygltf::Model& model, const tinygltf::Node& node, Mesh& mesh, mat4 nodeTransform)
{
    std::cout << "Node: " << node.name << std::endl;
    
    nodeTransform = nodeTransform * getNodeTransformation(node);

    if (node.mesh >= 0)
    {
        tinygltf::Mesh& nodeMesh = model.meshes[node.mesh];

        for (auto& primitive: nodeMesh.primitives)
        {
            int offset = mesh.vertices.size();
            fillVertices(model, primitive, node, mesh, nodeTransform);
            fillIndices(model, primitive, mesh, offset);
        }
    }
    
    for (const auto& childIndex : node.children)
        traverseNodes(model, model.nodes[childIndex], mesh, nodeTransform);
}

Mesh loadModel(const std::string& filePath)
{
    tinygltf::TinyGLTF  loader;
    tinygltf::Model     model;
    std::string         errors;
    std::string         warnings;
    bool loaded = loader.LoadBinaryFromFile(&model, &errors, &warnings, filePath);

    std::cout << "errors:"      << errors   << std::endl;
    std::cout << "warnings:"    << warnings << std::endl;

    if (!loaded) return Mesh();

    Mesh result;
    mat4 nodeTransform = mat4(1.0);

    for (tinygltf::Scene& scene: model.scenes)
    {
        for (int nodeIndex : scene.nodes)
        {
            tinygltf::Node& node = model.nodes[nodeIndex];
            std::cout << "traversing node " << node.name << std::endl;
            traverseNodes(model, node, result, nodeTransform);
        }
    }

    return result;
}