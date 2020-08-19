#include "window.h"
#include "translate.h"
#include "stdio.h"
#include "toolbar.h"
#include <QApplication>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_SequenceOfPrinters.hxx>
#include <Message_Printer.hxx>

#include <iostream>

class CustomPrinter : public Message_Printer
{
public:
    CustomPrinter() {
        // initial function for class
    }
    virtual void Send(const TCollection_ExtendedString& theString,
                      const Message_Gravity, const Standard_Boolean putEndl) const
    {
        char buf[theString.LengthOfCString()];
        char* alias = buf;
        theString.ToUTF8CString(alias);
        printf("%s\n", alias);
    }
};

void setOpenCASCADEPrinters()
{
    // OSD_Timer writes directly to std::cout. We disable...
    // This does kill all of its users, but this application
    // doesn't have any worth keeping. The "proper" solution
    // is to enable/disable it exactly around the operation
    // to be excised.
    std::cout.setstate(std::ios_base::failbit);

    // Update standard OpenCASCADE output method
    const Message_SequenceOfPrinters& p = Message::DefaultMessenger()->Printers();
    for (int i = 1; i <= p.Length(); i++) {
        Message::DefaultMessenger()->RemovePrinter(p.Value(i));
    }
    //Message::DefaultMessenger()->AddPrinter(new CustomPrinter());
}


int main(int argc, char* argv[])
{
    setOpenCASCADEPrinters();

    QApplication app(argc, argv);
    QStringList args = app.arguments();

    //! argüman sayısı 2den küçükse GUI başlatır
    //! eğer 2. argüman var ise o dosyayı da açar
    if (args.length() <= 2) {
        QString ifile;
        if (args.length() == 2) {
            ifile = args[1];
        }
        MainWindow w(ifile);
        w.show();
        return app.exec();
    }

    //! argüman sayısı 3 ise
    //! ./step-gdml "file_name.step" "output_name.gdml"
    //!   args[0]        args[1]          args[2]
    else if (args.length() == 3) {
        // File path
        QString inputFile = args[1];
        QString outputFile = args[2];

        // shapes, li
        Handle(TopTools_HSequenceOfShape) shapes = new TopTools_HSequenceOfShape();
        QList< QPair <QString,QColor> > li;

        // step açma kontrolü
        if (!Translator::importProcess(inputFile, shapes, li)) {
            printf("Import failed. :-(\n");
            return -1;
        }

        QVector<SolidMetadata> metadata(shapes->Length());

        // step datalarının parselenip etiketlendirilmesi
        for (int i = 0; i < metadata.size(); i++) {
            metadata[i].color = Quantity_Color();
            metadata[i].item = 0;
            metadata[i].object = 0;
            metadata[i].name = QString::number(i);
            metadata[i].transp = 0.0;
            metadata[i].material = "ALUMINUM";
            // TODO: edit material list
        }

        // gdml yazma kontrolü
        if (!Translator::exportProcess(outputFile, shapes, metadata)) {
            printf("Export failed. :-(\n");
            return -1;
        }


        return 0;
    }

    //! Argüman eksik
    else {
        printf("Usage: step-gdml [INPUT_STEP_FILE] [OUTPUT_GDML_FILE]\n");
        return -1;
    }
}

