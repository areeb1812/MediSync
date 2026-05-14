#ifndef MISSEDWIDGET_H
#define MISSEDWIDGET_H

#include <QWidget>
#include <QListWidget>

class MainWindow;

class MissedWidget : public QWidget {
    Q_OBJECT

public:
    explicit MissedWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    void refresh();

private slots:
    void clearLog();

private:
    void setupUI();

    MainWindow *m_mainWindow;
    QListWidget *m_missedList;
};

#endif
