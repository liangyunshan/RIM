/*!
 *  @brief     自定义聊天窗口中字体设置窗口
 *  @details   允许选择气泡模式/文本模式、设置系统/个性字体、字号、加粗、斜体、颜色
 *  @file      setfontwidget.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.05.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180511:LYS:未完成
 *      20180607:LYS:完成框架搭建，功能待完善
 */
#ifndef SETFONTWIDGET_H
#define SETFONTWIDGET_H

#include <QWidget>

class SetFontWidgetPrivate;
class QColor;

class SetFontWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SetFontWidget)
public:
    enum FontMode
    {
        BUBBLEMODE,     //气泡模式
        TEXTMODE        //文本模式
    };
    enum FontType
    {
        SYSTEMFONT,     //系统字体
        PERSONALFONT    //个性化字体
    };
    explicit SetFontWidget(QWidget *parent = 0);
    ~SetFontWidget();
    int currentFontMode() const;
    void setDefault();

signals:
    void fontModeChanged(int curMode);              //显示模式变化
    void fontChanged(const QFont &font);            //字体变化
    void fontColorChanged(const QColor &color);     //文本模式下字体颜色吧变化
    void bubbleTypeChanged();                       //气泡类型变化

public slots:
    void setCurFontMode(FontMode mode);


private slots:
    void chooseMode(bool);
    void chooseTextFont(bool);
    void chooseTextColor(bool);
    void setTextColorTure(const QColor&);
    void chooseBubbleFont(int);

private:
    SetFontWidgetPrivate *d_ptr;
};

#endif // SETFONTWIDGET_H
