#include "MedicineDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>

MedicineDialog::MedicineDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Add/Edit Medicine");
    setMinimumWidth(450);
    setupUI();
}

QGroupBox* MedicineDialog::createDaysGroup()
{
    QGroupBox* group = new QGroupBox("Schedule Days (Leave all unchecked for Every Day)");
    QHBoxLayout* layout = new QHBoxLayout(group);

    QStringList days = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for(const QString& day : days) {
        QCheckBox* box = new QCheckBox(day);
        m_dayCheckBoxes.append(box);
        layout->addWidget(box);
    }
    return group;
}

void MedicineDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 25, 25, 25);

    mainLayout->addWidget(new QLabel("Medicine Name:"));
    m_nameEdit = new QLineEdit(); m_nameEdit->setPlaceholderText("e.g. Paracetamol");
    mainLayout->addWidget(m_nameEdit);

    mainLayout->addWidget(new QLabel("Dosage:"));
    m_dosageEdit = new QLineEdit(); m_dosageEdit->setPlaceholderText("e.g. 500mg");
    mainLayout->addWidget(m_dosageEdit);

    mainLayout->addWidget(new QLabel("Times (HH:MM, comma separated):"));
    m_timesEdit = new QLineEdit(); m_timesEdit->setPlaceholderText("08:00, 14:00");
    mainLayout->addWidget(m_timesEdit);

    mainLayout->addWidget(new QLabel("Instructions:"));
    m_instructionsEdit = new QTextEdit(); m_instructionsEdit->setMaximumHeight(80);
    mainLayout->addWidget(m_instructionsEdit);

    mainLayout->addWidget(createDaysGroup());

    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *cancelBtn = new QPushButton("Cancel");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);

    QPushButton *saveBtn = new QPushButton("Save");
    connect(saveBtn, &QPushButton::clicked, this, &MedicineDialog::validate);
    buttonLayout->addWidget(saveBtn);

    mainLayout->addLayout(buttonLayout);
}

void MedicineDialog::setMedicine(const Medicine& med)
{
    m_nameEdit->setText(med.name());
    m_dosageEdit->setText(med.dosage());
    m_timesEdit->setText(med.times().join(", "));
    m_instructionsEdit->setText(med.instructions());

    QStringList savedDays = med.days();
    QStringList fullDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    QStringList shortDays = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    for(auto b : m_dayCheckBoxes) b->setChecked(false);

    if (!savedDays.isEmpty()) {
        for(int i=0; i<7; i++) {
            if(savedDays.contains(fullDays[i]) || savedDays.contains(shortDays[i])) {
                m_dayCheckBoxes[i]->setChecked(true);
            }
        }
    }
}

Medicine MedicineDialog::medicine() const
{
    QStringList times;
    QString timesText = m_timesEdit->text();
    for (const QString& t : timesText.split(',')) {
        QString time = t.trimmed();
        if (!time.isEmpty()) times.append(time);
    }

    QStringList selectedDays;
    QStringList fullDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

    bool anyChecked = false;
    for(int i=0; i<7; i++) {
        if(m_dayCheckBoxes[i]->isChecked()) {
            selectedDays.append(fullDays[i]);
            anyChecked = true;
        }
    }

    if(!anyChecked) selectedDays.clear();

    return Medicine(
        m_nameEdit->text().trimmed(),
        m_dosageEdit->text().trimmed(),
        times,
        m_instructionsEdit->toPlainText().trimmed(),
        selectedDays
        );
}

void MedicineDialog::validate()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Medicine name is required.");
        return;
    }
    if (m_timesEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "At least one time is required.");
        return;
    }
    accept();
}
