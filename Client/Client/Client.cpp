#include"Client.h"
#include <fstream>
#include <string>
#include "rsa.h"
#include "modes.h"
#include "hex.h"
#include "crc.h"
#include "filters.h"
#include "osrng.h"
#include "base64.h"
#include <files.h>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

using namespace std; using namespace CryptoPP;
using namespace std;
Client::Client(uint8_t* clientName, uint32_t clientNameLen, ClientSocket* socket) {
	this->clientName = clientName;
	this->clientNameLen = clientNameLen;
	this->filePath = nullptr;
	this->recFileContent = nullptr;
	version = 3;
	this->socket = socket;
	this->hexClientId = "";
}
void Client::setClientId(uint8_t* clientId) {
	memcpy(this->clientId, clientId, CLIENT_ID);
	this->hexClientId = convert_to_hex_string(this->clientId, CLIENT_ID);
}

void Client::setHexClientId(string clientId) {
	this->hexClientId = clientId;
}
void Client::setPubKey(string pubKey) {
	this->pubKey = pubKey;
}
void Client::setAesKey(uint8_t* aesKey) {
	// Load the private key
	RSA::PrivateKey privateKey;
	FileSource file("private.pem", true);
	privateKey.Load(file);

	// Decode the received ciphertext
	std::string encoded_ciphertext(reinterpret_cast<const char*>(aesKey), aesKeyLen);
	std::vector<uint8_t> ciphertext(encoded_ciphertext.begin(), encoded_ciphertext.end());

	// Decrypt the ciphertext
	AutoSeededRandomPool rng;
	RSAES_OAEP_SHA_Decryptor d(privateKey);
	std::vector<uint8_t> decrypted_key(d.MaxPlaintextLength(ciphertext.size()));
	DecodingResult result = d.Decrypt(rng, ciphertext.data(), ciphertext.size(), decrypted_key.data());
	if (result.isValidCoding) {
		// Decryption was successful
		decrypted_key.resize(result.messageLength);
		// Extract the AES key from the decrypted plaintext
		std::vector<uint8_t> decoded_aes_key(decrypted_key.begin(), decrypted_key.begin() + AES::DEFAULT_KEYLENGTH);
		std::string key(decoded_aes_key.begin(), decoded_aes_key.end());
		this->aesKey = key;
	}
	else {
		this->aesKey = "";
	}
}
void Client::setAesKeyLen(uint32_t aesKeyLen) {
	this->aesKeyLen = aesKeyLen;
}
void Client::setFilePath(uint8_t* filePath) {
	this->filePath = filePath;
}
void Client::setFileName(string fileName) {
	this->fileName = fileName;
}
void Client::setChecksum(uint32_t checksum) {
	this->recChecksum = checksum;
}
void Client::setPubKeyLen(uint32_t pubKeyLen) {
	this->pubKeyLen = pubKeyLen;
}
void Client::setRecFileSize(uint32_t recFileSize) {
	this->recFileSize = recFileSize;
}

//common function
string Client::convert_to_hex_string(uint8_t* array, uint32_t len) {
	std::stringstream ss;
	ss << std::setfill('0') << std::hex << std::setw(2);
	for (int i = 0; i < len; i++) {
		ss << (int)array[i];
	}
	return ss.str();
}

int Client::create_rsa_key(){
	AutoSeededRandomPool rng;

	// Generate a new random RSA key
	RSA::PrivateKey privateKey;
	privateKey.GenerateRandomWithKeySize(rng, 2048);

	// Save the private key to a file
	FileSink privateFile("private.pem");
	privateKey.Save(privateFile);
	privateFile.MessageEnd();

	byte privateKeyArray[2048];
	// Send the public key to the server
	ArraySink sink(privateKeyArray, sizeof(privateKeyArray));
	privateKey.Save(sink);

	size_t written = sink.TotalPutLength();

	// Encode the byte array in base64 before sending it over the network
	StringSource base64Encoder(privateKeyArray, written, true,
		new Base64Encoder(
			new StringSink(this->priKey)
		)
	);

	// Save me file with the info
	std::ofstream meFile("me.info");
	if (meFile.is_open()) {
		meFile << this->clientName << endl;
		meFile << convert_to_hex_string(this->clientId, CLIENT_ID) << endl;
		meFile << this->priKey << endl;
	}
	meFile.close();

	this->priKeyLen = this->priKey.length();

	// Derive the public key from the private key
	RSA::PublicKey publicKey(privateKey);

	// Save the public key to a file
	FileSink publicFile("public.pem");
	publicKey.Save(publicFile);
	publicFile.MessageEnd();
	byte publicKeyArray[2048];
	// Send the public key to the server
	ArraySink pub_sink(publicKeyArray, sizeof(publicKeyArray));
	publicKey.Save(pub_sink);

	size_t pub_written = pub_sink.TotalPutLength();

	// Encode the byte array in base64 before sending it over the network
	StringSource pubBase64Encoder(publicKeyArray, pub_written, true,
		new Base64Encoder(
			new StringSink(this->pubKey)
		)
	);
	this->pubKeyLen = this->pubKey.length();

	return 0;
}
uint8_t* Client::getClientId() {
	return clientId;
}

string Client::getHexClientId() {
	return hexClientId;
}
uint8_t* Client::getClientName() {
	return clientName;
}
string Client::getPubKey() {
	return pubKey;
}
string Client::getAesKey() {
	return aesKey;
}
uint8_t* Client::getFilePath() {
	return filePath;
}
string Client::getFileName() {
	return fileName;
}
uint8_t Client::getVersion() {
	return version;
}
uint32_t Client::getClientNameLen() {
	return clientNameLen;
}

uint32_t Client::getPubKeyLen() {
	return pubKeyLen;
}
ClientSocket* Client::getSocket() {
	return socket;
}
void Client::setRecFileContent(uint8_t* fileContent) {
	recFileContent = fileContent;
}

std::vector<byte> read_file(std::string const& fileName) {
	std::ifstream ifs(fileName, std::ios::binary | std::ios::ate);
	std::ifstream::pos_type pos = ifs.tellg();

	std::vector<byte> result(pos);

	ifs.seekg(0, std::ios::beg);
	ifs.read(reinterpret_cast<char*>(result.data()), pos);

	return result;
}

void write_to_file(std::vector<byte> const& data, std::string const& fileName) {
	std::ofstream ofs(fileName, std::ios::binary);
	ofs.write(reinterpret_cast<char const*>(data.data()), data.size());
	ofs.close();
}

std::vector<uint8_t> Client::getFileContent(uint32_t* EncFileLen) {

	// Convert the key string to a byte array
	byte key[AES::DEFAULT_KEYLENGTH];
	memcpy(key, aesKey.data(), min((size_t)AES::DEFAULT_KEYLENGTH, aesKey.size()));

	// Set up the AES IV (16 bytes)
	byte iv[AES::BLOCKSIZE];
	memset(iv, 0x0, AES::BLOCKSIZE);
	
	// Read the plaintext file
	std::vector<byte> plaintext = read_file(fileName);
	std::vector<byte> ciphertext(plaintext.size() + CryptoPP::AES::BLOCKSIZE);

	// Encrypt the plaintext
	CryptoPP::AES::Encryption aesEncryption(reinterpret_cast<byte*>(&key[0]), CryptoPP::AES::DEFAULT_KEYLENGTH);
	CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, reinterpret_cast<byte*>(&iv[0]));
	CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::ArraySink(ciphertext.data(), ciphertext.size()), CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING);
	stfEncryptor.Put(plaintext.data(), plaintext.size());
	stfEncryptor.MessageEnd();

	// Write the encrypted data to a file
	write_to_file(ciphertext, "ciphertext.enc");
	// read the content of the file into vector
	std::vector<byte> cipher_vec = read_file("ciphertext.enc");
	std::vector<uint8_t> array(cipher_vec.begin(), cipher_vec.end());


	*EncFileLen = array.size();
	return array;
}

bool Client::calcChecksum() {
	// Read the plaintext file
	std::vector<byte> plaintext = read_file(fileName);

	// Calculate the CRC checksum
	CRC32 crc;
	byte digest[CryptoPP::CRC32::DIGESTSIZE];
	crc.CalculateDigest(digest, plaintext.data(), plaintext.size());

	// Get the result in 32 bit format
	uint32_t crc32 = *((uint32_t*)digest);

	// Print the checksum
	cout << "Checksum: " << crc32 << endl;
	this->checksum = crc32;
	if (this->checksum == this->recChecksum)
		return true;
	return false;
}
int Client::recResponseHeader(response * res) {
	uint8_t* header = (uint8_t*)malloc(sizeof(response));

	int err = this->socket->RecData(sizeof(response), header);
	memcpy_s(res, sizeof(response), header, sizeof(response));
	return err;
}

void Client::closeSession() {
	socket->CloseSession();
}