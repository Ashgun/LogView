#ifndef LOGLINEHEADERPARSINGPARAMSEDITWIDGET_H
#define LOGLINEHEADERPARSINGPARAMSEDITWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>

#include "Events.h"

class LogLineHeaderParsingParamsEditWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogLineHeaderParsingParamsEditWidget(QWidget *parent = nullptr);
    LogLineHeaderParsingParamsEditWidget(const LogLineHeaderParsingParams& params, QWidget *parent = nullptr);

    void SetParams(const LogLineHeaderParsingParams& params);
    LogLineHeaderParsingParams GetParams() const;

signals:

public slots:
    void slot_paramsTable_itemChanged(QTableWidgetItem *item);

private:
    QTableWidget* gui_paramsTable;
    QComboBox* gui_groupingHeaderCombo;
    QComboBox* gui_sortingHeaderCombo;
};

#endif // LOGLINEHEADERPARSINGPARAMSEDITWIDGET_H
