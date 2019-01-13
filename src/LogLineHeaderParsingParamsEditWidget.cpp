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

    QStringList headers;
    headers << tr("Header") << tr("RegExp") << tr("Delimiter");
    gui_paramsTable = new QTableWidget(20, headers.size());
    gui_paramsTable->setHorizontalHeaderLabels(headers);

    QVBoxLayout* centralLayout = new QVBoxLayout();
    centralLayout->addWidget(new QLabel(tr("Headers with regexps:")));
    centralLayout->addWidget(gui_paramsTable);
    centralLayout->addLayout(groupingElementLayout);

    setLayout(centralLayout);

    connect(gui_paramsTable, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(slot_paramsTable_itemChanged(QTableWidgetItem*)), Qt::DirectConnection);

    SetParams(params);
}

void LogLineHeaderParsingParamsEditWidget::SetParams(const LogLineHeaderParsingParams& params)
{
    gui_paramsTable->blockSignals(true);

    QSet<QString> items;

    for (int i = 0; i < params.HeaderGroupDatas.size(); ++i)
    {
        gui_paramsTable->setItem(i, 0, new QTableWidgetItem(params.HeaderGroupDatas.at(i).Name));
        gui_paramsTable->setItem(i, 1, new QTableWidgetItem(params.HeaderGroupDatas.at(i).RegExp));
        gui_paramsTable->setItem(i, 2, new QTableWidgetItem(params.HeaderGroupDatas.at(i).Delimiter));

        items.insert(params.HeaderGroupDatas.at(i).Name);
    }

    gui_groupingHeaderCombo->clear();
    gui_groupingHeaderCombo->addItems(QStringList(items.toList()));

    gui_sortingHeaderCombo->clear();
    gui_sortingHeaderCombo->addItems(QStringList(items.toList()));

    if (!params.GroupNameForGrouping.isEmpty())
    {
        gui_groupingHeaderCombo->setCurrentText(params.GroupNameForGrouping);
    }

    if (!params.SortingGroup.isEmpty())
    {
        gui_sortingHeaderCombo->setCurrentText(params.SortingGroup);
    }

    gui_paramsTable->blockSignals(false);
}

LogLineHeaderParsingParams LogLineHeaderParsingParamsEditWidget::GetParams() const
{
    LogLineHeaderParsingParams result;

    for (int i = 0; i < gui_paramsTable->rowCount(); ++i)
    {
        if (gui_paramsTable->item(i, 0) == nullptr ||
            gui_paramsTable->item(i, 1) == nullptr)
        {
            continue;
        }

        const QString header = gui_paramsTable->item(i, 0)->text();
        const QString regExp = gui_paramsTable->item(i, 1)->text();
        const QString delimiter =
                (gui_paramsTable->item(i, 2) == nullptr || gui_paramsTable->item(i, 2)->text().isEmpty()) ?
                    QString("\\s*") : gui_paramsTable->item(i, 2)->text();

        if (header.isEmpty() || regExp.isEmpty())
        {
            continue;
        }

        result.HeaderGroupDatas.append(LogLineHeaderParsingParams::GroupData({header, regExp, delimiter}));
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
        if (gui_paramsTable->item(i, 0) == nullptr)
        {
            continue;
        }

        const QString header = gui_paramsTable->item(i, 0)->text();

        if (header.isEmpty())
        {
            continue;
        }

        items.insert(header);
    }

    gui_groupingHeaderCombo->clear();
    gui_groupingHeaderCombo->addItems(QStringList(items.toList()));
    if (item != nullptr)
    {
        gui_groupingHeaderCombo->setCurrentText(item->text());
    }

    gui_sortingHeaderCombo->clear();
    gui_sortingHeaderCombo->addItems(QStringList(items.toList()));
    if (item != nullptr)
    {
        gui_sortingHeaderCombo->setCurrentText(item->text());
    }
}
