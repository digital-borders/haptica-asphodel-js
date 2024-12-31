
{
  'targets': [
    {
      'target_name': 'haptica-asphodel-js',
      'sources': [ 'src/asp.cc' ],
      'include_dirs': [
          "<!@(node -p \"require('node-addon-api').include\")",
          "./asphodel-headers"
      ],
      'libraries': [
          # switch library to use here
          "<(module_root_dir)/asphodel/builds/linux/libasphodel.so"
      ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions', '-Wextrai' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}