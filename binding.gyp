{
  "targets": [
    {
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
              'opengl32.lib'
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
                  './lib/<(target_arch)/glew32.dll'
                 ]
              }
            ],
          }
        ],
      ],
    }
  ]
}