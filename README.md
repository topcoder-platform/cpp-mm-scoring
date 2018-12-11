# Topcoder - Marathon Match New Scoring System - Final PoC

For this contest, we are looking at trying some new technologies to assembly the new scoring system for Marathon Match challenges.

For that we would like to experiment the [CLING](https://github.com/root-project/cling) tool, a c++ language interpreter.

## Prerequisite

### Build tools and OS
- node 8
- [node-gyp](https://github.com/nodejs/node-gyp)
- cmake 3.2+
- g++ 7+
- Python 2.7
- curl
- Operation system: MacOS, Linux(recommend to use Ubuntu 18.04)
- docker and docker-compose (optional if you want to test with docker image)
Require xcode under macOS or proper C/C++ compiler toolchain, like GCC under linux.

Please ensure node-gyp tool is installed successfully to build Node.js native addon later.

Please note currently dependencies will installed by `scripts/setup.sh` for you and you just make sure curl exist in your OS.

### C++ libraries

- [cling](https://github.com/root-project/cling)
- [nlohmann json](https://github.com/nlohmann/json)
- [Boost TTI](https://www.boost.org/doc/libs/1_68_0/libs/tti/doc/html/index.html)

## Install dependencies

Please follow steps of [cling build page](https://cdn.rawgit.com/root-project/cling/master/www/build.html) to build and install cling manually. You may also use package tools such as Homebrew and apt-get instead of building it yourself.

Here's a brew under macOS example:

```
brew install cling
```

Ensure that cling was properly installed by typing `cling` command:

```
$ cling

****************** CLING ******************
* Type C++ code and press enter to run it *
*             Type .q to exit             *
*******************************************
[cling] $
```

Please follow steps of [nlohmann json github page](https://github.com/nlohmann/json) to build and install nlohmann json library manually. But, once again, I would suggest you to use the package tool.

Here's a brew under macOS example:

```
brew tap nlohmann/json
brew install nlohmann_json
```

### Install dependencies notes for Ubuntu

You can download latest valid package match your version of Ubuntu from [cling download page](https://root.cern.ch/download/cling/).
If you are using Ubuntu 18.04 you can download [cling_2018-12-11_sources.tar.bz2](https://root.cern.ch/download/cling/cling_2018-12-11_sources.tar.bz2).

* Unzip the source code to a folder, like `/usr/local/cling/0.5_1`.

* Install dependent libraries `sudo apt install libtinfo-dev libz-dev` to make the `make` pass.

You will need the following actions after building the code:

* Make a link to cling's `include` directory (for example /usr/local/cling/0.5_1/include) into `/usr/local` to make the interpreter work correctly.
* Update library path. Run: `echo "absolute path of /usr/local/cling/0.5_1/lib" > /etc/ld.so.conf.d/cling.conf && sudo ldconfig` and use `ldconfig -p` to make sure node addons could find **libcling.so** successfully.

## Build node addons code

Edit `env.sh` to configure environment variables for cling and nlohmann/json correctly.

**Cling\_LIB_\DIR** is the directory that contains `lib` folder of cling tool.
**Cling\_DIR** is the directory that contains file `ClingConfig.cmake`.
**LLVM\_INSTALL\_PREFIX** is the directory that contains `c++-analyzer`, `ccc-analyzer` file and  `lib` folder of cling tool.
**NLOHMANN\_JSON\_INCLUDE\_DIR** is the c++ header folder that contains `nlohmann/json.hpp`.

Now run following commands to build the source code(it will compile addons codes during `npm install` process)(no need to run `source env.sh` at first any more):

```
npm install
npm run lint # lint with airbnb code style
```
You can run `npm run rebuild` to rebuild after install process is over.

If there is no errors you will find node addons `MarathonMatchScoring.node` would be generated in folder `build\Release`.
You can quickly test by run `bash demo.sh 1` and you can check successful result for file **test_files\case1_result.txt**

If you can run successfully you can then run `npm test` in same console or make sure related environment are configured rightly.

## Build demo code

```
npm run rebuild-demo
```

The target executable `MarathonMatchScoring` would be generated in folder `demo`.

### Running notes

You must preload **libcling** to resolve run errors like `undefined symbol: _ZN4llvm11raw_ostream6handleEv`.

If you  still meet errors, you can frist try to use absolute path for **LD_PRELOAD** under linux or **DYLD_INSERT_LIBRARIES** under osx.
For example, `export LD_PRELOAD=$CLING_LIB_DIR/libcling.so`, or you may use `export LD_LIBRARY_PATH=$CLING_LIB_DIR`(may also use `sudo ldconfig` later) and then `LD_PRELOAD=libcling.so <application>` under linux.
Or you may use similar feature **DYLD_LIBRARY_PATH**(`export LD_LIBRARY_PATH=$CLING_LIB_DIR`) & **DYLD_INSERT_LIBRARIES** (`DYLD_INSERT_LIBRARIES=libcling.dylib <application>`) under osx with SIP is turned off.

Please notes **DYLD_INSERT_LIBRARIES** will not extend by subprocess so you cannot run mocha directly under OSX and you have to call `node node_modules/.bin/mocha`.

Please notes some recent OSX with SIP enabled may not work for [DYLD_INSERT_LIBRARIES doesn't work in recent versions of Mac OS](https://github.com/mxe/mxe/issues/1666).

Possible fix from  [Library not loaded](https://trac.macports.org/ticket/54939) is `sudo ln -s /Applications/Xcode.app/Contents/Frameworks/libcling.dylib $CLING_LIB_DIR/libcling.dylib`
Last option for recent OSX need to copy `libcling.dylib` to running directory with `cp $CLING_LIB_DIR/libcling.dylib .`·(recommend actually to avoid many issues for osx).

I test codes on OSX 10.14 and Ubuntu 18.04 and in both of them everything worked fine.

If you try to run docker image under OSX please make sure you remove `boost.tar.bz2` in `.sources` folder at first to avoid conflicts.

You may see such warning under OSX while all tests are passed or failed.
```
Warning in cling::IncrementalParser::CheckABICompatibility():
Possible C++ standard library mismatch, compiled with _LIBCPP_VERSION '4000'
Extraction of runtime standard library version was: '3700'
```
or similar logs under Ubuntu.
```
Warning in cling::IncrementalParser::CheckABICompatibility():
  Possible C++ standard library mismatch, compiled with __GLIBCXX__ '20160609'
  Extraction of runtime standard library version was: '20180720'
```
You may need to recompile cling using your local development tools.

### Docker notes
To save your time I provide docker image you can build with `docker-compose build` and then run `docker-compose run mm-cpp`
You have to run commands `npm install && npm run lint && npm test && npm run rebuild-demo` again one by one after run `docker-compose run mm-cpp` command.

You can then follow Validation.md to verify codes build in Ubuntu 18.04 docker image.
