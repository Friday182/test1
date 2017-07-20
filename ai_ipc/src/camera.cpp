
#include <QDebug>
#include <QFile>

#include "camera.h"

#define FPS_MS              25                          // Capture an img every 25 ms
#define COUNT_100MS         (100/FPS_MS)
#define COUNT_500MS         (500/FPS_MS)
#define COUNT_1000MS        (1000/FPS_MS)
#define COUNT_3000MS        (3000/FPS_MS)

using namespace cv;

camera::camera(QObject *parent) : QObject(parent)
{

}

void camera::initProcessSlot( void )
{
    qDebug() << "Init camera thread:";

    counter100ms = 0;
    counter500ms = 0;
    counter1000ms = 0;
    idx = 0;

    camTimer25ms = new QTimer();
    calTime = new QTime();

    // Function: Timer to trigger image capture periodically
    connect( camTimer25ms, SIGNAL(timeout()), this, SLOT(camTimer25msSlot()) );

    //cam = cvCaptureFromCAM(CV_CAP_ANY);     // open default camera
    cam = new VideoCapture(CV_CAP_ANY);

    if( cam->isOpened() )
    {
        cam->set(CV_CAP_PROP_FRAME_WIDTH, 256);
        cam->set(CV_CAP_PROP_FRAME_HEIGHT,256);

        camTimer25ms->start( FPS_MS );
        qDebug() << "Open camera OK!";
    }
    else
    {
        qDebug() << "Open camera failed!";

        // something wrong
        emit errorIndicator();
    }

}


void camera::camTimer25msSlot( void )
{
    counter100ms++;
    counter500ms++;
    counter1000ms++;

    /* Note: capture an image need 11ms(1280x1024)
     * To save an image cause 30ms, so the timing is an issue here!!
     * Pass the img to CNN may save the time because avoid file write/read
     * Run on my HP Z15 laptop, with ubuntu 14, QT5.8 */
    //calTime->restart();
    static Mat img;
    *cam >> img;           // get a new frame from camera

    //qDebug() << "read img time: " + QString::number( calTime->elapsed() );
    if(!img.empty())
    {
        //cv::cvtColor(img, img, CV_BGR2RGB);
        //cv::flip(img, img, 1);
        //cvShowImage("Test", img);
        imwrite("../ai_ipc/test/test.jpg", img);
        emit imgReady25msSig();

        if( counter100ms >= COUNT_100MS )
        {
            imwrite("../ai_ipc/test/test100.jpg", img);

            // Inform other process
            emit imgReady100msSig();

            counter100ms = 0;
        }

        if( counter500ms >= COUNT_500MS )
        {
            imwrite("../ai_ipc/test/test500.jpg", img);

            // Inform other process
            emit imgReady500msSig();
            //QImage qimg((uchar*)img.data, img.cols, img.rows, QImage::Format_RGB32);
            //emit imgReadySig(img);

            counter500ms = 0;
        }

        if( counter1000ms >= COUNT_1000MS )
        {
            imwrite("../ai_ipc/test/test1000.jpg", img);

            // Inform other process
            emit imgReady1000msSig();
            cas.casGetImgSlot(img);
            //emit imgReadySig(&img);

            //calTime->restart();
            //qDebug() << "save img time: " + QString::number( calTime->elapsed() );
            counter1000ms = 0;
        }

        //processImg( "../ai_ipc/test/test.jpg" );
    }
    else
    {
        counter100ms = 0;
        counter500ms = 0;
        counter1000ms = 0;
    }
}
