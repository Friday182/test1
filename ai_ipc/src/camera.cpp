
#include <QDebug>

#include "camera.h"

#define FPS_MS              25                          // Capture an img every 25 ms
#define COUNT_100MS         (100/FPS_MS)
#define COUNT_500MS         (500/FPS_MS)
#define COUNT_1000MS        (1000/FPS_MS)

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

    camTimer25ms = new QTimer();
    calTime = new QTime();

    // Function: Timer to trigger image capture periodically
    connect( camTimer25ms, SIGNAL(timeout()), this, SLOT(camTimer25msSlot()) );

    cam = cvCaptureFromCAM(CV_CAP_ANY);     // open default camera

    if( NULL != cam )
    {
        cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_WIDTH, 1280);//设置图像属性 宽和高
        cvSetCaptureProperty(cam, CV_CAP_PROP_FRAME_HEIGHT,1080);

        camTimer25ms->start( FPS_MS );
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
    IplImage *img = 0;

    counter100ms++;
    counter500ms++;
    counter1000ms++;

    /* Note: capture an image need 11ms(1280x1024)
     * To save an image cause 30ms, so the timing is an issue here!!
     * Pass the img to CNN may save the time because avoid file write/read
     * Run on my HP Z15 laptop, with ubuntu 14, QT5.8 */
    //calTime->restart();
    img = cvQueryFrame(cam);
    //qDebug() << "read img time: " + QString::number( calTime->elapsed() );
    if(NULL != img)
    {
        //cvShowImage("Test", img);
        cvSaveImage("../ai_ipc/test/test.jpg", img, 0);
        emit imgReady25msSig();

        if( counter100ms >= COUNT_100MS )
        {
            cvSaveImage("../ai_ipc/test/test100.jpg", img, 0);

            // Inform other process
            emit imgReady100msSig();

            counter100ms = 0;
        }

        if( counter500ms >= COUNT_500MS )
        {
            cvSaveImage("../ai_ipc/test/test500.jpg", img, 0);

            // Inform other process
            emit imgReady500msSig();

            counter500ms = 0;
        }

        if( counter1000ms >= COUNT_1000MS )
        {
            //calTime->restart();
            cvSaveImage("../ai_ipc/test/test1000.jpg", img, 0);
            //qDebug() << "save img time: " + QString::number( calTime->elapsed() );

            // Inform other process
            emit imgReady1000msSig();

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
