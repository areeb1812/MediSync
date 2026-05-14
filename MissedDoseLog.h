#ifndef MISSEDDOSELOG_H
#define MISSEDDOSELOG_H

#include <QString>
#include <QList>
#include <QDateTime>

struct MissedDose {
    QString medicineName;
    QString scheduledTime;
    QDateTime missedAt;

    MissedDose() = default;
    MissedDose(const QString& name, const QString& time)
        : medicineName(name), scheduledTime(time), missedAt(QDateTime::currentDateTime()) {}
};

class MissedDoseLog {
public:
    MissedDoseLog() = default;

    void add(const QString& medicineName, const QString& scheduledTime);
    QList<MissedDose> allMissedDoses() const { return m_missedDoses; }
    int count() const { return m_missedDoses.size(); }
    void clear() { m_missedDoses.clear(); }

    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

private:
    QList<MissedDose> m_missedDoses;
};

#endif
