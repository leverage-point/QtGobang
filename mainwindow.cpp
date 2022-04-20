#include<QPainter>
#include<QTimer>
#include<QSound>
#include<QMouseEvent>
#include<QMessageBox>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QDebug>
#include<math.h>
#include<utility>//pair
#include"mainwindow.h"

//*************************************************

const int kBoardMargin = 30; //���̱�Ե��϶
const int kRadius = 18; //���Ӱ뾶
const int kMarkSize = 6; //���ӱ�Ǳ߳�
const int kBlockSize = 40; //���ӵĴ�С
const int kPosDelta = kBlockSize/2; //�������ģ����������
const int kAIDelay = 700; //AI�����˼��ʱ��

#define CHESS_ONE_SOUND ":/new/sound/chessone.wav"
#define WIN_SOUND ":/new/sound/win.wav"
#define LOSE_SOUND ":/new/sound/lose.wav"

//*************************************************

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //�������̴�С
    setFixedSize(kBoardMargin * 2 + kBlockSize * kBoardSizeNum, kBoardMargin * 2 + kBlockSize * kBoardSizeNum);

    //�������hover����
    setMouseTracking(true);

    //��Ӳ˵�
    //ѡ�����˶�ս
    QMenu *gameMenu = menuBar()->addMenu(tr("Game Option"));
    QAction *actionPVP = new QAction("Person VS Person", this);
    connect(actionPVP, SIGNAL(triggered()), this, SLOT(initPVPGame()));
    gameMenu->addAction(actionPVP);

    //ѡ���˻���ս
    QAction *actionPVE = new QAction("Person VS Computer(primary)", this);
    connect(actionPVE, SIGNAL(triggered()), this, SLOT(initPVEGame()));
    gameMenu->addAction(actionPVE);

    QAction *actionPVE2 = new QAction("Person VS Computer(advanced)", this);
    connect(actionPVE2, SIGNAL(triggered()), this, SLOT(initPVEGame2()));
    gameMenu->addAction(actionPVE2);

    //������Ϸ��ʼ����
    QAction *back = new QAction("To Start Screen", this);
    connect(back, &QAction::triggered, [=](){
        emit this->chooseScenceBack();
    });
    gameMenu->addAction(back);

    //��ʼ����Ϸ
    initGame();
}


MainWindow::~MainWindow()
{
    if (game) delete game;
}


void MainWindow::initGame()
{
    //��ʼ����Ϸģ��
    game = new GameModel;
    initPVPGame();
}


void MainWindow::initPVPGame()
{
    game_type = PERSON;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::initPVEGame()
{
    game_type = BOT;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::initPVEGame2()
{
    game_type = BOT2;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}


void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QPixmap pix;
    pix.load(":/new/image/background2.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    //��������
    painter.setRenderHint(QPainter::Antialiasing, true); //Indicates that the engine should antialias edges of primitives if possible.

    for (int i = 0; i < kBoardSizeNum + 1; i++)
    {
        painter.drawLine(kBoardMargin + kBlockSize * i, kBoardMargin, kBoardMargin + kBlockSize * i, size().height() - kBoardMargin);
        painter.drawLine(kBoardMargin, kBoardMargin + kBlockSize * i, size().width() - kBoardMargin, kBoardMargin + kBlockSize * i);
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    //�������ӱ��(��ֹ������Խ��)
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum && clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        if (game->playerFlag)
            brush.setColor(Qt::white);
        else
            brush.setColor(Qt::black);
        painter.setBrush(brush);
        painter.drawRect(kBoardMargin + kBlockSize * clickPosCol - kMarkSize / 2, kBoardMargin + kBlockSize * clickPosRow - kMarkSize / 2, kMarkSize, kMarkSize);
    }

    //�ж���Ӯ
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum &&
        clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        (game->gameMapVec[clickPosRow][clickPosCol] == 1 ||
            game->gameMapVec[clickPosRow][clickPosCol] == -1))
    {
        if (game->isWin(clickPosRow, clickPosCol) && game->gameStatus == PLAYING)
        {
            game->gameStatus = WIN;
            QSound::play(WIN_SOUND);
            QString str;
            if (game->gameMapVec[clickPosRow][clickPosCol] == 1)
                str = "white player";
            else if (game->gameMapVec[clickPosRow][clickPosCol] == -1)
                str = "black player";
            QMessageBox::StandardButton btnValue = QMessageBox::information(this, "congratulations", str + "win!");

            //������Ϸ״̬������������ѭ��
            if (btnValue == QMessageBox::Ok)
            {
                game->startGame(game_type);
                game->gameStatus = PLAYING;
            }
        }
    }

    //��������
    for (int i = 0; i < kBoardSizeNum; i++){
        for (int j = 0; j < kBoardSizeNum; j++)
        {
            if (game->gameMapVec[i][j] == 1)
            {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(kBoardMargin + kBlockSize * j - kRadius, kBoardMargin + kBlockSize * i - kRadius, kRadius * 2, kRadius * 2);
            }
            else if (game->gameMapVec[i][j] == -1)
            {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(kBoardMargin + kBlockSize * j - kRadius, kBoardMargin + kBlockSize * i - kRadius, kRadius * 2, kRadius * 2);
            }
        }
    }

    //�ж����֣������Ӳ�������ʱ��
    if (game->isDeadGame())
    {
        QSound::play(LOSE_SOUND);
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "notice", "dead game!");
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(game_type);
            game->gameStatus = PLAYING;
        }

    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //ͨ������hoverȷ�����ӵı��
    int x = event->x();
    int y = event->y();

    //���̱�Ե��������
    if (x >= kBoardMargin + kBlockSize / 2 && x < size().width() - kBoardMargin &&
            y >= kBoardMargin + kBlockSize / 2 && y < size().height()- kBoardMargin)
    {
        //��ȡ��������Ͻǵĵ�(leftTopPosX,leftTopPosY)
        int col = x / kBlockSize;
        int row = y / kBlockSize;

        int leftTopPosX = kBoardMargin + kBlockSize * col;
        int leftTopPosY = kBoardMargin + kBlockSize * row;

        //���ݾ���������ʵĵ��λ��,һ���ĸ��㣬���ݰ뾶����ѡ�����
        clickPosRow = -1; //��ʼ�����յ�ֵ
        clickPosCol = -1;//��ʼ�����յ�ֵ
        int len = 0;

        //ȷ��һ������ڷ�Χ�ڵĵ㣬��ֻ����ȷ��һ������
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < kPosDelta)
        {
            clickPosRow = row;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - kBlockSize) * (x - leftTopPosX - kBlockSize) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < kPosDelta)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
        }
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY - kBlockSize) * (y - leftTopPosY - kBlockSize));
        if (len < kPosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - kBlockSize) * (x - leftTopPosX - kBlockSize) + (y - leftTopPosY - kBlockSize) * (y - leftTopPosY - kBlockSize));
        if (len < kPosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }
    //qDebug()<<clickPosRow<<" "<<clickPosCol;
    //����������ػ�
    update();
}


void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    //�����壬���Ҳ�������������
    if (!((game_type == BOT || game_type ==BOT2) && !game->playerFlag))//���˻�ģʽ���ֵ�BOT���������������
    {
        chessOneByPerson();
        //������˻�ģʽ����Ҫ����AI����
        if (game->gameType == BOT && !game->playerFlag)
        {
            //�ö�ʱ����һ���ӳ�
            QTimer::singleShot(kAIDelay, this, SLOT(chessOneByAI()));
        }
        if (game->gameType == BOT2 && !game->playerFlag)
        {
            //�ö�ʱ����һ���ӳ�
            QTimer::singleShot(kAIDelay, this, SLOT(chessOneByAI2()));
        }
    }
}


void MainWindow::chessOneByPerson()
{
    //���ݵ�ǰ�洢����������
    //ֻ����Ч��������ӣ����Ҹô�û���ӣ�clickPosRow��clickPosCol�ĳ�ֵΪ-1
    if (clickPosRow != -1 && clickPosCol != -1 && game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        game->actionByPerson(clickPosRow, clickPosCol);
        QSound::play(CHESS_ONE_SOUND);
        //�ػ�
        update();
    }
}


void MainWindow::chessOneByAI()
{
    pair<int,int> temp(-1,-1);
    temp = game->actionByAI();
    clickPosRow = temp.first;
    clickPosCol = temp.second;
    QSound::play(CHESS_ONE_SOUND);
    update();
}


void MainWindow::chessOneByAI2()
{
    pair<int,int> temp(-1,-1);
    temp = game->actionByAI2();
    clickPosRow = temp.first;
    clickPosCol = temp.second;
    QSound::play(CHESS_ONE_SOUND);
    update();
}

