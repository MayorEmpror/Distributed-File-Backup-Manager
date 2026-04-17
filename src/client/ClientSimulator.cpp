#include "ClientSimulator.h"
#include <chrono>
#include <cstdlib>

ClientSimulator::ClientSimulator() {
    clientCount = 0;
    running = false;
}

void ClientSimulator::setDispatcher(std::function<void(const BackupRequest&)> func) {
    dispatcher = func;
}

void ClientSimulator::addClient(Client* client) {
    if (clientCount >= MAX_CLIENTS)
        return;

    client->setRequestHandler(dispatcher);
    clients[clientCount++] = client;
}

void ClientSimulator::runClient(Client* client) {
    while (running) {
        client->generateRequest();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(200 + rand() % 800)
        );
    }
}

void ClientSimulator::start() {
    running = true;

    int i;
    for (i = 0; i < clientCount; i++) {
        threads[i] = std::thread(&ClientSimulator::runClient, this, clients[i]);
    }
}

void ClientSimulator::stop() {
    running = false;
}

void ClientSimulator::wait() {
    int i;
    for (i = 0; i < clientCount; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
}