[![GitHub license](https://img.shields.io/github/license/haknkayaa/step-2-gdml?style=for-the-badge)](https://github.com/haknkayaa/step-2-gdml/blob/master/LICENSE.md)


# Step-gdml

org: https://github.com/frsfnrrg/step-gdml

A project to convert STEP files to GDML files in a clean and fast fashion.
Requires OpenCASCADE (tested with 7.4.0) and Qt4 (tested with 4.8.6).

# Require:
- Qt4 or Qt5
- OpenCASCADE

# Installation:
```bash
qmake step-gdml.pro
make
```


# Usage
```bash
./step-gdml
```

Errors may occur if $CASROOT can not be found. Either set it to
the root location of OpenCASCADE or edit step-gdml.pro.


