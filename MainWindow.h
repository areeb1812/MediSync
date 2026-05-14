#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "DailyLog.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QLabel>
#include "MedicineDatabase.h"
#include "ConflictChecker.h"
#include "MissedDoseLog.h"

class DashboardWidget;
class MedicinesWidget;
class ScheduleWidget;
class ConflictsWidget;
class MissedWidget;
class ChatbotWidget;
struct SnoozeItem {
    QString name;
    QTime triggerTime;
};
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    MedicineDatabase& database() { return m_database; }
    ConflictChecker& conflictChecker() { return m_conflictChecker; }
    MissedDoseLog& missedLog() { return m_missedLog; }
    DailyLog& dailyLog() { return m_dailyLog; }

signals:
    void dataChanged();

public slots:
    void refreshAll();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onNavItemClicked(QListWidgetItem *item);
    void checkReminders();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupUI();
    void setupNavigation();
    void setupSystemTray();
    void loadData();
    void saveData();
    void applyStyles();
    // Navigation
    QWidget *m_navWidget;
    QListWidget *m_navList;
    QStackedWidget *m_stackedWidget;
    // Pages
    DashboardWidget *m_dashboardWidget;
    MedicinesWidget *m_medicinesWidget;
    ScheduleWidget *m_scheduleWidget;
    ConflictsWidget *m_conflictsWidget;
    MissedWidget *m_missedWidget;
    ChatbotWidget *m_chatbotWidget;
    // System tray
    QSystemTrayIcon *m_trayIcon;
    QTimer *m_reminderTimer;
    // Data
    MedicineDatabase m_database;
    ConflictChecker m_conflictChecker;
    MissedDoseLog m_missedLog;
    DailyLog m_dailyLog;
    QList<SnoozeItem> m_snoozeList;
    // File paths
    QString m_dataPath;
};

#endif
