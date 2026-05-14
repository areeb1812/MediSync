#include "ConflictsWidget.h"
#include "MainWindow.h"
#include "Medicine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>
#include <QTime>
#include <QDebug>

ConflictsWidget::ConflictsWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    setupUI();
}

void ConflictsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *headerLabel = new QLabel("Drug Conflicts");
    headerLabel->setObjectName("headerLabel");
    headerLayout->addWidget(headerLabel);

    headerLayout->addStretch();

    // Add Button
    QPushButton *addBtn = new QPushButton("➕ Add Conflict");
    connect(addBtn, &QPushButton::clicked, this, &ConflictsWidget::addConflict);
    headerLayout->addWidget(addBtn);

    // Delete Button
    m_deleteBtn = new QPushButton("🗑️ Delete");
    m_deleteBtn->setObjectName("dangerButton");
    m_deleteBtn->setEnabled(false);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ConflictsWidget::deleteConflict);
    headerLayout->addWidget(m_deleteBtn);

    mainLayout->addLayout(headerLayout);

    // Info box
    QLabel *infoLabel = new QLabel(
        "⚠️ <b>Drug Conflicts</b><br><br>"
        "When two medicines that conflict are scheduled at the same time, "
        "you will receive a warning."
        );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("background-color: rgba(255, 193, 7, 0.1); border: 1px solid #ffc107; border-radius: 8px; padding: 15px; color: #ffc107;");
    mainLayout->addWidget(infoLabel);

    // List
    m_conflictsList = new QListWidget();
    m_conflictsList->setStyleSheet(R"(
        QListWidget { background-color: transparent; border: none; font-size: 16px; }
        QListWidget::item { background-color: #0f3460; border-radius: 12px; padding: 20px; margin: 8px 0; border-left: 5px solid #dc3545; }
        QListWidget::item:hover { background-color: #1a4a7a; }
        QListWidget::item:selected { background-color: #2a5a8a; border: 1px solid #e94560; }
    )");

    connect(m_conflictsList, &QListWidget::itemSelectionChanged, this, [this]() {
        m_deleteBtn->setEnabled(!m_conflictsList->selectedItems().isEmpty());
    });

    mainLayout->addWidget(m_conflictsList);
}

void ConflictsWidget::refresh()
{
    m_conflictsList->clear();
    auto conflicts = m_mainWindow->conflictChecker().allConflicts();

    if (conflicts.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("✓ No conflicts defined.");
        item->setTextAlignment(Qt::AlignCenter);
        m_conflictsList->addItem(item);
        m_deleteBtn->setEnabled(false);
        return;
    }

    for (const auto& pair : conflicts) {
        QString text = QString("⚡ %1  ←→  %2").arg(pair.first, pair.second);
        m_conflictsList->addItem(text);
    }
}

void ConflictsWidget::addConflict()
{
    auto medicines = m_mainWindow->database().allMedicines();
    if (medicines.size() < 2) {
        QMessageBox::information(this, "Info", "You need at least 2 medicines to add a conflict.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add Conflict");
    dialog.setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label1 = new QLabel("First Medicine:");
    layout->addWidget(label1);
    QComboBox *combo1 = new QComboBox();
    for (const auto& med : medicines) combo1->addItem(med.name());
    layout->addWidget(combo1);

    QLabel *label2 = new QLabel("Second Medicine:");
    layout->addWidget(label2);
    QComboBox *combo2 = new QComboBox();
    for (const auto& med : medicines) combo2->addItem(med.name());
    if (combo2->count() > 1) combo2->setCurrentIndex(1);
    layout->addWidget(combo2);

    QHBoxLayout *btns = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Check & Save");
    connect(saveBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    btns->addWidget(saveBtn);
    layout->addLayout(btns);

    if (dialog.exec() == QDialog::Accepted) {
        QString med1Name = combo1->currentText();
        QString med2Name = combo2->currentText();

        if (med1Name == med2Name) {
            QMessageBox::warning(this, "Error", "Select different medicines.");
            return;
        }

        if (!m_mainWindow->conflictChecker().hasConflict(med1Name, med2Name)) {
            m_mainWindow->conflictChecker().addConflict(med1Name, med2Name);
            emit m_mainWindow->dataChanged();
        }

        // Force check schedule overlap
        Medicine m1 = m_mainWindow->database().get(med1Name);
        Medicine m2 = m_mainWindow->database().get(med2Name);
        QStringList overlaps;

        for (QString t1 : m1.times()) {
            for (QString t2 : m2.times()) {
                QTime time1 = QTime::fromString(t1.trimmed(), "H:m");
                if (!time1.isValid()) time1 = QTime::fromString(t1.trimmed(), "HH:mm");
                QTime time2 = QTime::fromString(t2.trimmed(), "H:m");
                if (!time2.isValid()) time2 = QTime::fromString(t2.trimmed(), "HH:mm");

                if (time1.isValid() && time2.isValid() && time1 == time2) overlaps.append(t1);
            }
        }

        if (!overlaps.isEmpty()) {
            QMessageBox::warning(this, "⚠️ Dangerous Schedule Detected!",
                                 QString("<b>Warning:</b> Conflict detected between <b>%1</b> and <b>%2</b> at:<br>"
                                         "<h3 style='color:red'>%3</h3>").arg(med1Name, med2Name, overlaps.join(", ")));
        }
    }
}

void ConflictsWidget::deleteConflict()
{
    QList<QListWidgetItem*> selected = m_conflictsList->selectedItems();
    if (selected.isEmpty()) return;

    QString text = selected.first()->text();
    if (!text.contains("⚡")) return;

    QString cleanText = text.mid(2);
    QStringList parts = cleanText.split("  ←→  ");

    if (parts.size() == 2) {
        if (QMessageBox::Yes == QMessageBox::question(this, "Confirm", "Delete this rule?")) {
            m_mainWindow->conflictChecker().removeConflict(parts[0], parts[1]);
            emit m_mainWindow->dataChanged();
        }
    }
}
