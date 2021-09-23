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
                'destination': './build/<(CONFIGURATION_NAME)/',
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
    },
    {
        "target_name": "audio",
        "sources": [ "src/node-audio.cpp" ],
        "defines": [],
        "cflags": ["-std=c++11", "-Wall", "-pedantic"],
        'cflags!': [ '-fno-exceptions' ],
        'cflags_cc!': [ '-fno-exceptions' ],
        "include_dirs": [ 
          "<!(node -p \"require('node-addon-api').include_dir\")",
          "src/miniaudio.h" 
        ],
        "libraries": [],
        "dependencies": [],
        "conditions": [
            ['OS=="win"', {
              'msvs_settings': {
                'VCCLCompilerTool': { 'ExceptionHandling': 1 },
              }
            }],
            ['OS=="mac"', {
              'cflags+': ['-fvisibility=hidden'],
              'xcode_settings': {
                'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                'CLANG_CXX_LIBRARY': 'libc++',
                'MACOSX_DEPLOYMENT_TARGET': '10.7',
                'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES', # -fvisibility=hidden
              },
            }],
            ['OS=="linux"', {}],
        ],
    },
    {
        "target_name": "zed",
        "sources": [ "src/node-zed.cpp" ],
        "defines": [],
        "cflags": ["-std=c++11", "-Wall", "-pedantic"],
        "include_dirs": [ 
          "<!(node -p \"require('node-addon-api').include_dir\")",
          "$(CUDA_PATH)/include",
          "$(ZED_SDK_ROOT_DIR)/include"
        ],
        "libraries": [],
        "dependencies": [],
        "conditions": [
            ['OS=="win"', {
              'include_dirs': [],
              'library_dirs': [
                '$(ZED_SDK_ROOT_DIR)/lib',
              ],
              'libraries': [
                'sl_zed64.lib'
              ],
              'msvs_settings': {
                'VCCLCompilerTool': { 'ExceptionHandling': 1 }
              }
            }],
            ['OS=="mac"', {
              'cflags+': ['-fvisibility=hidden'],
              'xcode_settings': {},
            }],
            ['OS=="linux"', {}],
        ],
    },
    {
        "target_name": "realsense",
        "sources": [ "src/node-realsense.cpp" ],
        "defines": [],
        "cflags": ["-std=c++11", "-Wall", "-pedantic"],
        "include_dirs": [ 
          "<!(node -p \"require('node-addon-api').include_dir\")"
        ],
        "libraries": [],
        "dependencies": [],
        "conditions": [
            ['OS=="win"', {
              'include_dirs': [
                "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include"
              ],
              'library_dirs': [
                'C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64',
              ],
              'libraries': [
                '-lrealsense2.lib'
              ],
              'msvs_settings': {
                'VCCLCompilerTool': { 'ExceptionHandling': 1 }
              },
              "copies": [{
                'destination': './build/<(CONFIGURATION_NAME)/',
                'files': ['C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64\\realsense2.dll']
              }]
            }],
            ['OS=="mac"', {
              'cflags+': ['-fvisibility=hidden'],
              'xcode_settings': {},
            }],
            ['OS=="linux"', {}],
        ],
    }
  ]
}