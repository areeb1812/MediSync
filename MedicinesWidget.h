#ifndef MEDICINESWIDGET_H
#define MEDICINESWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class MainWindow;

class MedicinesWidget : public QWidget {
    Q_OBJECT

public:
    explicit MedicinesWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    void refresh();

private slots:
    void addMedicine();
    void editMedicine();
    void deleteMedicine();

private:
    void setupUI();

    MainWindow *m_mainWindow;
    QTableWidget *m_table;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
};

#endif
