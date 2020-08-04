[![Build](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/haknkayaa/step-2-gdml)
[![Release](https://img.shields.io/badge/release-v0.1-blue)](https://github.com/haknkayaa/step-2-gdml)
[![GitHub license](https://img.shields.io/github/license/haknkayaa/step-2-gdml)](https://github.com/haknkayaa/step-2-gdml/blob/master/LICENSE.md)

# STEP TO GDML CONVERTER

A project to convert STEP files to GDML files in a clean and fast fashion.

Errors may occur if $CASROOT can not be found. Either set it to
the root location of OpenCASCADE or edit step-gdml.pro.

# Requirements:
- Qmake  (tested with 2.01a)
- Qt4 or Qt5 (tested with 4.8.7)
- OpenCASCADE (tested with 7.4.0)

# Installation:
```bash
qmake step-gdml.pro
make
```

# Usage
```bash
./step-gdml
```

# Supported Platform
- Linux (tested with Ubuntu 18.04)



