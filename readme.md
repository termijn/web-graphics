# Emscripten-based webgl 2 renderer

A web renderer for gltf models. Built to experiment and learn webassembly and webgl 2. 

See the renderer in action at [Github Pages](https://termijn.github.io/index.html)

![Example rendering of the gltf sample helmet](/images/helmet.png)
![Example rendering of the gltf sample a beautiful game](/images/chess.png)

## Features
- Scene graph
- Clean API
- Softened dynamic shadow map using poisson disc sampling
- PBR lighting
- Normal maps
- Emmisive maps
- Metallic / Roughness maps
- Occlusion map
- gltf loading support
- gltf binary scene loading

## Planned features
- Mesh instancing
- Texture resource sharing
- Environment mapping
- Multiple lights and Light intensity / color
- HDR and tonemapping
- Picking
- BVH tree
- AABB intersection testing

## Getting started
Before opening vs code, first call
`PS C:\..\web\emsdk> .\emcmdprompt.bat`

## Tools
See https://floooh.github.io/2023/11/11/emscripten-ide.html
use Groove (windows) or Homebrew (mac OS) to install:
- git
- cmake
- ninja

## Dependencies
- glm (https://github.com/g-truc/glm/tree/master)
- WebGL
- SDL2

## VS Code extensions
- ms-vscode.cpptools
- ms-vscode.cmake-tools
- ms-vscode.wasm-dwarf-debugging
- ms-vscode.live-server
