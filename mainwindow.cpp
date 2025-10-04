#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QActionGroup"
#include "QToolBar"
#include "QTimer"
#include "QMessageBox"
#include "QCloseEvent"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = new Canvas(this);
    setCentralWidget(canvas);
    setWindowTitle("Qt Paint TP");

    createPenMenus();
    createShapeMenu();
    createToolBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createPenMenus(){
    QMenu* colorMenu = menuBar()->addMenu(tr("Color"));
    QActionGroup* colorGroup = new QActionGroup(this);

    addColorAction(colorMenu, colorGroup, tr("Black"), Qt::black, true);
    addColorAction(colorMenu, colorGroup, tr("Red"), Qt::red);
    addColorAction(colorMenu, colorGroup, tr("Blue"), Qt::blue);

    QMenu* widthMenu = menuBar()->addMenu(tr("Width"));
    QActionGroup* widthGroup = new QActionGroup(this);
    addWidthAction(widthMenu, widthGroup, tr("1 px"), 1);
    addWidthAction(widthMenu, widthGroup, tr("2 px"), 2, true);
    addWidthAction(widthMenu, widthGroup, tr("4 px"), 4);
    addWidthAction(widthMenu, widthGroup, tr("8 px"), 8);

    QMenu* styleMenu = menuBar()->addMenu(tr("Style"));
    QActionGroup* styleGroup = new QActionGroup(this);
    addStyleAction(styleMenu, styleGroup, tr("Solid"), Qt::SolidLine, true);
    addStyleAction(styleMenu, styleGroup, tr("Dash"),  Qt::DashLine);
    addStyleAction(styleMenu, styleGroup, tr("Dot"),   Qt::DotLine);
}

void MainWindow::addColorAction(QMenu* menu, QActionGroup* group, const QString& name, const QColor& color, bool checked){
    QAction* a = group->addAction(name);
    a->setChecked(checked);
    a->setCheckable(true);
    menu->addAction(a);

    connect(a, &QAction::triggered, this, [=]{
        canvas->setPenColor(color);
    });
}
void MainWindow::addWidthAction(QMenu* menu, QActionGroup* group, const QString& name, int width, bool checked){
    QAction* a = group->addAction(name);
    a->setChecked(checked);
    a->setCheckable(true);
    menu->addAction(a);

    connect(a, &QAction::triggered, this, [=]{
        canvas->setPenWidth(width);
    });
}
void MainWindow::addStyleAction(QMenu* menu, QActionGroup* group, const QString& name, Qt::PenStyle style, bool checked){
    QAction* a = group->addAction(name);
    a->setCheckable(true);
    a->setChecked(checked);
    menu->addAction(a);

    connect(a, &QAction::triggered, this, [=] {
        canvas->setPenStyle(style);
    });
}

void MainWindow::createShapeMenu(){
    QMenu* shapeMenu = menuBar()->addMenu(tr("Shape"));
    QActionGroup* shapeGroup = new QActionGroup(this);

    {
        QAction* a = new QAction(tr("Line"), this);
        a->setCheckable(true);
        a->setChecked(true);
        shapeGroup->addAction(a);
        shapeMenu->addAction(a);
        connect(a, &QAction::triggered, this, [=]{
            canvas->setTool(Canvas::Tool::Line);
        });
    }

    {
        QAction* a = new QAction(tr("Rectangle"), this);
        a->setCheckable(true);
        shapeGroup->addAction(a);
        shapeMenu->addAction(a);
        connect(a, &QAction::triggered, this, [=]{
            canvas->setTool(Canvas::Tool::Rect);
        });
    }

    {
        QAction* a = new QAction(tr("Ellipse"), this);
        a->setCheckable(true);
        shapeGroup->addAction(a);
        shapeMenu->addAction(a);
        connect(a, &QAction::triggered, this, [=]{
            canvas->setTool(Canvas::Tool::Ellipse);
        });
    }
}

void MainWindow::createToolBar(){
    QToolBar* toolBar = addToolBar(tr("Mode"));
    toolBar->setMovable(false);

    addModeSwitch(toolBar);
    addSaveButton(toolBar);
}

void MainWindow::addModeSwitch(QToolBar* toolBar){
    QAction* editAction = new QAction(tr("Edit Mode: OFF"), this);
    editAction->setCheckable(true);
    toolBar->addAction(editAction);

    connect(editAction, &QAction::toggled, this, [=](bool on){
        canvas->setEditMode(on);
        editAction->setText(on ? tr("Edit Mode: ON") : tr("Edit Mode: OFF"));
    });
}

void MainWindow::addSaveButton(QToolBar* tb) {
    QAction* saveAction = new QAction(tr("Save"), this);
    tb->addAction(saveAction);

    connect(saveAction, &QAction::triggered, this, [=]{
        if (canvas->save()) {
            canvas->setDirty(false);
            saveAction->setText(tr("Saved :)"));
            QTimer::singleShot(1200, this, [=]{ saveAction->setText(tr("Save")); });
        } else {
            QMessageBox::warning(this, tr("Save Failed"), tr("Could not save picture."));
        }
    });
}

void MainWindow::closeEvent(QCloseEvent* e) {
    if (!canvas || !canvas->isDirty()) { e->accept(); return; }

    auto ret = QMessageBox::warning(this, tr("Unsaved changes"),
                                    tr("You have unsaved changes."),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                    QMessageBox::Save);

    if (ret == QMessageBox::Cancel) { e->ignore(); return; }
    if (ret == QMessageBox::Save) {
        if (canvas->save()) { canvas->setDirty(false); e->accept(); }
        else { e->ignore(); QMessageBox::warning(this, tr("Save Failed"), tr("Could not save picture.")); }
        return;
    }
    e->accept();
}




