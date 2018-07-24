#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <windows.h>
#include <stdlib.h>
#include <QMainWindow>
#include "Item.h"
#include "qmap.h"

namespace Ui2 {
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget *parent = 0);
    ~MainWindow2();

    void InitItems();			//初始化所有棋子位置
    void NewGame();			//新游戏


    void write(QJsonObject &json) const;
    void read(const QJsonObject &json);                         //写入主函数内的数据

    bool loadgame();

    void SearchMain();                //主搜索函数

    int getValue(Item &targetItem);
    int getAllMoveArea(QVector<Item>&allItem,QVector<QPoint>& DetArea);                  //获取所有可能的走法%%%%
    int SearchFull(int vlAlpha, int vlBeta, int nDepth , int flag);        //alpha—Beta剪裁搜索

    bool FindItemAtPoint(QPoint pt,Item& item);					//给定一个棋盘上的点，查找是否存在棋子
    bool DeleteItemAtPoint(QPoint pt,bool& bDeleteSHUAI);		//bDeleteSHUAI：是否打掉的是“帅”或“将”
    void SetItemShow(Item item,bool bShow);						//设置棋子显隐
    bool MoveItem(Item item,QPoint ptMoveTo);					//走棋逻辑
    void ChangeItemPoint(QPoint ptItem,QPoint pt,bool eats);				//改变棋子位置属性
    void ChangeItemPoint2(QPoint ptItem,QPoint pt,QVector<Item>&x);
    void DrawItem(QPainter& painter,Item item);					//绘制棋子

    //获取棋子能移动的区域
    void GetMoveArea(Item item,QVector<QPoint>& area);
    void GetMoveArea_JU(Item item,QVector<QPoint>& area);
    void GetMoveArea_MA(Item item,QVector<QPoint>& area);
    void GetMoveArea_XIANG(Item item,QVector<QPoint>& area);
    void GetMoveArea_SHI(Item item,QVector<QPoint>& area);
    void GetMoveArea_SHUAI(Item item,QVector<QPoint>& area);
    void GetMoveArea_PAO(Item item,QVector<QPoint>& area);
    void GetMoveArea_BING(Item item,QVector<QPoint>& area);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_save_clicked();


private:
    Ui2::MainWindow2 *ui;

    int m_nItemWidth;
    int m_nItemHeight;
    QPixmap m_ItemsImage;		//棋子图片

    int RedValue;
    int BlackValue;
    int nDistance;                  // 距离根节点的步数

    QVector<Item> m_items;	//所有棋子


    Item m_SelectedItem;
    bool m_bExistSelectedItem;	//是否已存在选中的棋子
    bool m_bIsRedTurn;		//当前该红方下
    QList<Item> previous;
    QList<Item> deletes;
    QList<bool> bools;
    QList<int> xu;
    int shu;
    int zi;
public:
    int evaluate(QVector<Item>&x);
};

#endif // MAINWINDOW_H

