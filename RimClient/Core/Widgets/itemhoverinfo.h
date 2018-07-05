/*!
 *  @brief     用户头像信息显示
 *  @details   鼠标放置于头像之后，悬浮显示用户基础信息。
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20171226:wey:添加动画渐隐/渐现
 */

#ifndef ITEMHOVERINFO_H
#define ITEMHOVERINFO_H

#include "widget.h"
#include "../protocol/datastruct.h"
#include "../protocol/protocoldata.h"

class ItemHoverInfoPrivate;

class ItemHoverInfo : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ItemHoverInfo)
public:
    ItemHoverInfo(QWidget * parent = 0);
    ~ItemHoverInfo();

    void fadein(QPoint itemPos);
    void fadeout();

    void setSimpleUserInfo(SimpleUserInfo info);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    ItemHoverInfoPrivate * d_ptr;
};

#endif // ITEMHOVERINFO_H
