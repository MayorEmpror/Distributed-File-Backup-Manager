#ifndef CLIENT_H
#define CLIENT_H

#include "IClient.h"
#include <functional>
#include <string>

class Client : public IClient {
private:
    int clientId;
    std::string name;

    // callback to send request to system
    std::function<void(const BackupRequest&)> requestHandler;

public:
    Client(int id, const std::string& name);

    void setRequestHandler(std::function<void(const BackupRequest&)> handler);

    int getId() const override;
    void generateRequest() override;
};

#endif