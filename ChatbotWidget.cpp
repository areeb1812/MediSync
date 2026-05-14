#include "ChatbotWidget.h"
#include "MainWindow.h"
#include "Chatbot.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>

ChatbotWidget::ChatbotWidget(MainWindow *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_mainWindow(mainWindow)
{
    setupUI();
    addMessage("Hello! 👋 I'm your medicine assistant. Ask me about your medications, "
               "schedules, or say \"help\" for more options.", false);
}

void ChatbotWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);
    QLabel *headerLabel = new QLabel("Medicine Assistant");
    headerLabel->setObjectName("headerLabel");
    mainLayout->addWidget(headerLabel);
    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setStyleSheet(R"(
        QTextEdit {
            background-color: #0f3460;
            border: none;
            border-radius: 12px;
            padding: 20px;
            font-size: 14px;
            line-height: 1.6;
        }
    )");
    mainLayout->addWidget(m_chatDisplay, 1);
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(12);

    m_inputEdit = new QLineEdit();
    m_inputEdit->setPlaceholderText("Type your message...");
    m_inputEdit->setStyleSheet(R"(
        QLineEdit {
            padding: 15px 20px;
            font-size: 14px;
            border-radius: 25px;
        }
    )");
    connect(m_inputEdit, &QLineEdit::returnPressed, this, &ChatbotWidget::sendMessage);
    inputLayout->addWidget(m_inputEdit);

    QPushButton *sendBtn = new QPushButton("Send");
    sendBtn->setStyleSheet("padding: 15px 30px; border-radius: 25px;");
    connect(sendBtn, &QPushButton::clicked, this, &ChatbotWidget::sendMessage);
    inputLayout->addWidget(sendBtn);

    mainLayout->addLayout(inputLayout);
}

void ChatbotWidget::sendMessage()
{
    QString text = m_inputEdit->text().trimmed();
    if (text.isEmpty()) return;

    m_inputEdit->clear();
    addMessage(text, true);

    // Get bot response
    Chatbot bot(&m_mainWindow->database());
    QString response = bot.respond(text);
    addMessage(response, false);
}

void ChatbotWidget::addMessage(const QString& message, bool isUser)
{
    QString html;
    if (isUser) {
        html = QString(R"(
            <div style="text-align: right; margin: 10px 0;">
                <span style="background-color: #e94560; color: white; padding: 12px 18px;
                             border-radius: 18px 18px 4px 18px; display: inline-block;
                             max-width: 70%; text-align: left;">
                    %1
                </span>
            </div>
        )").arg(message);
    } else {
        html = QString(R"(
            <div style="text-align: left; margin: 10px 0;">
                <span style="background-color: rgba(74, 144, 217, 0.2); color: #e4e6eb;
                             padding: 12px 18px; border-radius: 18px 18px 18px 4px;
                             display: inline-block; max-width: 70%;">
                    %1
                </span>
            </div>
        )").arg(message);
    }

    m_chatDisplay->insertHtml(html);
    m_chatDisplay->insertPlainText("\n");
    QScrollBar *scrollBar = m_chatDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
