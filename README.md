## PTV

PTV (Port to Virtuality) is a prototype physics engine for robot auto-design scenario.
Code is run on Ubuntu 20.04, for developing usage, you should:

---

### Install

```bash
bash install.sh

# For glad update
# Web service: http://glad.dav1d.de/
# Choose: C/C++; OpenGL; Profile: Core; gl>=3.3; choose Generate a loader
# Download zip file, unzip it, and copy to *./thirdparty/*
```

---

### Run demo

```python
import ptv
window = ptv.window('demo', 1920, 1080)
window.run()
```
