#ifndef MEDICINEDATABASE_H
#define MEDICINEDATABASE_H

#include "Medicine.h"
#include <QMap>
#include <QList>
#include <QString>

class MedicineDatabase {
public:
    MedicineDatabase() = default;

    void addOrUpdate(const Medicine& medicine);
    void remove(const QString& name);
    bool exists(const QString& name) const;
    Medicine get(const QString& name) const;
    QList<Medicine> allMedicines() const;
    int count() const { return m_medicines.size(); }

    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

private:
    static QString normalizeKey(const QString& name);
    QMap<QString, Medicine> m_medicines;
};

#endif
