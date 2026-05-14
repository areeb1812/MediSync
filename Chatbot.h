#ifndef CHATBOT_H
#define CHATBOT_H

#include "MedicineDatabase.h"
#include <QString>

class Chatbot {
public:
    explicit Chatbot(MedicineDatabase* database) : m_database(database) {}
    QString respond(const QString& query) {
        QString q = query.toLower().trimmed();
        for (const auto& med : m_database->allMedicines()) {
            if (q.contains(med.name().toLower())) {
                QString response = QString("💊 <b>%1</b><br><br>").arg(med.name());
                response += QString("<b>Dosage:</b> %1<br>").arg(med.dosage());
                response += QString("<b>Times:</b> %1<br>").arg(med.times().join(", "));
                response += QString("<b>Instructions:</b> %1").arg(med.instructions());
                return response;
            }
        }

        // Help command
        if (q.contains("help") || q.contains("how")) {
            return "I can help you with:<br><br>"
                   "• Ask about any medicine: \"Tell me about Paracetamol\"<br>"
                   "• List all medicines: \"List medicines\"<br>"
                   "• Get schedule: \"What's my schedule?\"<br>"
                   "• General tips: \"Give me tips\"";
        }

        // List medicines
        if (q.contains("list") || q.contains("all medicine")) {
            auto meds = m_database->allMedicines();
            if (meds.isEmpty()) {
                return "You don't have any medicines in your database yet.";
            }
            QString response = "📋 <b>Your Medicines:</b><br><br>";
            for (const auto& med : meds) {
                response += QString("• %1 (%2)<br>").arg(med.name(), med.dosage());
            }
            return response;
        }

        // Schedule
        if (q.contains("schedule") || q.contains("when")) {
            auto meds = m_database->allMedicines();
            if (meds.isEmpty()) {
                return "No medicines scheduled. Add some medicines first!";
            }

            QMap<QString, QStringList> schedule;
            for (const auto& med : meds) {
                for (const auto& time : med.times()) {
                    schedule[time].append(med.name());
                }
            }

            QString response = "📅 <b>Today's Schedule:</b><br><br>";
            for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                response += QString("<b>%1</b> - %2<br>").arg(it.key(), it.value().join(", "));
            }
            return response;
        }

        // Tips
        if (q.contains("tip")) {
            return "💡 <b>Medication Tips:</b><br><br>"
                   "• Take medicines at the same time daily<br>"
                   "• Don't skip doses even if you feel better<br>"
                   "• Store medicines in a cool, dry place<br>"
                   "• Check expiration dates regularly<br>"
                   "• Never share prescription medications";
        }

        // Default response
        return "I didn't quite understand that. Try asking:<br><br>"
               "• \"Tell me about [medicine name]\"<br>"
               "• \"List all medicines\"<br>"
               "• \"What's my schedule?\"<br>"
               "• \"Help\"";
    }

private:
    MedicineDatabase* m_database;
};

#endif
