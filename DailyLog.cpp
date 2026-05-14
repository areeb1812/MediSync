#include "DailyLog.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

DailyLog::DailyLog() {
    m_lastDate = QDate::currentDate();
}

void DailyLog::checkDateReset() {
    if (m_lastDate != QDate::currentDate()) {
        m_log.clear();
        m_lastDate = QDate::currentDate();
    }
}

void DailyLog::markTaken(const QString& name, const QString& time) {
    checkDateReset();
    m_log[name + "|" + time] = "TAKEN";
}

void DailyLog::markMissed(const QString& name, const QString& time) {
    checkDateReset();
    m_log[name + "|" + time] = "MISSED";
}

DoseStatus DailyLog::getStatus(const QString& name, const QString& time) {
    checkDateReset();
    QString key = name + "|" + time;
    if (m_log.value(key) == "TAKEN") return DoseStatus::Taken;
    if (m_log.value(key) == "MISSED") return DoseStatus::Missed;
    return DoseStatus::Pending;
}

void DailyLog::save(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << m_lastDate.toString(Qt::ISODate) << "\n";
        for (auto it = m_log.begin(); it != m_log.end(); ++it) {
            out << it.key() << "=" << it.value() << "\n";
        }
    }
}

void DailyLog::load(const QString& path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString dateStr = in.readLine().trimmed();
        if (dateStr != QDate::currentDate().toString(Qt::ISODate)) {
            m_log.clear();
            m_lastDate = QDate::currentDate();
            return;
        }

        m_lastDate = QDate::fromString(dateStr, Qt::ISODate);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split('=');
            if (parts.size() == 2) {
                m_log[parts[0]] = parts[1];
            }
        }
    }
}
