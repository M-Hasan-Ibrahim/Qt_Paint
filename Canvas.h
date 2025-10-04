#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPointF>
#include <QLineF>
#include <QPen>

#include "Shape.h"

class Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();

    enum class Tool {Line, Rect, Ellipse};

    bool isDirty() const {
        return dirty;
    }
    void setDirty(bool isdirty){
        dirty = isdirty;
    }

    bool save();

public slots:
    void setPenColor(const QColor& c);
    void setPenWidth(int w);
    void setPenStyle(Qt::PenStyle s);

    void setTool(Tool t);

    void setEditMode(bool on);

protected:
    void paintEvent(QPaintEvent* e) override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;

private:
    bool isDrawing = false;
    QPointF startPoint;
    QPointF currentPoint;

    QList<Shape*> shapes;

    QPen pen;
    Tool tool = Tool::Line;

    bool editMode = false;

    Shape* selected = nullptr;
    int activeHandle = -2;
    bool isEditing = false;
    QPointF lastMousePos;
    const qreal tolerance = 6.0;

    bool dirty = false;
};

#endif // CANVAS_H
