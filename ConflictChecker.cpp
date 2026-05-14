#include "ConflictChecker.h"
#include <QFile>
#include <QTextStream>

QString ConflictChecker::normalize(const QString& name)
{
    return name.toLower().trimmed();
}

void ConflictChecker::addConflict(const QString& med1, const QString& med2)
{
    QString n1 = normalize(med1);
    QString n2 = normalize(med2);

    if (n1 == n2) return;
    for (const auto& pair : m_conflicts) {
        if ((pair.first == n1 && pair.second == n2) ||
            (pair.first == n2 && pair.second == n1)) {
            return;
        }
    }

    m_conflicts.append({n1, n2});
}

bool ConflictChecker::hasConflict(const QString& med1, const QString& med2) const
{
    QString n1 = normalize(med1);
    QString n2 = normalize(med2);

    for (const auto& pair : m_conflicts) {
        if ((pair.first == n1 && pair.second == n2) ||
            (pair.first == n2 && pair.second == n1)) {
            return true;
        }
    }
    return false;
}

bool ConflictChecker::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    for (const auto& pair : m_conflicts) {
        out << pair.first << "|" << pair.second << "\n";
    }

    file.close();
    return true;
}

bool ConflictChecker::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    m_conflicts.clear();
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        int pos = line.indexOf('|');
        if (pos == -1) continue;

        QString med1 = line.left(pos).trimmed();
        QString med2 = line.mid(pos + 1).trimmed();

        if (!med1.isEmpty() && !med2.isEmpty()) {
            m_conflicts.append({med1, med2});
        }
    }

    file.close();
    return true;
}
void ConflictChecker::removeConflict(const QString& med1, const QString& med2)
{
    QString n1 = normalize(med1);
    QString n2 = normalize(med2);

    for (int i = 0; i < m_conflicts.size(); ++i) {
        if ((m_conflicts[i].first == n1 && m_conflicts[i].second == n2) ||
            (m_conflicts[i].first == n2 && m_conflicts[i].second == n1)) {
            m_conflicts.removeAt(i);
            return;
        }
    }
}
