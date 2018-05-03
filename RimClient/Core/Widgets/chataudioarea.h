/*!
 *  @brief     聊天窗口录音区
 *  @details   提供音频的录制、发送。同一时刻只能有一个页面处于录音状态，因此在打开录音功能前判断是否处于
 *             录音状态
 *  @author    wey
 *  @version   1.0
 *  @date      2018.05.03
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef CHATAUDIOAREA_H
#define CHATAUDIOAREA_H

#include <QWidget>

class ChatAudioAreaPrivate;

class ChatAudioArea : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ChatAudioArea)
public:
    explicit ChatAudioArea(QWidget *parent = 0);
    ~ChatAudioArea();

    void start();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void prepareSendAudio();
    void preapreCancelAudio();

private slots:
    void sendAudio();
    void cancelSend();
    void updateRecordProgress();

private:
    ChatAudioAreaPrivate * d_ptr;

};

#endif // CHATAUDIOAREA_H
