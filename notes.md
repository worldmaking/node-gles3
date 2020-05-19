
Core idea: present native openGL (ES3) capabilities to node.js using the WebGL2 API.

Why native GL? Native performance, ability to create multiple windows/contexts with fully shared CPU and GPU memory, etc. for applications that require other native capabilities and do not need a DOM or other browser-centric capabilities. 

Why WebGL2 API? A fairly stable API that makes a decent fit between GLES3 and the javascript language without being strange, + ability to prototype in a browser and copy code directly into native app.

Compare:
- https://github.com/node-3d/webgl-raub / https://github.com/node-3d/glfw-raub has a similar approach, but webgl1 only, and uses Document/Canvas interface that isn't really needed (and is part of a bigger Node3D project that has a lot of other things going on)
- https://www.npmjs.com/package/exokit 

Completeness is tested from the GLES3 headers



{
  "targets": [{
    "target_name": "gles3",
    "sources": [ "src/gles3.cpp" ],
	  'include_dirs': [
		  'src', 'src/include'
	  ],
	  'conditions': [
        ['OS=="mac"',
          {
            'libraries': ['-lGLEW','-framework OpenGL'],
            'include_dirs': ['./node_modules/native-graphics-deps/include'],
            'library_dirs': ['./node_modules/native-graphics-deps/lib/macos/glew'],
            'cflags':["-Wno-unused-but-set-variable","-Wno-unused-parameter","-Wno-unused-variable"]
          }
        ],
        ['OS=="linux"', {
          'libraries': [
            '-lGLEW','-lGL']
          }
        ],
        ['OS=="win"',
          {
            'include_dirs': [
              './node_modules/native-graphics-deps/include',
              ],
            'library_dirs': [
              'lib/<(target_arch)',
              ],
            'libraries': [
              'glew32.lib',
              'openvr_api.lib',
              'opengl32.lib',
              'glfw3dll.lib'
              ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN'
            ],
            'msvs_settings' : {
              'VCCLCompilerTool' : {
                'AdditionalOptions' : ['/O2','/Oy','/GL','/GF','/Gm-','/EHsc','/MT','/GS','/Gy','/GR-','/Gd']
              },
              'VCLinkerTool' : {
                'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
              },
            },
            'copies': [
              {
                'destination': './build/Release/',
                'files': [
                  './lib/<(target_arch)/glew32.dll',
                  './lib/<(target_arch)/openvr_api.dll',
                  './lib/<(target_arch)/glfw3.dll'
                 ]
              }
            ],
          }
        ],
      ],
    }
  ]
}