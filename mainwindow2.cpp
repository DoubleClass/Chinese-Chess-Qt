#include "mainwindow2.h"
#include "ui_mainwindow2.h"







#include<QListWidget>
#include<QFile>
#include "mainwindow2.h"
#include "ui_mainwindow.h"
#include "qpainter.h"
#include "qevent.h"
#include "qmessagebox.h"
#include"mydialog.h"
#include"Item.h"
#include<QListWidgetItem>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>

#include<time.h>
#define CHESS_ROWS		9
#define CHESS_COLUMES	8
#define RECT_WIDTH		55
#define RECT_HEIGHT		56
const int LIMIT_DEPTH = 32;    // 最大的搜索深度
const int ADVANCED_VALUE = 3;
const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int WIN_VALUE = MATE_VALUE - 100; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
#define START_X	(36-RECT_WIDTH/2)           //棋盘左上定点像素值（35，45），棋盘的格子大小（96，96）
#define START_Y	((45-RECT_HEIGHT/2)+35)


bool isSinglePlayerMode;


MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui2::MainWindow2)
{
    ui->setupUi(this);

   // ui->mainToolBar->hide();
    //ui->menuBar->hide();
    ui->menubar->hide();
    //窗口以棋盘图片为大小
    QPixmap pixChessBoard(":/image/image/h.jpg");
//	resize(pixChessBoard.size());
    setFixedSize(pixChessBoard.size());

    QPalette palette;
    palette.setBrush( QPalette::Window,QBrush(pixChessBoard));
    setPalette(palette);

    m_ItemsImage = QPixmap(":/image/image/items.png");
    m_nItemWidth = m_ItemsImage.width() / COLOR_MAX;
    m_nItemHeight = m_ItemsImage.height() / ITEM_MAX;
    shu=0;
    zi=0;
    NewGame();
    startTimer(500);		//启动显隐选中元素的定时器
}

MainWindow2::~MainWindow2()
{
    delete ui;
}

void MainWindow2::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    for (int i = 0; i<m_items.size(); i++)
    {
        DrawItem(painter,m_items[i]);
    }
    update();
}




void MainWindow2::write(QJsonObject &json)const
{
    QJsonArray itemsArray;
    for(int i = 0 ; i<m_items.size() ; i++)
    {
        QJsonObject itemObject;
        m_items[i].write(itemObject);
        itemsArray.append(itemObject);
    }
    /*foreach (Item, m_items) {
        QJsonObject itemObject;
        Item.write(itemObject);
        itemsArray.append(itemObject);
    }*/
    json["m_items"] = itemsArray;


    if(m_bExistSelectedItem == true)
        json["m_bExistSelectedItem"] = 1;
    else
        json["m_bExistSelectedItem"] = 0;
    if(m_bIsRedTurn == true)
        json["m_bIsRedTurn"] = 1;
    else
        json["m_bIsRedTurn"] = 0;


    QJsonArray previousArray;
    for(int i = 0 ; i < previous.size() ; i++)
    {
        QJsonObject itemObject;
        m_items[i].write(itemObject);
        previousArray.append(itemObject);
    }
    /*foreach (Item, previous) {
        QJsonObject itemObject;
        Item.write(itemObject);
        previousArray.append(itemObject);
    }*/
    json["previous_Items"] = previousArray;


    QJsonArray deletesArray;
    for(int i = 0 ; i < deletes.size() ; i++)
    {
        QJsonObject itemObject;
        m_items[i].write(itemObject);
        deletesArray.append(itemObject);
    }
    /*foreach (Item, deletes) {
        QJsonObject itemObject;
        Item.write(itemObject);
        deletesArray.append(itemObject);
    }*/
    json["deletes_Items"] = previousArray;


    QJsonArray boolArray;
    for(int i = 0 ; i < bools.size() ; i++)
    {
        if(bools[i]== true)
            boolArray.append(1);
        else
            boolArray.append(0);
    }
    /*foreach (x, bools) {
        if(x == true)
            boolArray.append(1);
        else
            boolArray.append(0);
    }*/
    json["bools"] = boolArray;

    QJsonArray intArray;
    for(int i = 0 ; i<xu.size() ; i++)
    {
        intArray.push_back(xu[i]);
    }
    /*foreach (y, xu) {
        intArray.append(y);
    }*/
    json["xu"] = intArray;

    json["shu"] = shu;
    json["zi"] = zi;
}


void MainWindow2::read(const QJsonObject &json)
{
    m_items.clear();
    QJsonArray itemArray = json["m_items"].toArray();
    for(int i=0 ; i<itemArray.size() ;i++)
    {
        QJsonObject itemObject = itemArray[i].toObject();
        Item itemx;
        itemx.read(itemObject);
        m_items.append(itemx);
    }


    if(json["m_bExistSelectedItem"].toDouble() == 1)
        m_bExistSelectedItem = true;
    else
        m_bExistSelectedItem = false;

    if(json["m_bIsRedTurn"].toDouble() == 1)
        m_bIsRedTurn = true;
    else
        m_bIsRedTurn = false;


    previous.clear();
    QJsonArray previousArray = json["previous_Items"].toArray();
    for(int i = 0; i < previousArray.size() ; i++)
    {
        QJsonObject itemObeject = previousArray[i].toObject();
        Item itemy;
        itemy.read(itemObeject);
        previous.append(itemy);
    }


    deletes.clear();
    QJsonArray deleteArray = json["deletes_Items"].toArray();
    for(int i = 0 ; i < deleteArray.size() ; i++)
    {
        QJsonObject itemObeject = deleteArray[i].toObject();
        Item itemz;
        itemz.read(itemObeject);
        deletes.append(itemz);
    }


    bools.clear();
    QJsonArray boolsArray = json["bools"].toArray();
    for(int i = 0 ; i < bools.size() ; i++)
    {
        if(boolsArray[i].toDouble() == 1)
            bools.append(true);
        else
            bools.append(false);
    }


    xu.clear();
    QJsonArray intsArray = json["xu"].toArray();
    for(int i = 0 ; i < intsArray.size() ; i++)
    {
        xu.append(qRound(intsArray[i].toDouble()));
    }


    shu = qRound(json["shu"].toDouble());
    zi = qRound(json["zi"].toDouble());
}



bool MainWindow2::loadgame()
{
    QFile loadFile(QStringLiteral("save.json"));

         if (!loadFile.open(QIODevice::ReadOnly)) {
             qWarning("Couldn't open save file.");
             return false;
         }

         QByteArray saveData = loadFile.readAll();

         QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

         read(loadDoc.object());

         return true;
}




int MainWindow2::getAllMoveArea(QVector<Item>allItem,QVector<QPoint> &DetArea)
{
    int posibleMoves = 0;                                //所有能走的步数
    for(int i = 0 ; i < 10 ; i++)
    {
        for(int j = 0 ; j < 9 ; j++)
        {
            Item item;
            QPoint pt;
            pt.setX(j);
            pt.setY(i);
            if(!FindItemAtPoint(pt,item) )        //找到本方棋子
                continue;
            if(!(m_bIsRedTurn == item.m_color))
                continue;
            GetMoveArea(item , DetArea);
            allItem.append(item);
            posibleMoves++;

        }
    }
    return posibleMoves;
}


/*int MainWindow::SearchFull(int vlAlpha, int vlBeta, int nDepth)
{
    int vlBest,mvBest,vl;                                      //杀棋（最佳走法），比较好的走法
    Item bestMove[2];
    int nGenMoves=0;
    if (nDepth == 0)
    {
      return evaluate();
    }
    vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
    mvBest = 0;
    QVector<Item>allItem;
    QVector<QPoint>allArea;
    nGenMoves = getAllMoveArea(allItem,allArea);
    for (i = 0 ; i < nGenMoves; i ++)
    {
        if(MoveItem(allItem[i],allArea[i]))                       //存在逻辑漏洞大概，待修复
        {
              vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
              on_pushButton_3_clicked();
        }
        if (vl > vlBest)     // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
        {
          vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
          if (vl >= vlBeta) // 找到一个Beta走法
          {
            bestMove[0] = allItem[i];  // Beta走法要保存到历史表
            bestMove[1] = allArea[i];
            break;            // Beta截断
          }
          if (vl > vlAlpha)  // 找到一个PV走法
          {
            bestMove[0] = allItem[i];
            bestMove[1] = allArea[i]; // PV走法要保存到历史表
            vlAlpha = vl;     // 缩小Alpha-Beta边界
          }
        }
    }

    if (vlBest == -MATE_VALUE) {
      // 如果是杀棋，就根据杀棋步数给出评价
      return nDistance - MATE_VALUE;
    }
    if (mvBest != 0) {
      // 如果不是Alpha走法，就将最佳走法保存到历史表
      Search.nHistoryTable[mvBest] += nDepth * nDepth;
      if (pos.nDistance == 0) {
        // 搜索根节点时，总是有一个最佳走法(因为全窗口搜索不会超出边界)，将这个走法保存下来
        Search.mvResult = mvBest;
      }
    }
    return vlBest;
  }*/
Item bestMove[2];
QVector<Item>copyItem;

struct bestMove2
{
    Item item;
    QPoint point;
}bestMove22;
int MainWindow2::SearchFull(int vlAlpha, int vlBeta, int nDepth, int flag)
{
    if(flag == 0)
    {


        for(int i = 0 ; i < m_items.size() ; i++)
        {
            Item x = m_items[i];
            copyItem.append(x);
        }
    }
    int eval;
    if(nDepth == 0)
        return evaluate(copyItem);

    QVector<Item>allItem;
    QVector<QPoint>allArea;
    int nGenMoves;
    nGenMoves = getAllMoveArea(allItem,allArea);
    if(m_bIsRedTurn == 0)
    {
        int maxEval = -MATE_VALUE;
        for(int i = 0 ; i < nGenMoves ; i++)
            {
                eval = SearchFull(vlAlpha , vlBeta , nDepth - 1 , 0);
                if(eval > maxEval)
                    maxEval = eval;
                if(eval >= vlBeta)
                {
                    bestMove22.item = allItem[i];
                    //bestMove[0] = allItem[i];
                    bestMove22.point = allArea[i];
                    break;
                }
                if(eval > vlAlpha)
                {
                    bestMove22.item = allItem[i];
                    bestMove22.point = allArea[i];
                    vlAlpha = eval;
                }
            }
        ChangeItemPoint2(bestMove22.item , bestMove22.point , copyItem);
        return maxEval;
    }
    else
    {
        int minEval = MATE_VALUE;
        for(int i = 0 ; i < nGenMoves ; i++)
        {
            eval = SearchFull(vlAlpha , vlBeta , nDepth - 1 , 0);
            if(eval < minEval)
                minEval = eval;
            if(eval <= vlAlpha)
            {
                bestMove[0] = allItem[i];
                bestMove[1] = allArea[i];
                break;
            }
            if(eval < vlBeta)
            {
                bestMove[0] = allItem[i];
                bestMove[1] = allArea[i];
                vlBeta = eval;
            }
        }
        ChangeItemPoint2(bestMove22.item , bestMove22.point , copyItem);
        return minEval;
    }
}

void MainWindow2::SearchMain() {
  int i, t, vl;

  // 初始化

  t = clock();       // 初始化定时器
  //pos.nDistance = 0; // 初始步数

  // 迭代加深过程
  for (i = 1; i <= LIMIT_DEPTH; i ++) {
    vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
    // 搜索到杀棋，就终止搜索
    if (vl > WIN_VALUE || vl < -WIN_VALUE) {
      break;
    }
    // 超过一秒，就终止搜索
    if (clock() - t > 2*CLOCKS_PER_SEC) {
      break;
    }
  }
}


 void MainWindow2::DrawItem(QPainter& painter, Item item )
{
    if (!item.m_bShow)
    {
        return;
    }
    QRect rcSrc(m_nItemWidth*item.m_color,m_nItemHeight*item.m_type,m_nItemWidth,m_nItemHeight);

    QRect rcTarget(	START_X + item.m_pt.x()*RECT_WIDTH,
                    START_Y + item.m_pt.y()*RECT_HEIGHT,
                    RECT_WIDTH,RECT_HEIGHT);

    painter.drawPixmap(rcTarget,m_ItemsImage,rcSrc);
}

void MainWindow2::InitItems()
{
    m_items.clear();

    //初始化所有黑方棋子
    Item item1(ITEM_JU,COLOR_BLACK,QPoint(0,0),1);
    Item item2(ITEM_MA,COLOR_BLACK,QPoint(1,0),2);
    Item item3(ITEM_XIANG,COLOR_BLACK,QPoint(2,0),3);
    Item item4(ITEM_SHI,COLOR_BLACK,QPoint(3,0),4);
    Item item5(ITEM_SHUAI,COLOR_BLACK,QPoint(4,0),5);
    Item item6(ITEM_SHI,COLOR_BLACK,QPoint(5,0),6);
    Item item7(ITEM_XIANG,COLOR_BLACK,QPoint(6,0),7);
    Item item8(ITEM_MA,COLOR_BLACK,QPoint(7,0),8);
    Item item9(ITEM_JU,COLOR_BLACK,QPoint(8,0),9);
    Item item10(ITEM_PAO,COLOR_BLACK,QPoint(1,2),10);
    Item item11(ITEM_PAO,COLOR_BLACK,QPoint(7,2),11);
    Item item12(ITEM_BING,COLOR_BLACK,QPoint(0,3),12);
    Item item13(ITEM_BING,COLOR_BLACK,QPoint(2,3),13);
    Item item14(ITEM_BING,COLOR_BLACK,QPoint(4,3),14);
    Item item15(ITEM_BING,COLOR_BLACK,QPoint(6,3),15);
    Item item16(ITEM_BING,COLOR_BLACK,QPoint(8,3),16);

    //初始化所有红方棋子
    Item item_1(ITEM_JU,COLOR_RED,QPoint(0,9),17);
    Item item_2(ITEM_MA,COLOR_RED,QPoint(1,9),18);
    Item item_3(ITEM_XIANG,COLOR_RED,QPoint(2,9),19);
    Item item_4(ITEM_SHI,COLOR_RED,QPoint(3,9),20);
    Item item_5(ITEM_SHUAI,COLOR_RED,QPoint(4,9),21);
    Item item_6(ITEM_SHI,COLOR_RED,QPoint(5,9),22);
    Item item_7(ITEM_XIANG,COLOR_RED,QPoint(6,9),23);
    Item item_8(ITEM_MA,COLOR_RED,QPoint(7,9),24);
    Item item_9(ITEM_JU,COLOR_RED,QPoint(8,9),25);
    Item item_10(ITEM_PAO,COLOR_RED,QPoint(1,7),26);
    Item item_11(ITEM_PAO,COLOR_RED,QPoint(7,7),27);
    Item item_12(ITEM_BING,COLOR_RED,QPoint(0,6),28);
    Item item_13(ITEM_BING,COLOR_RED,QPoint(2,6),29);
    Item item_14(ITEM_BING,COLOR_RED,QPoint(4,6),30);
    Item item_15(ITEM_BING,COLOR_RED,QPoint(6,6),31);
    Item item_16(ITEM_BING,COLOR_RED,QPoint(8,6),32);

    m_items.push_back(item1);
    m_items.push_back(item2);
    m_items.push_back(item3);
    m_items.push_back(item4);
    m_items.push_back(item5);
    m_items.push_back(item6);
    m_items.push_back(item7);
    m_items.push_back(item8);
    m_items.push_back(item9);
    m_items.push_back(item10);
    m_items.push_back(item11);
    m_items.push_back(item12);
    m_items.push_back(item13);
    m_items.push_back(item14);
    m_items.push_back(item15);
    m_items.push_back(item16);

    m_items.push_back(item_1);
    m_items.push_back(item_2);
    m_items.push_back(item_3);
    m_items.push_back(item_4);
    m_items.push_back(item_5);
    m_items.push_back(item_6);
    m_items.push_back(item_7);
    m_items.push_back(item_8);
    m_items.push_back(item_9);
    m_items.push_back(item_10);
    m_items.push_back(item_11);
    m_items.push_back(item_12);
    m_items.push_back(item_13);
    m_items.push_back(item_14);
    m_items.push_back(item_15);
    m_items.push_back(item_16);
}

//鼠标点击，走棋、吃棋等逻辑
void MainWindow2::mousePressEvent(QMouseEvent * e)
{


    //获得鼠标点所对应的棋盘点pt
    QPoint pt;
    pt.setX( (e->pos().x() - START_X ) / RECT_WIDTH);
    pt.setY( (e->pos().y() - START_Y ) / RECT_HEIGHT);

    //是否有选中的棋子



    if(m_bExistSelectedItem && m_bIsRedTurn)
    {
        //已存在棋子，判断鼠标点击的是否是选中棋子
        if (pt == m_SelectedItem.m_pt)
        {
            //再次点击已经选择的棋子，什么也不做
            return;
        }
        //点击其它棋子
        Item ClickedItem;
        if (FindItemAtPoint(pt,ClickedItem))
        {
            //点击的同色的另外一个棋子，改选
            if ( (m_bIsRedTurn && ClickedItem.m_color == COLOR_RED) ||
                 (!m_bIsRedTurn && ClickedItem.m_color != COLOR_RED))
            {
                SetItemShow(m_SelectedItem,true);
                m_SelectedItem = ClickedItem;
                return;
            }
        }
        //点击的异色棋子，判断是否能走能吃
        QVector<QPoint> moveArea;
        GetMoveArea(m_SelectedItem,moveArea);		//获取已选择棋子的可移动区域
        if (moveArea.contains(pt))
        {
            //包含当前鼠标点中的棋子，则能吃
            bool bDeleteSHUAI = false;
            bool eat=false;
            eat=DeleteItemAtPoint(pt,bDeleteSHUAI);
            ChangeItemPoint(m_SelectedItem.m_pt,pt,eat);
            if (bDeleteSHUAI)
            {
                QString str = m_bIsRedTurn?QStringLiteral("红方胜利！    "):QStringLiteral("黑方胜利！      ");
                QMessageBox::information(NULL,  "GAME OVER  ",str, QMessageBox::Yes , QMessageBox::Yes);
                NewGame();
                return ;
            }
            m_bExistSelectedItem = false;
            m_bIsRedTurn = !m_bIsRedTurn;
            update();
            return ;
        }
        else
        {
            //不能走到该位置
            return ;
        }
    }

    else
    {
        //当前没有选中棋子
        Item ClickedItem;
        if (FindItemAtPoint(pt,ClickedItem))
        {
            //如果点中一个棋子,是当前走棋方的颜色，就选中了
            if ( (m_bIsRedTurn && ClickedItem.m_color == COLOR_RED))
            {
                m_SelectedItem = ClickedItem;
                m_bExistSelectedItem = true;
                return;
            }
        }
    }

    SearchMain();
    MoveItem(bestMove22.item , bestMove22.point);
    return;

}

bool MainWindow2::MoveItem(Item item,QPoint pt)
{
    QVector<QPoint> moveArea;
    GetMoveArea(item,moveArea);
    if (moveArea.contains(pt))
    {
        bool bDeleteSHUAI = false;
        bool eat=false;
        eat=DeleteItemAtPoint(pt,bDeleteSHUAI);
        ChangeItemPoint(item.m_pt,pt,eat);
        if (bDeleteSHUAI)
        {
            QString str = m_bIsRedTurn?QStringLiteral("红方胜利！"):QStringLiteral("黑方胜利！");
            QMessageBox::information(NULL,  "GAME OVER  ",str, QMessageBox::Yes , QMessageBox::Yes);
            NewGame();
        }
        //nDistance++;
        m_bIsRedTurn = 1 - m_bIsRedTurn;
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow2::FindItemAtPoint(QPoint pt,Item& item)
{
    for (int i = 0; i<m_items.size(); i++)
    {
        if (m_items[i].m_pt == pt)
        {
            item = m_items[i];
            return true;
        }
    }
    return false;
}

bool MainWindow2::DeleteItemAtPoint(QPoint pt,bool& bDeleteSHUAI)
{
    bool bDeleted = false;
    for (int i = 0; i<m_items.size(); i++)
    {
        if (m_items[i].m_pt == pt)
        {
            bDeleteSHUAI = (m_items[i].m_type == ITEM_SHUAI);
            deletes.append(m_items[i]);
            xu.append(i);
            zi++;
            m_items.erase(m_items.begin()+i);
            bDeleted = true;
            break;
        }
    }
    return bDeleted;
}

void MainWindow2::GetMoveArea(Item item,QVector<QPoint>& moveArea)
{
    switch (item.m_type)
    {
    case ITEM_JU:
        {
            GetMoveArea_JU(item,moveArea);
            break;
        }
    case ITEM_MA:
        {
            GetMoveArea_MA(item,moveArea);
            break;
        }
    case ITEM_XIANG:
        {
            GetMoveArea_XIANG(item,moveArea);
            break;
        }
    case ITEM_SHI:
        {
            GetMoveArea_SHI(item,moveArea);
            break;
        }
    case ITEM_SHUAI:
        {
            GetMoveArea_SHUAI(item,moveArea);
            break;
        }
    case ITEM_PAO:
        {
            GetMoveArea_PAO(item,moveArea);
            break;
        }
    case ITEM_BING:
        {
            GetMoveArea_BING(item,moveArea);
            break;
        }
    }
}

void MainWindow2::ChangeItemPoint(QPoint ptItem,QPoint pt,bool eats)
{
    for (int i = 0; i<m_items.size(); i++)
    {
        if (m_items[i].m_pt == ptItem)
        {
            m_SelectedItem.m_pt = QPoint(-1,-1);
            previous.append(m_items[i]);
            bools.append(eats);
            shu++;
            x[i].m_pt = pt;
            x[i].m_bShow = true;
            break;
        }
    }
}


void MainWindow2::ChangeItemPoint2(QPoint ptItem,QPoint pt,QVector<Item>&x)
{
    for (int i = 0; i<x.size(); i++)
    {
        if (x[i].m_pt == ptItem)
        {
            m_SelectedItem.m_pt = QPoint(-1,-1);

            m_items[i].m_pt = pt;
            m_items[i].m_bShow = true;
            break;
        }
    }
}

void MainWindow2::timerEvent( QTimerEvent * )
{
    for (int i = 0; i<m_items.size(); i++)
    {
        if (m_items[i].m_pt == m_SelectedItem.m_pt)
        {
            m_items[i].m_bShow = !m_items[i].m_bShow;
        }
    }
}

void MainWindow2::GetMoveArea_JU(Item item,QVector<QPoint>& moveArea)
{
    //棋子“车”的计算可移动区域算法简介：
    //1，向4个方向上依次遍历点，把空点都加入。
    //当碰到棋子时停止，如果是同色棋子，不加入；是异色棋子，则加入（该处认为可走，因为可以吃子）
    //左边
    int x = item.m_pt.x() - 1;
    while(x>=0)
    {
        QPoint ptNew = QPoint(x,item.m_pt.y());
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (item.m_color != item2.m_color)
            {
                moveArea.append(ptNew);
            }
            break;
        }
        moveArea.append(ptNew);
        x--;
    }
    //右边
    x = item.m_pt.x() + 1;
    while(x<=8)
    {
        QPoint ptNew = QPoint(x,item.m_pt.y());
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (item.m_color != item2.m_color)
            {
                moveArea.append(ptNew);
            }
            break;
        }
        moveArea.append(ptNew);
        x++;
    }

    //上边
    int y = item.m_pt.y() - 1;
    while(y>=0)
    {
        QPoint ptNew = QPoint(item.m_pt.x(),y);
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (item.m_color != item2.m_color)
            {
                moveArea.append(ptNew);
            }
            break;
        }
        moveArea.append(ptNew);
        y--;
    }
    //下方
    y = item.m_pt.y() + 1;
    while(y<=9)
    {
        QPoint ptNew = QPoint(item.m_pt.x(),y);
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (item.m_color != item2.m_color)
            {
                moveArea.append(ptNew);
            }
            break;
        }
        moveArea.append(ptNew);
        y++;
    }
}

void MainWindow2::GetMoveArea_MA( Item item,QVector<QPoint>& moveArea )
{
    //棋子“马”的计算可移动区域算法简介：
    //1，求出8个待选位置，8个位置的偏移是（-2，-1）（-2，1）（2，-1）（2，1）（1，-2）（1，2）（-1，-2）（-1，2）存在关系：|x|+|y|=3
    //2，判断待选位置是否在棋盘内
    //3，判断中间是否有卡位的棋子
    //4，位置上是否已存在同色棋子
    Item item2;
    for (int i = -2; i<=2; i++)
    {
        for(int j = -2; j<=2; j++)
        {
            if (qAbs(i) + qAbs(j) == 3)
            {
                QPoint ptNew = item.m_pt + QPoint(i,j);
                if (ptNew.x() >= 0 && ptNew.x() <= 8 && ptNew.y()>=0 && ptNew.y() <= 9)
                {

                }
                else
                {
                    continue;
                }

                //求该方向行走路线的 卡位元素位置
                QPoint ptDirect(0,0);
                if (qAbs(i) > qAbs(j))
                {
                    if (i>0)
                    {
                        ptDirect = QPoint(1,0);
                    }
                    else
                    {
                        ptDirect = QPoint(-1,0);
                    }
                }
                else
                {
                    if (j>0)
                    {
                        ptDirect = QPoint(0,1);
                    }
                    else
                    {
                        ptDirect = QPoint(0,-1);
                    }
                }
                QPoint ptHit = item.m_pt + ptDirect;	//马的卡位元素位置
                if (FindItemAtPoint(ptHit,item2))
                {
                    //卡位
                    continue;
                }
                if (FindItemAtPoint(ptNew ,item2) && item.m_color == item2.m_color)
                {
                    //有本组item
                    continue;
                }
                moveArea.append(ptNew);
            }
        }
    }
}

void MainWindow2::GetMoveArea_XIANG( Item item,QVector<QPoint>& moveArea )
{
    //棋子“相”的计算可移动区域算法简介：
    //1，求出4个待选位置
    //2，判断是否在该“相”对应的半边棋盘内
    //3，判断中间是否有卡位的棋子
    //4，位置上是否已存在同色棋子
    QVector<QPoint> directions;
    directions.push_back(QPoint(-2,-2));
    directions.push_back(QPoint(2,-2));
    directions.push_back(QPoint(2,2));
    directions.push_back(QPoint(-2,2));

    for (int i = 0; i<directions.size(); i++)
    {
        QPoint ptNew = item.m_pt + directions[i];									//移动后的点
        if (item.m_pt.y() <= 4)														//上方的
        {
            if (ptNew.x() >= 0 && ptNew.x() <= 8 && ptNew.y()>=0 && ptNew.y() <= 4)		//移动后需仍在上半棋盘
            {

            }
            else
            {
                continue;
            }
        }
        else
        {
            if (ptNew.x() >= 0 && ptNew.x() <= 8 && ptNew.y()>=5 && ptNew.y() <= 9)		//移动后需仍在下半棋盘
            {

            }
            else
            {
                continue;
            }
        }

        QPoint ptHit = item.m_pt + QPoint(directions[i].x()/2,directions[i].y()/2);		//卡位点在中点
        Item item2;
        if (FindItemAtPoint(ptHit,item2))
        {
            //存在卡位棋子
            continue;
        }
        if (FindItemAtPoint(ptNew ,item2) && item.m_color == item2.m_color)
        {
            //终点是同色的棋子
            continue;
        }
        moveArea.append(ptNew);
    }
}

void MainWindow2::GetMoveArea_SHI( Item item,QVector<QPoint>& moveArea )
{
    //移动算法与相类似
    //4个待选的位置偏移
    QVector<QPoint> directions;
    directions.push_back(QPoint(-1,-1));
    directions.push_back(QPoint(1,-1));
    directions.push_back(QPoint(1,1));
    directions.push_back(QPoint(-1,1));

    for (int i = 0; i<directions.size(); i++)
    {
        QPoint ptNew = item.m_pt + directions[i];									//移动后的点
        if (item.m_pt.y() <= 4)														//上方的
        {
            if (ptNew.x() >= 3 && ptNew.x() <= 5 && ptNew.y()>=0 && ptNew.y() <= 2)		//移动后在上棋盘的(3,0)~(5,2)田字内
            {

            }
            else
            {
                continue;
            }
        }
        else
        {
            if (ptNew.x() >= 3 && ptNew.x() <= 5 && ptNew.y()>=7 && ptNew.y() <= 9)		//移动后需仍在下半棋盘
            {

            }
            else
            {
                continue;
            }
        }

        //QPoint ptHit = item.m_pt + QPoint(directions[i].x()/2,directions[i].y()/2);		//卡位点在中点
        Item item2;
        //if (FindItemAtPoint(ptHit,item2))
        //{
        //	//存在卡位的棋子
        //	continue;
        //}
        if (FindItemAtPoint(ptNew ,item2) && item.m_color == item2.m_color)
        {
            //终点是同色的棋子
            continue;
        }
        moveArea.append(ptNew);
    }
}

void MainWindow2::GetMoveArea_SHUAI( Item item,QVector<QPoint>& moveArea )
{
    QVector<QPoint> directions;
    directions.push_back(QPoint(-1,0));
    directions.push_back(QPoint(1,0));
    directions.push_back(QPoint(0,1));
    directions.push_back(QPoint(0,-1));

    for (int i = 0; i<directions.size(); i++)
    {
        QPoint ptNew = item.m_pt + directions[i];									//移动后的点
        if (item.m_pt.y() <= 4)														//上方的
        {
            if (ptNew.x() >= 3 && ptNew.x() <= 5 && ptNew.y()>=0 && ptNew.y() <= 2)		//移动后在上棋盘的(3,0)~(5,2)田字内
            {

            }
            else
            {
                continue;
            }
        }
        else
        {
            if (ptNew.x() >= 3 && ptNew.x() <= 5 && ptNew.y()>=7 && ptNew.y() <= 9)		//移动后需仍在下半棋盘
            {

            }
            else
            {
                continue;
            }
        }
        Item item2;
        if (FindItemAtPoint(ptNew ,item2) && item.m_color == item2.m_color)
        {
            //终点是同色的棋子
            continue;
        }
        moveArea.append(ptNew);
    }
}

void MainWindow2::GetMoveArea_PAO( Item item,QVector<QPoint>& moveArea )
{
    //炮的可移动区域分为两部分：第一部分同“车”，第二部分为直线上隔一个棋子后的异色棋子
    //QVector<QPoint> juMoveArea;
    //GetMoveArea_JU(item,moveArea);

    //左边
    bool bFind = false;
    int x = item.m_pt.x() - 1;
    while(x>=0)
    {
        QPoint ptNew = QPoint(x,item.m_pt.y());
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (bFind)
            {
                //如果之前找到过一个点，那么这是第2个点
                if(item.m_color != item2.m_color)
                {
                    //如果这第2个点是异色的，那么是可以吃的
                    moveArea.append(ptNew);
                }
                break;
            }
            bFind = true;
            x--;
            continue;
        }
        if (!bFind)
        {
            moveArea.append(ptNew);
        }
        x--;

    }
    //右边
    bFind = false;
    x = item.m_pt.x() + 1;
    while(x<=8)
    {
        QPoint ptNew = QPoint(x,item.m_pt.y());
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (bFind)
            {
                //如果之前找到过一个点，那么这是第2个点
                if(item.m_color != item2.m_color)
                {
                    //如果这第2个点是异色的，那么是可以吃的
                    moveArea.append(ptNew);
                }
                break;
            }
            bFind = true;
            x++;
            continue;
        }
        if (!bFind)
        {
            moveArea.append(ptNew);
        }
        x++;

    }

    //上边
    bFind = false;
    int y = item.m_pt.y() - 1;
    while(y>=0)
    {
        QPoint ptNew = QPoint(item.m_pt.x(),y);
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (bFind)
            {
                //如果之前找到过一个点，那么这是第2个点
                if(item.m_color != item2.m_color)
                {
                    //如果这第2个点是异色的，那么是可以吃的
                    moveArea.append(ptNew);
                }
                break;
            }
            bFind = true;
            y--;
            continue;
        }
        if (!bFind)
        {
            moveArea.append(ptNew);
        }
        y--;

    }
    //下方
    bFind = false;
    y = item.m_pt.y() + 1;
    while(y<=9)
    {
        QPoint ptNew = QPoint(item.m_pt.x(),y);
        Item item2;
        if (FindItemAtPoint(ptNew ,item2))
        {
            if (bFind)
            {
                //如果之前找到过一个点，那么这是第2个点
                if(item.m_color != item2.m_color)
                {
                    //如果这第2个点是异色的，那么是可以吃的
                    moveArea.append(ptNew);
                }
                break;
            }
            bFind = true;
            y++;
            continue;
        }
        if (!bFind)
        {
            moveArea.append(ptNew);
        }
        y++;

    }
}

void MainWindow2::GetMoveArea_BING( Item item,QVector<QPoint>& moveArea )
{
    QVector<QPoint> directions;

    if (item.m_color == COLOR_RED)
    {
        directions.push_back(QPoint(0,-1));	//红色上移
        if (item.m_pt.y() < 5 )			//红色在下方，y<5就是已经过河，可以横向移动
        {
            directions.push_back(QPoint(-1,0));
            directions.push_back(QPoint(1,0));
        }
    }
    else
    {
        directions.push_back(QPoint(0,1));	//黑色下移
        if (item.m_pt.y() >= 5 )			//黑色在上方，y>=5就是已经过河，可以横向移动
        {
            directions.push_back(QPoint(-1,0));
            directions.push_back(QPoint(1,0));
        }
    }

    for (int i = 0; i<directions.size(); i++)
    {
        QPoint ptNew = item.m_pt + directions[i];									//移动后的点
        if (ptNew.x() >= 0 && ptNew.x() <= 8 && ptNew.y()>=0 && ptNew.y() <= 9)		//移动后需仍在棋盘
        {

        }
        else
        {
            continue;
        }
        Item item2;
        if (FindItemAtPoint(ptNew ,item2) && item.m_color == item2.m_color)
        {
            //终点是同色的棋子
            continue;
        }
        moveArea.append(ptNew);
    }

}

void MainWindow2::NewGame()
{
    InitItems();
    m_SelectedItem.m_pt = QPoint(-1,-1);
    m_bIsRedTurn = true;
    m_bExistSelectedItem = false;

}

void MainWindow2::SetItemShow( Item item,bool bShow )
{
    for (int i = 0; i<m_items.size(); i++)
    {
        if (m_items[i].m_pt == item.m_pt)
        {
            m_items[i].m_bShow = bShow;
            break;
        }
    }
}



void MainWindow2::on_pushButton_clicked()
{
    close();
    MyDialog dlg;
    if(dlg.exec()==QDialog::Accepted)show();
}

void MainWindow2::on_pushButton_2_clicked()
{
    NewGame();
}

void MainWindow2::on_pushButton_3_clicked()
{
    if(shu==0){
        NewGame();
        m_bIsRedTurn=false;
        previous.clear();
        deletes.clear();
    }
    else{
        bool chi;
        if(m_bExistSelectedItem){
            m_SelectedItem.m_pt = QPoint(-1,-1);
            m_bExistSelectedItem=false;
        }
        for(int i=0;i<m_items.size();i++){
            if(m_items[i].num==previous[shu-1].num){
                m_items[i].m_pt=previous[shu-1].m_pt;
                chi=bools[shu-1];
                previous.erase(previous.begin()+shu-1);
                bools.erase(bools.begin()+shu-1);
                shu--;
                break;
            }
        }
        if(chi==true){
            zi--;
            m_items.insert(m_items.begin()+xu[zi],deletes[zi]);
            deletes.erase(deletes.begin()+zi);

        }
    }
    m_bIsRedTurn=!m_bIsRedTurn;
    update();
    return;


}



int MainWindow2::getValue(Item &targetItem)
{
    switch(targetItem.m_type)
    {
        case ITEM_SHUAI:
              {
                if(targetItem.m_color == 0)
                    return RedShuaiValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                else
                    return BlackShuaiValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
              }
        case ITEM_SHI:
                {
                   if(targetItem.m_color == 0)
                       return RedShiValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackShiValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
        case ITEM_XIANG:
                {
                   if(targetItem.m_color == 0)
                       return RedXiangValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackXiangValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
        case ITEM_MA:
                {
                   if(targetItem.m_color == 0)
                       return RedMaValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackMaValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
        case ITEM_JU:
                {
                   if(targetItem.m_color == 0)
                       return RedCheValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackCheValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
        case ITEM_PAO:
                {
                   if(targetItem.m_color == 0)
                       return RedPaoValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackPaoValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
        case ITEM_BING:
                {
                   if(targetItem.m_color == 0)
                       return RedBingValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                   else
                       return BlackBingValue[targetItem.m_pt.y()][targetItem.m_pt.x()];
                }
    }
}


int MainWindow2::evaluate(QVector<Item>&x)
{

    for(int i = 0 ; i < x.size() ; i++)
    {
        if(x[i].m_color == 0)
            RedValue += getValue(x[i]);
        else
            BlackValue += getValue(x[i]);
    }
    return (m_bIsRedTurn == true ?  RedValue - BlackValue : BlackValue - RedValue) + ADVANCED_VALUE;
}

void MainWindow2::on_save_clicked()
{
    QFile saveFile(QStringLiteral("save.json"));
    saveFile.open(QIODevice::WriteOnly);
    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());

}
