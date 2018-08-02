/*!
 *  @brief     历史消息查看显示
 *  @details   显示历史聊天消息
 *  @file      historymsgrecord.h
 *  @author    LYS
 *  @version   1.0.1
 *  @date      2018.06.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *
 */
#ifndef HISTORYMSGRECORD_H
#define HISTORYMSGRECORD_H

#include <QWidget>

class HistoryMsgRecord : public QWidget
{
    Q_OBJECT
public:
    explicit HistoryMsgRecord(QWidget *parent = 0);
    ~HistoryMsgRecord();

signals:

public slots:

};

#endif // HISTORYMSGRECORD_H
