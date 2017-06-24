#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTimer>
#include <QTime>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

class camera : public QObject
{
    Q_OBJECT
public:
    explicit camera(QObject *parent = 0);

signals:
    void imgReady25msSig( void );
    void imgReady100msSig( void );
    void imgReady500msSig( void );
    void imgReady1000msSig( void );

    void errorIndicator( void );
public slots:
    void initProcessSlot();

    void camTimer25msSlot( void );

private:
    QTimer *camTimer25ms;
    QTime *calTime;
    int counter100ms;
    int counter500ms;
    int counter1000ms;
    int idx;

    CvCapture* cam;
};

#endif // CAMERA_H
