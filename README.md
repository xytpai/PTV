## PTV

PTV (Port to Virtuality) is a prototype physics engine for robot auto-design scenario.
Code is run on Ubuntu 20.04, for developing usage, you should:

---

#### 1. Install GLFW

GLFW source code: https://www.glfw.org/download.html

```bash
unzip glfw-3.3.4.zip; cd glfw-3.3.4
mkdir build; cd build
cmake ..; make
sudo make install; sudo updatedb
```

One can check where GLFW is

```bash
# /usr/include/GLFW or /usr/local/include/GLFW or
sudo apt-get install mlocate
sudo updatedb
locate glfw3.h
```

#### 2. Update GLAD if necessary

Web service: http://glad.dav1d.de/
Choose: C/C++; OpenGL; Profile: Core; gl>=3.3; choose Generate a loader
Download zip file, unzip it, and copy to *./thirdparty/*

---

### Run demo

```bash
bash build.sh; ./build/PTV
```

