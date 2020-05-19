{
  "targets": [{
    "target_name": "gles3",
    "sources": [ "src/gles3.cpp" ],
	  'include_dirs': [
		  'src', 'src/include'
	  ],
    'cflags':[],
	  'conditions': [
        ['OS=="mac"',
          {
            'libraries': [
              '-lGLEW',
              '-framework OpenGL',
              '../node_modules/native-graphics-deps/lib/macos/glfw/libglfw3.a'
            ],
            'include_dirs': [
              './node_modules/native-graphics-deps/include'
            ],
            'library_dirs': [
              '../node_modules/native-graphics-deps/lib/macos/glew'
            ],
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.13',
              'OTHER_CFLAGS': [
                "-Wno-unused-but-set-variable","-Wno-unused-parameter","-Wno-unused-variable"
              ],
            }
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
              './node_modules/native-graphics-deps/lib/windows/glew',
              './node_modules/native-graphics-deps/lib/windows/glfw',
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
                  './node_modules/native-graphics-deps/lib/windows/glew/glew32.dll',
                  './lib/<(target_arch)/openvr_api.dll',
                  './node_modules/native-graphics-deps/lib/windows/glfw/glfw3.dll'
                 ]
              }
            ],
          }
        ],
      ],
    }
  ]
}