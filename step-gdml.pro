TEMPLATE = app
CONFIG += debug_and_release qt

#################
### VARIABLES ###
#################

CASROOT = $$(CASROOT)
isEmpty (CASROOT) {
    CASROOT = /opt/OpenCASCADE
}
!exists($$CASROOT) {
    CASROOT = /opt/opencascade
}
!exists($$CASROOT) {
    CASROOT = /opt/opencascade-7.4.0/inc
}

OCCLIB=$$CASROOT/lib/
!exists($$OCCLIB) {
    OCCLIB=$$CASROOT/lin64/gcc/lib
}
message (OCCLIB is $$OCCLIB)

#################
#### TARGETS ####
#################

TARGET = step-gdml

HEADERS = src/util.h \
    src/window.h \
    src/translate.h \
    src/gdmlwriter.h \
    src/metadata.h \
    src/helpdialog.h \
    src/viewer.h
SOURCES = src/main.cpp \
    src/util.cpp \
    src/window.cpp \
    src/translate.cpp \
    src/gdmlwriter.cpp \
    src/helpdialog.cpp \
    src/viewer.cpp \
    src/triangulate.cpp

OTHER_FILES=.astylerc

DESTDIR = .
OBJECTS_DIR = ./build/obj
MOC_DIR = ./build/moc
RCC_DIR = ./build/rcc

DEFINES = CSFDB

############
#### QT ####
############

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5): DEFINES += QT_NO_DEPRECATED

##################
#### INCLUDES ####
##################

INCLUDEPATH = $$CASROOT $$CASROOT/inc $$CASROOT/include/opencascade $(QTDIR)/include/QtCore \
              $(QTDIR)/include/QtGui $(QTDIR)/include
INCLUDEPATH += $$QMAKE_INCDIR_X11 $$QMAKE_INCDIR_OPENGL $$QMAKE_INCDIR_THREAD
DEFINES += LIN LININTEL OCC_CONVERT_SIGNALS HAVE_CONFIG_H HAVE_WOK_CONFIG_H

##############
#### LIBS ####
##############

# To place CASROOT before -L/usr/lib in case we override it
#QMAKE_CXXFLAGS +=  -fsanitize=address
#QMAKE_LFLAGS += -fsanitize=address
QMAKE_LFLAGS +=   -L$$OCCLIB

LIBS +=  -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl \
         -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
         -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
         -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
         -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset

# Delta to 7.1.0
#LIBS +=- lTKernel -lTKShapeSchema -lTKPShape

#Note: -lTKXSDRAW leads to a crash on unload.
LIBS += -lTKCAF -lTKLCAF -lTKXCAF -lTKXDESTEP

# All OpenCASCADE modules
#LIBS += -lTKGeomBase -lTKPShape -lTKBool -lTKBO -lTKXSBase -lTKStdLSchema -lTKSTEPAttr -lTKXmlTObj -lTKXSDRAW -lTKSTEP -lTKPrim -lTKAdvTools -lTKFillet -lTKXmlL -lTKTObj -lTKCAF -lTKCDF -lTKViewerTest -lTKService -lTKG2d -lTKG3d -lTKBin -lTKTopAlgo -lTKHLR -lTKXDEIGES -lTKVoxel -lTKDraw -lTKXMesh -lTKXCAFSchema -lTKNIS -lTKPCAF -lTKBinTObj -lTKXmlXCAF -lTKMath -lTKFeat -lTKIGES -lTKSTL -lTKV3d -lTKMesh -lTKVRML -lTKOpenGl -lTKXml -lTKXCAF -lTKBRep -lTKDCAF -lTKTObjDRAW -lTKernel -lTKQADraw -lTKMeshVS -lTKOffset -lTKXDEDRAW -lTKBinXCAF -lTKLCAF -lTKPLCAF -lTKShHealing -lPTKernel -lTKBinL -lTKSTEPBase -lTKShapeSchema -lTKXDESTEP -lTKStdSchema -lFWOSPlugin -lTKSTEP209 -lTKTopTest -lTKGeomAlgo


LIBS += -L$$QMAKE_LIBDIR_X11 $$QMAKE_LIBS_X11
LIBS += -L$$QMAKE_LIBDIR_OPENGL $$QMAKE_LIBS_OPENGL $$QMAKE_LIBS_THREAD

