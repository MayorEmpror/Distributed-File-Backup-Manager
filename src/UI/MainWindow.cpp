#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QWidget>
#include <QMetaObject>
#include <QString>
#include <QHeaderView>
#include <QDateTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), manager(4, 2), started(false) {

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setSpacing(10);
    layout->setContentsMargins(14, 14, 14, 14);

    startBtn = new QPushButton("Start");
    stopBtn = new QPushButton("Stop");
    logBox = new QTextEdit();
    progressBar = new QProgressBar();
    speedDial = new QDial();
    speedLabel = new QLabel("Speed Dial: 5");
    processedLabel = new QLabel("Total requests\n0");
    failedLabel = new QLabel("Failures\n0");
    queueLabel = new QLabel("Queue depth\n0");
    workersLabel = new QLabel("Active workers\n0");
    compressionLabel = new QLabel("Compression\n0%");
    searchInput = new QLineEdit();
    sourceFilter = new QComboBox();
    requestsTable = new QTableWidget();
    compressionChart = new LineChartWidget();
    queueChart = new LineChartWidget();
    failureChart = new LineChartWidget();

    logBox->setReadOnly(true);
    logBox->setMinimumHeight(120);
    logBox->setMaximumHeight(160);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    speedDial->setRange(1, 10);
    speedDial->setValue(5);
    speedDial->setNotchesVisible(true);
    speedDial->setFixedSize(64, 64);
    searchInput->setPlaceholderText("Search by file/client...");
    sourceFilter->addItems(QStringList() << "All Sources" << "Booking.com" << "Expedia" << "airbnb");

    requestsTable->setColumnCount(9);
    requestsTable->setHorizontalHeaderLabels(
        QStringList() << "ID" << "Client" << "File" << "Size" << "Stored" << "Source" << "Status" << "Compression" << "Time"
    );
    requestsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    requestsTable->verticalHeader()->setVisible(false);
    requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    requestsTable->setAlternatingRowColors(true);

    compressionChart->setTitle("Compression Ratio");
    compressionChart->setLineColor(QColor("#3B82F6"));
    queueChart->setTitle("Queue Pressure");
    queueChart->setLineColor(QColor("#F59E0B"));
    failureChart->setTitle("Failure Pressure");
    failureChart->setLineColor(QColor("#EF4444"));

    QHBoxLayout* headerBar = new QHBoxLayout();
    QLabel* title = new QLabel("Backup Operations Dashboard");
    title->setObjectName("titleLabel");
    headerBar->addWidget(title);
    headerBar->addStretch();
    headerBar->addWidget(searchInput);
    headerBar->addWidget(sourceFilter);
    headerBar->addWidget(startBtn);
    headerBar->addWidget(stopBtn);
    headerBar->addWidget(speedLabel);
    headerBar->addWidget(speedDial);

    auto makeCard = [](QLabel* label) -> QFrame* {
        QFrame* card = new QFrame();
        card->setObjectName("kpiCard");
        QVBoxLayout* c = new QVBoxLayout(card);
        c->addWidget(label);
        return card;
    };

    QHBoxLayout* kpiRow = new QHBoxLayout();
    kpiRow->addWidget(makeCard(processedLabel));
    kpiRow->addWidget(makeCard(failedLabel));
    kpiRow->addWidget(makeCard(queueLabel));
    kpiRow->addWidget(makeCard(workersLabel));
    kpiRow->addWidget(makeCard(compressionLabel));

    QHBoxLayout* chartRow = new QHBoxLayout();
    chartRow->addWidget(compressionChart);
    chartRow->addWidget(queueChart);
    chartRow->addWidget(failureChart);

    layout->addLayout(headerBar);
    layout->addLayout(kpiRow);
    layout->addWidget(progressBar);
    layout->addLayout(chartRow);
    layout->addWidget(requestsTable);
    layout->addWidget(logBox);

    setCentralWidget(central);
    setWindowTitle("Distributed File Backup Manager - Analytics");
    setStyleSheet(
        "QMainWindow, QWidget { background-color: #f5f7fb; color: #1f2937; }"
        "#titleLabel { font-size: 22px; font-weight: 700; }"
        "#kpiCard { background: white; border: 1px solid #e5e7eb; border-radius: 10px; }"
        "QPushButton { background: #22c55e; color: white; border-radius: 12px; padding: 8px 14px; }"
        "QPushButton:hover { background: #16a34a; }"
        "QLineEdit, QComboBox { background: white; border: 1px solid #d1d5db; border-radius: 10px; padding: 6px 10px; }"
        "QTableWidget { background: white; border: 1px solid #e5e7eb; border-radius: 10px; }"
        "QHeaderView::section { background: #f3f4f6; border: none; padding: 8px; font-weight: 600; }"
    );

    connect(startBtn, &QPushButton::clicked,
            this, &MainWindow::onStartClicked);

    connect(stopBtn, &QPushButton::clicked,
            this, &MainWindow::onStopClicked);
    connect(speedDial, &QDial::valueChanged,
            this, &MainWindow::onSpeedChanged);
    connect(searchInput, &QLineEdit::textChanged,
            this, &MainWindow::onFilterChanged);
    connect(sourceFilter, &QComboBox::currentTextChanged,
            this, &MainWindow::onFilterChanged);

    manager.setLogCallback([this](const std::string& msg) {
        QMetaObject::invokeMethod(this, [this, msg]() {
            log(QString::fromStdString(msg));
        }, Qt::QueuedConnection);
    });

    manager.setProgressCallback([this](int value) {
        QMetaObject::invokeMethod(this, [this, value]() {
            progressBar->setValue(value);
            compressionChart->addPoint(value);
            compressionLabel->setText("Compression\n" + QString::number(value) + "%");
        }, Qt::QueuedConnection);
    });

    manager.setStatsCallback([this](const BackupManager::Stats& stats) {
        QMetaObject::invokeMethod(this, [this, stats]() {
            processedLabel->setText("Total requests\n" + QString::number(static_cast<qulonglong>(stats.processed)));
            failedLabel->setText("Failures\n" + QString::number(static_cast<qulonglong>(stats.failed)));
            queueLabel->setText("Queue depth\n" + QString::number(static_cast<qulonglong>(stats.queueDepth)));
            workersLabel->setText("Active workers\n" + QString::number(stats.activeWorkers));
            queueChart->addPoint(static_cast<int>(stats.queueDepth > 100 ? 100 : stats.queueDepth));
            int failurePressure = stats.processed > 0
                ? static_cast<int>((100.0 * static_cast<double>(stats.failed)) / static_cast<double>(stats.processed))
                : 0;
            failureChart->addPoint(failurePressure);
        }, Qt::QueuedConnection);
    });
    manager.setRequestCallback([this](const BackupManager::RequestEvent& event) {
        QMetaObject::invokeMethod(this, [this, event]() {
            addRequestRow(event);
            applyTableFilter();
        }, Qt::QueuedConnection);
    });

    simulator.setDispatcher([this](const BackupRequest& req) {
        manager.submit(req);
    });
    onSpeedChanged(speedDial->value());
}

MainWindow::~MainWindow() {
    onStopClicked();
    for (std::size_t i = 0; i < clients.size(); ++i) {
        delete clients[i];
    }
    clients.clear();
}

void MainWindow::log(const QString& msg) {
    logBox->append(msg);
}

void MainWindow::onStartClicked() {
    if (started) {
        log("System already running.");
        return;
    }

    started = true;
    progressBar->setValue(0);
    requestsTable->setRowCount(0);
    log("Backup system started...");

    if (clients.empty()) {
        for (int i = 0; i < 10; ++i) {
            Client* c = new Client(i + 1, "Client_" + std::to_string(i + 1));
            clients.push_back(c);
            simulator.addClient(c);
        }
    }

    manager.start();
    simulator.start();
}

void MainWindow::onStopClicked() {
    if (!started) {
        return;
    }

    started = false;
    simulator.stop();
    simulator.wait();
    manager.stop();
    manager.wait();
    log("Backup system stopped.");
}

void MainWindow::onSpeedChanged(int value) {
    speedLabel->setText("Speed Dial: " + QString::number(value));
    int minMs = 80 + (10 - value) * 40;
    int maxMs = minMs + 250;
    simulator.setPacing(minMs, maxMs);
}

void MainWindow::addRequestRow(const BackupManager::RequestEvent& event) {
    int row = requestsTable->rowCount();
    requestsTable->insertRow(row);
    requestsTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    requestsTable->setItem(row, 1, new QTableWidgetItem(QString::number(event.clientId)));
    requestsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(event.fileName)));
    requestsTable->setItem(row, 3, new QTableWidgetItem(QString::number(static_cast<qulonglong>(event.fileSize))));
    requestsTable->setItem(row, 4, new QTableWidgetItem(QString::number(static_cast<qulonglong>(event.storedBytes))));
    requestsTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(event.source)));
    requestsTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(event.status)));
    requestsTable->setItem(row, 7, new QTableWidgetItem(QString::number(event.compressionPct) + "%"));
    requestsTable->setItem(row, 8, new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss")));
    requestsTable->scrollToBottom();
}

void MainWindow::applyTableFilter() {
    QString source = sourceFilter->currentText();
    QString query = searchInput->text().trimmed();
    for (int r = 0; r < requestsTable->rowCount(); ++r) {
        QString file = requestsTable->item(r, 2)->text();
        QString client = requestsTable->item(r, 1)->text();
        QString rowSource = requestsTable->item(r, 5)->text();
        bool sourceOk = (source == "All Sources") || (rowSource == source);
        bool queryOk = query.isEmpty() || file.contains(query, Qt::CaseInsensitive) || client.contains(query, Qt::CaseInsensitive);
        requestsTable->setRowHidden(r, !(sourceOk && queryOk));
    }
}

void MainWindow::onFilterChanged() {
    applyTableFilter();
}