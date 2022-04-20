#include "mypushbutton.h"
#include<QDebug>
#include<qpropertyanimation>

MyPushButton::MyPushButton(QWidget *parent) : QPushButton(parent)
{}

MyPushButton::MyPushButton(QString normalImg, QString pressImg/* = ""*/)
{
    this->normalImgPath =  normalImg;
    this->pressImgPath = pressImg;
    state = 0;

    QPixmap pix;
    if(!pix.load(normalImg)){
        qDebug() << "fail to load pix!";
        return;
    }

    //设置图片固定大小
    this->setFixedSize(pix.width(), pix.height());

    //设置不规则图片的样式
    this->setStyleSheet("QPushButton{border:0px;}");

    //设置图标
    this->setIcon(pix);
    //设置图标大小
    this->setIconSize(QSize(pix.width(), pix.height()));
}


void MyPushButton::zoom1()
{
    //创建动画特效
    QPropertyAnimation * animation = new QPropertyAnimation(this, "geometry");
    //设置动画的时间间隔
    animation->setDuration(200);

    //起始位置
    animation->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(), this->y()+5, this->width(), this->height()));

    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);

    //执行动画
    animation->start();

}

void MyPushButton::zoom2()
{
    //创建动画特效
    QPropertyAnimation * animation = new QPropertyAnimation(this, "geometry");
    //设置动画的时间间隔
    animation->setDuration(300);

    //起始位置
    animation->setStartValue(QRect(this->x(), this->y()+5, this->width(), this->height()));
    //结束位置
    animation->setEndValue(QRect(this->x(), this->y(), this->width(), this->height()));

    //设置弹跳曲线
    animation->setEasingCurve(QEasingCurve::OutBounce);

    //执行动画
    animation->start();
}

void MyPushButton::mousePressEvent(QMouseEvent *e)
{
    if(this->pressImgPath!="")//传入的按下图片不为空 说明需要有按下的状态
    {
        QPixmap pix;
        if(state == 0){
            state = 1;
            if(!pix.load(this->pressImgPath)){
                qDebug() << "fail to load pix!";
                return;
            }
        }
        else{
            state = 0;
            if(!pix.load(this->normalImgPath)){
                qDebug() << "fail to load pix!";
                return;
            }
        }
        //设置图片固定大小
        this->setFixedSize(pix.width(), pix.height());
        //设置不规则图片的样式
        this->setStyleSheet("QPushButton{border:0px;}");
        //设置图标
        this->setIcon(pix);
        //设置图标大小
        this->setIconSize(QSize(pix.width(), pix.height()));
    }
    else{}
    //让父类执行其它的内容
    return QPushButton::mousePressEvent(e);
}

