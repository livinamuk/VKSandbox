# OpenGL Depth Peeled Hair

![Screenshot](Screenshot.jpg)

Hair rendered with depth peeling, implemented in OpenGL. Should build off the rip in both Debug/Release.

```
git clone https://github.com/livinamuk/GLDepthPeeledHair.git
```

Controls

```
WSAD:    walk
CTRL:    lower camera
SPACE:   raise camera
Q:       lower depth peel count
E:       increase depth peel count
SHIFT:   slow movement
F:       fullscreen
```

Been using this repo as a playground to implement a new asset loading system too. It uploads compressed dds files with embedded mipmaps asynchronously to the gpu in parallel at full framerate via alternating persistently mapped Pixel Buffer Objects.

![AsyncLoading](AsyncLoading.gif)

Never quit, never waver 🌹
