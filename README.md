[![GitHub issues](https://img.shields.io/github/issues/haknkayaa/step-2-gdml)](https://github.com/haknkayaa/step-2-gdml/issues)
[![GitHub license](https://img.shields.io/github/license/haknkayaa/step-2-gdml)](https://github.com/haknkayaa/step-2-gdml)
[(https://img.shields.io/badge/version-v0.1-green)]


# Step-gdml

org: https://github.com/frsfnrrg/step-gdml

A project to convert STEP files to GDML files in a clean and fast fashion.
Requires OpenCASCADE (tested with 7.4.0) and Qt4 (tested with 4.8.6).

To build:
> qmake step-gdml.pro
> make

# Usage
```
> ./step-gdml
```

Errors may occur if $CASROOT can not be found. Either set it to
the root location of OpenCASCADE or edit step-gdml.pro.


