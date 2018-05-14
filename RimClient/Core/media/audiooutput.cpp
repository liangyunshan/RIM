#include "audiooutput.h"

#include <QAudioOutput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QDir>

#include <QDebug>

AudioOutput::AudioOutput():audio(nullptr)
{

}

AudioOutput::~AudioOutput()
{
    if(audio){
        audio->stop();
        delete audio;
    }

    if(sourceFile.isOpen())
        sourceFile.close();
}

void AudioOutput::setAudioSaveDir(QString path)
{
    audioSaveDir = path;
}

bool AudioOutput::setAudioFormat(QAudioFormat &audioFormat)
{
    this->format = audioFormat;

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        format = info.nearestFormat(format);
        return format.isValid();
    }
    return true;
}

bool AudioOutput::start(QString fileName)
{
    if(sourceFile.isOpen())
        sourceFile.close();

    sourceFile.setFileName(audioSaveDir + QDir::separator() + fileName);

    if(!sourceFile.open(QFile::ReadOnly)){
        return false;
    }

    if(!audio){
        audio = new QAudioOutput(format, this);
        connect(audio, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    }

    audio->start(&sourceFile);

    return true;
}

bool AudioOutput::stop()
{
    if(audio == nullptr || audio->state() == QAudio::StoppedState || audio->state() == QAudio::IdleState)
        return false;

    audio->stop();
    sourceFile.close();

    return true;
}

void AudioOutput::handleStateChanged(QAudio::State state)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<state;
}
