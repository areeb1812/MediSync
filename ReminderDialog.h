#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    ReminderDialog(const QString& medicine, const QString& dosage,
                   const QString& time, const QString& warningText = "",
                   QWidget *parent = nullptr);

    bool wasMissed() const { return m_missed; }
    bool wasSnoozed() const { return m_snoozed; }

private slots:
    void onTaken();
    void onSnooze();
    void onMissed();

private:
    void setupUI();

    QString m_medicine;
    QString m_dosage;
    QString m_time;
    QString m_warningText;
    bool m_missed = false;
    bool m_snoozed = false;
};

#endif
