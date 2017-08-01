#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QByteArray>
#include <QTextCodec>
#include <QPalette>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

const QString displayImgPath = "../ai_ipc/test/test500.jpg";

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QPixmap bkgnd("/home/phoenix/Desktop/AiHome/sw/Ai_IPC/ai_ipc/images/Background_1.jpg");
    QPixmap bkgnd("../ai_ipc/images/Background_1.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);

    calTime = new QTime();
    inImg = new QPixmap();

    ui->label_img->setPixmap( QPixmap("../ai_ipc/images/DeCom.png") );

    ui->label_info->setText("等待输入图片");

    connect( ui->pushButton_ipc, SIGNAL(clicked()), this, SLOT(openIpcSlot()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


// Send signal to camera thread
void MainWindow::openIpcSlot(void)
{
     emit openIpcSig();
}

void MainWindow::displayImgSlot()
{
    QImage img( displayImgPath );

    // Conver image to accepted size and format
    *inImg = inImg->fromImage(img.scaled(256,256,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    ui->label_img->setPixmap( *inImg );
}

void MainWindow::getPredictResultSlot( int code, int percent )
{
    // Display result of predict
    ui->label_info->setText( "<h4>Predict time： " + QString::number( code ) + "ms" + "</h4>");
}
