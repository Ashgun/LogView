#ifndef LOGFILESSELECTIONWIDGET_H
#define LOGFILESSELECTIONWIDGET_H

#include <QWidget>

#include <QListWidget>
#include <QPushButton>

class LogFilesSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogFilesSelectionWidget(QWidget *parent = nullptr);

    QStringList GetOpenLogFileNames() const;
    QString GetEventPatternConfig() const;

signals:
    void FilesSelected();

private slots:
    void slot_openFilesButton_clicked(bool);

private:
    QListWidget* gui_fileList;
    QPushButton* gui_openFilesButton;

    QStringList m_LogFileNames;
    QString m_EventPatternConfig;
};

#endif // LOGFILESSELECTIONWIDGET_H
