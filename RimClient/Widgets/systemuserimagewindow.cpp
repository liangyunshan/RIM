#include "systemuserimagewindow.h"

#include <QGridLayout>
#include <QFileInfo>
#include <QScrollArea>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "Util/rsingleton.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "widget/riconlabel.h"

class SystemUserImageWindowPrivate : public GlobalData<SystemUserImageWindow>
{
    Q_DECLARE_PUBLIC(SystemUserImageWindow)

private:
    SystemUserImageWindowPrivate(SystemUserImageWindow * q):q_ptr(q)
    {
        initWidget();
    }

    SystemUserImageWindow * q_ptr;

    QScrollArea * scrollArea;
    QWidget * contentWidget;

    void initWidget();

};

void SystemUserImageWindowPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Widget_ContentWidget");

    QGridLayout * layout = new QGridLayout;

    scrollArea = new QScrollArea(q_ptr);

    QFileInfoList icons = RSingleton<ImageManager>::instance()->systemIcons();

    for(int i = 0; i < icons.size(); i++)
    {
        RIconLabel * label = new RIconLabel(contentWidget);
        label->setCorner(false);
        label->resizeByPixmap(true);
        label->setEnterHighlight(true);
        label->setTransparency(true);
        label->setPixmap(icons.at(i).absoluteFilePath());

        QObject::connect(label,SIGNAL(mousePressed()),q_ptr,SLOT(processImagePresses()));

        layout->addWidget(label,i/5,i%5,1,1);
    }

    layout->setRowStretch(icons.size()/5+1,1);

    contentWidget->setLayout(layout);

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    q_ptr->setContentWidget(scrollArea);
}


SystemUserImageWindow::SystemUserImageWindow(QWidget *parent):
    d_ptr(new SystemUserImageWindowPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Local Image"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    const int width = 500;
    const int height = 600;

    ToolBar * bar = enableToolBar(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM_16));

        bar->setWindowTitle(tr("Local Image"));
    }

    setFixedSize(width,height);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - width)/2,(screenSize.height() - height)/2,width,height);
}

SystemUserImageWindow::~SystemUserImageWindow()
{

}

void SystemUserImageWindow::processImagePresses()
{
    RIconLabel * label = dynamic_cast<RIconLabel *>(QObject::sender());
    if(label)
    {
        emit selectedFileBaseName(label->getPixmapFileInfo().fileName());
    }
}
