{
  "targets": [
    {
      "target_name": "gles3",
      "sources": [ "src/node-gles3.cpp" ],
      'include_dirs': [
        'src', 'src/include'
      ],
      'cflags':[],
      'conditions': [
        ['OS=="mac"',
          {
            'libraries': [
              '-lGLEW',
              '-framework OpenGL'
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
                "-Wno-unused-but-set-variable","-Wno-unused-parameter","-Wno-unused-variable","-Wno-int-to-void-pointer-cast"
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
              'lib/<(target_arch)',
              ],
            'libraries': [
              'glew32.lib',
              'opengl32.lib'
              ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN'
            ],
            'msvs_settings' : {
              'VCCLCompilerTool' : {
                'AdditionalOptions' : []
              },
              'VCLinkerTool' : {
                'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
              },
            },
            'copies': [
              {
                'destination': './build/<(CONFIGURATION_NAME)/',
                'files': [
                  './node_modules/native-graphics-deps/lib/windows/glew/glew32.dll'
                 ]
              }
            ],
          }
        ],
      ],
    },
    {
      "target_name": "glfw3",
      "sources": [ "src/node-glfw3.cpp" ],
      'include_dirs': [
        'src', 'src/include'
      ],
      'cflags':[],
      'conditions': [
        ['OS=="mac"',
          {
            'libraries': [
              '-framework Cocoa',
              '../node_modules/native-graphics-deps/lib/macos/glfw/libglfw3.a'
            ],
            'include_dirs': [
              './node_modules/native-graphics-deps/include'
            ],
            'library_dirs': [
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
          'libraries': []
          }
        ],
        ['OS=="win"',
          {
            'include_dirs': [
              './node_modules/native-graphics-deps/include',
              ],
            'library_dirs': [
              './node_modules/native-graphics-deps/lib/windows/glfw'
              ],
            'libraries': [
              'glfw3dll.lib'
            ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN'
            ],
            'msvs_settings' : {
              'VCCLCompilerTool' : {
                'AdditionalOptions' : []
              },
              'VCLinkerTool' : {
                'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
              },
            },
            'copies': [
              {
                'destination': './build/<(CONFIGURATION_NAME)/',
                'files': [
                  './node_modules/native-graphics-deps/lib/windows/glfw/glfw3.dll'
                 ]
              }
            ],
          }
        ],
      ],
    },
    
    {
      "target_name": "openvr",
      "sources": [ "src/node-openvr.cpp" ],
      'include_dirs': [
        './node_modules/native-graphics-deps/include',
        './node_modules/native-openvr-deps/headers',
        'src', 'src/include'
      ],
      "cflags": ["-std=c++11", "-Wall", "-pedantic"],
      'conditions': [
        ['OS=="mac"',
          {
            'libraries': [
            ],
            'library_dirs': [
            ],
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.13',
              'OTHER_CFLAGS': [
                "-Wno-unused-but-set-variable","-Wno-unused-parameter","-Wno-unused-variable","-Wno-int-to-void-pointer-cast"
              ],
            },
            'copies': [
              {
                'destination': './build/Release/',
                'files': [
                  './node_modules/native-openvr-deps/lib/osx32/libopenvr_api.dylib'
                 ]
              }
            ],
          }
        ],
        ['OS=="linux"', {
          'libraries': [
            '-lGLEW','-lGL']
          }
        ],
        ['OS=="win"',
          {
            'library_dirs': [
              './node_modules/native-openvr-deps/lib/win64',
            ],
            'libraries': [
              'openvr_api.lib'
              ],
            'defines' : [
              'WIN32_LEAN_AND_MEAN',
              'VC_EXTRALEAN'
            ],
            'msvs_settings' : {
              'VCCLCompilerTool' : {
                'AdditionalOptions' : []
              },
              'VCLinkerTool' : {
                'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
              },
            },
            'copies': [
              {
                'destination': './build/<(CONFIGURATION_NAME)/',
                'files': [
                  './node_modules/native-openvr-deps/bin/win64/openvr_api.dll'
                 ]
              }
            ],
          }
        ],
      ],
    }
  ]
}