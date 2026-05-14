#ifndef SCHEDULEWIDGET_H
#define SCHEDULEWIDGET_H

#include <QWidget>
#include <QListWidget>

class MainWindow;

class ScheduleWidget : public QWidget {
    Q_OBJECT

public:
    explicit ScheduleWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    void refresh();

private:
    void setupUI();

    MainWindow *m_mainWindow;
    QListWidget *m_scheduleList;
};

#endif
