#include "mediaplayer.h"

#include <QMediaPlayer>
#include <QDir>
#include <QUrl>
#include <QMetaEnum>
#include <QApplication>
#include <QFileInfo>

#include "constants.h"
#include "Util/rutil.h"

MediaPlayer::MediaPlayer(QObject *parent):
    QObject(parent)
{
    init();
}

bool MediaPlayer::play(MediaPlayer::MediaType type)
{
    if(playerlist.value(type)->exist)
    {
        player->setMedia(playerlist.value(type)->url);
        player->play();
        return true;
    }
    return false;
}

void MediaPlayer::setVolume(int volume)
{
    player->setVolume(volume);
}

int MediaPlayer::getVolume()
{
    return player->volume();
}

void MediaPlayer::setMuted(bool flag)
{
    player->setMuted(flag);
}

bool MediaPlayer::isMuted()
{
    return player->isMuted();
}

bool MediaPlayer::updatePlayest(const MediaPlayer::MediaType type, const QString fileName)
{
    if(type > MediaAudio || type < MediaSystem)
    {
        return false;
    }

    QFileInfo fileInfo(fileName);
    if(fileInfo.exists())
    {
        MediaFile * file = playerlist.value(type);
        file->url = QUrl::fromLocalFile(fileName);

        QSettings * settings = RUtil::globalSettings();
        QMetaEnum metaEnum = QMetaEnum::fromType<MediaType>();
        settings->beginGroup(Constant::USER_SETTING_SOUND_GROUP);
        settings->setValue(metaEnum.key((int)type),fileName);
        settings->endGroup();

        return true;
    }
    return true;
}

const QMap<int, MediaPlayer::MediaFile *> MediaPlayer::getPlayerList()
{
    return playerlist;
}

void MediaPlayer::switchMedia(QMediaContent content)
{
    player->stop();
}

void MediaPlayer::init()
{
    player = new QMediaPlayer();
    connect(player,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(switchMedia(QMediaContent)));

    QSettings * settings = RUtil::globalSettings();
    settings->beginGroup(Constant::USER_SETTING_SOUND_GROUP);

    QString defaultFilePath = qApp->applicationDirPath()+Constant::PATH_ConfigPath+Constant::CONFIG_SountPath;

    QMetaEnum metaEnum = QMetaEnum::fromType<MediaType>();
    for(int i = 0; i < metaEnum.keyCount();i++)
    {
        QString wavName = metaEnum.key(i);
        QString defaultFile = QString("%1/%2.wav").arg(defaultFilePath).arg(wavName);

        QString filePath = settings->value(wavName,defaultFile).toString();

        QFileInfo info(filePath);

        MediaFile * mediaFile = new MediaFile;
        mediaFile->exist = info.exists();

        if(info.exists())
        {
            mediaFile->url = QUrl::fromLocalFile(filePath);
        }

        playerlist.insert((MediaType)i,mediaFile);
    }

    settings->endGroup();
}
