[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/A98mEP3-)
# Virtual Museum Assignment

<p align="center">
  <img src="model images/Model1.jpeg" alt="Model1" width="150"/>
  <img src="model images/Model2.jpg" alt="Model2" width="150"/>
  <img src="model images/Model3.jpeg" alt="Model3" width="150"/>
  <img src="model images/Model4.jpg" alt="Model4" width="150"/>
  <img src="model images/Model5.jpg" alt="Model5" width="50"/>
</p>




**Team 8 Computer Engineering Virtual Museum Assignment**

This is our final project for the computer graphics course. We built a 3D virtual museum using C++ and OpenGL. In this app, you can control a robot, walk around the museum, and see 3D models of ancient objects from the Adana Museum. You can also read information about each object when the robot gets close to it.

### Team Members

| Name                   | Student ID   | Role                                  |
|------------------------|--------------|----------------------------------------|
| Ömer Faruk Dinçoğlu    | 2021556025   | Rendering, UI, Resource and Museum Management,Performance Optimization,Museum Scene design with Blender,Cmake |
| Hüsnü Önder Kabadayı   | 2021556039   | Modeling Artifacts with Blender, Textures, Camera and Controls |
| Ertuğrul Tetik         | 2021556063   | Robot Simulation, Lighting, Robot Modelling with blender,Controlling Robot,Robot Scanner Arm,Technical Report and Diagrams |

### Project Overview

We made a museum scene with 5 ancient artifacts. These are real objects from the Adana Museum. The idea is to give people a chance to explore a museum from their computer. The robot moves inside the museum, and when it gets close to an object, a small window shows details like the name, period, and material.

There is a lighting system with ambient light and spotlights. You can also see the shaders and models that are active. A command-line window shows the current FPS, your GPU, and what has been loaded. You can also run the app in fullscreen.

# Features:
 
   
Adjustable spotlights and global illumination

Five accurately modeled artifacts
   
Interactive Mobile Robot:
   - Keyboard-controlled movement (Arrow keys + Q/E)
   - Articulated scanning arm (R)
Educational Information System:
   - Info pop-ups appear when robot scans an artifact
   - Data includes name, period, material, and description
Usability-Focused UI:
   - ImGui-based control panels
   - Adjustable lighting, camera views, and debug options

# Additional Features

-Fullscreen(F11)
-FPS Display
-Cluster Optimization:We used clustering techniques to improve the frame rate. This way, the app runs faster even when many models are loaded.
-Cross-platform Support:Our project works on both Windows and Linux.
-Error Checking at Every Step:Our project checks for errors at each stage
-Supports Multiple Model Formats (.glb and .obj):Our project can load 3D models in both .glb and .obj formats.

### Controls

| Key / Input       | What It Does                                 |
|-------------------|----------------------------------------------|
| W / A / S / D     | Move the camera                              |
| Mouse Movement    | Look around with the camera                  |
| Arrow Keys        | Move the robot                               |
| R                 | Move the robot arm                           |
| Q / E             | Control and change robot rotation            |
| F11 / F           | Toggle fullscreen                            |
| Left Click        | Click on UI buttons                          |
| TAB               | Lock and hide the mouse                      |

### Tools and Libraries

- C++ programming language
- OpenGL for graphics
- Libraries: GLFW, GLAD, GLM, Assimp, stb_image, ImGui
- Blender for 3D modeling
- Visual Studio 2022 + CMake for building the project
- Git + GitHub for version control

### System Support

Tested on:
- Windows 10/11 (RTX 3060, 180Hz monitor)
- Ubuntu 22.04

### Performance Info

| Info               | Value                          |
|--------------------|--------------------------------|
| FPS (RTX 3060)     | 180 FPS                        |
| Scene Load Time    | ~30-35 sec (Windows), ~12 sec (Linux) |
| Memory Usage       | Under 350 MB                   |
| VSync              | Locks FPS to monitor refresh rate |

### Documents

- [See our technical report here] https://github.com/yuemco/virtual-adana-museum-v2-team-8/tree/main/TechnicalReport
                                               
  [See our technical diagrams here] https://github.com/yuemco/virtual-adana-museum-v2-team-8/tree/main/Docs/Diagrams


### How to Build

#### Linux (Ubuntu/Debian)

1. Install required packages:
```bash
sudo apt update
sudo apt install build-essential cmake git libglfw3-dev libassimp-dev libgl1-mesa-dev
```

2. Clone the project and go to folder:
```bash
git clone https://github.com/yuemco/virtual-adana-museum-v2-team-8
cd virtual-adana-museum-v2-team-8/Project1
```

3. Build the project:
```bash
mkdir build
cd build
cmake ..
make
```

4. Run it:
```bash
./Project1.exe
```

#### Windows

1. Install Visual Studio 2019 or newer with C++ tools.
2. Install CMake.
3. Clone and build the project:
```bash
git clone https://github.com/yuemco/virtual-adana-museum-v2-team-8
cd virtual-adana-museum-v2-team-8/Project1
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

4. Run the app:
```bash
Release/Project1.exe
```

### Screenshots & Video

For Windows=https://www.youtube.com/watch?v=B3McXQTyJMo
For Linux=https://www.youtube.com/watch?v=2UTruvvfl34

![eklenecek](https://github.com/user-attachments/assets/be5b9511-28cc-41a3-8036-25a9cd6e931c)
![curobot](https://github.com/user-attachments/assets/48555a2a-d1ac-47cc-81ae-26f067b9fdb1)


### Future Ideas

- Add VR support  
- Add a smarter robot guide  
- Make more museum rooms  
- Link to a database of artifacts

### Thanks

This project was created as a final assignment for the Computer Graphics course at Çukurova University.

