#ifndef CHATBOTWIDGET_H
#define CHATBOTWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

class MainWindow;

class ChatbotWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatbotWidget(MainWindow *mainWindow, QWidget *parent = nullptr);

private slots:
    void sendMessage();

private:
    void setupUI();
    void addMessage(const QString& message, bool isUser);

    MainWindow *m_mainWindow;
    QTextEdit *m_chatDisplay;
    QLineEdit *m_inputEdit;
};

#endif
