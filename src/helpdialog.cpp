#include "helpdialog.h"
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

OrientationMapper::OrientationMapper(QObject* source, QObject* target,
                                     V3d_TypeOfOrientation o) : QObject(source)
{
    orientation = o;
    connect(source, SIGNAL(triggered()), SLOT(recieve()));
    connect(this, SIGNAL(send(V3d_TypeOfOrientation)), target,
            SLOT(setOrientation(V3d_TypeOfOrientation)));
}

void OrientationMapper::recieve()
{
    emit send(orientation);
}

class ClsClkNone : public MouseButtonMode
{
public:
    NAME_IMPL("ClkNone")
    CURSOR_IMPL(Qt::ForbiddenCursor)
    COLOR_IMPL(Qt::darkGray)
} ClkNone;


class ClsClkHover : public MouseButtonMode
{
public:
    NAME_IMPL("ClkHover")
    CURSOR_IMPL(Qt::PointingHandCursor)
    COLOR_IMPL(Qt::darkGreen)
    virtual void drag(const ViewActionData& data)
    {
        data.context->MoveTo(data.x, data.y, data.view AND_OPTIONAL_UPDATE_ARGUMENT);
    }
} ClkHover;

class BaseClkRubberBandType : public MouseButtonMode
{
public:
    CURSOR_IMPL(Qt::CrossCursor)
    virtual void click(const ViewActionData& data)
    {
        x1 = data.x, y1 = data.y;
        data.band->setGeometry(x1, y1, 0, 0);
        data.band->show();
    }
    virtual void drag(const ViewActionData& data)
    {
        data.band->setGeometry(QRect(x1, y1, data.x - x1, data.y - y1).normalized());
    }
    virtual void release(const ViewActionData& data)
    {
        data.band->hide();
    }
private:
    int x1, y1;
};

class ClsClkSelect : public BaseClkRubberBandType
{
public:
    NAME_IMPL("ClkSelect")
    COLOR_IMPL(Qt::darkCyan)
    virtual void release(const ViewActionData& data)
    {
        BaseClkRubberBandType::release(data);
        const QRect& r = data.band->geometry();
        if (r.isEmpty()) {

#if OCC_VERSION_HEX >= 0x070000
        data.context->Select(true);
#else
         data.context->Select(true);
#endif

        } else {
            int x1, y1, x2, y2;
            r.getCoords(&x1, &y1, &x2, &y2);
            data.context->Select(x1, y1, x2, y2, data.view AND_OPTIONAL_UPDATE_ARGUMENT);
        }
    }
} ClkSelect;

class ClsClkSelectToggle : public BaseClkRubberBandType
{
public:
    NAME_IMPL("ClkSelectToggle")
    COLOR_IMPL(Qt::darkCyan)
    virtual void release(const ViewActionData& data)
    {
        BaseClkRubberBandType::release(data);
        const QRect& r = data.band->geometry();
        if (r.isEmpty()) {
            data.context->ShiftSelect(OPTIONAL_UPDATE_ARGUMENT);
        } else {
            QPoint tl = r.topLeft();
            QPoint br = r.bottomRight();
            data.context->ShiftSelect(tl.x(), tl.y(), br.x(), br.y(), data.view AND_OPTIONAL_UPDATE_ARGUMENT);
        }
    }
} ClkSelectToggle;

class ClsClkRotate : public MouseButtonMode
{
public:
    NAME_IMPL("ClkRotate")
    COLOR_IMPL(Qt::darkMagenta)
    CURSOR_IMPL(Qt::ClosedHandCursor)
    virtual void click(const ViewActionData& data)
    {
        data.view->StartRotation(data.x, data.y);
    }
    virtual void drag(const ViewActionData& data)
    {
        data.view->Rotation(data.x, data.y);
        data.view->Redraw();
    }
} ClkRotate;

class ClsClkPan : public MouseButtonMode
{
public:
    NAME_IMPL("ClkPan")
    COLOR_IMPL(Qt::darkMagenta)
    CURSOR_IMPL(Qt::OpenHandCursor)
    virtual void click(const ViewActionData& data)
    {
        x1 = data.x, y1 = data.y;
    }
    virtual void drag(const ViewActionData& data)
    {
        data.view->Pan(data.x - x1, y1 - data.y);
        x1 = data.x, y1 = data.y;
    }
private:
    int x1, y1;
} ClkPan;

class ClsClkZoom : public MouseButtonMode
{
public:
    NAME_IMPL("ClkZoom")
    COLOR_IMPL(Qt::darkMagenta)
    CURSOR_IMPL(Qt::SizeVerCursor)
    virtual void click(const ViewActionData& data)
    {
        x1 = data.x, y1 = data.y;
    }
    virtual void drag(const ViewActionData& data)
    {
        data.view->Zoom(x1, y1, data.x, data.y);
        x1 = data.x, y1 = data.y;
    }
private:
    int x1, y1;
} ClkZoom;


class ClsClkPopup: public MouseButtonMode
{
public:
    NAME_IMPL("ClkPopup")
    COLOR_IMPL(Qt::darkYellow)
    CURSOR_IMPL(Qt::ArrowCursor)
    virtual void click(const ViewActionData& s)
    {
        menu = new QMenu();
        menu->addAction("Reset View", s.viewer, SLOT(resetView()));
        menu->addSeparator();
        addProjectionAction(menu, s.viewer, "Right", V3d_Xpos);

        s.viewer->connect(menu, SIGNAL(aboutToHide()), SLOT(startHover()));
        menu->connect(menu, SIGNAL(aboutToHide()), SLOT(deleteLater()));
        menu->exec(QPoint(s.globalX, s.globalY));
    }
    static void addProjectionAction(QMenu* m, Viewer* v, QString text,
                                    V3d_TypeOfOrientation orientation)
    {
        QAction* action = new QAction(text, m);
        new OrientationMapper(action, v, orientation);
        m->addAction(action);
    }
private:
    QMenu* menu;
} ClkPopup;

class BaseClkZoomBoxType : public BaseClkRubberBandType
{
public:
    COLOR_IMPL(Qt::darkRed)
    virtual void release(const ViewActionData& data)
    {
        BaseClkRubberBandType::release(data);
        const QRect& r = data.band->geometry();
        if (r.isEmpty()) {
            qDebug("Zoom to point: (%d, %d) ignoring.", r.x(), r.y());
        } else {
            int x1, y1, x2, y2;
            r.getCoords(&x1, &y1, &x2, &y2);
            QPoint c = r.center();
            double x_ratio = double(x2 - x1) / double(data.sizeX);
            double y_ratio = double(y2 - y1) / double(data.sizeY);
            double scale;
            if (outward) {
                scale = data.view->Scale() * qMax(x_ratio, y_ratio);
            } else {
                scale = data.view->Scale() / qMax(x_ratio, y_ratio);
            }
            data.view->Place(c.x(), c.y(), scale);
        }
    }
    bool outward;
};

class ClsClkZoomInBox : public BaseClkZoomBoxType
{
public:
    NAME_IMPL("ClkZoomInBox")
    ClsClkZoomInBox()
    {
        outward = false;
    }
} ClkZoomInBox;

class ClsClkZoomOutBox : public BaseClkZoomBoxType
{
public:
    NAME_IMPL("ClkZoomOutBox")
    ClsClkZoomOutBox()
    {
        outward = true;
    }
} ClkZoomOutBox;


void doScroll(const Handle(V3d_View)& v, double step)
{
    double factor;
    if (step < 0) {
        factor = (1.0 + step);
    } else {
        factor = 1.0 / (1.0 - step);
    }
    // TODO: develop a safer way to handle zooming amounts
    double initial = v->Scale();
    try {
        v->SetScale(initial * factor);
    } catch (V3d_BadValue& err) {
        qDebug("Too far. Disregarding");
        v->SetScale(initial);
    }
}

class ClsScrZoom : public MouseScrollMode
{
public:
    NAME_IMPL("ScrZoom")
    COLOR_IMPL(Qt::darkBlue)
    virtual void scroll(V3d_View* v, int delta)
    {
        doScroll(v, delta / 360.0);
    }
} ScrZoom;

class ClsScrZoomInv : public MouseScrollMode
{
public:
    NAME_IMPL("ScrZoomInv")
    COLOR_IMPL(Qt::darkBlue)
    virtual void scroll(V3d_View* v, int delta)
    {
        doScroll(v, -delta / 360.0);
    }
} ScrZoomInv;

class ClsScrNone : public MouseScrollMode
{
public:
    NAME_IMPL("ScrNone")
    COLOR_IMPL(Qt::darkGray)
} ScrNone;

class ClsScrPanStepV : public MouseScrollMode
{
public:
    NAME_IMPL("ScrPanStepV")
    COLOR_IMPL(Qt::black)
    virtual void scroll(V3d_View* v, int delta)
    {
        v->Pan(0, delta / 2);
    }
} ScrPanStepV;

class ClsScrPanStepH : public MouseScrollMode
{
public:
    NAME_IMPL("ScrPanStepH")
    COLOR_IMPL(Qt::black)
    virtual void scroll(V3d_View* v, int delta)
    {
        v->Pan(delta / 2, 0);
    }
} ScrPanStepH;

#undef COLOR_IMPL
#undef NAME_IMPL
#undef CURSOR_IMPL

MouseMode* const defaultConfig[5][4] = {
    {
        &ClkSelect,
        &ClkSelectToggle,
        &ClkSelect,
        &ClkSelectToggle
    },
    {
        &ClkPopup,
        &ClkZoomInBox,
        &ClkRotate,
        &ClkZoomOutBox,
    },
    {
        &ClkPan,
        &ClkRotate,
        &ClkZoom,
        &ClkNone
    },
    {
        &ClkHover,
        &ClkHover,
        &ClkHover,
        &ClkHover
    },
    {
        &ScrZoom,
        &ScrPanStepV,
        &ScrPanStepH,
        &ScrNone
    }
};

MouseMode* modeConfig[5][4];

MouseScrollMode* const allScrollModes[] = {
    &ScrZoom,
    &ScrZoomInv,

    &ScrPanStepV,
    &ScrPanStepH,

    &ScrNone
};

MouseButtonMode* const allButtonModes[] = {

    &ClkPopup,

    &ClkSelect,
    &ClkSelectToggle,

    &ClkZoomInBox,
    &ClkZoomOutBox,

    &ClkRotate,
    &ClkPan,
    &ClkZoom,
    &ClkHover,

    &ClkNone
};

MouseButtonMode* getButtonAction(Qt::MouseButton button,
                                 Qt::KeyboardModifiers modifiers)
{
    int idx = 0;
    if (modifiers & Qt::ShiftModifier) {
        idx += 1;
    }
    if (modifiers & Qt::ControlModifier) {
        idx += 2;
    }

    if (button == Qt::MiddleButton) {
        return (MouseButtonMode*)modeConfig[2][idx];
    } else if (button == Qt::RightButton) {
        return (MouseButtonMode*)modeConfig[1][idx];
    } else if (button == Qt::LeftButton) {
        return (MouseButtonMode*)modeConfig[0][idx];
    } else {
        return (MouseButtonMode*)modeConfig[3][idx];
    }
}

MouseScrollMode* getScrollAction(Qt::KeyboardModifiers modifiers)
{
    int idx = 0;
    if (modifiers & Qt::ShiftModifier) {
        idx += 1;
    }
    if (modifiers & Qt::ControlModifier) {
        idx += 2;
    }

    return (MouseScrollMode*)modeConfig[4][idx];
}


ModeComboBox::ModeComboBox()
{
}

void ModeComboBox::addModeItem(MouseMode* mode)
{
    this->addItem(mode->getName());
    list.append(mode);
    this->setItemData(this->count() - 1, QColor(mode->getColor()),
                      Qt::TextColorRole);
}

void ModeComboBox::hidePopup()
{
    QComboBox::hidePopup();
    emit hidePopupSignal();
}

void ModeComboBox::showPopup()
{
    QComboBox::showPopup();
    emit showPopupSignal();
}

MouseMode* ModeComboBox::getSelectedPointer()
{
    return list[this->currentIndex()];
}

QLabel* makeLabel(MouseMode* m)
{
    QLabel* w = new QLabel(m->getName());
    QString color = QColor(m->getColor()).name();
    w->setStyleSheet(QString("QLabel{color:%1;}").arg(color));
    return w;
}

HelpDialog::HelpDialog(QWidget* parent) :
    QDialog(parent)
{
    table = new QTableWidget(this);
    table->setRowCount(5);
    table->setColumnCount(4);
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 4; x++) {
            table->setCellWidget(y, x, makeLabel(modeConfig[y][x]));
        }
    }
    table->setHorizontalHeaderLabels(QStringList() << "Plain" << "Shift" << "Ctrl"
                                     << "Ctrl+Shift");
    table->setVerticalHeaderLabels(QStringList() << "Left" << "Right" << "Middle" <<
                                   "Hover" << "Scroll");
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    for (int i = 0; i < table->columnCount(); i++) {
        table->setColumnWidth(i, 120);
    }
    for (int i = 0; i < table->rowCount(); i++) {
        table->setRowHeight(i, 30);
    }

    int xw = table->horizontalHeader()->length() +
             table->verticalHeader()->length();
    int yw = table->horizontalHeader()->height() +
             table->verticalHeader()->height();
    table->setMinimumSize(xw, yw);
    connect(table, SIGNAL(cellClicked(int, int)), SLOT(editCell(int, int)));

    setWindowTitle("Step-Gdml View Settings");

    QPushButton* resetButton = new QPushButton("Reset");
    resetButton->setStyleSheet("QPushButton{font:bold;color:orange;background:black}");
    connect(resetButton, SIGNAL(clicked()), SLOT(resetConfig()));
    QPushButton* closeButton = new QPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), SLOT(hide()));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(resetButton);
    hlayout->addStretch(5);
    hlayout->addWidget(closeButton);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(table);
    layout->addSpacing(4);
    layout->addLayout(hlayout);

    setLayout(layout);
    resize(sizeHint());
}

ModeComboBox* makeHelperComboBox(int row, int col)
{
    if (col < 0 || col >= 4) {
        qFatal("Column out of bounds");
    }

    ModeComboBox* p = new ModeComboBox();

    if (row < 4) {
        for (unsigned i = 0; i < sizeof(allButtonModes) / sizeof(allButtonModes[0]);
             i++) {
            MouseMode* m = allButtonModes[i];
            if (row == 3 && m == &ClkPopup) {
                // Choice not allowed.
                // Do you want a new, blocking popup every mouse move?
                continue;
            }

            p->addModeItem(m);
            if (modeConfig[row][col] == m) {
                p->setCurrentIndex(p->count() - 1);
            }
        }
    } else if (row == 4) {
        for (unsigned i = 0; i < sizeof(allScrollModes) / sizeof(allScrollModes[0]);
             i++) {
            MouseMode* m = allScrollModes[i];
            p->addModeItem(m);
            if (modeConfig[row][col] == m) {
                p->setCurrentIndex(p->count() - 1);
            }
        }
    } else {
        qFatal("Row out of bounds");
    }
    return p;
}

void HelpDialog::editCell(int row, int col)
{
    lastRow = row;
    lastCol = col;

    popup = makeHelperComboBox(row, col);
    table->setCellWidget(row, col, popup);
    popup->showPopup();
    connect(popup, SIGNAL(hidePopupSignal()), this, SLOT(clearPopup()));
    connect(popup, SIGNAL(activated(int)), this, SLOT(changeItem()));
}

void HelpDialog::changeItem()
{
    MouseMode* m = popup->getSelectedPointer();
    modeConfig[lastRow][lastCol] = m;
    table->setCellWidget(lastRow, lastCol, makeLabel(m));
}

void HelpDialog::clearPopup()
{
    popup->hide();
    popup->deleteLater();
    table->setCellWidget(lastRow, lastCol, makeLabel(modeConfig[lastRow][lastCol]));
}

void HelpDialog::loadConfig()
{
    QSettings s;
    if (s.contains("view-actions")) {
        QByteArray array = s.value("view-actions").toByteArray();
        int y;
        for (y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                modeConfig[y][x] = allButtonModes[array[y * 4 + x]];
            }
        }
        for (int x = 0; x < 4; x++) {
            modeConfig[y][x] = allScrollModes[array[y * 4 + x]];
        }
    } else {
        for (int y = 0; y < 5; y++) {
            for (int x = 0; x < 4; x++) {
                modeConfig[y][x] = defaultConfig[y][x];
            }
        }
    }
}

unsigned findIndex(void* arr[], void* target, unsigned len)
{
    for (unsigned i = 0; i < len; i++) {
        if (target == arr[i]) {
            return i;
        }
    }
    qFatal("Oughta find it.");
}

void HelpDialog::saveConfig()
{
    QSettings s;
    QByteArray array(20, 0);
    int y;
    for (y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            array[y * 4 + x] = findIndex((void**)allButtonModes, modeConfig[y][x],
                                         sizeof(allButtonModes) / sizeof(allButtonModes[0]));
        }
    }
    for (int x = 0; x < 4; x++) {
        array[y * 4 + x] = findIndex((void**)allScrollModes, modeConfig[y][x],
                                     sizeof(allScrollModes) / sizeof(allScrollModes[0]));
    }
    s.setValue("view-actions", array);
}

void HelpDialog::resetConfig()
{
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 4; x++) {
            modeConfig[y][x] = defaultConfig[y][x];
            table->setCellWidget(y, x, makeLabel(modeConfig[y][x]));
        }
    }
}
