#ifndef CLSCAFFE_H
#define CLSCAFFE_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QIODevice>
#include <QFile>
#include <QFileDialog>
#include <QTime>
#include <QProcess>

class clscaffe : public QObject
{
    Q_OBJECT
public:
    explicit clscaffe(QObject *parent = 0);

signals:
    void cnnResultSig(int,int);

public slots:
    void cnnCaffeInitSlot(void);
    void inImgSlot(void);
    void imgProcFinishSlot( int code );

private:
    QTime       *calTime;
    QProcess    *imgProcess;
    QPixmap      *inImg;
};

#endif // CLSCAFFE_H
