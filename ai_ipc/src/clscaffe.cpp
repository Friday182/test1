#include "clscaffe.h"
#include <QDebug>

//const QString caffeBin = "../ai_ipc/3party/caffe/bin/classification.bin ";
//const QString param = "../ai_ipc/3party/caffe/modelbin/deploy.prototxt ../ai_ipc/3party/caffe/modelbin/ajguard.caffemodel ../ai_ipc/3party/caffe/modelbin/mean.binaryproto ../ai_ipc/3party/caffe/modelbin/labels.txt ../ai_ipc/test/temp.jpg";


const QString caffeBin = "../ai_ipc/3party/caffe/bin/classification.bin ";
const QString param = "../ai_ipc/3party/caffe/alexmodel/deploy.prototxt ../ai_ipc/3party/caffe/alexmodel/ajguard.caffemodel ../ai_ipc/3party/caffe/alexmodel/mean.binaryproto ../ai_ipc/3party/caffe/alexmodel/labels.txt ../ai_ipc/test/temp.jpg";

const QString inImgPath = "../ai_ipc/test/test1000.jpg";
const QString tmpImgPath = "../ai_ipc/test/temp.jpg";

clscaffe::clscaffe(QObject *parent) : QObject(parent)
{
    // Don't init here, otherwise, all object running in main thread
}


void clscaffe::cnnCaffeInitSlot()
{
    calTime     = new QTime();
    imgProcess  = new QProcess( this );
    inImg       = new QPixmap();

    connect( imgProcess, SIGNAL(finished(int)), this, SLOT(imgProcFinishSlot(int)));
}

void clscaffe::inImgSlot()
{
    QImage img( inImgPath );

    calTime->restart();

    // Conver image to accepted size and format
    *inImg = inImg->fromImage(img.scaled(256,256,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QFile file( tmpImgPath );
    file.open(QIODevice::WriteOnly);
    inImg->save(&file, "jpeg",100);
    file.close();

    qDebug() << "command:" + caffeBin + param;

    // Call external exe to process detection and get result
    imgProcess->start( caffeBin+param, QIODevice::ReadWrite );
    imgProcess->waitForFinished( 3000 );
}


void clscaffe::imgProcFinishSlot( int code )
{
    int pos = 0;
    int pos1 = 0;

    QByteArray outArray = imgProcess->readAllStandardOutput();
    QString result = QString::fromStdString( outArray.toStdString() );

    pos = result.indexOf( "\"k", 0 );
    pos1 = result.indexOf( "\"o", 0 );

    qDebug() << "Output:" + result;
    qDebug() << "pos:" + QString::number(pos);
    qDebug() << "pos1:" + QString::number(pos1);

    emit cnnResultSig(pos, pos1);


    // Display how long to process this image
    qDebug() << "<h4>识别此图片耗时： " + QString::number( calTime->elapsed() ) + "ms(毫秒）" + "</h4>";
}
