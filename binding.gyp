{
  "targets": [
    {
      "target_name": "haptica-asphodel-js",
      "sources": [
        "src/asp.cc",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "./asphodel-headers"
      ],
      "conditions" : [
        [
          'OS=="mac"', {
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'CLANG_CXX_LIBRARY': 'libc++',
              'MACOSX_DEPLOYMENT_TARGET': '10.7'   
            },
            "link_settings": {
              "libraries": [
                "-Wl,-rpath,@loader_path"
              ]
            },
            "libraries": [
              "<(module_root_dir)/asphodel/builds/osx/arm64/libasphodel.dylib"
            ],
            "copies": [
              {
                "destination": "build/Release/",
                "files": [
                  "<!@(ls -1 asphodel/builds/osx/arm64/libasphodel.dylib)"
                ]
              }
            ]
          },
        ],
        [
          'OS=="win"', {
            "configurations": {
              "Release": {
                "msvs_settings": {
                  "VCCLCompilerTool": {
                    "ExceptionHandling": 1
                  }
                }
              }
            },
            "libraries": [
               "-l../asphodel/builds/windows/x64/Asphodel64.dll",
               "-l../asphodel/builds/windows/x64/Unpack64.dll",
                
            ],
            "copies": [
              {
                "destination": "build/Release",
                "files": [
                  "asphodel/builds/windows/x64/Asphodel64.dll",
                  "asphodel/builds/windows/x64/Unpack64.dll"
                ]
              }
            ]
          },
        ],
        [
          'OS=="linux"', {
            "conditions": [
              ['target_arch=="arm"', {
                "cflags_cc!": [
                  "-fno-rtti",
                  "-fno-exceptions"
                ],
                "cflags_cc": [
                  #"-std=c++11",
                  "-fexceptions"
                ],
                "link_settings": {
                  "libraries": [
                    "<@(module_root_dir)/build/Release/libasphodel.so",
                    "<@(module_root_dir)/build/Release/libunpack.so"
                  ],
                  "ldflags": [
                    "-L<@(module_root_dir)/build/Release",
                    "-Wl,-rpath,<@(module_root_dir)/build/Release"
                  ]
                },
                "copies": [
                  {
                    "destination": "build/Release/",
                    "files": [
                      "<@(module_root_dir)/asphodel/builds/linux/libasphodel.so",
                      "<@(module_root_dir)/asphodel/builds/linux/libunpack.so"
                    ]
                  }
                ]
              },
              { # ia32 or x64
                "cflags_cc!": [
                  "-fno-rtti",
                  "-fno-exceptions"
                 ],
                 "cflags_cc": [
                   #"-std=c++11",
                   "-fexceptions"
                 ],
                "link_settings": {
                  "libraries": [
                    "<@(module_root_dir)/build/Release/libasphodel.so",
                    "<@(module_root_dir)/build/Release/libunpack.so"
                  ],
                  "ldflags": [
                  "-L<@(module_root_dir)/build/Release",
                  "-Wl,-rpath,<@(module_root_dir)/build/Release"
                  ]
                },
                "copies": [
                  {
                    "destination": "build/Release/",
                    "files": [
                      "<@(module_root_dir)/asphodel/builds/linux/libasphodel.so",
                      "<@(module_root_dir)/asphodel/builds/linux/libunpack.so"
                    ]
                  }
                ]
              }]
            ]
          }
        ]
      ]
    }
  ]
}