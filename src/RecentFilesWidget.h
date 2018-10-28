#ifndef RECENTFILESWIDGET_H
#define RECENTFILESWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>

class RecentFilesWidget : public QWidget
{
    Q_OBJECT
public:
    RecentFilesWidget(
            const QString& filesGroupInConfig,
            const QString& configData,
            QWidget *parent = nullptr);
    ~RecentFilesWidget() override;

    QString GetSelectedFile() const;

signals:

private slots:
    void slot_openFileButton_clicked(bool);
    void slot_recentFilesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    void FillFromConfig();

private:
    const QString m_filesGroupInConfig;
    const QString m_configData;

    QListWidget* gui_recentFilesListWidget;
    QLineEdit* gui_selectedFileEdit;
    QPushButton* gui_openFileButton;
};

#endif // RECENTFILESWIDGET_H
