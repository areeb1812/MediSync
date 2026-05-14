#include "DashboardWidget.h"
#include "MainWindow.h"
#include <QHBoxLayout>
#include <QFrame>
#include <QTime>
#include <QDate>

DashboardWidget::DashboardWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent), m_mainWindow(mainWindow)
{
    setupUI();
}

void DashboardWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    QLabel *headerLabel = new QLabel("Dashboard");
    headerLabel->setObjectName("headerLabel");
    mainLayout->addWidget(headerLabel);

    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("dddd, MMMM d, yyyy"));
    dateLabel->setStyleSheet("color: #a0a0a0; font-size: 16px; margin-bottom: 20px;");
    mainLayout->addWidget(dateLabel);

    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    QFrame *medicinesCard = createStatCard("💊", "0", "Total Medicines", "#4a90d9");
    m_medicinesValue = medicinesCard->findChild<QLabel*>("statValue");
    statsLayout->addWidget(medicinesCard);

    QFrame *dosesCard = createStatCard("📅", "0", "Today's Doses", "#28a745");
    m_dosesValue = dosesCard->findChild<QLabel*>("statValue");
    statsLayout->addWidget(dosesCard);

    QFrame *missedCard = createStatCard("❌", "0", "Missed Doses", "#dc3545");
    m_missedValue = missedCard->findChild<QLabel*>("statValue");
    statsLayout->addWidget(missedCard);

    QFrame *conflictsCard = createStatCard("⚠️", "0", "Conflicts", "#ffc107");
    m_conflictsValue = conflictsCard->findChild<QLabel*>("statValue");
    statsLayout->addWidget(conflictsCard);

    mainLayout->addLayout(statsLayout);

    QFrame *upcomingFrame = new QFrame();
    upcomingFrame->setObjectName("card");
    QVBoxLayout *upcomingLayout = new QVBoxLayout(upcomingFrame);

    QLabel *upcomingLabel = new QLabel("⏰ Upcoming Doses (Today)");
    upcomingLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px;");
    upcomingLayout->addWidget(upcomingLabel);

    m_upcomingList = new QListWidget();
    m_upcomingList->setStyleSheet(R"(
        QListWidget { background-color: transparent; border: none; font-size: 14px; }
        QListWidget::item { background-color: rgba(255, 255, 255, 0.05); border-radius: 8px; padding: 15px; margin: 5px 0; border-left: 4px solid #e94560; }
        QListWidget::item:hover { background-color: rgba(255, 255, 255, 0.1); }
    )");
    upcomingLayout->addWidget(m_upcomingList);

    mainLayout->addWidget(upcomingFrame, 1);
}

QFrame* DashboardWidget::createStatCard(const QString& icon, const QString& value,
                                        const QString& label, const QString& color)
{
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setStyleSheet(QString("QFrame#statCard { border-left: 4px solid %1; }").arg(color));

    QVBoxLayout *layout = new QVBoxLayout(card);

    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 32px;");
    layout->addWidget(iconLabel);

    QLabel *valueLabel = new QLabel(value);
    valueLabel->setObjectName("statValue");
    valueLabel->setStyleSheet(QString("font-size: 36px; font-weight: bold; color: %1;").arg(color));
    layout->addWidget(valueLabel);

    QLabel *textLabel = new QLabel(label);
    textLabel->setObjectName("statLabel");
    layout->addWidget(textLabel);

    return card;
}

void DashboardWidget::refresh()
{
    // Update counters
    m_medicinesValue->setText(QString::number(m_mainWindow->database().count()));
    m_missedValue->setText(QString::number(m_mainWindow->missedLog().count()));
    m_conflictsValue->setText(QString::number(m_mainWindow->conflictChecker().count()));
    QString currentTime = QTime::currentTime().toString("HH:mm");
    QString currentDay = QDate::currentDate().toString("dddd");

    int todayDosesTotal = 0;
    QMap<QString, QStringList> upcomingSchedule;

    for (const auto& med : m_mainWindow->database().allMedicines()) {

        // 1. Day Check
        if (!med.days().isEmpty() && !med.days().contains(currentDay)) {
            continue;
        }

        for (const auto& time : med.times()) {
            todayDosesTotal++;
            DoseStatus status = m_mainWindow->dailyLog().getStatus(med.name(), time);

            if (status == DoseStatus::Pending) {
                upcomingSchedule[time].append(med.name());
            }
        }
    }

    m_dosesValue->setText(QString::number(todayDosesTotal));

    // Update the UI List
    m_upcomingList->clear();

    if (upcomingSchedule.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("✓ All doses completed for today!");
        item->setTextAlignment(Qt::AlignCenter);
        item->setForeground(QColor("#28a745"));
        m_upcomingList->addItem(item);
    } else {
        int count = 0;
        for (auto it = upcomingSchedule.begin(); it != upcomingSchedule.end() && count < 5; ++it, ++count) {
            QString text = QString("%1  →  %2").arg(it.key(), it.value().join(", "));
            m_upcomingList->addItem(text);
        }
    }
}
