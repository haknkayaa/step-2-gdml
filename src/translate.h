#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "metadata.h"

#include <QString>
#include <QVector>
#include <QPair>
#include <QColor>

#include <Standard.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_DisplayMode.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <STEPCAFControl_Reader.hxx>

#include <TDocStd_Document.hxx>

#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>

#include <TDF_LabelSequence.hxx>
#include <TDataStd_Name.hxx>

class GdmlWriter;
class Graphic3d_MaterialAspect;

class Translator
{
public:
    Translator(const Handle(AIS_InteractiveContext) context);
    QList<AIS_InteractiveObject*> importSTEP(QString, QList<QPair<QString, QColor> >&);

    bool exportGDML(QString, const QVector<SolidMetadata>&);

    static bool importProcess(QString, const Handle(TopTools_HSequenceOfShape)&, QList<QPair<QString, QColor> >&);
    static bool exportProcess(QString, const Handle(TopTools_HSequenceOfShape)&, const QVector<SolidMetadata>&);
    static bool findAllShapes(const Handle(AIS_InteractiveContext)&, const Handle(TopTools_HSequenceOfShape)&);
    static QList<AIS_InteractiveObject*> displayShapes(const Handle(AIS_InteractiveContext)&, const Handle(TopTools_HSequenceOfShape)&);
    static QList<AIS_InteractiveObject*> getInteractiveObjects(const Handle(AIS_InteractiveContext)&);

private:
    static GdmlWriter* gdmlWriter;
    const Handle(AIS_InteractiveContext) context;
};

#endif

