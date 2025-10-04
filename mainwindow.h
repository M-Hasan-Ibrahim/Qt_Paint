#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    Ui::MainWindow *ui;
    Canvas* canvas = nullptr;

    void createPenMenus();
    void addColorAction(QMenu* menu, QActionGroup* group, const QString& name, const QColor& color, bool checked = false);
    void addWidthAction(QMenu* menu, QActionGroup* group, const QString& name, int width, bool checked = false);
    void addStyleAction(QMenu* menu, QActionGroup* group, const QString& name, Qt::PenStyle style, bool checked = false);

    void createShapeMenu();

    void createToolBar();
    void addModeSwitch(QToolBar* toolBar);
    void addSaveButton(QToolBar* toolBar);
};
#endif // MAINWINDOW_H
