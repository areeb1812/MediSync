#include "ReminderDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QTimer>

ReminderDialog::ReminderDialog(const QString& medicine, const QString& dosage,
                               const QString& time, const QString& warningText,
                               QWidget *parent)
    : QDialog(parent)
    , m_medicine(medicine)
    , m_dosage(dosage)
    , m_time(time)
    , m_warningText(warningText)
    , m_missed(false)
{
    setWindowTitle("Medicine Reminder");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setModal(true);
    setMinimumWidth(420);

    QString bgColor = m_warningText.isEmpty() ? "#1a1a2e" : "#3d0000";
    QString borderColor = m_warningText.isEmpty() ? "#1a4a7a" : "#ff0000";
    QString titleColor = m_warningText.isEmpty() ? "#e94560" : "#ff5555";
    QString timeColor = m_warningText.isEmpty() ? "#4a90d9" : "#ffaaaa";

    setStyleSheet(QString(R"(
        QDialog { background-color: %1; color: #e4e6eb; border-radius: 16px; border: 2px solid %2; }
        QLabel#title { font-size: 32px; font-weight: bold; color: %3; }
        QLabel#time { font-size: 56px; font-weight: bold; color: %4; margin: 10px 0; }
        QLabel#medicine { font-size: 28px; font-weight: bold; }
        QLabel#dosage { font-size: 18px; color: #a0a0a0; }
        QLabel#warningLabel { font-size: 14px; font-weight: bold; color: #ffffff; background-color: #cc0000; padding: 15px; border-radius: 8px; border: 1px solid #ff5555; }
        QPushButton { padding: 14px 28px; border: none; border-radius: 12px; font-size: 16px; font-weight: bold; min-width: 120px; }
        QPushButton#taken { background-color: #28a745; color: white; }
        QPushButton#snooze { background-color: #ffc107; color: #212529; }
        QPushButton#missed { background-color: #dc3545; color: white; }
    )").arg(bgColor, borderColor, titleColor, timeColor));

    setupUI();
    QApplication::beep();

    QTimer::singleShot(120000, this, [this]() {
        if (isVisible()) {
            m_missed = true;
            reject();
        }
    });
}

void ReminderDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel *titleLabel = new QLabel("Medicine Reminder");
    titleLabel->setObjectName("title");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *timeLabel = new QLabel(m_time);
    timeLabel->setObjectName("time");
    timeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(timeLabel);

    QLabel *medicineLabel = new QLabel(m_medicine);
    medicineLabel->setObjectName("medicine");
    medicineLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(medicineLabel);

    QLabel *dosageLabel = new QLabel(m_dosage);
    dosageLabel->setObjectName("dosage");
    dosageLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(dosageLabel);

    if (!m_warningText.isEmpty()) {
        QLabel *warnLabel = new QLabel("⚠️ " + m_warningText);
        warnLabel->setObjectName("warningLabel");
        warnLabel->setWordWrap(true);
        warnLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(warnLabel);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton *takenBtn = new QPushButton("Taken");
    takenBtn->setObjectName("taken");
    connect(takenBtn, &QPushButton::clicked, this, &ReminderDialog::onTaken);
    buttonLayout->addWidget(takenBtn);

    QPushButton *snoozeBtn = new QPushButton("💤 Snooze (10 min)");
    snoozeBtn->setObjectName("snooze");
    connect(snoozeBtn, &QPushButton::clicked, this, &ReminderDialog::onSnooze);
    buttonLayout->addWidget(snoozeBtn);

    QPushButton *missedBtn = new QPushButton("Missed");
    missedBtn->setObjectName("missed");
    connect(missedBtn, &QPushButton::clicked, this, &ReminderDialog::onMissed);
    buttonLayout->addWidget(missedBtn);

    mainLayout->addLayout(buttonLayout);
}

void ReminderDialog::onTaken() { m_missed = false; m_snoozed = false; accept(); }
void ReminderDialog::onSnooze() { m_missed = false; m_snoozed = true; accept(); }
void ReminderDialog::onMissed() { m_missed = true; m_snoozed = false; accept(); }
