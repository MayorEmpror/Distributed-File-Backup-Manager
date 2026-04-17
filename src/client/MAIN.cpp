//this file is to be deleteed this just gives the usecase :
#include "ClientSimulator.h"
#include <iostream>

void handleRequest(const BackupRequest& req) {
    std::cout << "[DISPATCH] Client " << req.clientId
              << " -> " << req.fileName
              << " (" << req.fileSize << " bytes)\n";
}

int main() {
    ClientSimulator simulator;

    simulator.setDispatcher(handleRequest);

    simulator.addClient(new Client(1, "Alpha"));
    simulator.addClient(new Client(2, "Beta"));
    simulator.addClient(new Client(3, "Gamma"));

    simulator.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    simulator.stop();
    simulator.wait();

    return 0;
}