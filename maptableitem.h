#ifndef MAPTABLEITEM_H
#define MAPTABLEITEM_H

#include "GraphicsMapLib_global.h"
#include <QGraphicsItem>
#include <QGeoCoordinate>
#include <QFont>
#include <QPen>

/*!
 * \brief 图表
 * \details 显示字段名和字段值,由于其显示的过程更新值较多，需手动调用updateTableSize()  提高其计算图表大小
 * 提升计算效率，其绘画效率
 * \warning 测试后得出 存在同一viewport的图元 GrapihcsView的更新模式，会存在图元较少，为提高更新效率，
 * 选用的视口更新模式为SmartViewportUpdate  更新其视口所有图元。
 */
class GRAPHICSMAPLIB_EXPORT MapTableItem : public QObject, public QGraphicsItem
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
    MapTableItem(const QGeoCoordinate &coord = {0, 0, 0}, QGraphicsItem * parent = nullptr);
    ~MapTableItem();

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
    /// 设置启用固定方向
    void setFixDirect(bool bFixDirect, double fixedangle = 0.0);
    /// 设置圆角半径
    void setRoundedRadius(int roundRadius);
    /// 设置边框画笔
    void setBorderPen(const QPen &borderPen);
    /// 设置背景画刷
    void setBackBrush(const QBrush &brush);
    /// 设置背景图片
    void setBackPixmap(const QPixmap &pixmap);
    /// 获取牌匾大小
    QRectF tabletRect();
    /// 获取字段值.
    QString getValue(const QString &field);
    /// 实时修改值时, 手动调用一次计算并更新牌匾大小
    void updateTableSize();

    /// 设置是否允许鼠标事件，影响是否能触发点击信号(但是可以收到press信号)以及切换选中状态
    void setAllowMouseEvent(bool enable);
    /// 设置鼠标可拖拽
    void setMoveable(bool movable);
    /// 设置选中性
    void setCheckable(bool checkable);
    /// 设置选中状态(不会触发信号)
    void setChecked(bool checked);
    /// 切换选中状态(不会触发信号)
    void toggle();
    /// 是否选中
    bool isChecked() const;
protected:
    /// 更新牌匾大小.
    void UpdateInfo();
    QRectF boundingRect() const override;
    QPainterPath shape()  const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
signals:
    void doubleClicked();
    void coordinateChanged(const QGeoCoordinate &coord);
    void coordinateDragged(const QPointF &point);
protected:
    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
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
    public:
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

    qreal					m_FixedAngle;//固定角度值.  暂没用上
    int						m_Margin[_upper];
    int						m_nMaxWide;//记录最大宽度.
    int						m_nCurHigh;//记录当前高度.   暂没用上
    int						m_BackWide;//背景矩形框宽度. 暂没用上
    int						m_RoundRadius;//圆角半径.   暂没用上
    bool                    m_bFixedDirect;//固定方向.

    QSize                   m_ScutSize;  // 标牌大小
    QPen                    m_BorderPen; // 边框画笔.
    QBrush					m_BackBrush;    // 背景画刷.
    AnchorPosition          m_AnchorPos; // 锚点位置.
    QStringList				m_Fields;     //字段名列表(存在是需要有序显示).
    QMap<QString, FieldInfo *>  m_Infoes; //字段信息.
    QPixmap					m_backPixmap; //背景底图.

    QGraphicsRectItem       m_border;  // 外围边框
    //
    bool m_enableMouse = true;
    bool m_checkable = false;
    bool m_checked = false;
    //
    QPoint m_pressPos;
    QGeoCoordinate          m_coord;
};


#endif // MAPTABLEITEM_H
