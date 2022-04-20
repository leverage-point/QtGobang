#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QPushButton>

class MyPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushButton(QWidget *parent = nullptr);

    //���캯��������һΪ������ʾ��ͼƬ·����������Ϊ���º���ʾ��ͼƬ·��
    MyPushButton(QString normalImg, QString pressImg = "");

    //��Ա���ԣ������û������Ĭ����ʾ·�� �Լ����º���ʾ��ͼƬ·��
    QString normalImgPath;
    QString pressImgPath;
    int state;//״̬�������캯���еڶ�������ʱ����0��ǰ���ǰΪ��һ��ͼƬ����1��ǰ���ǰ�ǵڶ���ͼƬ

    //������Ч
    void zoom1();//up
    void zoom2();//down

    //��д��ť�İ��º��ͷ��¼�
    void mousePressEvent(QMouseEvent *e);

signals:

public slots:
};

#endif // MYPUSHBUTTON_H
