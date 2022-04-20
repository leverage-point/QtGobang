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
    GameModel *game;//��Ϸָ��

signals:
    //�Զ����źţ���֪��ʼ�������˷���
    //�Զ����ź�ֻ��Ҫ����������Ҫʵ��
    void chooseScenceBack();

protected:
    //����
    void paintEvent(QPaintEvent *);
    //��������ƶ��������������
    void mouseMoveEvent(QMouseEvent *event);
    //ʵ������
    void mouseReleaseEvent(QMouseEvent *);

private:
    GameType game_type;//�洢��Ϸ����
    int clickPosRow, clickPosCol;//�洢�������λ��
    void initGame();//��ʼ����Ϸ

private slots:
    void chessOneByPerson();//��ִ��
    void chessOneByAI();//AI����
    void chessOneByAI2();//AI����

    void initPVPGame();//���˶�ս
    void initPVEGame();//�˻���ս
    void initPVEGame2();//�˻���ս
};

#endif // MAINWINDOW_H
