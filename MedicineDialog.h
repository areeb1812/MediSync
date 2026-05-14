#ifndef MEDICINEDIALOG_H
#define MEDICINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QGroupBox>
#include "Medicine.h"

class MedicineDialog : public QDialog {
    Q_OBJECT

public:
    explicit MedicineDialog(QWidget *parent = nullptr);

    void setMedicine(const Medicine& med);
    Medicine medicine() const;

private slots:
    void validate();

private:
    void setupUI();
    QGroupBox* createDaysGroup();

    QLineEdit *m_nameEdit;
    QLineEdit *m_dosageEdit;
    QLineEdit *m_timesEdit;
    QTextEdit *m_instructionsEdit;
    QList<QCheckBox*> m_dayCheckBoxes;
};

#endif
