/*!
 *  @brief     音频播放管理
 *  @details   1.默认从配置文件中读取，若不存在则从系统config/sound目录下加载；
 *             2.支持修改音频文件，并保存至系统配置文件中；
 *  @file      mediaplayer.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QObject>
#include <QMediaContent>
#include <QUrl>

class QMediaPlayer;
class QMediaPlaylist;

class MediaPlayer : public QObject
{
    Q_OBJECT
public:
    explicit MediaPlayer(QObject * parent = 0);

    enum MediaType
    {
        MediaSystem,
        MediaMsg,
        MediaShake,
        MediaOnline,
        MediaSendMess,
        MediaAudio
    };
    Q_FLAG(MediaType)

    struct MediaFile
    {
        QUrl url;
        bool exist;
    };

    bool play(MediaType type);

    void setVolume(int volume);
    int getVolume();

    void setMuted(bool flag);
    bool isMuted();

    bool updatePlayest(const MediaType type,const QString fileName);

    const QMap<int, MediaFile *> getPlayerList();

private slots:
    void switchMedia(QMediaContent content);

private:
    void init();

private:
    QMediaPlayer * player;
    QMap<int,MediaFile *> playerlist;
};

#endif // MEDIAPLAYER_H
