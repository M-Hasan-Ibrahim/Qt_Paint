#ifndef SHAPE_H
#define SHAPE_H

#include "QPen"
#include "QPainter"

class Shape{
public:
    explicit Shape(const QPen& p) : pen(p) {}
    virtual ~Shape() = default;

    virtual void paint(QPainter& painter) const = 0;

    virtual int hitTest(const QPointF& p, qreal tolerance) const = 0;
    virtual void moveBy(const QPointF& d) = 0;
    virtual void resizeTo(int handle, const QPointF& p) = 0;
    virtual void paintSelection(QPainter& painter, qreal tolerance) const = 0;

    const QPen& getPen() const {return pen;}
    void setPen(const QPen& p) {pen = p;}

    static inline QRectF handleRectAt(const QPointF& p, qreal tolerance){
        return QRectF(p.x() - tolerance, p.y() - tolerance, 2*tolerance, 2*tolerance);
    }

protected:
    QPen pen;
};

class LineShape : public Shape {
public:
    LineShape(const QLineF& l, const QPen& p) : Shape(p), line(l){}
    void paint(QPainter& painter) const override{
        painter.setPen(pen);
        painter.drawLine(line);
    }

    int hitTest(const QPointF& p, qreal tolerance) const override {
        if (handleRectAt(line.p1(), tolerance).contains(p)){
            return 0;
        }
        if (handleRectAt(line.p2(), tolerance).contains(p)){
            return 1;
        }

        QLineF clickSegment1(p.x() - tolerance, p.y() - tolerance, p.x() + tolerance, p.y() + tolerance);
        QLineF clickSegment2(p.x() - tolerance, p.y() + tolerance, p.x() + tolerance, p.y() - tolerance);

        QPointF ignore;

        if (line.intersects(clickSegment1, &ignore) == QLineF::BoundedIntersection){
            return -1;
        }
        if (line.intersects(clickSegment2, &ignore) == QLineF::BoundedIntersection){
            return -1;
        }

        return -2;
    }

    void moveBy(const QPointF& d) override {
        line.translate(d);
    }

    void resizeTo(int handle, const QPointF& p) override {
        if (handle == 0) {
            line.setP1(p);
        }
        else if (handle == 1){
            line.setP2(p);
        }
    }

    void paintSelection(QPainter& painter, qreal tolerance) const override {
        QPen selection(Qt::black);
        selection.setStyle(Qt::DashLine);
        painter.setPen(selection);
        painter.drawLine(line);
        painter.drawRect(handleRectAt(line.p1(), tolerance));
        painter.drawRect(handleRectAt(line.p2(), tolerance));
    }

private:
    QLineF line;
};

class RectShape : public Shape{
public:
    RectShape(const QRectF& r, const QPen& p) : Shape(p), rect (r){}

    void paint(QPainter& painter) const override {
        painter.setPen(pen);
        painter.drawRect(rect);
    }

    int hitTest(const QPointF& p, qreal tolerance) const override{
        QPointF p0 = rect.topLeft();
        QPointF p1 = rect.topRight();
        QPointF p2 = rect.bottomRight();
        QPointF p3 = rect.bottomLeft();

        if(handleRectAt(p0, tolerance).contains(p)){
            return 0;
        }
        if(handleRectAt(p1, tolerance).contains(p)){
            return 1;
        }
        if(handleRectAt(p2, tolerance).contains(p)){
            return 2;
        }
        if(handleRectAt(p3, tolerance).contains(p)){
            return 3;
        }

        if(rect.contains(p)){
            QRectF inner = rect.adjusted(tolerance, tolerance, -tolerance, -tolerance);
            if(!inner.contains(p)){
                return 4;
            }
            return -1;
        }
        return -2;
    }

    void moveBy(const QPointF& d) override {
        rect.translate(d);
    }

    void resizeTo(int handle, const QPointF& p) override{
        QRectF r = rect;
        switch(handle){
        case 0: r.setTopLeft(p); break;
        case 1: r.setTopRight(p); break;
        case 2: r.setBottomRight(p); break;
        case 3: r.setBottomLeft(p); break;
        case 4: {
            QPointF corners[4] = {r.topLeft(), r.topRight(), r.bottomRight(), r.bottomLeft()};
            int best = 0;
            qreal bestd = QLineF(p, corners[0]).length();
            for(int i = 1; i < 4; i++){
                qreal d = QLineF(p, corners[i]).length();
                if(d < bestd){
                    best = i;
                    bestd = d;
                }
            }
            switch(best){
            case 0: r.setTopLeft(p); break;
            case 1: r.setTopRight(p); break;
            case 2: r.setBottomRight(p); break;
            case 3: r.setBottomLeft(p); break;
            }
        }
        break;
        default: break;
        }
        rect = r.normalized();
    }

    void paintSelection(QPainter& painter, qreal tolerance) const override{
        QPen selection(Qt::black);
        selection.setStyle(Qt::DashLine);
        painter.setPen(selection);
        painter.drawRect(rect);

        painter.drawRect(handleRectAt(rect.topLeft(), tolerance));
        painter.drawRect(handleRectAt(rect.topRight(), tolerance));
        painter.drawRect(handleRectAt(rect.bottomRight(), tolerance));
        painter.drawRect(handleRectAt(rect.bottomLeft(), tolerance));
    }

private:
    QRectF rect;
};

class EllipseShape : public Shape {
public:
    EllipseShape(const QRectF& e, const QPen& p) : Shape(p), ellipse(e) {}

    void paint(QPainter& painter) const override {
        painter.setPen(pen);
        painter.drawEllipse(ellipse);
    }

    int hitTest(const QPointF& p, qreal tolerance) const override{
        QPointF p0 = ellipse.topLeft();
        QPointF p1 = ellipse.topRight();
        QPointF p2 = ellipse.bottomRight();
        QPointF p3 = ellipse.bottomLeft();

        if(handleRectAt(p0, tolerance).contains(p)){
            return 0;
        }
        if(handleRectAt(p1, tolerance).contains(p)){
            return 1;
        }
        if(handleRectAt(p2, tolerance).contains(p)){
            return 2;
        }
        if(handleRectAt(p3, tolerance).contains(p)){
            return 3;
        }

        if(ellipse.contains(p)){
            QRectF inner = ellipse.adjusted(tolerance, tolerance, -tolerance, -tolerance);
            if(!inner.contains(p)){
                return 4;
            }
            return -1;
        }

        return -2;
    }

    void moveBy(const QPointF& d) override {
        ellipse.translate(d);
    }

    void resizeTo(int handle, const QPointF& p) override {
        QRectF e = ellipse;
        switch (handle) {
        case 0: e.setTopLeft(p); break;
        case 1: e.setTopRight(p); break;
        case 2: e.setBottomRight(p); break;
        case 3: e.setBottomLeft(p); break;
        case 4:
        {
            QPointF corners[4] = {e.topLeft(), e.topRight(), e.bottomRight(), e.bottomLeft()};
            int best = 0; qreal bestd = QLineF(p, corners[0]).length();
            for (int i=1;i<4;++i){ qreal d = QLineF(p, corners[i]).length(); if (d<bestd){best=i;bestd=d;}}
            switch(best){
            case 0: e.setTopLeft(p); break;
            case 1: e.setTopRight(p); break;
            case 2: e.setBottomRight(p); break;
            case 3: e.setBottomLeft(p); break;
            }
        }
        break;
        default: break;
        }
        ellipse = e.normalized();
    }

    void paintSelection(QPainter& painter, qreal tolerance) const override {
        QPen selection(Qt::black);
        selection.setStyle(Qt::DashLine);
        painter.setPen(selection);
        painter.drawRect(ellipse);

        painter.drawRect(handleRectAt(ellipse.topLeft(), tolerance));
        painter.drawRect(handleRectAt(ellipse.topRight(), tolerance));
        painter.drawRect(handleRectAt(ellipse.bottomRight(), tolerance));
        painter.drawRect(handleRectAt(ellipse.bottomLeft(), tolerance));
    }

private:
    QRectF ellipse;
};

#endif // SHAPE_H
