# ikulab motion viwer

BVH Motion Viewer Using [ikura](https://github.com/ikulab/ikura)

![Peek 2022-10-12 15-24](https://user-images.githubusercontent.com/48763656/195266038-dc9b9f62-c2c4-45ba-8026-d746cb16a5ea.gif)

## Implemented Features

- Visualize BVH file
- Move viewpoint(camera) by mouse
  - Rotation / Shift / Zoom in / Zoom out
- Animation control window
  - Stop / Resume animation
  - Go to next / prev frame
  - Go to beggin / end frame
  - Modifying animation speed
- Select BVH file by file select dialog

## Future Features

- Load multiple BVH files
- Select bone by mouse click
- Modify BVH file (i.g. split)
- etc...

## License

MIT License

## External Dependencies

This software uses the following external libraries:

- Vulkan Loader
  - Khronos Group
  - Apache License Version 2.0
    - https://github.com/KhronosGroup/Vulkan-Loader/blob/master/LICENSE.txt

- Vulkan-ValidationLayers
  - Khronos Group
  - Apache License Version 2.0
    - https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/LICENSE.txt

- VulkanMemoryAllocator
  - Copyright (c) 2017-2022 Advanced Micro Devices, Inc.
  - MIT License
    - https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/blob/master/LICENSE.txt

- GLFW
  - Copyright (c) 2002-2006 Marcus Geelnard
  - Copyright (c) 2006-2019 Camilla Löwy
  - zlib/libpng License
    - https://www.glfw.org/license.html

- GLM
  - Copyright (c) 2005 - G-Truc Creation
  - The Happy Bunny License (Modified MIT License)
    - https://github.com/g-truc/glm/blob/master/manual.md#section0

- Dear ImGui
  - Copyright (c) 2014-2022 Omar Cornut
  - MIT License
    - https://github.com/ocornut/imgui/blob/master/LICENSE.txt

- Shaderc
  - Google
  - Apache License Version 2.0 
    - https://github.com/google/shaderc/blob/main/LICENSE

- tiny file dialogs
  - zlib/libpng License
  - https://sourceforge.net/projects/tinyfiledialogs
