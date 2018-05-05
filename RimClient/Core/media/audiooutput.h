/*!
 *  @brief     音频播放
 *  @details   提供基本的播放功能
 *  @author    wey
 *  @version   1.0
 *  @date      2018.05.03
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QFile>
#include <QAudio>
#include <QAudioFormat>
#include <QObject>
class QAudioOutput;

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    AudioOutput();
    ~AudioOutput();

    void setAudioSaveDir(QString path);
    bool setAudioFormat(QAudioFormat & audioFormat);

    bool start();
    bool stop();

private slots:
    void handleStateChanged(QAudio::State state);

private:
    QAudioOutput* audio;
    QAudioFormat format;
    QFile sourceFile;

    QString audioSaveDir;
    QString fileName;
};

#endif // AUDIOOUTPUT_H
