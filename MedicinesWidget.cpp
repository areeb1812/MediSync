#include "MedicinesWidget.h"
#include "MainWindow.h"
#include "MedicineDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>

MedicinesWidget::MedicinesWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    setupUI();
}

void MedicinesWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *headerLabel = new QLabel("Medicines");
    headerLabel->setObjectName("headerLabel");
    headerLayout->addWidget(headerLabel);

    headerLayout->addStretch();

    QPushButton *addBtn = new QPushButton("➕ Add Medicine");
    connect(addBtn, &QPushButton::clicked, this, &MedicinesWidget::addMedicine);
    headerLayout->addWidget(addBtn);

    m_editBtn = new QPushButton("✏️ Edit");
    m_editBtn->setObjectName("secondaryButton");
    m_editBtn->setEnabled(false);
    connect(m_editBtn, &QPushButton::clicked, this, &MedicinesWidget::editMedicine);
    headerLayout->addWidget(m_editBtn);

    m_deleteBtn = new QPushButton("🗑️ Delete");
    m_deleteBtn->setObjectName("dangerButton");
    m_deleteBtn->setEnabled(false);
    connect(m_deleteBtn, &QPushButton::clicked, this, &MedicinesWidget::deleteMedicine);
    headerLayout->addWidget(m_deleteBtn);

    mainLayout->addLayout(headerLayout);

    m_table = new QTableWidget();
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Name", "Dosage", "Times", "Instructions"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setAlternatingRowColors(true);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !m_table->selectedItems().isEmpty();
        m_editBtn->setEnabled(hasSelection);
        m_deleteBtn->setEnabled(hasSelection);
    });

    connect(m_table, &QTableWidget::doubleClicked, this, &MedicinesWidget::editMedicine);

    mainLayout->addWidget(m_table);
}

void MedicinesWidget::refresh()
{
    m_table->setRowCount(0);

    for (const auto& med : m_mainWindow->database().allMedicines()) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(med.name()));
        m_table->setItem(row, 1, new QTableWidgetItem(med.dosage()));
        m_table->setItem(row, 2, new QTableWidgetItem(med.times().join(", ")));
        m_table->setItem(row, 3, new QTableWidgetItem(med.instructions()));

        m_table->setRowHeight(row, 50);
    }

    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
}

void MedicinesWidget::addMedicine()
{
    MedicineDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Medicine med = dialog.medicine();
        m_mainWindow->database().addOrUpdate(med);
        emit m_mainWindow->dataChanged();
    }
}

void MedicinesWidget::editMedicine()
{
    QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return;

    QString name = m_table->item(selected.first()->row(), 0)->text();
    Medicine med = m_mainWindow->database().get(name);

    MedicineDialog dialog(this);
    dialog.setMedicine(med);
    dialog.setWindowTitle("Edit Medicine");

    if (dialog.exec() == QDialog::Accepted) {
        Medicine updatedMed = dialog.medicine();
        if (updatedMed.name() != name) {
            m_mainWindow->database().remove(name);
        }
        m_mainWindow->database().addOrUpdate(updatedMed);
        emit m_mainWindow->dataChanged();
    }
}

void MedicinesWidget::deleteMedicine()
{
    QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) return;

    QString name = m_table->item(selected.first()->row(), 0)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete '%1'?").arg(name),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_mainWindow->database().remove(name);
        emit m_mainWindow->dataChanged();
    }
}
