#ifndef MAPOBJECTTOOLTIPITEM_H
#define MAPOBJECTTOOLTIPITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsItem>
#include <QGeoCoordinate>
#include <QFont>
#include <QPen>

class GRAPHICSMAPLIB_EXPORT mapObjectToolTipItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    enum AnchorPosition{
        AP_TOPLEFT = 0X00,
        AP_TOPRIGHT,
        AP_BOTTOMLEFT,
        AP_BOTTOMRIGHT,
        AP_MIDDLELEFT,
        AP_MIDDLERIGHT,
        AP_MIDDLETOP,
        AP_MIDDLEBTTOM,
        AP_CENTER
    };
    mapObjectToolTipItem(const QGeoCoordinate &coord = {0, 0, 0});
    ~mapObjectToolTipItem();

    /// 设置描点位置边距和间距
    void setAnchorPosition(AnchorPosition anchorPos);
    void setMargins(int left, int top, int right, int bottom);
    void setSpacing(int space);
    /// 添加字段
    void addField(const QString &field, bool bVolatile);
    void inrField(int pos, const QString &field, bool bVolatile);
    void delField(const QString &field);
    /// 设置字段名字体
    void setFieldFont(const QString &field, const QFont &font);
    void setValueFont(const QString &field, const QFont &font);
    void setFieldPen(const QString &field, const QPen &pen);
    void setValuePen(const QString &field, const QPen &pen);
    /// 设置字段值
    void setValue(const QString &field, const QString &value);
    /// 设置背景矩形框显示宽度
    void setBackWidth(int width);
    /// 设置启用固定方向
    void setFixDirect(bool bFixDirect, double fixedangle = 0.0);
    /// 设置圆角半径
    void SetRoundedRadius(int roundRadius);
    /// 设置边框画笔
    void SetBorderPen(const QPen &borderPen);
    /// 设置背景画刷
    void SetBackBrush(const QBrush &brush);
    /// 设置符号大小
    void SetSymbolSize(const QSize &size);
    /// 获取牌匾大小
    QSize TabletSize();
    /// 获取字段值.
    QString GetValue(const QString &field);
protected:
    /// m_Margin字段对应下标类型
    enum
    {
        _left = 0x00,//左边距.
        _top = 0x01, //上边距.
        _right = 0x02,//右边距.
        _bottom = 0x03,//下边距.
        _space = 0x04, //字段名与字段值间隔值.
        _maxField = 0x05,//所有字段中最長像素.
        _upper = 0x06, //描述数组值个数.
    };

    class FieldInfo//字段信息.
    {
    private:
        int     fieldHigh;//字段占用高度.
        bool    bVolatile;//标记字段值是否为经常改变性质.
        QFont   fieldFont;//字段名字体.
        QFont   valueFont;//字段值字体.
        QPen    fieldPen;//字段名画笔.
        QPen    valuePen;//字段值画笔.
        QPointF valuePos;//字段值绘制位置.
        QString value;   //字段值.

        FieldInfo();
        FieldInfo(bool _bVolatile);
        FieldInfo(const FieldInfo &ot);
    };

    qreal					m_FixedAngle;//固定角度值.
    int						m_Margin[_upper];
    int						m_nMaxWide;//记录最大宽度.
    int						m_nCurHigh;//记录当前高度.
    int						m_BackWide;//背景矩形框宽度.
    int						m_RoundRadius;//圆角半径.
    bool						m_bFixedDirect;//固定方向.

    /*默认信息.*/
    QSize                   m_SymSize;//符号大小.
    QSize                   m_IncSize;//符号增量.
    /*包围矩形.*/
    QRectF					m_BoundRect;
    /*轮廓路径.*/
    QPainterPath				m_OutlinePath;
    /*边框信息.*/
    QPen						m_BorderPen;
    /*背景画刷.*/
    QBrush					m_BackBrush;
    /*锚点位置.*/
    AnchorPosition				m_AnchorPos;

    QStringList				m_Fields;//字段名列表(存在是需要有序显示).
    QMap<QString, FieldInfo>  m_Infoes;//字段信息.
    QPixmap					m_backPixmap;//背景底图.
};


#endif // MAPOBJECTTOOLTIPITEM_H
