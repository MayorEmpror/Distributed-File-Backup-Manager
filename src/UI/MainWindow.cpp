#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    startBtn = new QPushButton("Start Backup");
    stopBtn = new QPushButton("Stop Backup");
    logBox = new QTextEdit();

    logBox->setReadOnly(true);

    layout->addWidget(startBtn);
    layout->addWidget(stopBtn);
    layout->addWidget(logBox);

    setCentralWidget(central);

    connect(startBtn, &QPushButton::clicked,
            this, &MainWindow::onStartClicked);

    connect(stopBtn, &QPushButton::clicked,
            this, &MainWindow::onStopClicked);
}

void MainWindow::log(const QString& msg) {
    logBox->append(msg);
}

void MainWindow::onStartClicked() {
    log("Backup System Started...");
}

void MainWindow::onStopClicked() {
    log("Backup System Stopped...");
}