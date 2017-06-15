
#include <QDebug>
#include "clsdnn.h"

using namespace cv;
using namespace cv::dnn;

#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

const string modelFile = "../ai_ipc/3party/caffe/Model/googlenet_train_val.prototxt";
//const string trainedFile = "../ai_ipc/3party/caffe/Model/ajguard.caffemodel";
const string trainedFile = "../ai_ipc/3party/caffe/Model/bvlc_googlenet.caffemodel";
//const string modelFile = "../ai_ipc/3party/caffe/model_hands/submitnet.prototxt";
//const string trainedFile = "../ai_ipc/3party/caffe/model_hands/1miohandsv2.caffemodel";

const string meanFile = "../ai_ipc/3party/caffe/Model/mean.binaryproto";
const string labelFile = "../ai_ipc/3party/caffe/Model/labels.txt";
const string imageFile = "../ai_ipc/test/test1000.jpg";

clsdnn::clsdnn(QObject *parent) : QObject(parent)
{

}

void clsdnn::initClsDnnSlot()
{
    qDebug() << "Init DNN thread!";

    cv::dnn::initModule();  //Required if OpenCV is built as static libs
    net = dnn::readNetFromCaffe(modelFile, trainedFile);
    if (net.empty())
    {
        qDebug() << "Init DNN failed!";

        emit initDnnFailedSig();
    }

    calTime = new QTime();
}

void clsdnn::inputImgReadySlot()
{
    Mat img = imread(imageFile);
    if (img.empty())
    {
        qDebug() << "Init DNN failed!";

        //emit initDnnFailedSig();
    }
    resize(img, img, Size(224, 224));                   //GoogLeNet accepts only 224x224 RGB-images
    Mat inputBlob = blobFromImage(img);                 //Convert Mat to batch of images
    net.setBlob(".data", inputBlob);                    //set the network input
    calTime->restart();
    net.forward();                                      //compute output

    Mat prob = net.getBlob("softmax");                     //gather output of "prob" layer
    int classId;
    double classProb;
    getMaxClass(prob, &classId, &classProb);            //find the best class

    qDebug() << "Best class: #" + QString::number( classId );
    qDebug() << "Probability: " + QString::number( classProb * 100 ) + "%";
    qDebug() << "Dnn predict time: " + QString::number( calTime->elapsed() );

    // Send result to others
    emit dnnResultSig(classId, (int)(classProb * 100));
}

/* Find best class for the blob (i. e. class with maximal probability) */
void clsdnn::getMaxClass(const Mat &probBlob, int *classId, double *classProb)
{
    Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix
    Point classNumber;
    minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
    *classId = classNumber.x;
}
