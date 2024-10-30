#include "loader.h"

#include "renderable.h"

#include <iostream>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

std::map<int, Image>    imageCache;
std::map<int, Mesh>     meshCache;

Image getImage(int i, std::function<Image(void)> creator)
{
    if (imageCache.find(i) == imageCache.end())
        imageCache.emplace(std::make_pair(i, creator()));

    return imageCache[i];
}

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
            transform *= glm::toMat4(rotation);
        }

        if (node.scale.size() == 3) {
            glm::vec3 scale = make_vec3(node.scale.data());
            transform = glm::scale(transform, scale);
            std::cout <<  node.name << " has scale: " << scale.x << "," << scale.y << "," << scale.z << std::endl;
        }
    }
    return transform;
}


void fillVertices(const tinygltf::Model& model, tinygltf::Primitive& primitive, Mesh& mesh, glm::mat4 nodeTransform)
{
    vec4 max = vec4(-100000);
    vec4 min = vec4(100000);
    
    const tinygltf::Accessor* normalAccessor        = nullptr;
    const tinygltf::BufferView* normalBufferView    = nullptr;
    const tinygltf::Buffer* normalBuffer            = nullptr;
    const unsigned char* normalData                 = nullptr;
    size_t normalStride = 0;
    
    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) 
    {
        int normalAccessorIndex = primitive.attributes.at("NORMAL");
        normalAccessor          = &model.accessors[normalAccessorIndex];
        normalBufferView        = &model.bufferViews[normalAccessor->bufferView];
        normalBuffer            = &model.buffers[normalBufferView->buffer];
        normalData              = &normalBuffer->data[normalBufferView->byteOffset + normalAccessor->byteOffset];
        normalStride            = normalAccessor->ByteStride(*normalBufferView);
        if (normalStride == -1) normalStride = sizeof(vec3); // Default to vec3 size
    }

    const tinygltf::Accessor* texCoordAccessor        = nullptr;
    const tinygltf::BufferView* texCoordBufferView    = nullptr;
    const tinygltf::Buffer* texCoordBuffer            = nullptr;
    const unsigned char* texCoordData                 = nullptr;
    size_t texCoordStride = 0;

     if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
        int texCoordAccessorIndex   = primitive.attributes.at("TEXCOORD_0");
        texCoordAccessor            = &model.accessors[texCoordAccessorIndex];
        texCoordBufferView          = &model.bufferViews[texCoordAccessor->bufferView];
        texCoordBuffer              = &model.buffers[texCoordBufferView->buffer];
        texCoordData                = &texCoordBuffer->data[texCoordBufferView->byteOffset + texCoordAccessor->byteOffset];
        texCoordStride              = texCoordAccessor->ByteStride(*texCoordBufferView);
        if (texCoordStride == 0) texCoordStride = sizeof(vec2); // Default to vec3 size
    }

    const tinygltf::Accessor& accessor      = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView& bufferView  = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer          = model.buffers[bufferView.buffer];
    
    const unsigned char* dataPtr    = &buffer.data[bufferView.byteOffset + accessor.byteOffset];
    size_t vertexCount              = accessor.count;
    size_t positionStride           = accessor.ByteStride(bufferView);

    mesh.vertices().reserve(vertexCount);
    for (size_t i = 0; i < vertexCount; ++i) 
    {
        // Vertex position
        const float* position = reinterpret_cast<const float*>(dataPtr + i * positionStride);
        glm::vec3 vertexPosition(position[0], position[1], position[2]);

        // Apply node transformation to vertex position
        glm::vec4 transformedPosition = nodeTransform * glm::vec4(vertexPosition, 1.0f);

        glm::vec3 vertexNormal(0.0f);
        if (normalAccessor) 
        {
            const float* normal = reinterpret_cast<const float*>(normalData + i * normalStride);
            vertexNormal = glm::vec3(normal[0], normal[1], normal[2]);

            // Apply node transformation to normals (ignoring translation)
            glm::vec4 transformedNormal = transpose(inverse(nodeTransform)) * glm::vec4(vertexNormal, 0.0f);
            vertexNormal = glm::normalize(glm::vec3(transformedNormal));
        }

        glm::vec2 vertexTexCoord(0.0f);
        if (texCoordAccessor)
        {
            const float* texCoord = reinterpret_cast<const float*>(texCoordData + i * texCoordStride);
            vertexTexCoord = glm::vec2(texCoord[0], texCoord[1]);
        }

        mesh.vertices().emplace_back(Vertex{ vec4(transformedPosition), vec4(vertexNormal, 0.0f), vertexTexCoord });

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
        size_t triangleStride = bufferView.byteStride == 0 ? sizeof(glm::u32vec3) : bufferView.byteStride;

        mesh.indices().reserve(indexCount / 3);
        for (size_t i = 0; i < indexCount; i += 3) 
        {
            int index0, index1, index2;

            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) 
            {
                index0 = *reinterpret_cast<const uint16_t*>(dataPtr);
                index1 = *reinterpret_cast<const uint16_t*>(dataPtr + componentSize);
                index2 = *reinterpret_cast<const uint16_t*>(dataPtr + 2 * componentSize);
            } 
            else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) 
            {
                index0 = *reinterpret_cast<const uint32_t*>(dataPtr);
                index1 = *reinterpret_cast<const uint32_t*>(dataPtr + componentSize);
                index2 = *reinterpret_cast<const uint32_t*>(dataPtr + 2 * componentSize);
            }

            mesh.indices().emplace_back(vertexOffset + index0, vertexOffset + index1, vertexOffset + index2);
            dataPtr += componentSize * 3;
        }
    } 
    else 
    {
        std::cout << "Primitive has no indices; might be a non-indexed model." << std::endl;
    }
}

std::vector<vec2> getUVSet(int uvIndex, const tinygltf::Model& model)
{
    std::vector<vec2> result;

    const tinygltf::Accessor* texCoordAccessor        = nullptr;
    const tinygltf::BufferView* texCoordBufferView    = nullptr;
    const tinygltf::Buffer* texCoordBuffer            = nullptr;
    const unsigned char* texCoordData                 = nullptr;
    size_t texCoordStride = 0;

    int texCoordAccessorIndex   = uvIndex;
    texCoordAccessor            = &model.accessors[texCoordAccessorIndex];
    texCoordBufferView          = &model.bufferViews[texCoordAccessor->bufferView];
    texCoordBuffer              = &model.buffers[texCoordBufferView->buffer];
    texCoordData                = &texCoordBuffer->data[texCoordBufferView->byteOffset + texCoordAccessor->byteOffset];
    texCoordStride              = texCoordAccessor->ByteStride(*texCoordBufferView);
    if (texCoordStride == 0) texCoordStride = sizeof(vec2);

    int nrCoords = texCoordBufferView->byteLength / texCoordStride;
    std::cout << "nrUVCoords = " << nrCoords << std::endl;
    for (size_t i = 0; i < nrCoords; ++i) 
    {
        glm::vec2 vertexTexCoord(0.0f);
        if (texCoordAccessor)
        {
            const float* texCoord = reinterpret_cast<const float*>(texCoordData + i * texCoordStride);
            result.emplace_back(texCoord[0], texCoord[1]);
        }
    }
    return result;
}

TextureTransforms getTextureTransforms(const tinygltf::TextureInfo& textureInfo)
{
    TextureTransforms result;
    if (textureInfo.extensions.contains("KHR_texture_transform"))
    {
        auto extension = textureInfo.extensions.find("KHR_texture_transform");

        std::cout << "Extension found: " << extension->first << std::endl;

        const auto& transform = extension->second;

        if (transform.Has("offset")) 
        {
            std::cout << "it has offset" << std::endl;
            const auto& offsetArray = transform.Get("offset");
            if (offsetArray.IsArray() && offsetArray.ArrayLen() == 2) 
            {
                glm::vec2 offset(
                    offsetArray.Get(0).Get<double>(),
                    offsetArray.Get(1).Get<double>()
                );
                std::cout << "Texture offset: x = " << offset.x << ", y = " << offset.y << std::endl;
                result.offset = offset;
            }
        }
        
        if (transform.Has("scale")) 
        {
            std::cout << "it has scale" << std::endl;
            const auto& scaleArray = transform.Get("scale");
            if (scaleArray.IsArray() && scaleArray.ArrayLen() == 2) 
            {
                glm::vec2 scale(
                    scaleArray.Get(0).Get<double>(),
                    scaleArray.Get(1).Get<double>()
                );
                std::cout << "Texture scale: x = " << scale.x << ", y = " << scale.y << std::endl;
                result.scale = scale;
            }
        }

        if (transform.Has("texCoord"))
        {
             const auto& texCoord = transform.Get("texCoord");
             int uvIndex = texCoord.GetNumberAsInt();

             std::cout << "it overrides texCoord with " << uvIndex << std::endl;
             result.uvSet = uvIndex;
        }
    }
    return result;
}

Image convertTo8bits(Image& image)
{
    std::vector<uint8_t> newPixels(image.bytes() / 2);

    int nrBytes = newPixels.size();
    for (int i = 0; i < nrBytes; i++)
        newPixels[i] = image.getPixels()[i * 2];

    Image result(newPixels);
    return result;
}

template <typename T>
Image getImage(const T& textureInfo, const tinygltf::Model& model)
{
    Image img;
    if (textureInfo.index >= 0)
    {
        img = getImage(textureInfo.index, [&]
        {
            
            const tinygltf::Texture& texture    = model.textures[textureInfo.index];
            const tinygltf::Image& image        = model.images[texture.source];

            Image newImage(image.image);
            newImage.width           = image.width;
            newImage.height          = image.height;
            newImage.bytesPerPixel   = image.component * (image.bits / 8);

            std::cout << "component: " << image.component << std::endl;
            std::cout << "bits: " << image.bits << std::endl;

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE && image.component == 4)
            {
                newImage.bytesPerPixel   = 4;
                newImage.type            = Image::Type::RGBA;
                std::cout << "TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE" << std::endl;
            }

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT && image.component == 4)
            {
                newImage = convertTo8bits(newImage);
                newImage.bytesPerPixel   = 4;
                newImage.type            = Image::Type::RGBA;
                std::cout << "TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT" << std::endl;
            }

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE && image.component == 2)
            {
                newImage.type = Image::Type::RG8;
                std::cout << "TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE" << std::endl;
            }

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE && image.component == 1)
            {
                newImage.type = Image::Type::R8;
                std::cout << "TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE" << std::endl;
            }

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE && image.component == 3)
            {
                newImage.type = Image::Type::RGB;
                std::cout << "TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE" << std::endl;
            }

            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_FLOAT)
                std::cout << "TINYGLTF_COMPONENT_TYPE_FLOAT" << std::endl;
            if (image.pixel_type == TINYGLTF_COMPONENT_TYPE_DOUBLE)
                std::cout << "TINYGLTF_COMPONENT_TYPE_DOUBLE" << std::endl;

            std::cout << "image width "     << newImage.width << std::endl;
            std::cout << "image height "    << newImage.height << std::endl;
            std::cout << "image bpp "       << newImage.bytesPerPixel << std::endl;
            return newImage;
        });
    }

    return img;
}

void fillMaterial(const tinygltf::Model& model, int materialIndex, Material& material)
{
    if (materialIndex < 0 || materialIndex >= model.materials.size()) {
        std::cerr << "Invalid material index." << std::endl;
        return;
    }

    const tinygltf::Material& gltfMaterial = model.materials[materialIndex];
    std::cout << "Material: " << gltfMaterial.name << std::endl;

    auto baseColorFactor = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
    if (!baseColorFactor.empty()) 
        material.albedo = glm::vec4(baseColorFactor[0], baseColorFactor[1], baseColorFactor[2], baseColorFactor[3]);

    auto& pbr = gltfMaterial.pbrMetallicRoughness;

    material.metallic = pbr.metallicFactor;
    material.roughness = pbr.roughnessFactor;

    // Load Base Color Texture
    if (pbr.baseColorTexture.index >= 0)
    {
        material.baseColorTransforms = getTextureTransforms(pbr.baseColorTexture);
        material.baseColorTexture = getImage(pbr.baseColorTexture, model);

        if (material.baseColorTransforms.uvSet >= 0)
        {
            std::cout << "material.baseColorTransforms.uvSet >= 0" << std::endl;
            material.uvSet = getUVSet(material.baseColorTransforms.uvSet, model);
        }
    }

    if (pbr.metallicRoughnessTexture.index >= 0)
    {     
        material.metallicRoughnessTransforms = getTextureTransforms(pbr.metallicRoughnessTexture);
        material.metallicRoughness = getImage(pbr.metallicRoughnessTexture, model);
    }

    if (gltfMaterial.normalTexture.index >= 0)
    {
        std::cout << "Loading normal map" << std::endl;
        material.normalMap = getImage(gltfMaterial.normalTexture, model);
        material.normalMapScale = gltfMaterial.normalTexture.scale;
    }

    if (gltfMaterial.occlusionTexture.index >= 0)
    {
        std::cout << "Loading occlusion map" << std::endl;
        material.occlusion = getImage(gltfMaterial.occlusionTexture, model);
    }

    if (gltfMaterial.emissiveTexture.index >= 0)
    {
        std::cout << "Loading emmisive texture" << std::endl;
        material.emissive = getImage(gltfMaterial.emissiveTexture, model);
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
            int offset = mesh.vertices().size();
            fillVertices(model, primitive, mesh, nodeTransform);
            fillIndices(model, primitive, mesh, offset);

            mesh.generateTangentVectors();
        }
    }
    
    for (const auto& childIndex : node.children)
        traverseNodes(model, model.nodes[childIndex], mesh, nodeTransform);
}

void traverseNodes(tinygltf::Model& model, const tinygltf::Node& node, Scene& scene, Object& parent, mat4 nodeTransform)
{
    std::cout << "Node: " << node.name << std::endl;

    mat4 transform                          = getNodeTransformation(node);
    std::unique_ptr<RenderableObject>& item = scene.emplace_back(std::make_unique<RenderableObject>(parent));
    item->getObject().setTransform(getNodeTransformation(node));

    if (node.mesh >= 0)
    {
        tinygltf::Mesh& nodeMesh    = model.meshes[node.mesh];

        if (meshCache.contains(node.mesh))
        {            
            item->getRenderable().mesh = meshCache[node.mesh];
        } 
        else 
        {
            for (auto& primitive: nodeMesh.primitives)
            {
                Mesh& mesh = item->getRenderable().mesh;
                fillVertices (model, primitive, mesh, mat4(1.0));
                fillIndices  (model, primitive, mesh, 0);
                mesh.generateTangentVectors();
                meshCache[node.mesh] = mesh;
            }
        }

        Mesh& mesh = item->getRenderable().mesh;
        for (auto& primitive: nodeMesh.primitives)
        {
            if (primitive.material >= 0)
            {
                Material& material = item->getRenderable().material;
                fillMaterial(model, primitive.material, material);
                if (material.uvSet.has_value())
                {
                    std::cout << "overriding uvset " << std::endl;
                    int i = 0;
                    for (Vertex& vertex: mesh.vertices())
                        vertex.uv = material.uvSet.value()[i++];
                }
            }
        }
    }
    Object& object = item->getObject();
    for (const auto& childIndex : node.children)
        traverseNodes(model, model.nodes[childIndex], scene, object, nodeTransform);
}

std::optional<tinygltf::Model> loadGlTFModel(const std::string& filePath)
{
    std::cout << "Loading glTF model from " << filePath << std::endl;

    tinygltf::TinyGLTF  loader;
    tinygltf::Model     model;
    std::string         errors;
    std::string         warnings;
    
    bool loaded = loader.LoadBinaryFromFile(&model, &errors, &warnings, filePath);
    
    std::cout << "errors:"      << errors   << std::endl;
    std::cout << "warnings:"    << warnings << std::endl;

    return loaded ? std::optional<tinygltf::Model>(model) : std::nullopt;
}

Mesh loadModel(const std::string& filePath)
{
    imageCache.clear();
    std::optional<tinygltf::Model>  loadResult = loadGlTFModel(filePath);

    if (!loadResult.has_value()) return Mesh();

    tinygltf::Model& model = loadResult.value();

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

std::unique_ptr<Scene> loadModelObjects(const std::string &filePath, Object &parent)
{
    imageCache.clear();
    auto result = std::make_unique<Scene>();

    std::optional<tinygltf::Model>  loadResult = loadGlTFModel(filePath);
    if (!loadResult.has_value()) return result;

    tinygltf::Model& model = loadResult.value();
    mat4 transform = mat4(1.0);

    for (tinygltf::Scene& scene: model.scenes)
    {
        for (int nodeIndex : scene.nodes)
        {
            tinygltf::Node& node = model.nodes[nodeIndex];
            std::cout << "traversing node " << node.name << std::endl;
            traverseNodes(model, node, *result, parent, transform);
        }
    }

    return result;
}

RenderableObject::RenderableObject(const Object& parent)
    : object(parent)
    , renderable(object)
{
}

RenderableObject::~RenderableObject() 
{

}

Object &RenderableObject::getObject()
{
    return object;
}

Renderable &RenderableObject::getRenderable()
{
    return renderable;
}
