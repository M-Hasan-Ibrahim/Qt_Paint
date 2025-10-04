#include "Canvas.h"

#include <QPainter>
#include <QMouseEvent>
#include "QDir"

Canvas::Canvas(QWidget* parent): QWidget(parent){
    setMinimumSize(600, 400);
    pen.setWidth(2);
}

Canvas::~Canvas(){
    for(Shape* shape : shapes){
        delete shape;
    }
    shapes.clear();
}

void Canvas::setPenColor(const QColor& c){
    if(editMode && selected){
        QPen p = selected->getPen();
        p.setColor(c);
        selected->setPen(p);
        dirty = true;
    }
    else{
        pen.setColor(c);
    }

    update();
}

void Canvas::setPenWidth(int w){
    if(editMode && selected){
        QPen p = selected->getPen();
        p.setWidth(w);
        selected->setPen(p);
        dirty = true;
    }
    else{
        pen.setWidth(w);
    }

    update();
}

void Canvas::setPenStyle(Qt::PenStyle s){
    if(editMode && selected){
        QPen p = selected->getPen();
        p.setStyle(s);
        selected->setPen(p);
        dirty = true;
    }
    else{
        pen.setStyle(s);
    }

    update();
}

void Canvas::setTool(Canvas::Tool t){
    tool = t;
}

void Canvas::setEditMode(bool on){
    editMode = on;
    if(!editMode){
        isEditing = false;
        selected = nullptr;
        activeHandle = -2;
    }
    update();
}

void Canvas::paintEvent(QPaintEvent* e){
    QWidget::paintEvent(e);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    painter.setPen(pen);

    for(const Shape* shape : shapes){
        shape->paint(painter);
    }

    painter.setPen(pen);

    if(!editMode && isDrawing){
        switch(tool){
        case Tool::Line:
            painter.drawLine(QLineF(startPoint, currentPoint));
            break;
        case Tool::Rect:
            painter.drawRect(QRectF(startPoint, currentPoint).normalized());
            break;
        case Tool::Ellipse:
            painter.drawEllipse(QRectF(startPoint, currentPoint).normalized());
            break;
        }
    }

    if (editMode && selected){
        selected->paintSelection(painter, tolerance);
    }

}


void Canvas::mousePressEvent(QMouseEvent* e){
    if(e->button() != Qt::LeftButton){
        return;
    }

    QPointF P = e->pos();

    if(editMode){
        selected = nullptr;
        activeHandle = -2;

        for(int i = shapes.size() - 1; i >= 0; i--){
            int hit = shapes[i]->hitTest(P, tolerance);
            if(hit != -2){
                selected = shapes[i];
                activeHandle = hit;
                isEditing = true;
                lastMousePos = P;
                update();
                return;
            }
        }

        selected = nullptr;
        activeHandle = -2;
        isEditing = false;
        update();
        return;
    }

    isDrawing = true;
    startPoint = P;
    currentPoint = P;
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* e){
    QPointF P = e->pos();

    if (editMode){
        if (isEditing && selected){
            QPointF d = P - lastMousePos;
            if (activeHandle == -1)      selected->moveBy(d);
            else if (activeHandle >= 0)  selected->resizeTo(activeHandle, P);
            lastMousePos = P;
            update();
        }
        return;
    }

    if (isDrawing){
        currentPoint = P;
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* e){
    if (e->button() != Qt::LeftButton) return;

    if (editMode) {
        if (isEditing) {
            isEditing = false;
            dirty = true;
            update();
        }
        return;
    }

    if (isDrawing) {
        isDrawing = false;
        switch (tool) {
        case Tool::Line: {
            Shape* lineShape = new LineShape(QLineF(startPoint, e->pos()), pen);
            shapes.append(lineShape);
            break;
        }
        case Tool::Rect: {
            QRectF rect = QRectF(startPoint, e->pos()).normalized();
            Shape* rectShape = new RectShape(rect, pen);
            shapes.append(rectShape);
            break;
        }
        case Tool::Ellipse: {
            QRectF ellipseRect = QRectF(startPoint, e->pos()).normalized();
            Shape* ellipseShape = new EllipseShape(ellipseRect, pen);
            shapes.append(ellipseShape);
            break;
        }
        }
        dirty = true;
        update();
    }
}

bool Canvas::save(){
    QDir root(QCoreApplication::applicationDirPath() + "/../../..");
    root.mkpath("saved_pictures");
    const QString file = root.filePath(
        "saved_pictures/" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".jpg"
        );
    return this->grab().save(file, "JPG", 95);
}

