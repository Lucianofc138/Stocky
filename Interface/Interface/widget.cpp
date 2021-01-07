#include "widget.h"
#include "ui_widget.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    bool pf2=false;
    bool pmp2=true;
    ui->setupUi(this);
    mMediaPlayer = new QMediaPlayer(this);
    mVideoWidget= new QVideoWidget(this);
    mMediaPlayer->setVideoOutput(mVideoWidget);

    mCamera=new QCamera(this);
    mCameraViewfinder= new QCameraViewfinder(this);
    mCameraImageCapture= new QCameraImageCapture(mCamera,this);
    mLayout= new QVBoxLayout;
    mCamera->setViewfinder(mCameraViewfinder);

    mOpcionesMenu= new QMenu("Opciones",this);
    mEncenderAction= new QAction("Encender",this);
    mApagarAction= new QAction("Apagar",this);
    mProdF= new QAction("SimProdF",this);
    mPMP= new QAction("SimProdMP",this);

    mOpcionesMenu->addActions({mEncenderAction,mApagarAction,mProdF,mPMP});
    ui->options->setMenu(mOpcionesMenu);
    mLayout->addWidget(mCameraViewfinder);
    ui->scrollArea_2->setWidget(mVideoWidget);
    ui->scrollArea->setLayout(mLayout);

    connect(mEncenderAction,&QAction::triggered,[&](){
        mCamera->start();
    });

    connect(mApagarAction,&QAction::triggered,[&](){
        mCamera->stop();
    });
    connect(mApagarAction,&QAction::triggered,[&](){
        mCamera->stop();
    });
    connect(mProdF,&QAction::triggered,[&](){
        pf2 = !pf2;
        if(pf2){

            ui->pf2->setText("***Camara2");

        }
        else{
            ui->pf2->setText("");
        }



    });
    connect(mPMP,&QAction::triggered,[&](){
        pmp2 = !pmp2;
        if(pmp2){

            ui->pmp2->setText("***Camara2");

        }
        else{
            ui->pmp2->setText("");
        }

    });






}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_abrir_clicked()
{
    bool pf1=true;
    bool pmp1=true;
    /* Se elije nombre a procesar para video */
   QString filename= QFileDialog::getOpenFileName(this,"Abrir"); // Con este Nombre se elije el video a procesar
   if(filename.isEmpty()){
       return ;
   }
   /* Aca se ingresan funciones de procesar video (Cambiar filename por variable correpondiente)  */

   // Actualizar valores booleanos de pf1 y pmp1 para actaliuzar los labels segun se detecte producto faltantes o no..


   /* Cambiar valores de label*/

   if(pf1){
       ui->pf1->setText("***Camara1");
   }
   else{
       ui->pf1->setText("***Camara1");
   }

   if(pmp1){
       ui->pf1->setText("***Camara1");
   }
   else{
       ui->pf1->setText("***Camara1");
   }

   /* Proceso para mostrar video */
   mMediaPlayer->setMedia(QUrl::fromLocalFile(filename)); // Cambiar filename por video ya procesado creado
   mMediaPlayer->setVolume(0);


   on_play_clicked();
}

void Widget::on_play_clicked()
{
    mMediaPlayer->play();

}

void Widget::on_pause_clicked()
{
    mMediaPlayer->pause();

}
