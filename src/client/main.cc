#include "ClientSimulator.h"
#include <iostream>
#include <chrono>
#include <thread>

void dispatchRequest(const BackupRequest& req) {
    std::cout << "[DISPATCH] Client " << req.clientId
              << " -> " << req.fileName
              << " (" << req.fileSize << " bytes)" << std::endl;
}

int main() {
    ClientSimulator simulator;

    simulator.setDispatcher(dispatchRequest);

    simulator.addClient(new Client(1, "Alpha"));
    simulator.addClient(new Client(2, "Beta"));
    simulator.addClient(new Client(3, "Gamma"));

    simulator.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    simulator.stop();
    simulator.wait();

    return 0;
}