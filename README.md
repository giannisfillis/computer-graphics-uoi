# Computer Graphics - Academic Year 2024-2025

[![Course](https://img.shields.io/badge/Course-Computer_Graphics-blue.svg)]()
[![Institution](https://img.shields.io/badge/Institution-University_of_Ioannina-red.svg)]()
[![Language](https://img.shields.io/badge/Language-C++_%7C_C%23-orange.svg)]()
[![Framework](https://img.shields.io/badge/Framework-OpenGL_%7C_Unity3D-lightgrey.svg)]()

This repository contains the programming assignments and projects for the **Computer Graphics and Interaction Systems** course at the University of Ioannina, Department of Computer Engineering and Informatics. 

The repository is divided into two main parts: a three-part low-level graphics pipeline implementation using **C++ and OpenGL**, and a high-level 3D game development project using **Unity 3D**.

**Team Members:**
* Giannis Fillis, AM: 5380
* Konstantinos Zois, AM: 5226

---

## 📖 Table of Contents
1. [Project 1A: 2D Maze & Basic Rendering (OpenGL)](#project-1a-2d-maze--basic-rendering-opengl)
2. [Project 1B: 3D Maze & Camera Implementation (OpenGL)](#project-1b-3d-maze--camera-implementation-opengl)
3. [Project 1C: Treasure Hunt & Lighting (OpenGL)](#project-1c-treasure-hunt--lighting-opengl)
4. [Project 2: Treasure Bob Game (Unity 3D)](#project-2-treasure-bob-game-unity-3d)
5. [Compilation and Execution](#compilation-and-execution)

---

## 🟩 Project 1A: 2D Maze & Basic Rendering (OpenGL)

### Overview
The first assignment introduces basic graphics programming using OpenGL 3.3+. The objective is to render a 2D procedural maze and a controllable player character from scratch using basic geometric primitives (triangles).

### Implementation Details
* **Technologies:** C++, OpenGL (GLEW, GLFW, GLM).
* **Environment:** 750x750 window with a solid black background.
* **Maze Generation:** The maze is mapped via a 2D binary array (`1` for walls, `0` for paths). Blue square walls are constructed manually by calculating vertex coordinates and rendering them as combined triangles on the 2D plane (Z=0).
* **Player Movement:** The player (Character 'A') is a small square that navigates the maze using the `I`, `J`, `K`, `L` keys.
* **Collision Detection:** Strict boundary and wall collision logic prevents the player from passing through the blue walls or exiting the maze bounds.

---

## 🧊 Project 1B: 3D Maze & Camera Implementation (OpenGL)

### Overview
This project expands the 2D maze into a 3D environment, introducing depth, 3D transformations, and an interactive virtual camera.

### Implementation Details
* **3D Environment:** The window size is increased to 950x950. The maze walls are transformed into 3D blue cubes (constructed via grouped triangles), resting on the XY plane.
* **Player Character:** The player is updated to a 3D yellow cube that traverses the maze. Additional mechanics include teleportation (moving seamlessly from the exit back to the entrance and vice versa).
* **Interactive Camera:** Implementation of a custom view matrix. The camera starts at `(0, 0, 20)` looking towards the maze.
  * Pitch and Yaw rotation around the X-axis (`W`, `X`) and Y-axis (`Q`, `Z`).
  * Zoom in and zoom out targeted at the maze center (`+`, `-` on Numpad).

---

## 💰 Project 1C: Treasure Hunt & Lighting (OpenGL)

### Overview
The final OpenGL project transforms the 3D maze into a fully playable "Treasure Hunt" game, introducing texture mapping, timed events, and advanced lighting models.

### Implementation Details
* **Gameplay Mechanics:** A textured "Treasure" cube (`coins.jpg`) spawns at random, empty coordinates within the maze for a limited time. The player must navigate to touch the treasure before it vanishes. Upon collection, the treasure visibly shrinks and disappears.
* **Advanced Camera Panning:** Added X-axis panning (`G`, `H`) and Y-axis panning (`T`, `B`) to the existing rotation and zoom controls.

### 🌟 Bonus Features Implemented
* **Dynamic Textures:** The treasure randomly cycles through 3 different textures upon spawning.
* **Audio-Visual Feedback:** Integrated particle/visual effects and sound effects triggered upon collecting a treasure.
* **Phong Lighting Model:** Implemented the Phong reflection model entirely within the fragment shader, featuring a dynamic point light source located at `(10.0, 8.0, 4.0)`.
* **Light Controls:** Added 6 dedicated keyboard keys to manipulate the light source's position in real-time, mimicking the camera controls.

---

## 🎮 Project 2: Treasure Bob Game (Unity 3D)

### Overview
Moving away from low-level graphics, this project utilizes the Unity 3D engine to develop a complete interactive game called "Treasure Bob". 

### Implementation Details
* **Environment:** 1024x768 resolution. The game features a 100x100 textured floor (`floor.jpg`) and a 3D maze built with 10x10x10 blocks.
* **Character Controller:** The player controls "Treasure Bob", a sphere with a custom texture (`bob.jpg`). Movement is handled via the arrow keys or `I, J, K, L`. Physics constraints are applied to prevent Y-axis floating and wall phasing.
* **Collectibles & Hazards:**
  * **Treasures:** 3D cubes with varying textures (cherries, oranges, lemons) spawn randomly. Collecting them triggers a shrink animation before destroying the object.
  * **Death Traps:** Textured spheres (`death.jpg`) spawn dynamically. Touching them instantly kills the player and ends the game.
* **Camera System:** A fully decoupled free-look camera controlled via arrow keys (X/Z axis panning), `+/-` (Y-axis elevation), and `1` (camera rotation).

### 🌟 Bonus Features Implemented
* **Sound Effects:** Added specific audio cues for collecting treasures.
* **Speed Mechanics:** Implemented a 5-gear speed system allowing the player to increase or decrease Bob's movement speed dynamically.
* **Score System:** Fully functional UI score counter. Different treasure types (cherries, oranges, lemons) yield different point values.
* **Game Over State:** Implemented a UI Game Over screen that triggers upon hitting a death trap.

---

## 🚀 Compilation and Execution

### OpenGL Projects (1A, 1B, 1C)
These projects require a standard C++ compiler and the OpenGL libraries (GLFW, GLEW, GLM).
If using an IDE like Visual Studio, ensure the include and library paths for GLFW and GLEW are correctly configured in the project settings. 
For Linux/macOS via terminal:
```bash
# Example compilation command for Linux
g++ -o app Source-1C.cpp -lglfw -lGLEW -lGL
./app
```
### Unity Project (Project 2)
Ensure you have Unity installed. You can either run the project through the Unity Editor or execute the standalone build:
#### To run via Unity Editor:
1. Open the "Project2" folder via Unity Hub.
2. Navigate to the "Scenes" folder and open the main scene.
3. Press the "Play" button.

#### To run the standalone build (Windows):
Navigate to the build folder and double-click the executable, or run it via command line:
.\TreasureBob.exe
