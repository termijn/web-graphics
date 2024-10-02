# Emscripten-based webgl 2 renderer

A web renderer displaying and interacting with 3d scenes. Built to experiment and learn webassembly and webgl 2. 

See the renderer in action at [Github Pages](https://termijn.github.io/index.html)

![A knot mesh](/images/screenshot.png)

## Features: 
- Scene graph
- Softened shadow map
- PBR lighting

## Ideas
- Screen space ambient occlusion
- gltf loading support
- Higher quality soft shadows
- User interaction (roll, zoom, pan)
- Picking
- Color / texture supports
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
