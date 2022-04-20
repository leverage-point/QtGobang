#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include"mainwindow.h"
#include<QMediaplayer>
#include<QMediaPlaylist>

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

    QMediaPlayer *startSound/* = new QMediaPlayer*/;//”Œœ∑±≥æ∞“Ù¿÷
    QMediaPlaylist * musiclist;
    MainWindow * w = NULL;

protected:
    //÷ÿ–¥ªÊÕº ¬º˛£¨ª≠±≥æ∞Õº
    void paintEvent(QPaintEvent */*event*/);


private:
    Ui::StartWindow *ui;
};

#endif // STARTWINDOW_H
