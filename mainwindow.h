#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameModel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    GameModel *game;//游戏指针

signals:
    //自定义信号，告知开始界面点击了返回
    //自定义信号只需要声明，不需要实现
    void chooseScenceBack();

protected:
    //绘制
    void paintEvent(QPaintEvent *);
    //监听鼠标移动情况，方便落子
    void mouseMoveEvent(QMouseEvent *event);
    //实际落子
    void mouseReleaseEvent(QMouseEvent *);

private:
    GameType game_type;//存储游戏类型
    int clickPosRow, clickPosCol;//存储将点击的位置
    void initGame();//初始化游戏

private slots:
    void chessOneByPerson();//人执行
    void chessOneByAI();//AI下棋
    void chessOneByAI2();//AI下棋

    void initPVPGame();//人人对战
    void initPVEGame();//人机对战
    void initPVEGame2();//人机对战
};

#endif // MAINWINDOW_H
