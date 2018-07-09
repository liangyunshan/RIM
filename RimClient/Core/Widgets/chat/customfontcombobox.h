/*!
 *  @brief     自定义字体选择下拉框
 *  @details   显示某个路径下已有字体文件中可用字体
 *  @file      customfontcombobox.h
 *  @author    LYS
 *  @version   1.0.2
 *  @date      2018.06.12
 *  @warning   只加载设置的路径下的字体文件
 *  @copyright NanJing RenGu.
 *  @note
 *      1.20180612-LYS 修改字体文件路径后，移除之前路径下已经添加的字体。
 *      2.20180619-LYS 未设置自定义文件路径则使用默认路径。
 *      3.20180619-LYS 监控自定义字体文件夹，该文件夹中内容变化则及时更新数据。
 *
 */
#ifndef CUSTOMFONTCOMBOBOX_H
#define CUSTOMFONTCOMBOBOX_H

#include <QComboBox>
#include <QFontDatabase>

class CustomFontComboBoxPrivate;

class CustomFontComboBox : public QComboBox
{
    Q_OBJECT

public:
   explicit CustomFontComboBox(QWidget *parent = 0);
    ~CustomFontComboBox();

    void setWritingSystem(QFontDatabase::WritingSystem);
    QFontDatabase::WritingSystem writingSystem() const;

    enum FontFilter {
        AllFonts = 0,
        ScalableFonts = 0x1,
        NonScalableFonts = 0x2,
        MonospacedFonts = 0x4,
        ProportionalFonts = 0x8
    };
    Q_DECLARE_FLAGS(FontFilters, FontFilter)

    void setFontFilters(FontFilters filters);
    FontFilters fontFilters() const;

    QFont currentFont() const;
    QSize sizeHint() const;

    void setFontsFilePath(const QString &path = QString());
    QString fontsFilePath() const;

signals:
    void currentFontChanged(const QFont &font);

protected:
    bool event(QEvent *e);

public slots:
    void setCurrentFont(const QFont &font);

private slots:
    void fontChaged(const QString &);
    void fontDirChanged(const QString &);

private:
    Q_DECLARE_PRIVATE(CustomFontComboBox)
    CustomFontComboBoxPrivate *d_ptr;
};

#endif // CUSTOMFONTCOMBOBOX_H
