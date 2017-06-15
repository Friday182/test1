#ifndef CLADNN_H
#define CLADNN_H

#include <QObject>
#include <QTime>

#include <dnn.hpp>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "imgproc.hpp"
#include "imgcodecs.hpp"

using namespace cv;
using namespace cv::dnn;

class clsdnn : public QObject
{
    Q_OBJECT
public:
    explicit clsdnn(QObject *parent = 0);

signals:
    void initDnnFailedSig(void);
    void dnnResultSig(int,int);

public slots:
    void initClsDnnSlot(void);
    void inputImgReadySlot(void);

private:
    Net net;
    QTime *calTime;

    void getMaxClass(const Mat &probBlob, int *classId, double *classProb);
};

#endif // CLADNN_H
