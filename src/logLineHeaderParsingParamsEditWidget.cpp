#include "LogLineHeaderParsingParamsEditWidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

LogLineHeaderParsingParamsEditWidget::LogLineHeaderParsingParamsEditWidget(QWidget *parent) :
    LogLineHeaderParsingParamsEditWidget(LogLineHeaderParsingParams(), parent)
{    
}

LogLineHeaderParsingParamsEditWidget::LogLineHeaderParsingParamsEditWidget(const LogLineHeaderParsingParams& params, QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* groupingElementLayout = new QHBoxLayout();
    gui_groupingHeaderCombo = new QComboBox();
    groupingElementLayout->addWidget(new QLabel(tr("Header for grouping:")));
    groupingElementLayout->addWidget(gui_groupingHeaderCombo);

    gui_sortingHeaderCombo = new QComboBox();
    groupingElementLayout->addWidget(new QLabel(tr("Header for sorting:")));
    groupingElementLayout->addWidget(gui_sortingHeaderCombo);

    gui_paramsTable = new QTableWidget(10, 2);
    gui_paramsTable->setHorizontalHeaderLabels(QStringList() << tr("Header") << tr("RegExp"));

    QVBoxLayout* centralLayout = new QVBoxLayout();
    centralLayout->addWidget(new QLabel(tr("Headers with regexps:")));
    centralLayout->addWidget(gui_paramsTable);
    centralLayout->addLayout(groupingElementLayout);

    setLayout(centralLayout);

    for (int i = 0; i < params.HeaderGroupRegExps.size(); ++i)
    {
        gui_paramsTable->setItem(i, 0, new QTableWidgetItem(params.HeaderGroupRegExps.at(i).first));
        gui_paramsTable->setItem(i, 1, new QTableWidgetItem(params.HeaderGroupRegExps.at(i).second));
    }

    if (!params.GroupNameForGrouping.isEmpty())
    {
        gui_groupingHeaderCombo->setCurrentText(params.GroupNameForGrouping);
    }

    if (!params.SortingGroup.isEmpty())
    {
        gui_sortingHeaderCombo->setCurrentText(params.SortingGroup);
    }

    connect(gui_paramsTable, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(slot_paramsTable_itemChanged(QTableWidgetItem*)), Qt::DirectConnection);
}

LogLineHeaderParsingParams LogLineHeaderParsingParamsEditWidget::GetParams() const
{
    LogLineHeaderParsingParams result;

    for (int i = 0; i < gui_paramsTable->rowCount(); ++i)
    {
        const QString header = gui_paramsTable->item(i, 0)->text();
        const QString regExp = gui_paramsTable->item(i, 1)->text();

        if (header.isEmpty() || regExp.isEmpty())
        {
            continue;
        }

        result.HeaderGroupRegExps.append(QPair<QString, QString>(header, regExp + "\\s*"));
    }
    result.GroupNameForGrouping = gui_groupingHeaderCombo->currentText();
    result.SortingGroup = gui_sortingHeaderCombo->currentText();

    return result;
}

void LogLineHeaderParsingParamsEditWidget::slot_paramsTable_itemChanged(QTableWidgetItem* item)
{
    QSet<QString> items;

    for (int i = 0; i < gui_paramsTable->rowCount(); ++i)
    {
        if (gui_paramsTable->item(i, 0) != nullptr)
        {
            continue;
        }

        const QString header = gui_paramsTable->item(i, 0)->text();
        items.insert(header);
    }

    gui_groupingHeaderCombo->clear();
    gui_groupingHeaderCombo->addItems(QStringList(items.toList()));

    gui_groupingHeaderCombo->setCurrentText(item->text());

    gui_sortingHeaderCombo->clear();
    gui_sortingHeaderCombo->addItems(QStringList(items.toList()));

    gui_sortingHeaderCombo->setCurrentText(item->text());
}
