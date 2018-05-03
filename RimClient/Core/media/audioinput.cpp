#include "audioinput.h"

#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QDir>
#include <QTimer>

#include "head.h"
#include "constants.h"
#include "Util/rutil.h"

AudioInput::AudioInput(QObject *parent) : QObject(parent),audio(nullptr),timer(nullptr)
  ,recordState(false),maxRecordTime(Constant::MAX_RECORD_TIME)
{

}

AudioInput::~AudioInput()
{
    if(audio){
        audio->stop();
        delete audio;
    }

    if(destinationFile.isOpen())
        destinationFile.close();

    if(timer){
        if(timer->isActive())
            timer->stop();
        delete timer;
    }
}

void AudioInput::setAudioSaveDir(QString path)
{
    audioSaveDir = path;
}

/*!
 * @brief 设置录音编码格式
 * @details 对于设置的音频格式，若当前系统不支持，则寻找与此格式最接近的支持支持格式；若设置成功则返回true
 * @param[in] audioFormat 音频格式
 * @return 是否设置成功
 */
bool AudioInput::setAudioFormat(QAudioFormat &audioFormat)
{
    this->format = audioFormat;

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        format = info.nearestFormat(format);
        return format.isValid();
    }
    return true;
}

void AudioInput::setMaxRecordTime(uint maxTime)
{
    maxRecordTime = maxTime <=0 ? Constant::MAX_RECORD_TIME:maxTime;
}

QString AudioInput::lastRecordFullPath()
{
    return fullFileName;
}

bool AudioInput::start()
{
    if(recordState)
        return false;

    fileName = RUtil::UUID()+".raw";

    if(destinationFile.isOpen())
        destinationFile.close();

    fullFileName = audioSaveDir + QDir::separator() + fileName;
    destinationFile.setFileName(fullFileName);

    if(!destinationFile.open(QFile::WriteOnly | QFile::Truncate)){
        return false;
    }

    if(!audio){
        audio = new QAudioInput(format, this);
        connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    }

    if(!timer){
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer,SIGNAL(timeout()),this,SLOT(stopRecord()));
    }
    if(timer->isActive())
        timer->stop();

    timer->setInterval(maxRecordTime * 1000);
    timer->start();

    audio->start(&destinationFile);
    recordState = true;
    return true;
}

bool AudioInput::stop()
{
    if(!recordState)
        return false;

    if(audio == nullptr || audio->state() == QAudio::StoppedState || audio->state() == QAudio::IdleState)
        return false;

    audio->stop();
    destinationFile.close();
    recordState = false;

    if(timer->isActive())
        timer->stop();

    return true;
}

/*!
 * @brief 清空上次录音产生的文件
 * @return 是否清除成功
 */
bool AudioInput::clear()
{
    QFileInfo info(fullFileName);
    if(!info.exists())
        return true;

    return QFile::remove(fullFileName);
}

bool AudioInput::isRecording()
{
    return recordState;
}

void AudioInput::handleStateChanged(QAudio::State state)
{

}

void AudioInput::stopRecord()
{
    stop();
}
