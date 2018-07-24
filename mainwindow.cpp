#include<QListWidget>
#include<QFile>
#include "mainwindow.h"
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
#include<stdlib.h>
#include<time.h>
#define CHESS_ROWS		9
#define CHESS_COLUMES	8
#define RECT_WIDTH		55
#define RECT_HEIGHT		56
const int LIMIT_DEPTH = 32;    // 最大的搜索深度

const int MATE_VALUE = 10000;  // 最高分值，即将死的分值
const int WIN_VALUE = MATE_VALUE - 100; // 搜索出胜负的分值界限，超出此值就说明已经搜索出杀棋了
const int MAX_GEN_MOVES = 128; // 最大的生成走法数
//棋子编号
const int PIECE_KING = 0;
const int PIECE_ADVISOR = 1;
const int PIECE_BISHOP = 2;
const int PIECE_KNIGHT = 3;
const int PIECE_ROOK = 4;
const int PIECE_CANNON = 5;
const int PIECE_PAWN = 6;
#define START_X	(36-RECT_WIDTH/2)           //棋盘左上定点像素值（35，45），棋盘的格子大小（96，96）
#define START_Y	((45-RECT_HEIGHT/2)+35)



static struct {
  int mvResult;             // 电脑走的棋
  int nHistoryTable[65536]; // 历史表
} Search;



// 判断棋子是否在棋盘中的数组
static const char ccInBoard[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断棋子是否在九宫的数组
static const char ccInFort[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// 判断步长是否符合特定走法的数组，1=帅(将)，2=仕(士)，3=相(象)
static const char ccLegalSpan[512] = {
                       0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0
};

// 根据步长判断马是否蹩腿的数组
static const char ccKnightPin[512] = {
                              0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0
};

// 判断棋子是否在棋盘中
inline BOOL IN_BOARD(int sq) {
  return ccInBoard[sq] != 0;
}

// 判断棋子是否在九宫中
inline BOOL IN_FORT(int sq) {
  return ccInFort[sq] != 0;
}

// 帅(将)的步长
static const char ccKingDelta[4] = {-16, -1, 1, 16};
// 仕(士)的步长
static const char ccAdvisorDelta[4] = {-17, -15, 15, 17};
// 马的步长，以帅(将)的步长作为马腿
static const char ccKnightDelta[4][2] = {{-33, -31}, {-18, 14}, {-14, 18}, {31, 33}};
// 马被将军的步长，以仕(士)的步长作为马腿
static const char ccKnightCheckDelta[4][2] = {{-33, -18}, {-31, -14}, {14, 31}, {18, 33}};

static const BYTE cucpcStartup[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
  0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
  0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


bool isSinglePlayerMode;



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
    ui->menuBar->hide();
    MyDialog *dlg = new MyDialog;
    connect(dlg, SIGNAL(sendData(bool)), this, SLOT(receiveData(bool)));
    //窗口以棋盘图片为大小
    QPixmap pixChessBoard(":/image/image/h.jpg");
//	resize(pixChessBoard.size());
    setFixedSize(pixChessBoard.size());

    pos.Startup();
    isSingleGame = false;

	QPalette palette;
	palette.setBrush( QPalette::Window,QBrush(pixChessBoard));
	setPalette(palette); 
	
	m_ItemsImage = QPixmap(":/image/image/items.png");
    m_nItemWidth = m_ItemsImage.width() / COLOR_MAX;
    m_nItemHeight = m_ItemsImage.height() / ITEM_MAX;
    shu=0;
    zi=0;

    static PositionStruct pos; // 局面实例
    NewGame();
    startTimer(500);		//启动显隐选中元素的定时器
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PositionStruct::Startup() {
  int sq, pc;
  sdPlayer = vlWhite = vlBlack = nDistance = 0;
  memset(ucpcSquares, 0, 256);
  for (sq = 0; sq < 256; sq ++) {
    pc = cucpcStartup[sq];
    if (pc != 0) {
      AddPiece(sq, pc);
    }
  }
}
int SRC(int mv) {
  return mv & 255;
}
int DST(int mv) {
  return mv >> 8;
}
int MainWindow::PositionStruct::MovePiece(int mv) {
  int sqSrc, sqDst, pc, pcCaptured;
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pcCaptured = ucpcSquares[sqDst];
  if (pcCaptured != 0) {
    DelPiece(sqDst, pcCaptured);
  }
  pc = ucpcSquares[sqSrc];
  DelPiece(sqSrc, pc);
  AddPiece(sqDst, pc);
  return pcCaptured;
}

void MainWindow::PositionStruct::UndoMovePiece(int mv, int pcCaptured) {
  int sqSrc, sqDst, pc;
  sqSrc = SRC(mv);
  sqDst = DST(mv);
  pc = ucpcSquares[sqDst];
  DelPiece(sqDst, pc);
  AddPiece(sqSrc, pc);
  if (pcCaptured != 0) {
    AddPiece(sqDst, pcCaptured);
  }
}

BOOL MainWindow::PositionStruct::MakeMove(int mv, int &pcCaptured) {
  pcCaptured = MovePiece(mv);
  if (Checked()) {
    UndoMovePiece(mv, pcCaptured);
    return FALSE;
  }
  ChangeSide();
  nDistance ++;
  return TRUE;
}


// 获得红黑标记(红子是8，黑子是16)
inline int SIDE_TAG(int sd) {
  return 8 + (sd << 3);
}

// 获得对方红黑标记
inline int OPP_SIDE_TAG(int sd) {
  return 16 - (sd << 3);
}

// 格子水平镜像
inline int SQUARE_FORWARD(int sq, int sd) {
  return sq - 16 + (sd << 5);
}

BOOL MainWindow::PositionStruct::Checked() const {
  int i, j, sqSrc, sqDst;
  int pcSelfSide, pcOppSide, pcDst, nDelta;
  pcSelfSide = SIDE_TAG(sdPlayer);
  pcOppSide = OPP_SIDE_TAG(sdPlayer);
  // 找到棋盘上的帅(将)，再做以下判断：

  for (sqSrc = 0; sqSrc < 256; sqSrc ++) {
    if (ucpcSquares[sqSrc] != pcSelfSide + PIECE_KING) {
      continue;
    }

    // 1. 判断是否被对方的兵(卒)将军
    if (ucpcSquares[SQUARE_FORWARD(sqSrc, sdPlayer)] == pcOppSide + PIECE_PAWN) {
      return TRUE;
    }
    for (nDelta = -1; nDelta <= 1; nDelta += 2) {
      if (ucpcSquares[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
        return TRUE;
      }
    }

    // 2. 判断是否被对方的马将军(以仕(士)的步长当作马腿)
    for (i = 0; i < 4; i ++) {
      if (ucpcSquares[sqSrc + ccAdvisorDelta[i]] != 0) {
        continue;
      }
      for (j = 0; j < 2; j ++) {
        pcDst = ucpcSquares[sqSrc + ccKnightCheckDelta[i][j]];
        if (pcDst == pcOppSide + PIECE_KNIGHT) {
          return TRUE;
        }
      }
    }

    // 3. 判断是否被对方的车或炮将军(包括将帅对脸)
    for (i = 0; i < 4; i ++) {
      nDelta = ccKingDelta[i];
      sqDst = sqSrc + nDelta;
      while (IN_BOARD(sqDst)) {
        pcDst = ucpcSquares[sqDst];
        if (pcDst != 0) {
          if (pcDst == pcOppSide + PIECE_ROOK || pcDst == pcOppSide + PIECE_KING) {
            return TRUE;
          }
          break;
        }
        sqDst += nDelta;
      }
      sqDst += nDelta;
      while (IN_BOARD(sqDst)) {
        int pcDst = ucpcSquares[sqDst];
        if (pcDst != 0) {
          if (pcDst == pcOppSide + PIECE_CANNON) {
            return TRUE;
          }
          break;
        }
        sqDst += nDelta;
      }
    }
    return FALSE;
  }
  return FALSE;
}

inline int MOVE(int sqSrc, int sqDst) {
  return sqSrc + sqDst * 256;
}

// 是否未过河
inline BOOL HOME_HALF(int sq, int sd) {
  return (sq & 0x80) != (sd << 7);
}

// 是否已过河
inline BOOL AWAY_HALF(int sq, int sd) {
  return (sq & 0x80) == (sd << 7);
}
// 生成所有走法
int MainWindow::PositionStruct::GenerateMoves(int *mvs) const {
  int i, j, nGenMoves, nDelta, sqSrc, sqDst;
  int pcSelfSide, pcOppSide, pcSrc, pcDst;
  // 生成所有走法，需要经过以下几个步骤：

  nGenMoves = 0;
  pcSelfSide = SIDE_TAG(sdPlayer);
  pcOppSide = OPP_SIDE_TAG(sdPlayer);
  for (sqSrc = 0; sqSrc < 256; sqSrc ++) {

    // 1. 找到一个本方棋子，再做以下判断：
    pcSrc = ucpcSquares[sqSrc];
    if ((pcSrc & pcSelfSide) == 0) {
      continue;
    }

    // 2. 根据棋子确定走法
    switch (pcSrc - pcSelfSide) {
    case PIECE_KING:
      for (i = 0; i < 4; i ++) {
        sqDst = sqSrc + ccKingDelta[i];
        if (!IN_FORT(sqDst)) {
          continue;
        }
        pcDst = ucpcSquares[sqDst];
        if ((pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
      }
      break;
    case PIECE_ADVISOR:
      for (i = 0; i < 4; i ++) {
        sqDst = sqSrc + ccAdvisorDelta[i];
        if (!IN_FORT(sqDst)) {
          continue;
        }
        pcDst = ucpcSquares[sqDst];
        if ((pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
      }
      break;
    case PIECE_BISHOP:
      for (i = 0; i < 4; i ++) {
        sqDst = sqSrc + ccAdvisorDelta[i];
        if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, sdPlayer) && ucpcSquares[sqDst] == 0)) {
          continue;
        }
        sqDst += ccAdvisorDelta[i];
        pcDst = ucpcSquares[sqDst];
        if ((pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
      }
      break;
    case PIECE_KNIGHT:
      for (i = 0; i < 4; i ++) {
        sqDst = sqSrc + ccKingDelta[i];
        if (ucpcSquares[sqDst] != 0) {
          continue;
        }
        for (j = 0; j < 2; j ++) {
          sqDst = sqSrc + ccKnightDelta[i][j];
          if (!IN_BOARD(sqDst)) {
            continue;
          }
          pcDst = ucpcSquares[sqDst];
          if ((pcDst & pcSelfSide) == 0) {
            mvs[nGenMoves] = MOVE(sqSrc, sqDst);
            nGenMoves ++;
          }
        }
      }
      break;
    case PIECE_ROOK:
      for (i = 0; i < 4; i ++) {
        nDelta = ccKingDelta[i];
        sqDst = sqSrc + nDelta;
        while (IN_BOARD(sqDst)) {
          pcDst = ucpcSquares[sqDst];
          if (pcDst == 0) {
              mvs[nGenMoves] = MOVE(sqSrc, sqDst);
              nGenMoves ++;
          } else {
            if ((pcDst & pcOppSide) != 0) {
              mvs[nGenMoves] = MOVE(sqSrc, sqDst);
              nGenMoves ++;
            }
            break;
          }
          sqDst += nDelta;
        }
      }
      break;
    case PIECE_CANNON:
      for (i = 0; i < 4; i ++) {
        nDelta = ccKingDelta[i];
        sqDst = sqSrc + nDelta;
        while (IN_BOARD(sqDst)) {
          pcDst = ucpcSquares[sqDst];
          if (pcDst == 0) {
            mvs[nGenMoves] = MOVE(sqSrc, sqDst);
            nGenMoves ++;
          } else {
            break;
          }
          sqDst += nDelta;
        }
        sqDst += nDelta;
        while (IN_BOARD(sqDst)) {
          pcDst = ucpcSquares[sqDst];
          if (pcDst != 0) {
            if ((pcDst & pcOppSide) != 0) {
              mvs[nGenMoves] = MOVE(sqSrc, sqDst);
              nGenMoves ++;
            }
            break;
          }
          sqDst += nDelta;
        }
      }
      break;
    case PIECE_PAWN:
      sqDst = SQUARE_FORWARD(sqSrc, sdPlayer);
      if (IN_BOARD(sqDst)) {
        pcDst = ucpcSquares[sqDst];
        if ((pcDst & pcSelfSide) == 0) {
          mvs[nGenMoves] = MOVE(sqSrc, sqDst);
          nGenMoves ++;
        }
      }
      if (AWAY_HALF(sqSrc, sdPlayer)) {
        for (nDelta = -1; nDelta <= 1; nDelta += 2) {
          sqDst = sqSrc + nDelta;
          if (IN_BOARD(sqDst)) {
            pcDst = ucpcSquares[sqDst];
            if ((pcDst & pcSelfSide) == 0) {
              mvs[nGenMoves] = MOVE(sqSrc, sqDst);
              nGenMoves ++;
            }
          }
        }
      }
      break;
    }
  }
  return nGenMoves;
}

// 走法是否符合帅(将)的步长
inline BOOL KING_SPAN(int sqSrc, int sqDst) {
  return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

// 走法是否符合仕(士)的步长
inline BOOL ADVISOR_SPAN(int sqSrc, int sqDst) {
  return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

// 走法是否符合相(象)的步长
inline BOOL BISHOP_SPAN(int sqSrc, int sqDst) {
  return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}


// 是否在河的同一边
inline BOOL SAME_HALF(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x80) == 0;
}

// 是否在同一行
inline BOOL SAME_RANK(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

// 是否在同一列
inline BOOL SAME_FILE(int sqSrc, int sqDst) {
  return ((sqSrc ^ sqDst) & 0x0f) == 0;
}


// 相(象)眼的位置
inline int BISHOP_PIN(int sqSrc, int sqDst) {
  return (sqSrc + sqDst) >> 1;
}

// 马腿的位置
inline int KNIGHT_PIN(int sqSrc, int sqDst) {
  return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}


BOOL MainWindow::PositionStruct::LegalMove(int mv) const {
  int sqSrc, sqDst, sqPin;
  int pcSelfSide, pcSrc, pcDst, nDelta;
  // 判断走法是否合法，需要经过以下的判断过程：

  // 1. 判断起始格是否有自己的棋子
  sqSrc = SRC(mv);
  pcSrc = ucpcSquares[sqSrc];
  pcSelfSide = SIDE_TAG(sdPlayer);
  if ((pcSrc & pcSelfSide) == 0) {
    return FALSE;
  }

  // 2. 判断目标格是否有自己的棋子
  sqDst = DST(mv);
  pcDst = ucpcSquares[sqDst];
  if ((pcDst & pcSelfSide) != 0) {
    return FALSE;
  }

  // 3. 根据棋子的类型检查走法是否合理
  switch (pcSrc - pcSelfSide) {
  case PIECE_KING:
    return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
  case PIECE_ADVISOR:
    return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
  case PIECE_BISHOP:
    return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst) &&
        ucpcSquares[BISHOP_PIN(sqSrc, sqDst)] == 0;
  case PIECE_KNIGHT:
    sqPin = KNIGHT_PIN(sqSrc, sqDst);
    return sqPin != sqSrc && ucpcSquares[sqPin] == 0;
  case PIECE_ROOK:
  case PIECE_CANNON:
    if (SAME_RANK(sqSrc, sqDst)) {
      nDelta = (sqDst < sqSrc ? -1 : 1);
    } else if (SAME_FILE(sqSrc, sqDst)) {
      nDelta = (sqDst < sqSrc ? -16 : 16);
    } else {
      return FALSE;
    }
    sqPin = sqSrc + nDelta;
    while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
      sqPin += nDelta;
    }
    if (sqPin == sqDst) {
      return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
    } else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
      sqPin += nDelta;
      while (sqPin != sqDst && ucpcSquares[sqPin] == 0) {
        sqPin += nDelta;
      }
      return sqPin == sqDst;
    } else {
      return FALSE;
    }
  case PIECE_PAWN:
    if (AWAY_HALF(sqDst, sdPlayer) && (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
      return TRUE;
    }
    return sqDst == SQUARE_FORWARD(sqSrc, sdPlayer);
  default:
    return FALSE;
  }
}


BOOL MainWindow::PositionStruct::IsMate(void) {
  int i, nGenMoveNum, pcCaptured;
  int mvs[MAX_GEN_MOVES];

  nGenMoveNum = GenerateMoves(mvs);
  for (i = 0; i < nGenMoveNum; i ++) {
    pcCaptured = MovePiece(mvs[i]);
    if (!Checked()) {
      UndoMovePiece(mvs[i], pcCaptured);
      return FALSE;
    } else {
      UndoMovePiece(mvs[i], pcCaptured);
    }
  }
  return TRUE;
}

int CompareHistory(const void *lpmv1, const void *lpmv2) {
  return Search.nHistoryTable[*(int *) lpmv2] - Search.nHistoryTable[*(int *) lpmv1];
}

 int MainWindow::SearchFull(int vlAlpha, int vlBeta, int nDepth) {
  int i, nGenMoves, pcCaptured;
  int vl, vlBest, mvBest;
  int mvs[MAX_GEN_MOVES];
  // 一个Alpha-Beta完全搜索分为以下几个阶段

  // 1. 到达水平线，则返回局面评价值
  if (nDepth == 0) {
    return pos.Evaluate();
  }

  // 2. 初始化最佳值和最佳走法
  vlBest = -MATE_VALUE; // 这样可以知道，是否一个走法都没走过(杀棋)
  mvBest = 0;           // 这样可以知道，是否搜索到了Beta走法或PV走法，以便保存到历史表

  // 3. 生成全部走法，并根据历史表排序
  nGenMoves = pos.GenerateMoves(mvs);
  qsort(mvs, nGenMoves, sizeof(int), CompareHistory);

  // 4. 逐一走这些走法，并进行递归
  for (i = 0 ; i < nGenMoves; i ++) {
    if (pos.MakeMove(mvs[i], pcCaptured)) {
      vl = -SearchFull(-vlBeta, -vlAlpha, nDepth - 1);
      pos.UndoMakeMove(mvs[i], pcCaptured);

      // 5. 进行Alpha-Beta大小判断和截断
      if (vl > vlBest) {    // 找到最佳值(但不能确定是Alpha、PV还是Beta走法)
        vlBest = vl;        // "vlBest"就是目前要返回的最佳值，可能超出Alpha-Beta边界
        if (vl >= vlBeta) { // 找到一个Beta走法
          mvBest = mvs[i];  // Beta走法要保存到历史表
          break;            // Beta截断
        }
        if (vl > vlAlpha) { // 找到一个PV走法
          mvBest = mvs[i];  // PV走法要保存到历史表
          vlAlpha = vl;     // 缩小Alpha-Beta边界
        }
      }
    }
  }

  // 5. 所有走法都搜索完了，把最佳走法(不能是Alpha走法)保存到历史表，返回最佳值
  if (vlBest == -MATE_VALUE) {
    // 如果是杀棋，就根据杀棋步数给出评价
    return pos.nDistance - MATE_VALUE;
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
}

 // 迭代加深搜索过程
void MainWindow::SearchMain() {
   int i, t, vl;

   // 初始化
   memset(Search.nHistoryTable, 0, 65536 * sizeof(int)); // 清空历史表
   t = clock();       // 初始化定时器
   pos.nDistance = 0; // 初始步数
   // 迭代加深过程
   for (i = 1; i <= LIMIT_DEPTH; i ++) {
     vl = SearchFull(-MATE_VALUE, MATE_VALUE, i);
     // 搜索到杀棋，就终止搜索
     qDebug()<<"搜索深度："<<i;
     if (vl > WIN_VALUE || vl < -WIN_VALUE) {
       break;
     }
     // 超过一秒，就终止搜索
     if (clock() - t > CLOCKS_PER_SEC) {
       break;
     }
     qDebug()<<"outing SearchMain";
   }
 }



void MainWindow::paintEvent( QPaintEvent * )
{
	QPainter painter(this);
	for (int i = 0; i<m_items.size(); i++)
	{
		DrawItem(painter,m_items[i]);
	}
	update();
}

void MainWindow::receiveData(bool singleClicked)
{
    isSingleGame = singleClicked;
         //获取传递过来的数据
}



void MainWindow::write(QJsonObject &json)const
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


void MainWindow::read(const QJsonObject &json)
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



bool MainWindow::loadgame()
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




int MainWindow::getAllMoveArea(QVector<Item>&allItem,QVector<QPoint> &DetArea)
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
    int value = 0;
}bestMove22[100],bestMoveX;
/*int MainWindow::SearchFull(int vlAlpha, int vlBeta, int nDepth, int flag)
{
    if(flag == 1)
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
    qDebug()<<"I love you";

    int j = 0;
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
                    bestMove22[j].item = allItem[i];
                    //bestMove[0] = allItem[i];
                    bestMove22[j].point = allArea[i];
                    bestMove22[j].value += nDepth * nDepth;
                    j++;
                    break;
                }
                if(eval > vlAlpha)
                {
                    //bestMove22[i].item = allItem[i];
                    //bestMove22[i].point = allArea[i];
                    vlAlpha = eval;
                }
            }
        for(int i = j-1 ; i >= 0 ; i--)
        {
            if(bestMove22[i].item.m_color == 0)
                continue;
            else
            {
                bestMoveX.item = bestMove22[i].item;
                bestMoveX.point = bestMove22[i].point;
                bestMoveX.value = bestMove22[i].value;
            }
        }
        ChangeItemPoint2(bestMoveX.item.m_pt , bestMoveX.point , copyItem);
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
                bestMove22[j].item = allItem[i];
                //bestMove[0] = allItem[i];
                bestMove22[j].point = allArea[i];
                bestMove22[j].value += nDepth * nDepth;
                j++;
                break;
            }
            if(eval < vlBeta)
            {
                //bestMove22.item = allItem[i];
                //bestMove22.point = allArea[i];
                vlBeta = eval;
            }
        }
        for(int i = j-1 ; i >= 0 ; i--)
        {
            if(bestMove22[i].item.m_color == 0)
                continue;
            else
            {
                bestMoveX.item = bestMove22[i].item;
                bestMoveX.point = bestMove22[i].point;
                bestMoveX.value = bestMove22[i].value;
            }
        }
        ChangeItemPoint2(bestMoveX.item.m_pt , bestMoveX.point , copyItem);
        return minEval;
    }
}

*/
/*void MainWindow::SearchMain() {
  int i, t, vl;

  // 初始化

  t = clock();       // 初始化定时器
  //pos.nDistance = 0; // 初始步数

  // 迭代加深过程
  for (i = 1; i <= LIMIT_DEPTH; i ++) {
    vl = SearchFull(-MATE_VALUE, MATE_VALUE, i , 1);
    // 搜索到杀棋，就终止搜索
    if (vl > WIN_VALUE || vl < -WIN_VALUE) {
      break;
    }
    // 超过一秒，就终止搜索
    if (clock() - t > CLOCKS_PER_SEC) {
      break;
    }
  }
}

*/
 void MainWindow::DrawItem(QPainter& painter, Item item )
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

void MainWindow::InitItems()
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
void MainWindow::mousePressEvent(QMouseEvent * e)
{

        //获得鼠标点所对应的棋盘点pt
        QPoint pt;
        pt.setX( (e->pos().x() - START_X ) / RECT_WIDTH);
        pt.setY( (e->pos().y() - START_Y ) / RECT_HEIGHT);

        //是否有选中的棋子
    if(isSingleGame == false)
    {
        if(m_bExistSelectedItem)
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
                if ( (m_bIsRedTurn && ClickedItem.m_color == COLOR_RED) ||
                    (!m_bIsRedTurn && ClickedItem.m_color == COLOR_BLACK))
                {
                    m_SelectedItem = ClickedItem;
                    m_bExistSelectedItem = true;
                    return;
                }
            }
        }
    }
    else

    {
        if(m_bExistSelectedItem)
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
                if ( (m_bIsRedTurn && ClickedItem.m_color == COLOR_RED))
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
                m_bExistSelectedItem = false;                     //做过更改
                m_bIsRedTurn = !m_bIsRedTurn;
                update();
                //return ;
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
            qDebug()<<"inside searchMain";
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
        int pcCaptured;
        SearchMain();

        qDebug()<<"searchMain之后";
        qDebug()<<"pos内容"<<pos.ucpcSquares;
        qDebug()<<Search.mvResult;
        pos.MakeMove(Search.mvResult, pcCaptured);
        //qDebug()<<"新pos内容"<<pos.ucpcSquares;
        adaptMove();
        //qDebug()<<"adaptMove()之后";
        return;

    }

}

bool MainWindow::MoveItem(Item item,QPoint pt)
{
    qDebug()<<"inside MoveItem()";
    if(item.m_pt == pt)
        return true;
    qDebug()<<"走了后面的";
    QVector<QPoint> moveArea;
    GetMoveArea(item,moveArea);
    if (moveArea.contains(pt))
    {
        bool bDeleteSHUAI = false;
        bool eat=false;
        eat=DeleteItemAtPoint(pt,bDeleteSHUAI);
        qDebug()<<"inside MoveItem()";
        ChangeItemPoint(item.m_pt,pt,eat);
        if (bDeleteSHUAI)
        {
            QString str = m_bIsRedTurn?QStringLiteral("红方胜利！"):QStringLiteral("黑方胜利！");
            QMessageBox::information(NULL,  "GAME OVER  ",str, QMessageBox::Yes , QMessageBox::Yes);
            NewGame();
        }
        //nDistance++;
        m_bIsRedTurn = 1 - m_bIsRedTurn;                   //经改动
        int Src = (item.m_pt.y() + 3) * 16 + item.m_pt.x() + 3;
        int Dis = (pt.y() + 3) * 16 + pt.x() +3;
        pos.MakeMove(Src,Dis);
        return true;


    }
    else
    {
        return false;
    }
}

void MainWindow::adaptMove()
{
    qDebug()<<"inside adaptMove()";
    bool a1 = false , a2 = false , a3 = false , a4 = false , a5 = false;
    bool b1 = false , b2 = false , b3 = false , b4 = false , b5 = false;
    int indexBlackBing = 0;
    int indexRedBing = 0;
    int size = m_items.size();
    for(int i = 0 ; i < 256 ; i++)
    {
        if(pos.ucpcSquares[i] != 0)
        {
            switch (pos.ucpcSquares[i]) {
            case 20:                                            //车
            {
                qDebug()<<"这是黑车";
                if(a1 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 1)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    a1 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 9)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }
                break;
            case 19:                                                    //  马
            {
                qDebug()<<"这是黑马";
                if(a2 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 2)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    a2 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 8)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }
                break;
//            case 18:                                           //象
            {
                if(a3 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 3)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    a3 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 7)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 17:                                           //士
            {
                if(a4 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 4)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    a4 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 6)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 16:                                                  //帅
            {
                int m = 0;
                for(m ; m < size ; m++)
                {
                    if(m_items[m].num == 5)
                        break;
                }
                int x = m % 16 - 3;
                int y = m / 16 - 3;
                QPoint newPosition;
                newPosition.setX(x);
                newPosition.setY(y);
                MoveItem(m_items[m] , newPosition);
            }
                break;
            case 21:                                                //炮
            {
                if(a5 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 10)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    a5 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 11)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 22:                                                 //兵
            {
                if(indexBlackBing == 0)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 12)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexBlackBing++;
                }
                else if(indexBlackBing == 1)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 13)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexBlackBing++;
                }
                else if(indexBlackBing == 2)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 14)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexBlackBing++;
                }
                else if(indexBlackBing == 3)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 15)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexBlackBing++;
                }
                else if(indexBlackBing == 4)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 16)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }

       //红方
            case 12:                                            //车
            {
                if(b1 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 17)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    b1 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 25)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }
                break;
            case 11:                                                    //  马
            {
                if(b2 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 18)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    b2 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 24)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }
                break;
//            case 10:                                           //象
            {
                if(b3 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 19)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    b3 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 23)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 9:                                           //士
            {
                if(b4 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 20)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    b4 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 22)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 8:                                                  //帅
            {
                int m = 0;
                for(m ; m < size ; m++)
                {
                    if(m_items[m].num == 21)
                        break;
                }
                int x = m % 16 - 3;
                int y = m / 16 - 3;
                QPoint newPosition;
                newPosition.setX(x);
                newPosition.setY(y);
                MoveItem(m_items[m] , newPosition);
            }
                break;
            case 13:                                                //炮
            {
                if(b5 == false)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 26)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    b5 = true;
                }
                else
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 27)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                }
            }
                break;
            case 14:                                                 //兵
            {
                if(indexRedBing == 0)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 28)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexRedBing++;
                }
                else if(indexRedBing == 1)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 29)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexRedBing++;
                }
                else if(indexRedBing == 2)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 30)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexRedBing++;
                }
                else if(indexBlackBing == 3)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 31)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);
                    indexRedBing++;
                }
                else if(indexRedBing = 4)
                {
                    int m = 0;
                    for(m ; m < size ; m++)
                    {
                        if(m_items[m].num == 32)
                            break;
                    }
                    int x = m % 16 - 3;
                    int y = m / 16 - 3;
                    QPoint newPosition;
                    newPosition.setX(x);
                    newPosition.setY(y);
                    MoveItem(m_items[m] , newPosition);

                }
            }
            default:
                break;
            }
        }

    }
    qDebug()<<"outside adaptMove";
}

bool MainWindow::FindItemAtPoint(QPoint pt,Item& item)
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

bool MainWindow::DeleteItemAtPoint(QPoint pt,bool& bDeleteSHUAI)
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

void MainWindow::GetMoveArea(Item item,QVector<QPoint>& moveArea)
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

void MainWindow::ChangeItemPoint(QPoint ptItem,QPoint pt,bool eats)
{
    qDebug()<<"inside ChangeItemPoint";
    for (int i = 0; i<m_items.size(); i++)
	{
		if (m_items[i].m_pt == ptItem)
		{
			m_SelectedItem.m_pt = QPoint(-1,-1);
            previous.append(m_items[i]);
            bools.append(eats);
            shu++;
            m_items[i].m_pt = pt;
            m_items[i].m_bShow = true;
			break;
		}
	}
    int Src = (ptItem.y() + 3) * 16 + ptItem.x() + 3;
    int Dis = (pt.y() + 3) * 16 + pt.x() +3;
    pos.MakeMove(Src,Dis);
}


void MainWindow::ChangeItemPoint2(QPoint ptItem,QPoint pt,QVector<Item>&x)
{
    for (int i = 0; i<x.size(); i++)
    {
        if (x[i].m_pt == ptItem)
        {
            m_SelectedItem.m_pt = QPoint(-1,-1);

            x[i].m_pt = pt;
            x[i].m_bShow = true;
            break;
        }
    }
}

void MainWindow::timerEvent( QTimerEvent * )
{
	for (int i = 0; i<m_items.size(); i++)
	{
		if (m_items[i].m_pt == m_SelectedItem.m_pt)
		{
			m_items[i].m_bShow = !m_items[i].m_bShow;
		}
	}
}

void MainWindow::GetMoveArea_JU(Item item,QVector<QPoint>& moveArea)
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

void MainWindow::GetMoveArea_MA( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::GetMoveArea_XIANG( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::GetMoveArea_SHI( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::GetMoveArea_SHUAI( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::GetMoveArea_PAO( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::GetMoveArea_BING( Item item,QVector<QPoint>& moveArea )
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

void MainWindow::NewGame()
{
	InitItems();
	m_SelectedItem.m_pt = QPoint(-1,-1);
	m_bIsRedTurn = true;
	m_bExistSelectedItem = false;
	
}

void MainWindow::SetItemShow( Item item,bool bShow )
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



void MainWindow::on_pushButton_clicked()
{
    close();
    MyDialog dlg;
    if(dlg.exec()==QDialog::Accepted)show();
}

void MainWindow::on_pushButton_2_clicked()
{
    NewGame();
}

void MainWindow::on_pushButton_3_clicked()
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


int MainWindow::getValue(Item &targetItem)
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


int MainWindow::evaluate(QVector<Item>&x)
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

void MainWindow::on_save_clicked()
{
    QFile saveFile(QStringLiteral("save.json"));
    saveFile.open(QIODevice::WriteOnly);
    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());

}
