#ifndef CONFLICTCHECKER_H
#define CONFLICTCHECKER_H

#include <QString>
#include <QList>
#include <QPair>

class ConflictChecker {
public:
    using ConflictPair = QPair<QString, QString>;

    ConflictChecker() = default;
    void removeConflict(const QString& med1, const QString& med2);
    void addConflict(const QString& med1, const QString& med2);
    bool hasConflict(const QString& med1, const QString& med2) const;
    QList<ConflictPair> allConflicts() const { return m_conflicts; }
    int count() const { return m_conflicts.size(); }

    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

private:
    static QString normalize(const QString& name);
    QList<ConflictPair> m_conflicts;
};

#endif
