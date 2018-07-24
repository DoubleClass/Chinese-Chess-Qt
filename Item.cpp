#include "Item.h"
#include<QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonValue>
Item::Item(ITEM_TYPE t, ITEM_COLOR c, QPoint pt, int nums)
{
	m_type = t;
	m_color = c;
	m_pt = pt;
	m_bShow = true;
    num=nums;



}


Item::~Item(void)
{
}
void Item::write(QJsonObject &json)const
{

    json["type"] = m_type;
    json["color"] = m_color;
    json["pt_x"] = m_pt.x();
    json["pt_y"] = m_pt.y();
    json["num"] = num;
    if(m_bShow==true)
        json["m_bShow"] = 1;
    else
        json["m_bShow"] = 0;

}
void Item::read(const QJsonObject &json)
{
    m_type = ITEM_TYPE(qRound(json["type"].toDouble()));

    m_color = ITEM_COLOR(qRound(json["color"].toDouble()));
    m_pt.setX(qRound(json["pt_x"].toDouble()));
    m_pt.setY(qRound(json["pt_y"].toDouble()));
    num = qRound(json["num"].toDouble());
    if(json["m_bShow"].toDouble() == 1)
        m_bShow = true;
    else
        m_bShow = false;
}

