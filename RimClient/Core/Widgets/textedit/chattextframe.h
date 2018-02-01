/*!
 *  @brief     文本区域封装类
 *  @details   用于显示聊天界面中的不同的信息类型和风格
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.01.30
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef CHATTEXTFRAME_H
#define CHATTEXTFRAME_H

#include <QObject>

class QTextEdit;
class QTextFrameFormat;

class ChatTextFrame : public QObject
{
    Q_OBJECT
public:
    ChatTextFrame(QTextEdit *parent) ;
    ~ChatTextFrame() ;

    QTextFrameFormat* currTextFrameFormat();

private:
    QTextEdit *p_parentTextEdit;
    QTextFrameFormat* p_currTextFrameFormat;
};

#endif // CHATTEXTFRAME_H
