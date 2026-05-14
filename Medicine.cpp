#include "Medicine.h"
#include <QJsonArray>

Medicine::Medicine(const QString& name, const QString& dosage,
                   const QStringList& times, const QString& instructions,
                   const QStringList& days)
    : m_name(name)
    , m_dosage(dosage)
    , m_times(times)
    , m_instructions(instructions)
    , m_days(days)
{
}

QJsonObject Medicine::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["dosage"] = m_dosage;
    json["instructions"] = m_instructions;

    QJsonArray timesArray;
    for (const auto& time : m_times) {
        timesArray.append(time);
    }
    json["times"] = timesArray;

    QJsonArray daysArray;
    for (const auto& day : m_days) {
        daysArray.append(day);
    }
    json["days"] = daysArray;

    return json;
}

Medicine Medicine::fromJson(const QJsonObject& json)
{
    Medicine med;
    med.m_name = json["name"].toString();
    med.m_dosage = json["dosage"].toString();
    med.m_instructions = json["instructions"].toString();

    QJsonArray timesArray = json["times"].toArray();
    for (const auto& time : timesArray) {
        med.m_times.append(time.toString());
    }

    // Load Days
    QJsonArray daysArray = json["days"].toArray();
    for (const auto& day : daysArray) {
        med.m_days.append(day.toString());
    }

    return med;
}
