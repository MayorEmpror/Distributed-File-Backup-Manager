#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QPushButton* startBtn;
    QPushButton* stopBtn;
    QTextEdit* logBox;

public:
    MainWindow(QWidget* parent = nullptr);

    void log(const QString& msg);

public slots:
    void onStartClicked();
    void onStopClicked();
};

#endif