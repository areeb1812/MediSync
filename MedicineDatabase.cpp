#include "MedicineDatabase.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

QString MedicineDatabase::normalizeKey(const QString& name)
{
    return name.toLower().trimmed();
}

void MedicineDatabase::addOrUpdate(const Medicine& medicine)
{
    m_medicines[normalizeKey(medicine.name())] = medicine;
}

void MedicineDatabase::remove(const QString& name)
{
    m_medicines.remove(normalizeKey(name));
}

bool MedicineDatabase::exists(const QString& name) const
{
    return m_medicines.contains(normalizeKey(name));
}

Medicine MedicineDatabase::get(const QString& name) const
{
    return m_medicines.value(normalizeKey(name));
}

QList<Medicine> MedicineDatabase::allMedicines() const
{
    return m_medicines.values();
}

bool MedicineDatabase::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonArray array;
    for (const auto& med : m_medicines) {
        array.append(med.toJson());
    }

    QJsonDocument doc(array);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool MedicineDatabase::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return false;
    }

    m_medicines.clear();
    QJsonArray array = doc.array();
    for (const auto& item : array) {
        Medicine med = Medicine::fromJson(item.toObject());
        if (med.isValid()) {
            addOrUpdate(med);
        }
    }

    return true;
}
