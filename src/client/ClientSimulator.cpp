#include "ClientSimulator.h"
#include <chrono>
#include <cstdlib>

ClientSimulator::ClientSimulator() : running(false) {}

void ClientSimulator::setDispatcher(std::function<void(const BackupRequest&)> func) {
    dispatcher = func;
}

void ClientSimulator::addClient(Client* client) {
    client->setRequestHandler(dispatcher);
    clients.push_back(client);
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

    for (size_t i = 0; i < clients.size(); i++) {
        threads.push_back(std::thread(&ClientSimulator::runClient, this, clients[i]));
    }
}

void ClientSimulator::stop() {
    running = false;
}

void ClientSimulator::wait() {
    for (size_t i = 0; i < threads.size(); i++) {
        if (threads[i].joinable())
            threads[i].join();
    }
}