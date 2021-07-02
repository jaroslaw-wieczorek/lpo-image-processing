#include "hough_rectangles.h"

#include "corner_harris.h"
#include "hough_lines.h"

#include <QLine>
#include <QPainter>

struct RightAngle {
    QPoint middle;
    QPoint a;
    QPoint b;

    inline bool operator==(RightAngle r) {
           if (r.middle.x() == middle.x() && r.middle.y() == middle.y() &&
                   ((r.a.x()==a.x() && r.a.y()== a.y() && r.b.x() == b.x() && r.b.y() == b.y()) ||
                    (r.a.x()==b.x() && r.a.y()== b.y() && r.b.x() == a.x() && r.b.y() == a.y())))
              return true;
           else
              return false;
        }
} ;

struct Rectangle {
    QPoint a;
    QPoint b;
    QPoint c;
    QPoint d;
} ;


HoughRectangles::HoughRectangles(PNM* img) :
    Transformation(img)
{
}

HoughRectangles::HoughRectangles(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}

PNM* HoughRectangles::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(image->copy());

    CornerHarris* cornerHarris = new CornerHarris(image);
    cornerHarris->setParameter("threshold", 30000000);
    cornerHarris->setParameter("sigma", 1.00),
    cornerHarris->setParameter("sigma_weight", 0.78),
    cornerHarris->setParameter("k", 0.0500);
    PNM* cornersImage = cornerHarris->transform();

    HoughLines* houghLines = new HoughLines(image);

    houghLines->setParameter("threshold", 15);
    houghLines->setParameter("draw_whole_lines", true);
    PNM* linesImage = houghLines->transform();

    QPainter* qPainter = new QPainter(newImage);
    QPen* pen = new QPen(Qt::black);
    pen->setWidth(3);
    qPainter->setPen(*pen);

    QMap<int, QPoint> cl; //corners on lines

    //look for corners on lines
    int position = 0;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (QColor::fromRgb(cornersImage->pixel(i,j)) == Qt::color0)
            {
                if (QColor::fromRgb(linesImage->pixel(i,j)) == Qt::color1)
                {
                    cl.insert(position, QPoint(i,j));
                    position++;
                }
            }
        }
    }

    int s = cl.size();
    int points[s][s][s];

    QList<RightAngle> rightAngles;

    //look for 90 degree angles
    int count= 0;
    for (int a = 0; a < s; a++)
    {
        for (int b = 0; b < s; b++)
        {
            for (int c = 0; c < s; c++)
            {
                if (a == b || a == c || b == c)
                {
                    points[a][b][c] = -1;
                }
                else
                {
                    double s1 = (cl[b].x() - cl[a].x());
                    double s2 = (cl[b].x() - cl[c].x());
                    double s3 = (cl[b].y() - cl[a].y());
                    double s4 = (cl[b].y() - cl[c].y());

                    if (s1*s2 + s3*s4 > -50 && s1*s2 + s3*s4 < 50)
                     {
                        points[a][b][c] = 1;
                        RightAngle ra;
                        ra.a = cl[a];
                        ra.middle = cl[b];
                        ra.b = cl[c];
                        rightAngles.push_back(ra);
                        count++;
                    }
                    else
                    {
                        points[a][b][c] = 0;
                    }
                }
            }
        }
    }

    QList<Rectangle> rectangles;


    //match rectangles
    for (QList<RightAngle>::iterator r = rightAngles.begin(); r != rightAngles.end(); r++)
    {
        bool found = false;
        QPoint newPoint1;

        for (QList<RightAngle>::iterator r1 = rightAngles.begin(); r1!=rightAngles.end(); r1++)
        {
            if ((*r1).middle.x() == (*r).a.x() && (*r1).middle.y() == (*r).a.y() &&
                (((*r1).a.x() == (*r).middle.x() && (*r1).a.y() == (*r).middle.y()) ||
                ((*r1).b.x() == (*r).middle.x() && (*r1).b.y() == (*r).middle.y())))
            {
                newPoint1 = ((*r1).a.x() == (*r).middle.x()&& (*r1).a.y() == (*r).middle.y() ? (*r1).b : (*r1).a);

                QPoint newPoint2;
                for (QList<RightAngle>::iterator r2 = rightAngles.begin(); r2!=rightAngles.end();r2++)
                {
                    if ((*r2).middle.x() == (*r).b.x() && (*r2).middle.y() == (*r).b.y() &&
                       (((*r2).a.x() == (*r).middle.x() && (*r2).a.y() == (*r).middle.y()) ||
                       ((*r2).b.x() == (*r).middle.x() && (*r2).b.y() == (*r).middle.y())))
                    {
                        newPoint2 = (((*r2).a.x() == (*r).middle.x() && (*r2).a.y() == (*r).middle.y()) ? (*r2).b : (*r2).a);

                        if (newPoint1.x() == newPoint2.x() && newPoint1.y() == newPoint2.y())
                        {
                            Rectangle rect;
                            rect.a = (*r).a;
                            rect.b = (*r).middle;
                            rect.c = (*r).b;
                            rect.d = newPoint1;
                            rectangles.push_back(rect);

                            rightAngles.removeAll(*r);
                            rightAngles.removeAll(*r1);
                            rightAngles.removeAll(*r2);

                            found = true;
                            break;
                        }
                    }
                    if (found)
                    {
                        break;
                    }
                }
            }
            if (found)
                break;
        }
    }

    for (QList<Rectangle>::iterator rect = rectangles.begin(); rect != rectangles.end(); rect++)
    {
        qPainter->drawLine((*rect).a, (*rect).b);
        qPainter->drawLine((*rect).b, (*rect).c);
        qPainter->drawLine((*rect).c, (*rect).d);
        qPainter->drawLine((*rect).d, (*rect).a);
    }

    return newImage;
}



