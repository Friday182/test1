#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QIODevice>
#include <QFile>
#include <QFileDialog>
#include <QTime>
#include <QTimer>
#include <QProcess>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void imgIsReady(void);
    void openIpcSig( void );

public slots:
    void openIpcSlot(void);
    void getPredictResultSlot(int, int);
    void displayImgSlot(void);

private:
    Ui::MainWindow *ui;
    QTime *calTime;

    QPixmap *inImg;

};

#endif // MAINWINDOW_H
