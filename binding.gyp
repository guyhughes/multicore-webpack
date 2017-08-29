{
  "target_defaults": {
    "default_configuration": 'Release',
    "configurations": {
      "Debug": {
        "defines": ["DEBUG", "_DEBUG", "WEBPACK=/usr/bin/true"],
        "cflags": ["-g", "-Og"],
        "xcode_settings": {
          "GCC_OPTIMIZATION_LEVEL": '0',
        }
      },
      "Release": {
        "defines": ["NDEBUG"],
        "cflags": ["-O3"],
        "xcode_settings": {
          "GCC_OPTIMIZATION_LEVEL": '3',
        }
      }
    },

  },
  "xcode_settings": {
    "OTHER_CFLAGS!": ['-fno-strict-aliasing'],
  },
  "cflags!": ['-fno-strict-aliasing'],
  "targets": [
    {
      "target_name": "multicore-webpack",
      "sources": [ "src/multicore.c" ],
      "type": "executable",
      "conditions": [
        ['OS=="mac"', {
          "xcode_settings": {
            "OTHER_CFLAGS!": ['-fno-strict-aliasing'],
          },
        }],
        ['OS=="win"', {}, {
          "defines": [
            "_FORTIFY_SOURCE=2",
          ],
          "cflags": [
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Wformat=2",
            "-Wformat-security",
            "-Wstrict-aliasing=2",
            "-pedantic",
            "-Wshadow",
            "-Wstrict-aliasing=2",
            "-fstrict-aliasing",
          ],
          "cflags!": ['-fno-strict-aliasing'],
        }],
      ]
    }
  ]
}
