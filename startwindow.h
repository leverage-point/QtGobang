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

    QMediaPlayer *startSound/* = new QMediaPlayer*/;//��Ϸ��������
    QMediaPlaylist * musiclist;
    MainWindow * w = NULL;

protected:
    //��д��ͼ�¼���������ͼ
    void paintEvent(QPaintEvent */*event*/);


private:
    Ui::StartWindow *ui;
};

#endif // STARTWINDOW_H
