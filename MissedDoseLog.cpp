#include "MissedDoseLog.h"
#include <QFile>
#include <QTextStream>

void MissedDoseLog::add(const QString& medicineName, const QString& scheduledTime)
{
    m_missedDoses.append(MissedDose(medicineName, scheduledTime));
}

bool MissedDoseLog::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    for (const auto& dose : m_missedDoses) {
        out << dose.medicineName << "|" << dose.scheduledTime << "|"
            << dose.missedAt.toString(Qt::ISODate) << "\n";
    }

    file.close();
    return true;
}

bool MissedDoseLog::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    m_missedDoses.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split('|');
        if (parts.size() >= 2) {
            MissedDose dose;
            dose.medicineName = parts[0].trimmed();
            dose.scheduledTime = parts[1].trimmed();
            if (parts.size() >= 3) {
                dose.missedAt = QDateTime::fromString(parts[2].trimmed(), Qt::ISODate);
            }
            m_missedDoses.append(dose);
        }
    }

    file.close();
    return true;
}
