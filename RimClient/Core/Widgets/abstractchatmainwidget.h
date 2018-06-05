/*!
 *  @brief     聊天信息记录抽象窗口
 *  @details   抽象了个人、群聊窗口中聊天信息内容显示窗口特点
 *  @file      abstractchatmainwidget.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.05.10
 *  @warning   嵌入html显示聊天信息。
 *  @copyright NanJing RenGu.
 *  @note
 *      20180511:LYS:抽象化聊天窗口未完成
 */
#ifndef ABSTRACTCHATMAINWIDGET_H
#define ABSTRACTCHATMAINWIDGET_H

#include <QWidget>

class AbstractChatMainWidgetPrivate;

class AbstractChatMainWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatMainWidget)
public:
    explicit AbstractChatMainWidget(QWidget *parent = 0);
    void inserHtml(QUrl &htmlUrl);

signals:

public slots:

private slots:
    void finishLoadHTML(bool);

private:
    AbstractChatMainWidgetPrivate * d_ptr;
};

#endif // ABSTRACTCHATMAINWIDGET_H
