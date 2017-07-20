#include "clscaffe.h"
#include <QDebug>
#include <QThread>

//const QString caffeBin = "../ai_ipc/3party/caffe/bin_cpu/classification.bin ";
//const QString param = "../ai_ipc/3party/caffe/snmodel_hands_3/deploy.prototxt ../ai_ipc/3party/caffe/snmodel_hands_3/SN_hands_3.caffemodel ../ai_ipc/3party/caffe/snmodel_hands_3/mean.binaryproto ../ai_ipc/3party/caffe/snmodel_hands_3/labels.txt ../ai_ipc/test/test1000.jpg";

const QString caffeBin = "/home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/3party/caffe/bin_cpu/classification.bin ";
const QString param = "/home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/3party/caffe/snmodel_hands_3/deploy.prototxt /home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/3party/caffe/snmodel_hands_3/SN_hands_3.caffemodel /home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/3party/caffe/snmodel_hands_3/mean.binaryproto /home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/3party/caffe/snmodel_hands_3/labels.txt /home/phoenix/Desktop/AiHome/sw/test1/ai_ipc/test/test1000.jpg";

const QString inImgPath = "../ai_ipc/test/test1000.jpg";
const QString tmpImgPath = "../ai_ipc/test/temp.jpg";

clscaffe::clscaffe(QObject *parent) : QObject(parent)
{
    // Don't init here, otherwise, all object running in main thread
    cnnCaffeInitSlot();
}


void clscaffe::cnnCaffeInitSlot()
{
    calTime     = new QTime();
    imgProcess  = new QProcess();
    inImg       = new QPixmap();

    connect( imgProcess, SIGNAL(finished(int)), this, SLOT(imgProcFinishSlot(int)));
}

void clscaffe::inImgSlot()
{
    QImage img( inImgPath );

    // Conver image to accepted size and format
    *inImg = inImg->fromImage(img.scaled(256,256,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QFile file( tmpImgPath );
    file.open(QIODevice::WriteOnly);
    inImg->save(&file, "jpeg",100);
    file.close();

    qDebug() << "command:" + caffeBin + param;

    calTime->restart();
    // Call external exe to process detection and get result
    imgProcess->start( caffeBin+param, QIODevice::ReadWrite );
    imgProcess->waitForFinished( 3000 );
}


void clscaffe::imgProcFinishSlot( int code )
{
    int pos = 0;
    int pos1 = 0;

    QByteArray outArray = imgProcess->readAllStandardOutput();
    QByteArray errArray = imgProcess->readAllStandardError();
    qDebug() << "Error:" + QString::fromStdString( errArray.toStdString() );
    QString result = QString::fromStdString( outArray.toStdString() );
    qDebug() << "Output:" + result;

    pos = result.indexOf( "\"A", 0 );
    pos1 = result.indexOf( "\"B", 0 );


    qDebug() << "pos:" + QString::number(pos);
    qDebug() << "pos1:" + QString::number(pos1);

    emit cnnResultSig(pos, pos1);


    // Display how long to process this image
    qDebug() << "<h4>识别此图片耗时： " + QString::number( calTime->elapsed() ) + "ms(毫秒）" + "</h4>";
}
