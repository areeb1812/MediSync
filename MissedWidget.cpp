#include "MissedWidget.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

MissedWidget::MissedWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    setupUI();
}

void MissedWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();

    QLabel *headerLabel = new QLabel("Missed Doses Log");
    headerLabel->setObjectName("headerLabel");
    headerLayout->addWidget(headerLabel);

    headerLayout->addStretch();

    QPushButton *clearBtn = new QPushButton("🗑️ Clear Log");
    clearBtn->setObjectName("dangerButton");
    connect(clearBtn, &QPushButton::clicked, this, &MissedWidget::clearLog);
    headerLayout->addWidget(clearBtn);

    mainLayout->addLayout(headerLayout);

    // Missed list
    m_missedList = new QListWidget();
    m_missedList->setStyleSheet(R"(
        QListWidget {
            background-color: transparent;
            border: none;
            font-size: 16px;
        }
        QListWidget::item {
            background-color: #0f3460;
            border-radius: 12px;
            padding: 20px;
            margin: 8px 0;
            border-left: 5px solid #dc3545;
        }
        QListWidget::item:hover {
            background-color: #1a4a7a;
        }
    )");

    mainLayout->addWidget(m_missedList);
}

void MissedWidget::refresh()
{
    m_missedList->clear();

    auto missed = m_mainWindow->missedLog().allMissedDoses();

    if (missed.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem(
            "✓ No missed doses!\n\n"
            "Great job staying on track with your medication schedule."
            );
        item->setTextAlignment(Qt::AlignCenter);
        item->setForeground(QColor("#28a745"));
        m_missedList->addItem(item);
        return;
    }

    // Show most recent first (iterate backwards using int)
    for (int i = missed.size() - 1; i >= 0; --i) {
        const auto& dose = missed[i];
        QString text = QString("❌ %1\n⏰ Scheduled: %2\n📅 %3")
                           .arg(dose.medicineName)
                           .arg(dose.scheduledTime)
                           .arg(dose.missedAt.toString("yyyy-MM-dd hh:mm"));
        m_missedList->addItem(text);
    }
}

void MissedWidget::clearLog()
{
    if (m_mainWindow->missedLog().count() == 0) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Clear",
        "Are you sure you want to clear the missed doses log?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_mainWindow->missedLog().clear();
        emit m_mainWindow->dataChanged();
    }
}
