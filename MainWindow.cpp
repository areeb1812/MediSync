#include "MainWindow.h"
#include "DashboardWidget.h"
#include "MedicinesWidget.h"
#include "ScheduleWidget.h"
#include "ConflictsWidget.h"
#include "MissedWidget.h"
#include "ChatbotWidget.h"
#include "ReminderDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <QDate>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("MediSync");
    setWindowIcon(QIcon(":/icons/pills.png"));
    setMinimumSize(1200, 700);
    m_dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(m_dataPath);

    setupUI();
    setupNavigation();
    setupSystemTray();
    applyStyles();
    loadData();

    m_reminderTimer = new QTimer(this);
    connect(m_reminderTimer, &QTimer::timeout, this, &MainWindow::checkReminders);
    m_reminderTimer->start(2000);
}

MainWindow::~MainWindow()
{
    saveData();
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_navWidget = new QWidget();
    m_navWidget->setObjectName("navWidget");
    m_navWidget->setFixedWidth(250);

    QVBoxLayout *navLayout = new QVBoxLayout(m_navWidget);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(0);

    QLabel *logoLabel = new QLabel();
    logoLabel->setObjectName("logoLabel");
    logoLabel->setText("💊 MediSync");
    logoLabel->setAlignment(Qt::AlignCenter);
    navLayout->addWidget(logoLabel);

    m_navList = new QListWidget();
    m_navList->setObjectName("navList");

    QStringList navItems = {"🏠 Dashboard", "💊 Medicines", "📅 Schedule",
                            "⚠️ Conflicts", "❌ Missed Doses", "🤖 Chatbot"};

    for (const auto& item : navItems) {
        QListWidgetItem *listItem = new QListWidgetItem(item);
        listItem->setSizeHint(QSize(250, 50));
        m_navList->addItem(listItem);
    }

    m_navList->setCurrentRow(0);
    connect(m_navList, &QListWidget::itemClicked, this, &MainWindow::onNavItemClicked);
    navLayout->addWidget(m_navList);


    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setObjectName("stackedWidget");

    m_dashboardWidget = new DashboardWidget(this);
    m_medicinesWidget = new MedicinesWidget(this);
    m_scheduleWidget = new ScheduleWidget(this);
    m_conflictsWidget = new ConflictsWidget(this);
    m_missedWidget = new MissedWidget(this);
    m_chatbotWidget = new ChatbotWidget(this);

    m_stackedWidget->addWidget(m_dashboardWidget);
    m_stackedWidget->addWidget(m_medicinesWidget);
    m_stackedWidget->addWidget(m_scheduleWidget);
    m_stackedWidget->addWidget(m_conflictsWidget);
    m_stackedWidget->addWidget(m_missedWidget);
    m_stackedWidget->addWidget(m_chatbotWidget);

    mainLayout->addWidget(m_navWidget);
    mainLayout->addWidget(m_stackedWidget, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::setupNavigation()
{
    connect(this, &MainWindow::dataChanged, this, &MainWindow::refreshAll);
}

void MainWindow::setupSystemTray()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/pills.png"));
    m_trayIcon->setToolTip("Medicine Reminder");

    QMenu *trayMenu = new QMenu();
    QAction *showAction = trayMenu->addAction("Show");
    connect(showAction, &QAction::triggered, this, &QMainWindow::show);
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("Quit");
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);
}

void MainWindow::onNavItemClicked(QListWidgetItem *item)
{
    int index = m_navList->row(item);
    m_stackedWidget->setCurrentIndex(index);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        show();
        activateWindow();
    }
}

void MainWindow::checkReminders()
{
    QTime currentTime = QTime::currentTime();
    QString timeStr = currentTime.toString("HH:mm");
    QString currentDay = QDate::currentDate().toString("dddd");

    QList<Medicine> dueMedicines;

    static int lastProcessedMinute = -1;
    if (currentTime.minute() != lastProcessedMinute) {
        for (const auto& med : m_database.allMedicines()) {
            if (!med.days().isEmpty() && !med.days().contains(currentDay)) continue;

            for (const QString& tStr : med.times()) {
                QTime scheduledTime = QTime::fromString(tStr.trimmed(), "H:m");
                if (!scheduledTime.isValid()) scheduledTime = QTime::fromString(tStr.trimmed(), "HH:mm");

                if (scheduledTime.isValid() &&
                    scheduledTime.hour() == currentTime.hour() &&
                    scheduledTime.minute() == currentTime.minute()) {

                    if (m_dailyLog.getStatus(med.name(), tStr.trimmed()) == DoseStatus::Pending) {
                        dueMedicines.append(med);
                    }
                }
            }
        }
    }

    for (int i = 0; i < m_snoozeList.size(); ++i) {
        if (currentTime >= m_snoozeList[i].triggerTime) {
            QString name = m_snoozeList[i].name;


            if (m_database.exists(name) &&
                m_dailyLog.getStatus(name, timeStr) == DoseStatus::Pending) {

                dueMedicines.append(m_database.get(name));
            }
            m_snoozeList.removeAt(i);
            i--;
        }
    }

    if (dueMedicines.isEmpty()) return;
    if (currentTime.minute() != lastProcessedMinute) {
        lastProcessedMinute = currentTime.minute();
    }

    for (const auto& med : dueMedicines) {

        QStringList conflictNames;
        for (const auto& other : dueMedicines) {
            if (med.name() == other.name()) continue;
            if (m_conflictChecker.hasConflict(med.name(), other.name())) {
                conflictNames.append(other.name());
            }
        }
        QString conflictMsg = !conflictNames.isEmpty() ? "DANGEROUS INTERACTION: " + conflictNames.join(", ") : "";

        m_trayIcon->showMessage("Medicine Reminder", "Time for " + med.name());

        // Show Dialog
        ReminderDialog dialog(med.name(), med.dosage(), currentTime.toString("h:mm AP"), conflictMsg, this);
        dialog.exec();

        if (dialog.wasSnoozed()) {
            // Snooze Time Set
            QTime snoozeTime = QTime::currentTime().addSecs(600);
            m_snoozeList.append({med.name(), snoozeTime});

            qDebug() << "Snoozed" << med.name() << "until" << snoozeTime.toString("HH:mm:ss");
        }
        else if (dialog.wasMissed()) {
            m_missedLog.add(med.name(), timeStr);
            m_dailyLog.markMissed(med.name(), timeStr);
            emit dataChanged();
        }
        else {
            // Taken
            m_dailyLog.markTaken(med.name(), timeStr);
            emit dataChanged();
        }
    }
}


void MainWindow::loadData()
{
    m_database.loadFromFile(m_dataPath + "/medicines.json");
    m_conflictChecker.loadFromFile(m_dataPath + "/conflicts.txt");
    m_missedLog.loadFromFile(m_dataPath + "/missed.txt");
    m_dailyLog.load(m_dataPath + "/dailylog.txt");
    emit dataChanged();
}

void MainWindow::saveData()
{
    m_database.saveToFile(m_dataPath + "/medicines.json");
    m_conflictChecker.saveToFile(m_dataPath + "/conflicts.txt");
    m_missedLog.saveToFile(m_dataPath + "/missed.txt");
    m_dailyLog.save(m_dataPath + "/dailylog.txt");
}

void MainWindow::refreshAll()
{
    m_dashboardWidget->refresh();
    m_medicinesWidget->refresh();
    m_scheduleWidget->refresh();
    m_conflictsWidget->refresh();
    m_missedWidget->refresh();
    saveData();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon->isVisible()) {
        hide();
        m_trayIcon->showMessage(
            "Medicine Reminder",
            "Application minimized to tray. Double-click to restore.",
            QSystemTrayIcon::Information,
            2000
            );
        event->ignore();
    }
}

void MainWindow::applyStyles()
{
    // CSS
    QString styleSheet = R"(
        QMainWindow { background-color: #1a1a2e; }
        #navWidget { background-color: #16213e; border-right: 1px solid #0f3460; }
        #logoLabel { font-size: 24px; font-weight: bold; color: #e94560; padding: 30px; background-color: #16213e; border-bottom: 1px solid #0f3460; }
        #navList { background-color: transparent; border: none; outline: 0; font-size: 16px; color: #a0a0a0; padding: 10px; }
        #navList::item { padding: 15px 20px; border-radius: 8px; margin: 5px 10px; }
        #navList::item:hover { background-color: rgba(233, 69, 96, 0.1); color: #e94560; }
        #navList::item:selected { background-color: #e94560; border: none; color: white; }
        #navList::item:selected:!active { background-color: #e94560; color: white; }
        #stackedWidget { background-color: #1a1a2e; }
        QWidget { color: #e4e6eb; font-family: 'Segoe UI', Arial, sans-serif; }
        QPushButton { background-color: #e94560; color: white;border: none; padding: 12px 24px; border-radius: 8px; font-size: 14px; font-weight: bold; }
        QPushButton:hover { background-color: #d63d56; }
        QPushButton:pressed { background-color: #c13048; }
        QPushButton#secondaryButton { background-color: #0f3460; }
        QPushButton#secondaryButton:hover { background-color: #1a4a7a; }
        QPushButton#dangerButton { background-color: #dc3545; }
        QPushButton#dangerButton:hover { background-color: #c82333; }
        QLineEdit, QTextEdit, QComboBox { background-color: #0f3460; border: 1px solid #1a4a7a; border-radius: 8px; padding: 12px; color: #e4e6eb; font-size: 14px; }
        QLineEdit:focus, QTextEdit:focus { border: 2px solid #e94560; }
        QTableWidget { background-color: #0f3460; border: none; border-radius: 12px; gridline-color: #1a4a7a; }
        QTableWidget::item { padding: 12px; border-bottom: 1px solid #1a4a7a; }
        QTableWidget::item:selected { background-color: rgba(233, 69, 96, 0.3); }
        QHeaderView::section { background-color: #16213e; color: #e4e6eb; padding: 12px; border: none; border-bottom: 2px solid #e94560; font-weight: bold; }
        QScrollBar:vertical { background-color: #0f3460; width: 12px; border-radius: 6px; }
        QScrollBar::handle:vertical { background-color: #e94560; border-radius: 6px; min-height: 30px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
        QLabel#headerLabel { font-size: 28px; font-weight: bold; color: #e4e6eb; padding: 20px 0; }
        QFrame#card { background-color: #0f3460; border-radius: 12px; padding: 20px; }
        QFrame#statCard { background-color: #0f3460; border-radius: 12px; padding: 24px; min-width: 200px; }
        QLabel#statValue { font-size: 36px; font-weight: bold; color: #e94560; }
        QLabel#statLabel { font-size: 14px; color: #a0a0a0; }
        QDialog { background-color: #1a1a2e; }
        QMessageBox { background-color: #1a1a2e; }
        QMessageBox QLabel { color: #e4e6eb; }
        QAbstractItemView { outline: none; }
    )";

    qApp->setStyleSheet(styleSheet);
}
