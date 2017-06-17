/***********************************************************************************
 *  Ai IPC main funciton
 *  Author: pyin
 *
 *  Copyright @AiHome Ltd. 2017
 * ********************************************************************************/
#include <QApplication>
#include <QThread>

#include "mainwindow.h"
//#include "classifier.h"
#include "camera.h"
#include "clsdnn.h"
#include "clscaffe.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow      w;                      // For a released version, window is not required
    camera          newCam;
    //classifier      cls;
    clsdnn          dnn;
    clscaffe        cnn;

    QApplication::addLibraryPath("./imageformats");

    // Function: Open IPC in newCam thread through push button on GUI
    QObject::connect(&w, SIGNAL(openIpcSig()), &newCam, SLOT(initProcessSlot()));

    // Function: newCam thread send captured imgage to CNN model every 1000 ms
    //QObject::connect(&newCam, SIGNAL(imgReady1000msSig()), &cls, SLOT(classifySlot()));

    // Function: newCam thread send captured imgage to DNN model every 1000 ms
    QObject::connect(&newCam, SIGNAL(imgReady1000msSig()), &dnn, SLOT(inputImgReadySlot()));

    // Function: newCam thread send captured imgage to CNN model every 1000 ms
    QObject::connect(&newCam, SIGNAL(imgReady1000msSig()), &cnn, SLOT(inImgSlot()));

    // Function: newCam thread send captured imgage to GUI for display purpose every 500 ms
    QObject::connect(&newCam, SIGNAL(imgReady500msSig()), &w, SLOT(displayImgSlot()));

    // Function: newCam thread send image to RTSP server every 25 ms
    //QObject::connect(&newCam, SIGNAL(imgReady25msSig()), &w, SLOT(classify()));

    // Function: classifier send predict result to GUI for display
    //QObject::connect(&cls, SIGNAL(finishPredictSig( int, int )), &w, SLOT(getPredictResultSlot(int, int)));

    // Function: dnn classifier send predict result to GUI for display
    QObject::connect(&dnn, SIGNAL(dnnResultSig( int, int )), &w, SLOT(getPredictResultSlot(int, int)));

    // Function: dnn classifier send predict result to GUI for display
    //QObject::connect(&cnn, SIGNAL(cnnResultSig( int, int )), &w, SLOT(getPredictResultSlot(int, int)));

    // Function: classifier send predict result to other threads
    //QObject::connect(&cls, SIGNAL(finishPredictSig( int, int )), &w, SLOT(getPredictResult(int, int)));


    /***************************************************************************************
    * Here to make objects run in their own thread
    ***************************************************************************************/
    QThread *workerNewCam = new QThread();
    newCam.moveToThread( workerNewCam );
    workerNewCam->start();
    workerNewCam->setPriority( QThread::HighPriority );

/*
    QThread *workerCls = new QThread();
    cls.moveToThread( workerCls );
    workerCls->setPriority( QThread::HighPriority );
    workerCls->start();
*/
    QThread *workerDnn = new QThread();
    // Function: init dnn module when thread start
    QObject::connect(workerDnn, SIGNAL(started()), &dnn, SLOT(initClsDnnSlot()));
    dnn.moveToThread( workerDnn );
    workerDnn->start();
    workerDnn->setPriority( QThread::HighPriority );

    QThread *workerCnn = new QThread();
    // Function: init cnn module when thread start
    QObject::connect(workerCnn, SIGNAL(started()), &cnn, SLOT(cnnCaffeInitSlot()));
    cnn.moveToThread( workerCnn );
    workerCnn->start();
    workerCnn->setPriority( QThread::HighPriority );

    w.show();

    return a.exec();
}
