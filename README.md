# Pong

This repo contains the source code to my implementation of the game Pong.

## Supported Platforms

* Linux
* Windows

## Assets

Assets can be found in the `./assets` directory. Asset files (`.v`) are a collection of verticies, comma seperated. Below is an example.

```
0.0,0.0,
0.0,5.0,
5.0,5.0,
5.0,5.0,
5.0,0.0,
0.0,0.0,
```

This will render a 5x5 square. verticies are defined by x and y points on their own lines. They do not have to be on their own lines, but it is easier to visualize when they are. Verticies **MUST** be in X, Y order, and move in a clockwise direction.

## Shaders

Shaders can be found in the `./shaders` directory. 

## Dependencies

#### Glad

"Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs."

Glad lets us configure OpenGL easily across multiple platforms.

[Website](https://github.com/Dav1dde/glad)

#### GLFW

"GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop. It provides a simple API for creating windows, contexts and surfaces, receiving input and events."

[Website](https://www.glfw.org/)

#### GLM

"OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications."

[Website](https://github.com/g-truc/glm)

## Getting Running 

1) Clone the repo.
2) `git submodule init`
3) `git submodule update`
4) `cmake .`
5) `cmake --build .`

You might be missing some libs/packages, but the error messages provided by your build system should be enough to get you there.

## TODO / Notes

Morgan - We should be rendering "like" assets right after each other so we dont have to swap VAO's for each object we render. "like" assets should share a VAO pretty much. Honestly we could just create one VAO per asset type. that would be pretty shnazy as well.