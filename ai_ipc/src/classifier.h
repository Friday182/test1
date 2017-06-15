#ifndef CLASSIFIER_H
#define CLASSIFIER_H

// this should go to project define
#define CPU_ONLY    1

#include <QObject>

// caffe needed headers
#include "caffe/caffe.hpp"
#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "imgproc.hpp"
#include "imgcodecs.hpp"

using namespace caffe;
using std::string;

class classifier : public QObject
{
    Q_OBJECT
public:
    explicit classifier(QObject *parent = 0);

signals:
    void finishPredictSig( int, int );

public slots:
    void classifySlot(void);

private:
    void SetMean(const string& mean_file);
    std::vector<float> Predict(const cv::Mat& img);
    void WrapInputLayer(std::vector<cv::Mat>* input_channels);
    void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);

    boost::shared_ptr<Net<float> > cnn;
    cv::Size input_geometry;
    int num_channels;
    cv::Mat mean;
    std::vector<string> labels;
};

#endif // CLASSIFIER_H
