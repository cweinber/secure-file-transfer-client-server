#pragma once
#include <cstdint>
#include <atomic>
#include "ClientSocket.h"
#include "Response.h"
#include <string>
#include <vector>
using namespace std;

class Client {
private:
    uint8_t* clientName;
    uint32_t clientNameLen;
    uint8_t clientId[CLIENT_ID];
    string hexClientId;
    uint8_t* prKey;
    string priKey;
    string pubKey;
    uint32_t priKeyLen;
    uint32_t pubKeyLen;
    string aesKey;
    uint32_t aesKeyLen;
    uint8_t* filePath;
    string fileName;
    uint8_t version;
    ClientSocket* socket;
    uint8_t* recFileContent;
    uint32_t recChecksum;
    uint32_t recFileSize;
    uint32_t checksum;
   
public:
    Client(uint8_t* clientName, uint32_t clientNameLen, ClientSocket* socket);
    void setClientId(uint8_t* clientId);
    void setHexClientId(string clientId);
    void setPubKey(string pubKey);
    void setAesKey(uint8_t* aesKey);
    void setAesKeyLen(uint32_t aesKeyLen);
    void setFilePath(uint8_t* filePath);
    void setFileName(string fileName);
    void setChecksum(uint32_t checksum);
    void setRecFileSize(uint32_t recFileSize);
    void setPubKeyLen(uint32_t pubKeyLen);
    int create_rsa_key();
    string convert_to_hex_string(uint8_t* array, uint32_t len);

    uint8_t* getClientId();
    string getHexClientId();
    uint8_t* getClientName();
    string getAesKey();
    string getPubKey();
    uint8_t* getFilePath();
    string getFileName();
    uint8_t getVersion();
    uint32_t getPubKeyLen();
    uint32_t getClientNameLen();
    std::vector<uint8_t> getFileContent(uint32_t* EncFileLen);
    ClientSocket* getSocket();
    void setRecFileContent(uint8_t* fileContent);
    bool calcChecksum();
    void closeSession();
    int recResponseHeader(response *res);
};
