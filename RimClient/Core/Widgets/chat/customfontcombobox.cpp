#include "customfontcombobox.h"

#include "../../head.h"
#include "../../constants.h"
#include "../../datastruct.h"
#include "../../../Util/rutil.h"
#include "customfontfamilydelegate.h"

#include <QDir>
#include <QEvent>
#include <QStyle>
#include <QListView>
#include <QApplication>
#include <QFontDatabase>
#include <QItemDelegate>
#include <QFileInfoList>
#include <QStringlistmodel>
#include <QFileSystemWatcher>

#include <QDebug>

class CustomFontComboBoxPrivate : public GlobalData<CustomFontComboBox>
{
    Q_DECLARE_PUBLIC(CustomFontComboBox)

public:
    inline CustomFontComboBoxPrivate(CustomFontComboBox *q):q_ptr(q)
    {
        filters = CustomFontComboBox::AllFonts;
        fontsPath = defaultFontPath();
        RUtil::createDir(fontsPath);
        watcher = new QFileSystemWatcher;
        watcher->addPath(fontsPath);
        QObject::connect(watcher,SIGNAL(directoryChanged(QString)),q_ptr,SLOT(fontDirChanged(QString)));
    }

    CustomFontComboBox::FontFilters filters;
    QFont currentFont;
    QString fontsPath;
    QList <int> fontIDs;
    QFileSystemWatcher *watcher;    //文件监视器

    void setFontsPath(const QString &);
    void updateModel();
    void currentChanged(const QString &);
    QString defaultFontPath() const;

    CustomFontComboBox * q_ptr;
};

/*!
 * @brief CustomFontComboBoxPrivate::setFontsPath 设置自定义字体文件检索路径
 * @param path 自定义字体文件检索路径
 */
void CustomFontComboBoxPrivate::setFontsPath(const QString &path)
{
    if(!path.isEmpty() && path != fontsPath)
    {
        watcher->removePath(fontsPath);
        fontsPath = path;
        RUtil::createDir(fontsPath);
        watcher->addPath(fontsPath);
    }
}

/*!
 * @brief CustomFontComboBoxPrivate::updateModel 更新下拉框内容
 */
void CustomFontComboBoxPrivate::updateModel()
{
    const int scalableMask = (CustomFontComboBox::ScalableFonts | CustomFontComboBox::NonScalableFonts);
    const int spacingMask = (CustomFontComboBox::ProportionalFonts | CustomFontComboBox::MonospacedFonts);

    QStringListModel *m = qobject_cast<QStringListModel *>(q_ptr->model());
    if (!m)
        return;
    QFontDatabase fdb;
    fdb.removeAllApplicationFonts();

    fontIDs.clear();
    QDir fontDir(fontsPath);
    QStringList m_filters;
    m_filters << "*.ttf" << "*.TTF";
    QFileInfoList files = fontDir.entryInfoList(m_filters,QDir::Files|QDir::Readable,QDir::Name);
    for(int i=0; i<files.count(); i++)
    {
        QFile fontFile(files.at(i).absoluteFilePath());
        if(fontFile.open(QIODevice::ReadOnly))
        {
            int tempID = fdb.addApplicationFontFromData(fontFile.readAll());
            fontIDs.append(tempID);
        }
        fontFile.close();
    }

    //只显示自定义字体
    QStringList list;
    for(int index=0;index<fontIDs.count();index++)
    {
        QStringList t_families = QFontDatabase::applicationFontFamilies(fontIDs.at(index));
        for(int i=0;i<t_families.count();i++)
        {
            list.append(t_families.at(i));
        }
    }

    QStringList result;
    int offset = 0;
    QFontInfo fi(currentFont);

    for (int i = 0; i < list.size(); ++i)
    {
        if ((filters & scalableMask) && (filters & scalableMask) != scalableMask)
        {
            if (bool(filters & CustomFontComboBox::ScalableFonts) != fdb.isSmoothlyScalable(list.at(i)))
                continue;
        }
        if ((filters & spacingMask) && (filters & spacingMask) != spacingMask)
        {
            if (bool(filters & CustomFontComboBox::MonospacedFonts) != fdb.isFixedPitch(list.at(i)))
                continue;
        }
        result += list.at(i);
        if (list.at(i) == fi.family() || list.at(i).startsWith(fi.family() + QLatin1String(" [")))
            offset = result.count() - 1;
    }
    list = result;

    const bool old = m->blockSignals(true);
    m->setStringList(list);
    m->blockSignals(old);


    if (list.isEmpty())
    {
        if (currentFont != QFont())
        {
            currentFont = QFont();
            emit q_ptr->currentFontChanged(currentFont);
        }
    }
    else
    {
        q_ptr->setCurrentIndex(offset);
    }
}

/*!
 * @brief CustomFontComboBoxPrivate::currentChanged 选择字体变化
 * @param text 字体的family
 */
void CustomFontComboBoxPrivate::currentChanged(const QString &text)
{
    if (currentFont.family() != text)
    {
        currentFont.setFamily(text);
        emit q_ptr->currentFontChanged(currentFont);
    }
}

/*!
 * @brief CustomFontComboBoxPrivate::defaultFontPath 返回默认自定义字体文件检索路径
 * @return 默认自定义字体文件检索路径
 */
QString CustomFontComboBoxPrivate::defaultFontPath() const
{
    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);
    QString t_fontPath = configFullPath + QString(Constant::CONFIG_FontsPath);
    return t_fontPath;
}


/*!
 * @class CustomFontComboBox
 * @brief 获取本地配置文件字体路径下可用字体作为选项，提供使用
 * @param parent
 */
CustomFontComboBox::CustomFontComboBox(QWidget *parent) : QComboBox(parent),
        d_ptr(new CustomFontComboBoxPrivate(this))
{
    Q_D(CustomFontComboBox);

    d->currentFont = font();
    setEditable(true);

    QStringListModel *m = new QStringListModel(this);
    setModel(m);
    setItemDelegate(new CustomFontFamilyDelegate(this));
    QListView *lview = qobject_cast<QListView*>(view());
    if (lview)
        lview->setUniformItemSizes(true);
    setWritingSystem(QFontDatabase::Any);
    connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(fontChaged(QString)));
}

CustomFontComboBox::~CustomFontComboBox()
{

}

void CustomFontComboBox::setWritingSystem(QFontDatabase::WritingSystem script)
{
    Q_D(CustomFontComboBox);

    CustomFontFamilyDelegate *delegate = qobject_cast<CustomFontFamilyDelegate *>(view()->itemDelegate());
    if (delegate)
        delegate->writingSystem = script;
    d->updateModel();
}

QFontDatabase::WritingSystem CustomFontComboBox::writingSystem() const
{
    CustomFontFamilyDelegate *delegate = qobject_cast<CustomFontFamilyDelegate *>(view()->itemDelegate());
    if (delegate)
        return delegate->writingSystem;
    return QFontDatabase::Any;
}

void CustomFontComboBox::setFontFilters(FontFilters filters)
{
    Q_D(CustomFontComboBox);

    d->filters = filters;
    d->updateModel();
}

CustomFontComboBox::FontFilters CustomFontComboBox::fontFilters() const
{
    Q_D(const CustomFontComboBox);

    return d->filters;
}

/*!
 * @brief CustomFontComboBox::currentFont 返回当前字体
 * @return  当前选择的字体
 */
QFont CustomFontComboBox::currentFont() const
{
    Q_D(const CustomFontComboBox);

    return d->currentFont;
}

void CustomFontComboBox::setCurrentFont(const QFont &font)
{
    Q_D(CustomFontComboBox);

    if (font != d->currentFont)
    {
        d->currentFont = font;
        d->updateModel();
        if (d->currentFont == font)
        {
            emit currentFontChanged(d->currentFont);
        }
    }
}

void CustomFontComboBox::fontChaged(const QString &text)
{
    Q_D(CustomFontComboBox);

    d->currentChanged(text);
}

void CustomFontComboBox::fontDirChanged(const QString &)
{
    Q_D(CustomFontComboBox);

    d->updateModel();
}

/*!
 * @brief CustomFontComboBox::sizeHint 获取窗口尺寸
 * @return 窗口尺寸
 */
QSize CustomFontComboBox::sizeHint() const
{
    QSize sz = QComboBox::sizeHint();
    QFontMetrics fm(font());
    sz.setWidth(fm.width(QLatin1Char('m'))*14);
    return sz;
}

/*!
 * @brief CustomFontComboBox::setFontsFilePath 设置自定义字体文件检索路径
 * @param path 自定义字体文件检索路径
 */
void CustomFontComboBox::setFontsFilePath(const QString &path)
{
    Q_D(CustomFontComboBox);

    d->setFontsPath(path);
    d->updateModel();
}

/*!
 * @brief CustomFontComboBox::fontsFilePath 返回当前自定义字体文件检索路径
 * @return 自定义字体文件检索路径
 */
QString CustomFontComboBox::fontsFilePath() const
{
    return d_ptr->fontsPath;
}

bool CustomFontComboBox::event(QEvent *e)
{
    if (e->type() == QEvent::Resize)
    {
        QListView *lview = qobject_cast<QListView*>(view());
        if (lview)
            lview->window()->setFixedWidth(width() * 5 / 4);
    }
    return QComboBox::event(e);
}
