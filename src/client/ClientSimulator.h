#ifndef CLIENT_SIMULATOR_H
#define CLIENT_SIMULATOR_H

#include "Client.h"
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

class ClientSimulator {
private:
    std::vector<Client*> clients;
    std::vector<std::thread> threads;

    std::atomic<bool> running;

    // central dispatch callback (to job queue later)
    std::function<void(const BackupRequest&)> dispatcher;

    void runClient(Client* client);

public:
    ClientSimulator();

    void setDispatcher(std::function<void(const BackupRequest&)> func);

    void addClient(Client* client);

    void start();
    void stop();
    void wait();
};

#endif