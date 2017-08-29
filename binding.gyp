{
  "targets": [
    {
      "target_name": "multicore-webpack",
      "sources": [ "src/multicore.c" ],
      "type": "executable",
      "conditions": [
        ['OS=="win"', {}, {
          "cppflags": [
          ],
          "cflags": [
            "-D_FORTIFY_SOURCE=2",
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Wformat=2",
            "-Wformat-security",
            "-pedantic",
            "-Wshadow",
            "-Wstrict-aliasing=2",
            "-fstrict-aliasing",
            "-O0",
            "-g3"
          ]
        }]
      ]
    }
  ]
}
