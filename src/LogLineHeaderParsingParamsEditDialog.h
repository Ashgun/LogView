#ifndef LOGLINEHEADERPARSINGPARAMSEDITDIALOG_H
#define LOGLINEHEADERPARSINGPARAMSEDITDIALOG_H

#include "LogLineHeaderParsingParamsEditWidget.h"

#include <QDialog>

class LogLineHeaderParsingParamsEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LogLineHeaderParsingParamsEditDialog(QWidget *parent = nullptr);

signals:

protected slots:
    void slot_accepted();
    void slot_rejected();
    void slot_open();
    void slot_save();

private:
    void AcceptState();

private:
    LogLineHeaderParsingParamsEditWidget* gui_paramsEditWidget;

    QString m_openedFileName;
};

#endif // LOGLINEHEADERPARSINGPARAMSEDITDIALOG_H
