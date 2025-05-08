#include "widget.h"
#include "./ui_widget.h"
#include <QIcon>
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <random>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // UI initialization
    initializeUI();

    // Establish connections
    for (int i = 0; i < m_buttons.size(); ++i) {
        connect(m_buttons[i], &QPushButton::clicked, this, [this, i]() {
            flipCard(i);
        });
    }

    connect(ui->restartButton, &QPushButton::clicked, this, &Widget::restartGame);

    connect(m_gameTimer, &QTimer::timeout, this, [this]() {
        m_elapsedSeconds++;
        ui->timerLabel->setText(QString("Time: %1 s").arg(m_elapsedSeconds));
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initializeUI()
{
    // Set window title and icon
    this->setWindowTitle("Memory Match 🃏");
    QIcon app_icon(":/resources/memory.png");
    this->setWindowIcon(app_icon);
    this->setFixedSize(1000, 700);

    // Set background image
    QPixmap backgroundPixmap(":/resources/background.png");
    ui->backgroundLabel->setPixmap(backgroundPixmap);
    ui->backgroundLabel->setScaledContents(true);

    // Initialize card back icon
    QIcon back_icon(":/resources/card-game.png");

    // Populate the button list
    m_buttons = {
        ui->pushButton_1, ui->pushButton_2, ui->pushButton_3, ui->pushButton_4,
        ui->pushButton_5, ui->pushButton_6, ui->pushButton_7, ui->pushButton_8,
        ui->pushButton_9, ui->pushButton_10, ui->pushButton_11, ui->pushButton_12,
        ui->pushButton_13, ui->pushButton_14, ui->pushButton_15, ui->pushButton_16,
    };

    // Set default styles and icons for buttons
    for (QPushButton* button : std::as_const(m_buttons)) {
        button->setIcon(back_icon);
        button->setIconSize(QSize(80, 100));
        button->setFlat(true);
        button->setStyleSheet(R"(
            QPushButton {
                border: none;
                background-color: transparent;
            }
            QPushButton:hover {
                background-color: #E6F9D5;
                border-radius: 10px;
                box-shadow: 0px 0px 15px rgba(0, 150, 0, 0.4);
                transform: scale(1.05);
            }
        )");
    }

    // Initialize matched flag for each button
    m_flagButtons = std::vector<bool>(m_buttons.size(), false);

    // Initialize and shuffle card images
    QStringList cardImages = {
        ":/resources/chakra.png", ":/resources/chakra.png",
        ":/resources/favourite.png", ":/resources/favourite.png",
        ":/resources/hexagon.png", ":/resources/hexagon.png",
        ":/resources/navigation.png", ":/resources/navigation.png",
        ":/resources/octahedron.png", ":/resources/octahedron.png",
        ":/resources/pyramid.png", ":/resources/pyramid.png",
        ":/resources/trefoil.png", ":/resources/trefoil.png",
        ":/resources/maple-leaf.png", ":/resources/maple-leaf.png",
    };

    // Seed the random number generator with a random device to ensure different shuffles
    std::random_device rd;
    std::mt19937 g(rd());  // Mersenne Twister engine

    // Shuffle the card images
    std::shuffle(cardImages.begin(), cardImages.end(), g);

    // Store the shuffled images
    m_cardImages = cardImages;

    // Style restart button
    ui->restartButton->setStyleSheet(R"(
        QPushButton {
            background-color: #81C784;  /* Lighter green */
            color: white;
            border: none;
            border-radius: 10px;
            padding: 10px 20px;
            font-size: 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #66BB6A;  /* Slightly darker on hover */
        }
        QPushButton:pressed {
            background-color: #4CAF50;  /* Classic green when pressed */
        }
    )");

    // Style timer label
    ui->timerLabel->setStyleSheet(R"(
        QLabel {
            color: #2E7D32;
            font-size: 18px;
            font-weight: bold;
            background-color: rgba(255, 255, 255, 180);
            padding: 6px 12px;
            border-radius: 8px;
        }
    )");

    // Initialize timer
    m_gameTimer = new QTimer(this);
    m_elapsedSeconds = 0;
}

void Widget::flipCard(int index)
{
    if (m_flagButtons[index] || m_isProcessing)
        return;

    qDebug() << "button" << index << "clicked";

    if (!m_timerStarted) {
        m_timerStarted = true;
        m_gameTimer->start(1000);
    }

    QString cardImage = m_cardImages[index];
    QIcon cardIcon(cardImage);
    m_buttons[index]->setIcon(cardIcon);

    if (m_lastPushedButton != -1 && index != m_lastPushedButton)
    {
        if (m_lastCardName != cardImage)
        {
            m_isProcessing = true;

            QTimer::singleShot(1000, this, [this, index]() {
                QIcon backIcon(":/resources/card-game.png");

                m_buttons[index]->setIcon(backIcon);
                m_buttons[m_lastPushedButton]->setIcon(backIcon);

                m_lastPushedButton = -1;
                m_isProcessing = false;
            });
        }
        else
        {
            m_flagButtons[index] = true;
            m_flagButtons[m_lastPushedButton] = true;
            m_lastPushedButton = -1;
            m_lastCardName.clear();

            // Check if all cards are matched
            if (std::all_of(m_flagButtons.begin(), m_flagButtons.end(), [](bool v) { return v; })) {
                m_gameTimer->stop();
                qDebug() << "Game finished in" << m_elapsedSeconds << "seconds.";
            }
        }
    }
    else
    {
        m_lastPushedButton = index;
        m_lastCardName = cardImage;
    }
}

void Widget::restartGame()
{
    if (m_isProcessing)
        return;

    // Seed the random number generator with a random device to ensure different shuffles
    std::random_device rd;
    std::mt19937 g(rd());  // Mersenne Twister engine

    // Shuffle the card images
    std::shuffle(m_cardImages.begin(), m_cardImages.end(), g);

    m_flagButtons.assign(m_buttons.size(), false);
    m_lastCardName.clear();
    m_lastPushedButton = -1;
    m_isProcessing = false;
    m_gameTimer->stop();
    m_elapsedSeconds = 0;
    m_timerStarted = false;
    ui->timerLabel->setText(QString("Time: %0 s").arg(m_elapsedSeconds));
}
