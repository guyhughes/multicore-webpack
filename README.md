# multicore-webpack
multicore-webpack takes advantage of the multiple cores in your computer's
processor to get your bundles out as fast as possible by spinning up a child
process for each webpack config you target, and kills them all if one
fails.

Many webpack configs do so much file I/O and computationally intensive
operations to build your bundles, but Node.js is not designed to take advantage
of multicore processors.

```txt
usage: multicore-webpack NAME[,NAME...] ::: ARGS...

    NAME   The name argument of a webpack 3+ config.

    ARGS   The usual args you pass to webpack to build your config.
           This can include the --config option as well, if you do not use
           `webpack.config.js`.
```

##  Quickstart

webpack.config.js
```js
module.exports = [ require('./a.webpack.js'), require('b.webpack.js') ];
```

<table>
<tr>
<td>a.webpack.js</td>
<td>b.webpack.js</td>
</tr>
<tr>
<td>
<code>
module.exports = {
name: 'alpha',
      ...config
};
</code>

</td>
<td>
<code>
module.exports = {
name: 'bravo',
      ...config
};
</code>
</td>
</tr>
</table>

```bash
npm install -g multicore-webpack
multicore-webpack alpha,bravo ::: --env dev
```

## Dependencies
- webpack >= 3.0.0
- Runs on basically any POSIX-compatible system (BSD, GNU, Linux, macOS).
- Windows, cygwin and MinGW are unsupported. (PRs accepted!)
- A C compiler: clang and gcc tested.

## Limitations

- There's no speed improvement if you're only building a single webpack config.

- [Issues #3](https://github.com/ypcrts/multicore-webpack/issues/3) applies if
you are throwing more config targets than you have CPU cores.

- There is a hard limit of 99 webpack config targets at the moment to safeguard
against a fork bomb. This can be raised to 65536 as soon as #3 is resolved.

## Contributing

- Submit a PR ;)
- Target is C99; no features newer than C99 should berelied on, but it is
  recommended to compile targeting C11.
- CFLAGS include `-fstrict-aliasing`.
