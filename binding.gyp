{
  "targets": [
    {
      "target_name": "multicore-webpack",
      "sources": [ "src/multicore.c" ],
      "type": "executable",
      "conditions": [
        ['OS=="win"', {}, {
          "cflags": [
            "-std=c11",
            "-Wall",
            "-Wextra",
            "-Wformat=2",
            "-Wformat-security",
            "-pedantic",
            "-O3",
            "-g"
          ]
        }]
      ]
    }
  ]
}
