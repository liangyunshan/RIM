/*!
 *  @brief     音频录制
 *  @details   提供基本的录音功能
 *  @author    wey
 *  @version   1.0
 *  @date      2018.05.03
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QFile>
#include <QObject>
#include <QAudio>
#include <QAudioFormat>

class QTimer;
class QAudioInput;

class AudioInput : public QObject
{
    Q_OBJECT
public:
    AudioInput(QObject *parent = 0);
    ~AudioInput();

    void setAudioSaveDir(QString path);
    bool setAudioFormat(QAudioFormat & audioFormat);
    void setMaxRecordTime(uint maxTime = 60 );

    QString lastRecordFullPath();

    bool start();
    bool stop();
    bool clear();
    bool isRecording();

private slots:
    void handleStateChanged(QAudio::State state);
    void stopRecord();

private:
    QAudioInput * audio;
    QAudioFormat format;
    QFile destinationFile;

    QTimer * timer;

    QString audioSaveDir;
    QString fullFileName;           //全路径
    QString fileName;               //文件名

    bool recordState;
    uint maxRecordTime;
};


#endif // AUDIOINPUT_H
