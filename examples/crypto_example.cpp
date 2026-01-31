#include <neocpp/neocpp.hpp>
#include <iostream>
#include <iomanip>

using namespace neocpp;

void printHex(const std::string& label, const Bytes& data) {
    std::cout << label << ": ";
    for (uint8_t byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::dec << std::endl;
}

int main() {
    try {
        // Generate a new key pair
        ECKeyPair keyPair = ECKeyPair::generate();
        std::cout << "Generated new key pair" << std::endl;
        
        // Get public key
        auto publicKey = keyPair.getPublicKey();
        printHex("Public key", publicKey->getEncoded());
        
        // Get address
        std::string address = keyPair.getAddress();
        std::cout << "Address: " << address << std::endl;
        
        // Export as WIF
        std::string wif = keyPair.exportAsWIF();
        std::cout << "WIF: " << wif << std::endl;
        
        // Sign a message
        std::string message = "Hello, Neo!";
        Bytes messageBytes(message.begin(), message.end());
        Bytes messageHash = HashUtils::sha256(messageBytes);
        
        auto signature = keyPair.sign(messageHash);
        printHex("Signature", signature->getBytes());
        
        // Verify signature
        bool valid = publicKey->verify(messageHash, signature);
        std::cout << "Signature valid: " << (valid ? "true" : "false") << std::endl;
        
        // Test NEP-2 encryption
        std::string password = "TestPassword123";
        std::string nep2 = NEP2::encrypt(keyPair, password);
        std::cout << "NEP-2 encrypted: " << nep2 << std::endl;
        
        // Decrypt NEP-2
        ECKeyPair decrypted = NEP2::decryptToKeyPair(nep2, password);
        std::cout << "Decrypted address: " << decrypted.getAddress() << std::endl;
        
        // Test hashing
        Bytes data = {0x01, 0x02, 0x03, 0x04};
        printHex("SHA256", HashUtils::sha256(data));
        printHex("RIPEMD160", HashUtils::ripemd160(data));
        printHex("Double SHA256", HashUtils::doubleSha256(data));
        printHex("SHA256 then RIPEMD160", HashUtils::sha256ThenRipemd160(data));
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}