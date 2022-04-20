#include "startwindow.h"
#include "ui_startwindow.h"

#include<QPainter>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QMessageBox>
#include<QTimer>
#include"mypushbutton.h"
#include"mainwindow.h"

#define BACKGROUND_SOUND "qrc:new/sound/background.wav"

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    this->setFixedSize(660, 660);
    setWindowTitle("StartWindow");


    musiclist = new QMediaPlaylist;//���������б�
    startSound = new QMediaPlayer;//��Ϸ��������
    musiclist->addMedia(QUrl(BACKGROUND_SOUND));//�������
    musiclist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//ѭ������
    startSound->setPlaylist(musiclist);
    startSound->setVolume(20);
    startSound->play();


    //��ʼ��ť
    MyPushButton * startBtn = new MyPushButton(":/new/image/begingame.png");
    startBtn->setParent(this);
    startBtn->move(this->width()*0.5 - startBtn->width()*0.5, this->height()*0.75);

    // ��Ӳ˵�
    QMenu *gameMenu = menuBar()->addMenu(tr("Game option")); // menuBarĬ���Ǵ��ڵģ�ֱ�ӼӲ˵��Ϳ�����
    QAction *quitGame = new QAction("Quit the game", this);
    connect(quitGame, SIGNAL(triggered()), this, SLOT(close()));
    gameMenu->addAction(quitGame);

    QAction *help = new QAction("Game Rules", this);
    connect(help, &QAction::triggered, [=](){
        QMessageBox * dig = new QMessageBox(this);
        dig->setWindowTitle("Game Description");
        dig->setInformativeText("Gobang Game!");
        dig->exec();
    });
    gameMenu->addAction(help);

    //ʵ������Ϸ������
    w = new MainWindow;
    //������Ϸ����ķ��ذ�ť���ź�
    connect(w, &MainWindow::chooseScenceBack, this, [=](){
        w->hide();//������Ϸ������
        this->show();//������ʾ��ʼ����
    });

    connect(startBtn, &MyPushButton::clicked, [=](){
        //��������Ч
        startBtn->zoom1();
        startBtn->zoom2();

        //��ʱ 0.6s ���뵽��Ϸ����
        QTimer::singleShot(600,this,[=](){
            //��������
            this->hide();
            //��ʾ��Ϸ������
            w->show();
        });
    });


}

StartWindow::~StartWindow()
{
    delete ui;
}

void StartWindow::paintEvent(QPaintEvent */*event*/)
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/new/image/background.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

}
