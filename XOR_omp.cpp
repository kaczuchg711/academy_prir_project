#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <vector>
#include <omp.h>

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
        cout << "Test result: Pass" << endl;
    else {

        cout << "Test result: Fail" << endl;
        cout << fullText << endl << endl;
        cout << fulltextAfterDecode << endl;
    }
}

// Encryption using OpenMP (commented as this code base does not include OpenMP setup)
void encryptionOpenMP(string &characterAsBits, string &fullText, string &key, string &fullKey,
                      string &ciphertext, string &fullCiphertext, vector<string> &fullTextAsArray) {

    vector<pair<int, string>> orderedCiphertext(fullTextAsArray.size());
    vector<pair<int, string>> orderedKeys(fullTextAsArray.size());
#pragma omp parallel for private(characterAsBits, key, ciphertext)
    for (int i = 0; i < fullTextAsArray.size(); ++i) {
        string localKey, localCiphertext;
        unsigned int threadSeed = omp_get_thread_num() + time(NULL); // Unique seed per thread

        for (char ch : fullTextAsArray[i]) {
            characterAsBits = changeCharacterToBitsString(ch);
            // Use thr2ead-safe random number generator with the thread-specific seed
            key = toBinary(rand_r(&threadSeed) % 128 + 128);
            localKey.append(key);
            ciphertext = code(characterAsBits, key);
            localCiphertext.append(ciphertext);
        }

        orderedCiphertext[i] = make_pair(i, localCiphertext);
        orderedKeys[i] = make_pair(i, localKey);
    }

    // Concatenate in correct order
    for (int i = 0; i < fullTextAsArray.size(); ++i) {
        fullCiphertext.append(orderedCiphertext[i].second);
        fullKey.append(orderedKeys[i].second);
    }
}

// Run encryption routine
void runEncryption(encryption_fun_type fun, string& characterAsBits, string& fullText, string& key, string& fullKey,
                   string& ciphertext, string& fulltextAfterDecode, string& textAfterDecode, string& fullCiphertext,
                   vector<string>& fullTextAsArray, long& calculation_time) {
    

    fun(characterAsBits, fullText, key, fullKey, ciphertext, fullCiphertext, fullTextAsArray);

    // Decryption process
    // for (size_t i = 0; i < fullCiphertext.length(); i += 8) {
    //     textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
    //     fulltextAfterDecode += bitsToChar(textAfterDecode);
    // }

    // test(fulltextAfterDecode, fullText);
    fullKey.clear();
    fulltextAfterDecode.clear();
    fullCiphertext.clear();
}

// Main function
int main(int argc, char** argv) {
    string characterAsBits, fullText, key, fullKey, ciphertext, textAfterDecode, fulltextAfterDecode, fullCiphertext;
    encryption_fun_type ptr;
    long calculation_time;
    srand(time(nullptr));

    auto start = high_resolution_clock::now();

    fullText = readTextFile(argv[1]);
    vector<string> fullTextAsArray = readFileAndSplitIntoLines(argv[1]);

    int number_of_threads = atoi(argv[2]);
    omp_set_num_threads(number_of_threads);

    ptr = encryptionOpenMP;
    runEncryption(ptr, characterAsBits, fullText, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, fullTextAsArray, calculation_time);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    calculation_time = duration.count();
    // cout << "Time taken: " << calculation_time/1000. << " ms" << endl;

    return 0;
}