#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>

class MainWindow;

class DashboardWidget : public QWidget {
    Q_OBJECT

public:
    explicit DashboardWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    void refresh();

private:
    void setupUI();
    QFrame* createStatCard(const QString& icon, const QString& value,
                           const QString& label, const QString& color);

    MainWindow *m_mainWindow;

    QLabel *m_medicinesValue;
    QLabel *m_dosesValue;
    QLabel *m_missedValue;
    QLabel *m_conflictsValue;
    QListWidget *m_upcomingList;
};

#endif
