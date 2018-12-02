High End Visualiation forked (HEVf)
===

This a forked version of the NIST HEV. The purpose of the fork is to adapt it to build on Fedora and possibly other
linux dritributions, experiment with HMD's and porting it to the NCAT Arc display. The intention it to wrap this back
the NIST HEV, the source code repository for the HEV software that is used to create
immersive visualization applications that run on both the desktop and in the
NIST CAVE.

### Build Requirements

We have developed this software only on CentOS 7 and NVIDIA GPUs.

Additional packages:

- centos-release-scl
- python3
- cmake
- fltk-\*
- mesa-\*
- freeglut-devel
- alsa-lib-devel
- libsndfile-devel
- portaudio-devel
- tk-devel
- R
- GraphicsMagick
- ImageMagick

### Build Instructions

After a fresh clone:
```
$ cd <directory where cloned>
$ source .bashhev
$ hevhere
$ make install
```

Wesley Griffin
Updated: 2018-03-29

