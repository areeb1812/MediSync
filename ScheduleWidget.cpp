#include "ScheduleWidget.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QDate>

ScheduleWidget::ScheduleWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), m_mainWindow(mainWindow)
{
    setupUI();
}

void ScheduleWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QLabel *headerLabel = new QLabel("Today's Schedule");
    headerLabel->setObjectName("headerLabel");
    mainLayout->addWidget(headerLabel);

    m_scheduleList = new QListWidget();
    m_scheduleList->setStyleSheet(R"(
        QListWidget { background-color: transparent; border: none; font-size: 16px; }
        QListWidget::item { background-color: #0f3460; border-radius: 12px; padding: 20px; margin: 8px 0; border-left: 5px solid #e94560; }
        QListWidget::item:hover { background-color: #1a4a7a; }
    )");
    mainLayout->addWidget(m_scheduleList);
}

void ScheduleWidget::refresh()
{
    m_scheduleList->clear();
    QString currentTime = QTime::currentTime().toString("HH:mm");
    QString currentDay = QDate::currentDate().toString("dddd");

    QMap<QString, QList<QPair<QString, QString>>> schedule;

    for (const auto& med : m_mainWindow->database().allMedicines()) {
        if (!med.days().isEmpty() && !med.days().contains(currentDay)) continue;

        for (const auto& time : med.times()) {
            schedule[time].append({med.name(), med.dosage()});
        }
    }

    if (schedule.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("📭 No medicines scheduled for " + currentDay);
        item->setTextAlignment(Qt::AlignCenter);
        m_scheduleList->addItem(item);
        return;
    }

    for (auto it = schedule.begin(); it != schedule.end(); ++it) {
        QString time = it.key();
        for (const auto& details : it.value()) {
            QString name = details.first;
            QString dosage = details.second;
            DoseStatus status = m_mainWindow->dailyLog().getStatus(name, time);

            QString icon;
            QString style;
            QString statusText;

            if (status == DoseStatus::Taken) {
                icon = "✅ ";
                style = "color: #28a745; font-weight: bold;";
                statusText = "Taken";
            }
            else if (status == DoseStatus::Missed) {
                icon = "❌ ";
                style = "color: #dc3545; text-decoration: line-through;";
                statusText = "Missed";
            }
            else {
                // Pending
                if (time < currentTime) {
                    icon = "⚠️ ";
                    style = "color: #ffc107;";
                    statusText = "Overdue";
                } else {
                    icon = "⏰ ";
                    style = "color: #e4e6eb;";
                    statusText = "Scheduled";
                }
            }

            QString text = QString("%1 %2 - %3 (%4)").arg(icon, time, name, dosage);

            QListWidgetItem *item = new QListWidgetItem(text);
            if (status == DoseStatus::Taken) item->setForeground(QColor("#28a745"));
            else if (status == DoseStatus::Missed) item->setForeground(QColor("#dc3545"));
            else if (time < currentTime) item->setForeground(QColor("#ffc107"));

            m_scheduleList->addItem(item);
        }
    }
}
