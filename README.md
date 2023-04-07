# Dewarping With C++

Dewarping fish-eye image with C++ and OpenGL

OpenGL 3.3 is already included in glad.h and glad.c

Libraries needed to be install: opencv, glfw, glm

For glfw and glm, please download using these commands:


```
cd Dewarping
git submodule add https://github.com/glfw/glfw.git external/glfw
git submodule add https://github.com/g-truc/glm.git external/glm
```

After all the packages and libraries are installed

Please change the mode of the following files by:

```
chmod +x ./configure.sh ./build.sh ./run.sh ./doall.sh
```

Then run the following command to run all (cmake, make, etc.)
```
./doall.sh
```
