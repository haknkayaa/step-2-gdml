#pragma once

#include <QDialog>
#include <QRubberBand>
#include <QTableWidget>
#include <QComboBox>
#include <QList>

#include <V3d_TypeOfOrientation.hxx>

class Toolbar : public QMainWindow {

  Q_OBJECT

  public:
    Toolbar(QWidget *parent = 0);
};
