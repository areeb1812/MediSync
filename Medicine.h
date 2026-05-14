#ifndef MEDICINE_H
#define MEDICINE_H

#include <QString>
#include <QStringList>
#include <QJsonObject>

class Medicine {
public:
    Medicine() = default;
    Medicine(const QString& name, const QString& dosage,
             const QStringList& times, const QString& instructions,
             const QStringList& days = {});
    QString name() const { return m_name; }
    QString dosage() const { return m_dosage; }
    QStringList times() const { return m_times; }
    QString instructions() const { return m_instructions; }
    QStringList days() const { return m_days; }
    void setName(const QString& name) { m_name = name; }
    void setDosage(const QString& dosage) { m_dosage = dosage; }
    void setTimes(const QStringList& times) { m_times = times; }
    void setInstructions(const QString& instructions) { m_instructions = instructions; }
    void setDays(const QStringList& days) { m_days = days; }
    QJsonObject toJson() const;
    static Medicine fromJson(const QJsonObject& json);
    bool isValid() const { return !m_name.isEmpty(); }

private:
    QString m_name;
    QString m_dosage;
    QStringList m_times;
    QString m_instructions;
    QStringList m_days;
};

#endif
