#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;
using namespace std::chrono;

typedef void (*encryption_fun_type)(string&, string&, string&, string&,
                                    string&, string&, vector<string>&);


// Split string by newline character
vector<string> splitString(const string& str) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, '\n')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Read the text file
string readTextFile(const string& filePath) {
    string fullText;
    ifstream myReadFile(filePath);
    if (myReadFile.fail()) {
        cout << "Error while opening the file - check if the filepath is correct" << endl;
        exit(1);
    }

    stringstream buffer;
    buffer << myReadFile.rdbuf();
    fullText = buffer.str();
    return fullText;
}

// Select the text file, read it and split into lines vector
vector<string> readFileAndSplitIntoLines(const string& filePath) {
    string fullText = readTextFile(filePath);
    return splitString(fullText);
}

// Convert string of bits to a character
char bitsToChar(const string& str) {
    char parsed = 0;
    for (int i = 0; i < 8; ++i) {
        if (str[i] == '1')
            parsed |= 1 << (7 - i);
    }
    return parsed;
}

// Convert integer to binary string
string toBinary(int n) {
    string r;
    while (n != 0) {
        r = (n % 2 == 0 ? "0" : "1") + r;
        n /= 2;
    }
    return r;
}

// XOR encryption logic
string code(const string& text, const string& key) {
    string ciphertext(text.length(), ' ');
    for (size_t i = 0; i < text.length(); ++i) {
        ciphertext[i] = (text[i] == key[i]) ? '0' : '1';
    }
    return ciphertext;
}

// Decode ciphertext using key
string decode(const string& ciphertext, const string& key) {
    return code(ciphertext, key);
}

// Convert character to binary string
string changeCharacterToBitsString(char ch) {
    return bitset<8>(ch).to_string();
}

// Test encryption and decryption
void test(const string& fulltextAfterDecode, const string& fullText) {
    string cleanFullText = fullText;
    cleanFullText.erase(remove(cleanFullText.begin(), cleanFullText.end(), '\n'), cleanFullText.cend());
    if (fulltextAfterDecode == cleanFullText)
        cout << "Test Pass" << endl;
    else {

        cout << "Test Fail" << endl;
        cout << fullText << endl << endl;
        cout << fulltextAfterDecode << endl;
    }
}

// Encryption without parallelization
void encryptionWithoutParallelizing(string& characterAsBits, string& fullText, string& key, string& fullKey,
                                    string& ciphertext, string& fullCiphertext, vector<string>& fullTextAsArray) {
    for (const auto& line : fullTextAsArray) {
        for (char ch : line) {
            characterAsBits = changeCharacterToBitsString(ch);
            key = toBinary(rand() % 128 + 128);
            fullKey.append(key);
            ciphertext = code(characterAsBits, key);
            fullCiphertext.append(ciphertext);
        }
    }
}

// Run encryption routine
void runEncryption(encryption_fun_type fun, string& characterAsBits, string& fullText, string& key, string& fullKey,
                   string& ciphertext, string& fulltextAfterDecode, string& textAfterDecode, string& fullCiphertext,
                   vector<string>& fullTextAsArray, long& calculation_time) {
    auto start = high_resolution_clock::now();

    fun(characterAsBits, fullText, key, fullKey, ciphertext, fullCiphertext, fullTextAsArray);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    calculation_time = duration.count();
    cout << "Time taken: " << calculation_time << " us" << endl;

    // Decryption process
    for (size_t i = 0; i < fullCiphertext.length(); i += 8) {
        textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
        fulltextAfterDecode += bitsToChar(textAfterDecode);
    }

    test(fulltextAfterDecode, fullText);
    fullKey.clear();
    fulltextAfterDecode.clear();
    fullCiphertext.clear();
}

// Main function
int main() {
    string characterAsBits, fullText, key, fullKey, ciphertext, textAfterDecode, fulltextAfterDecode, fullCiphertext;
    encryption_fun_type ptr;
    long calculation_time;
    srand(time(nullptr));

    fullText = readTextFile("lalka.txt");
    vector<string> fullTextAsArray = readFileAndSplitIntoLines("lalka.txt");

    ptr = encryptionWithoutParallelizing;
    runEncryption(ptr, characterAsBits, fullText, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, fullTextAsArray, calculation_time);

    return 0;
}