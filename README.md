# node-gles3

A native (fast) WebGL2 emulator for node using native opengles3 windows, on OSX, Windows, and Linux.

The library also includes bindings to OpenVR for desktop VR support.

`
git clone https://github.com/worldmaking/node-gles3.git

npm install
`

## Why?

It's great to be able to port webgl2 code mostly unchanged to a native desktop context, where other non-browser features become available. This was important for us, for example, in overcoming bandwidth issues between C/C++ simulation and sensor code (which we now run in native Node.js modules) and webgl rendering. 

## Status

Most of the WebGL2 API is covered already, and any gaps found can be addressed. 

Several examples included demonstrate instanced multi-windows, rendering, distance-field fonts, VR, etc.

The windowing API is tied to GLFW currently, but may be abstracted further.

An effort is underway to see if libraries such as Three.js can run on this.
