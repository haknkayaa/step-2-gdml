#include "toolbar.h"
#include "viewer.h"

#include <QMenu>
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSettings>

#include <V3d_Viewer.hxx>
#include <V3d_BadValue.hxx>
#include <Standard_Version.hxx>

#define NAME_IMPL(X) virtual QString getName() {return QString(X);}
#define COLOR_IMPL(X) virtual Qt::GlobalColor getColor() {return X;}
#define CURSOR_IMPL(X) virtual Qt::CursorShape getCursor() {return X;}

#if OCC_VERSION_HEX >= 0x070000
#define OPTIONAL_UPDATE_ARGUMENT true
#define AND_OPTIONAL_UPDATE_ARGUMENT ,true
#else
#define OPTIONAL_UPDATE_ARGUMENT
#define AND_OPTIONAL_UPDATE_ARGUMENT
#endif

Toolbar::Toolbar(QWidget *parent) : QMainWindow (parent) {

  QPixmap newpix("../icons/new.png");
  QPixmap openpix("../icons/open.png");
  QPixmap quitpix("../icons/settings.png");

  QToolBar *toolbar = addToolBar("main toolbar");
  toolbar->addAction(QIcon(newpix), "New File");
  toolbar->addAction(QIcon(openpix), "Open File");
  toolbar->addSeparator();
  QAction *quit = toolbar->addAction(QIcon(quitpix),
      "Quit Application");

  connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}
