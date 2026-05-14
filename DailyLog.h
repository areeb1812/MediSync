#ifndef DAILYLOG_H
#define DAILYLOG_H

#include <QString>
#include <QMap>
#include <QDate>

enum class DoseStatus { Pending, Taken, Missed };

class DailyLog {
public:
    DailyLog();
    void markTaken(const QString& name, const QString& time);
    void markMissed(const QString& name, const QString& time);
    DoseStatus getStatus(const QString& name, const QString& time);
    void load(const QString& path);
    void save(const QString& path);

private:
    void checkDateReset();
    QMap<QString, QString> m_log;
    QDate m_lastDate;
};

#endif
