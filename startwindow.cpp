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


    musiclist = new QMediaPlaylist;//创建播放列表
    startSound = new QMediaPlayer;//游戏背景音乐
    musiclist->addMedia(QUrl(BACKGROUND_SOUND));//添加音乐
    musiclist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);//循环播放
    startSound->setPlaylist(musiclist);
    startSound->setVolume(20);
    startSound->play();


    //开始按钮
    MyPushButton * startBtn = new MyPushButton(":/new/image/begingame.png");
    startBtn->setParent(this);
    startBtn->move(this->width()*0.5 - startBtn->width()*0.5, this->height()*0.75);

    // 添加菜单
    QMenu *gameMenu = menuBar()->addMenu(tr("Game option")); // menuBar默认是存在的，直接加菜单就可以了
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

    //实例化游戏主界面
    w = new MainWindow;
    //监听游戏界面的返回按钮的信号
    connect(w, &MainWindow::chooseScenceBack, this, [=](){
        w->hide();//隐藏游戏主界面
        this->show();//重新显示开始界面
    });

    connect(startBtn, &MyPushButton::clicked, [=](){
        //做弹起特效
        startBtn->zoom1();
        startBtn->zoom2();

        //延时 0.6s 进入到游戏界面
        QTimer::singleShot(600,this,[=](){
            //自身隐藏
            this->hide();
            //显示游戏主界面
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
