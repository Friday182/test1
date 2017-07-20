#ifndef CASCADE_H
#define CASCADE_H

#include <QObject>

#include "highgui.hpp"
#include "imgproc.hpp"

using namespace cv;

class cascade : public QObject
{
    Q_OBJECT

public:
    explicit cascade(QObject *parent = 0);

public slots:
    void casGetImgSlot(Mat);

private:
    void detectAndDisplay( Mat frame );

};

#endif // CASCADE_H
