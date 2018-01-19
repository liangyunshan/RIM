#include "rmessagebox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QMouseEvent>
#include <QCoreApplication>
#include "qmath.h"

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "Widgets/toolbar.h"
#include "rlabel.h"
#include "Widgets/widget/rbutton.h"

#include "Widgets/widget.h"

class RMessageBoxPrivate : public GlobalData<RMessageBox>
{
    Q_DECLARE_PUBLIC(RMessageBox)
private:
    RMessageBoxPrivate(RMessageBox * q):q_ptr(q)
    {
        initWidget();
        clickButton = RMessageBox::Cancel;
    }

    ~RMessageBoxPrivate()
    {
        foreach(RButton * tmp,buttList)
        {
            delete tmp;
        }
        buttList.clear();
    }

    RMessageBox * q_ptr;

    void initWidget();
    void setIcon(RMessageBox::Icon type);
    void updateButtLayout();

    RIconLabel * iconLabel;
    QLabel * contentLabel;

    ToolBar * toolBar;
    QWidget * bottomWidget;

    QPoint  mousePressPoint;

    RMessageBox::Icon mesType;

    QList<RButton *> buttList;
    QHash<RButton *,RMessageBox::StandardButton> buttHash;
    RMessageBox::StandardButton clickButton;
};

void RMessageBoxPrivate::initWidget()
{
    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    toolBar = new ToolBar();
    toolBar->setObjectName("Widget_ToolBarWidget");
    toolBar->setFixedHeight(ABSTRACT_TOOL_BAR_HEGIHT);
    toolBar->setToolFlags(ToolBar::TOOL_MESSAGEBOX);
    toolBar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM_16));
    toolBar->setWindowTitle(QObject::tr("Information"));

    QObject::connect(toolBar,SIGNAL(closeWindow()),q_ptr,SLOT(close()));

    iconLabel = new RIconLabel();
    iconLabel->setCorner(false);
    iconLabel->setEnterCursorChanged(false);
    iconLabel->setTransparency(true);
    iconLabel->setFixedSize(40,40);

    contentLabel = new QLabel();
    contentLabel->setWordWrap(true);
    contentLabel->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    contentLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QWidget * contentWidget = new QWidget();
    contentWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QHBoxLayout * contentLayout = new QHBoxLayout;

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(contentLabel);

    contentWidget->setLayout(contentLayout);

    bottomWidget = new QWidget;
    bottomWidget->setFixedHeight(35);
    bottomWidget->setObjectName("Panel_Bottom_ContentWidget");
    bottomWidget->setMinimumSize(100,30);

    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(contentWidget);
    mainLayout->addWidget(bottomWidget);

    QWidget * mainWidget = new QWidget();
    mainWidget->setObjectName("AbstractWidget_ContentWidget");
    mainWidget->setLayout(mainLayout);

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setContentsMargins(WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE);
    layout->setSpacing(0);
    layout->addWidget(mainWidget);

    q_ptr->setLayout(layout);
}

void RMessageBoxPrivate::setIcon(RMessageBox::Icon type)
{
    mesType = type;

    switch(type)
    {
        case RMessageBox::Information :
                                        iconLabel->setPixmap(":/icon/resource/icon/alert_info.png");
                                        break;
        case RMessageBox::Warning :
                                        iconLabel->setPixmap(":/icon/resource/icon/alert_warning.png");
                                        break;

        case RMessageBox::Critical :
                                        iconLabel->setPixmap(":/icon/resource/icon/aler_error.png");
                                        break;
        default:
            break;
    }
}

void RMessageBoxPrivate::updateButtLayout()
{
    if(!bottomWidget->layout())
    {
        QHBoxLayout * layout = new QHBoxLayout;
        layout->setContentsMargins(6,1,1,1);
        bottomWidget->setLayout(layout);
    }

    QHBoxLayout * layout = dynamic_cast<QHBoxLayout *>(bottomWidget->layout());

    int i = layout->count() - 1;

    while(i >= 0)
    {
        QLayoutItem * item = layout->takeAt(i);
        if(QWidget *widget = item->widget())
        {
            widget->hide();
        }
        delete item;
        i--;
    }

    for(int j = 0; j < buttList.size(); j++)
    {
        if(j == 0)
        {
            layout->addStretch(1);
        }
        buttList.at(j)->show();
        layout->addWidget(buttList.at(j));
    }
}

#define MESSAGE_BOX_WIDTH 370
#define MESSAGE_BOX_HEIGHT 200

RMessageBox::RMessageBox(QWidget *parent):
    d_ptr(new RMessageBoxPrivate(this)),
    QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    QSize size = RUtil::screenSize();
    setGeometry((size.width() - MESSAGE_BOX_WIDTH) /2,(size.height() - MESSAGE_BOX_HEIGHT)/2,MESSAGE_BOX_WIDTH,MESSAGE_BOX_HEIGHT);
}

RMessageBox::~RMessageBox()
{
    delete d_ptr;
}

void RMessageBox::setWindowTitle(const QString &text)
{
    MQ_D(RMessageBox);
    d->toolBar->setWindowTitle(text);
}

QString RMessageBox::windowTitle() const
{
    MQ_D(RMessageBox);
    return d->toolBar->windowTitle();
}

void RMessageBox::setText(const QString &text)
{
    MQ_D(RMessageBox);
    d->contentLabel->setText(text);
}

QString RMessageBox::text() const
{
    MQ_D(RMessageBox);
    return d->contentLabel->text();
}

void RMessageBox::setIcon(RMessageBox::Icon icon)
{
    MQ_D(RMessageBox);
    d->setIcon(icon);
}

int RMessageBox::information(QWidget *parent, const QString &title, const QString &text, int butts, RMessageBox::StandardButton)
{
    return messagebox(parent,Information,title,text,butts);
}

int RMessageBox::warning(QWidget *parent, const QString &title, const QString &text, int butts, RMessageBox::StandardButton)
{
    return messagebox(parent,Warning,title,text,butts);
}

RMessageBox::StandardButton RMessageBox::clickedButton() const
{
    MQ_D(RMessageBox);
    return d->clickButton;
}

void RMessageBox::mousePressEvent(QMouseEvent * event)
{
    MQ_D(RMessageBox);
    d->mousePressPoint = event->pos();
}

void RMessageBox::mouseMoveEvent(QMouseEvent * event)
{
    MQ_D(RMessageBox);
    move(pos() + event->pos() - d->mousePressPoint);
}

void RMessageBox::paintEvent(QPaintEvent *)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(WINDOW_MARGIN_WIDTH, WINDOW_MARGIN_WIDTH, this->width()-WINDOW_MARGIN_WIDTH * 2, this->height()- WINDOW_MARGIN_WIDTH * 2);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i = 0; i< WINDOW_MARGIN_WIDTH; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(WINDOW_MARGIN_WIDTH-i, WINDOW_MARGIN_WIDTH-i, this->width()-(WINDOW_MARGIN_WIDTH-i)*2, this->height()-(WINDOW_MARGIN_WIDTH-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

void RMessageBox::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void RMessageBox::respButtonClicked()
{
    MQ_D(RMessageBox);
    RButton * button = dynamic_cast<RButton *>(QObject::sender());
    if(button)
    {
        d->clickButton =  d->buttHash.value(button);
        close();
    }
}

RButton *RMessageBox::addButton(RMessageBox::StandardButton butt)
{
    MQ_D(RMessageBox);
    RButton * button = new RButton(d->bottomWidget);
    button->setText(standardButtText(butt));
    connect(button,SIGNAL(pressed()),this,SLOT(respButtonClicked()));

    d->buttHash.insert(button,butt);
    d->buttList.append(button);
    d->updateButtLayout();

    return button;
}

QString RMessageBox::standardButtText(RMessageBox::StandardButton butt)
{
    if(butt)
    {
        switch(butt)
        {
            case RMessageBox::Ok:
                return QCoreApplication::translate("QPlatformTheme", "OK");
            case RMessageBox::Save:
                return QCoreApplication::translate("QPlatformTheme", "Save");
            case RMessageBox::SaveAll:
                return QCoreApplication::translate("QPlatformTheme", "SaveAll");
            case RMessageBox::Open:
                return QCoreApplication::translate("QPlatformTheme", "Open");
            case RMessageBox::Yes:
                return QCoreApplication::translate("QPlatformTheme", "Yes");
            case RMessageBox::YesToAll:
                return QCoreApplication::translate("QPlatformTheme", "YesToAll");
            case RMessageBox::No:
                return QCoreApplication::translate("QPlatformTheme", "No");
            case RMessageBox::NoToAll:
                return QCoreApplication::translate("QPlatformTheme", "NoToAll");
            case RMessageBox::Abort:
                return QCoreApplication::translate("QPlatformTheme", "Abort");
            case RMessageBox::Retry:
                return QCoreApplication::translate("QPlatformTheme", "Retry");
            case RMessageBox::Ignore:
                return QCoreApplication::translate("QPlatformTheme", "Ignore");
            case RMessageBox::Close:
                return QCoreApplication::translate("QPlatformTheme", "Close");
            case RMessageBox::Cancel:
                return QCoreApplication::translate("QPlatformTheme", "Cancel");
            case RMessageBox::Discard:
                return QCoreApplication::translate("QPlatformTheme", "Discard");
            case RMessageBox::Help:
                return QCoreApplication::translate("QPlatformTheme", "Help");
            case RMessageBox::Apply:
                return QCoreApplication::translate("QPlatformTheme", "Apply");
            case RMessageBox::Reset:
                return QCoreApplication::translate("QPlatformTheme", "Reset");
            case RMessageBox::RestoreDefaults:
                return QCoreApplication::translate("QPlatformTheme", "RestoreDefaults");
            default:
                break;
        }
    }
    return QString();
}

int RMessageBox::messagebox(QWidget *parent, RMessageBox::Icon type, const QString &title, const QString &text, int butts,StandardButton)
{
    RMessageBox msgBox(parent);

    StandardButtons buttons(butts);

    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setIcon(type);

    uint mask = RMessageBox::FirstButton;
    while (mask <= RMessageBox::LastButton)
    {
        uint sb = buttons & mask;
        mask <<= 1;
        if (!sb)
            continue;
        msgBox.addButton((RMessageBox::StandardButton)sb);
    }

    if(msgBox.exec() == -1)
    {
        return RMessageBox::Cancel;
    }

    return  msgBox.clickedButton();
}
