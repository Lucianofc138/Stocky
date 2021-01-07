#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QMediaPlayer;
class QVideoWidget;
class QCamera;
class QCameraViewfinder;
class QCameraImageCapture;
class QVBoxLayout;
class QMenu;
class QAction;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_abrir_clicked();

    void on_play_clicked();

    void on_pause_clicked();

private:
    Ui::Widget *ui;
    /* Video */
    QMediaPlayer *mMediaPlayer;
    QVideoWidget *mVideoWidget;
    /* Camera */
    QCamera *mCamera;
    QCameraViewfinder *mCameraViewfinder;
    QCameraImageCapture *mCameraImageCapture;
    QVBoxLayout *mLayout;
    QMenu *mOpcionesMenu;
    QAction *mEncenderAction;
    QAction *mApagarAction;
    QAction *mProdF;
    QAction *mPMP;



};
#endif // WIDGET_H
