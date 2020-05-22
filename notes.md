
Core idea: present native openGL (ES3) capabilities to node.js using the WebGL2 API.

Why native GL? Native performance, ability to create multiple windows/contexts with fully shared CPU and GPU memory, etc. for applications that require other native capabilities and do not need a DOM or other browser-centric capabilities. 

Why WebGL2 API? A fairly stable API that makes a decent fit between GLES3 and the javascript language without being strange, + ability to prototype in a browser and copy code directly into native app.

Compare:
- https://github.com/node-3d/webgl-raub / https://github.com/node-3d/glfw-raub has a similar approach, but webgl1 only, and uses Document/Canvas interface that isn't really needed (and is part of a bigger Node3D project that has a lot of other things going on)
- https://www.npmjs.com/package/exokit 


# differences

The gl context is not created from a canvas, but loaded from a module

gl = canvas.getContext("webgl2"); => gl = require("./gles3.js")

Instead of canvas, it needs a rendering surface, such as a GLFW window

gl.canvas does not exist

'./lib/<(target_arch)/openvr_api.dll'
