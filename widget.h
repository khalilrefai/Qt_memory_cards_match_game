#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QIcon>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void flipCard(int index);
    void restartGame();

private:
    Ui::Widget *ui;

    void initializeUI();

    int m_lastPushedButton = -1;
    int m_elapsedSeconds = 0;

    QString m_lastCardName = "";
    QStringList m_cardImages;
    QList<QPushButton*> m_buttons;
    QTimer *m_gameTimer;

    bool m_timerStarted = false;
    bool m_isProcessing = false;
    std::vector<bool> m_flagButtons;
};

#endif // WIDGET_H
