# Emscriptem-based webgl 2 renderer

A web renderer for gltf models. Built to experiment and learn webassembly and webgl 2. 

See the renderer in action at [Github Pages](https://termijn.github.io/index.html)

![A knot mesh](/images/screenshot.png)

## Features: 
- Scene graph
- Softened dynamic shadow map
- PBR lighting
- gltf loading support

## Ideas
- Screen space ambient occlusion
- Improved quality PCSS shadows
- User interaction (roll, zoom, pan)
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
