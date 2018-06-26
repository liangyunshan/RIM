#include "chataudioarea.h"

#include "head.h"
#include "../protocol/datastruct.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QEvent>
#include <QTimer>

#include "widget/rbutton.h"
#include "constants.h"

#define CHAT_AUDIO_MAX_HEIGHT 30

class ChatAudioAreaPrivate : public GlobalData<ChatAudioArea>
{
    Q_DECLARE_PUBLIC(ChatAudioArea)

private:
    explicit ChatAudioAreaPrivate(ChatAudioArea * q):q_ptr(q),isRecording(false),
        recordTimer(nullptr)
    {
        initWidget();
    }

    void initWidget();

    ChatAudioArea * q_ptr;

    QWidget * contentWidget;
    QWidget * audioProgressWidget;

    RButton * sendButton;
    RButton * cancelButton;

    QTimer * recordTimer;

    bool isRecording;
    int recordTime;
};

void ChatAudioAreaPrivate::initWidget()
{
    contentWidget = new QWidget;
    contentWidget->setObjectName("Widget_ContentWidget");

    audioProgressWidget = new QWidget();
    audioProgressWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    sendButton = new RButton(contentWidget);
    sendButton->setText(QObject::tr("Send"));
    QObject::connect(sendButton,SIGNAL(pressed()),q_ptr,SLOT(sendAudio()));

    cancelButton = new RButton(contentWidget);
    cancelButton->setText(QObject::tr("Cancel"));
    QObject::connect(cancelButton,SIGNAL(pressed()),q_ptr,SLOT(cancelSend()));

    QWidget * toolContainer = new QWidget();
    QHBoxLayout * toolLayout = new QHBoxLayout;
    toolLayout->setContentsMargins(0,0,0,0);
    toolLayout->setSpacing(1);
    toolLayout->addWidget(sendButton);
    toolLayout->addWidget(cancelButton);
    toolContainer->setLayout(toolLayout);

    QHBoxLayout * contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(1);
    contentLayout->addWidget(audioProgressWidget);
    contentLayout->addWidget(toolContainer);
    contentWidget->setLayout(contentLayout);

    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(contentWidget);

    q_ptr->setLayout(mainLayout);
    audioProgressWidget->installEventFilter(q_ptr);
}

ChatAudioArea::ChatAudioArea(QWidget *parent) : QWidget(parent),
    d_ptr(new ChatAudioAreaPrivate(this))
{
    setFixedHeight(CHAT_AUDIO_MAX_HEIGHT);
}

ChatAudioArea::~ChatAudioArea()
{
    MQ_D(ChatAudioArea);
    if(d->recordTimer){
        if(d->recordTimer->isActive())
            d->recordTimer->stop();
        delete d->recordTimer;
    }
}

void ChatAudioArea::start()
{
    MQ_D(ChatAudioArea);
    if(!d->recordTimer){
        d->recordTimer = new QTimer;
        d->recordTimer->setInterval(1000);
        connect(d->recordTimer,SIGNAL(timeout()),this,SLOT(updateRecordProgress()));
    }
    d->isRecording = true;
    d->recordTime = 0;
    d->recordTimer->start();
}

bool ChatAudioArea::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(ChatAudioArea);
    if(watched == d->audioProgressWidget){
        if(event->type() == QEvent::Paint){
            if(d->isRecording){
                QPainter painter(d->audioProgressWidget);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QBrush(QColor(152,169,238,180)));

                int width = ((double)d->recordTime/Constant::MAX_RECORD_TIME)*d->audioProgressWidget->rect().width();
                painter.drawRect(QRect(0,0,width,d->audioProgressWidget->height()));

                if(d->recordTime > 0){
                    painter.setPen(Qt::black);
                    painter.drawText(QRect(10,0,d->audioProgressWidget->rect().width()/2,d->audioProgressWidget->rect().height())
                                     ,Qt::AlignVCenter,tr("Recording...%1s").arg(d->recordTime));
                }
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

void ChatAudioArea::sendAudio()
{
    MQ_D(ChatAudioArea);
    emit prepareSendAudio();

    d->recordTimer->stop();
    d->recordTime = 0;
    d->isRecording = false;
    update();
}

void ChatAudioArea::cancelSend()
{
    MQ_D(ChatAudioArea);

    emit preapreCancelAudio();

    d->recordTimer->stop();
    d->recordTime = 0;
    d->isRecording = false;
    update();
}

void ChatAudioArea::updateRecordProgress()
{
    MQ_D(ChatAudioArea);
    if(d->recordTime < Constant::MAX_RECORD_TIME){
        d->recordTime++;
    }else{
        sendAudio();
    }
    update();
}

