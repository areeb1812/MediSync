#ifndef CONFLICTSWIDGET_H
#define CONFLICTSWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MainWindow;

class ConflictsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConflictsWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    void refresh();

private slots:
    void addConflict();
    void deleteConflict();

private:
    void setupUI();

    MainWindow *m_mainWindow;
    QListWidget *m_conflictsList;
    QPushButton *m_deleteBtn;
};

#endif
