#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include "omp.h"
using namespace std::chrono;

using namespace std;

typedef void (*encrytption_fun_type)(string &, string &, string &, string &,
                                     string &, string &, vector<string> &);

std::vector<std::string> splitString(const std::string &str) {
    std::vector<std::string> tokens;

    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, '\n')) {
        tokens.push_back(token);
    }

    return tokens;
}

char bitsToChar(string str) {
    char parsed = 0;
    for (int i = 0; i < 8; i++)
        if (str[i] == '1')
            parsed |= 1 << (7 - i);
    return parsed;
}

string toBinary(int n) {
    string r;
    while (n != 0) {
        r = (n % 2 == 0 ? "0" : "1") + r;
        n /= 2;
    }
    return r;
}

string code(const string &text, const string &key) {
    string ciphertext = string(8, ' ');

    for (int i = 0; i < text.length(); i++) {
        if (text[i] == '0')
            if (key[i] == '0')
                ciphertext[i] = '0';
            else
                ciphertext[i] = '1';
        else if (key[i] == '0')
            ciphertext[i] = '1';
        else
            ciphertext[i] = '0';
    }
    return ciphertext;
}

string decode(const string &ciphertext, const string &key) {
    return code(ciphertext, key);
}

string change_Character_to_bits_string(char ch) {
    return bitset<8>(ch).to_string();
}

void test(string fulltextAfterDecode, string fullText) {
    fullText.erase(std::remove(fullText.begin(), fullText.end(), '\n'), fullText.cend());
    if (fulltextAfterDecode == fullText)
        cout << "Test Pass" << endl;
    else {
        cout << "Test Fail" << endl;

//        for (int i = 0; i < 20; ++i) {
//            for (int j = 0; j < i; ++j) {
//                cout << "\t\t";
//            }
//            cout << "Test Fail" << endl;
//        }
    }
}


void encryption_without_parallelizing(string &characterAsBits, string &fullText, string &key, string &fullKey,
                                      string &ciphertext, string &fullCiphertext, vector<string> &fullTextAsArray) {
    for (string line: fullTextAsArray) {
        for (char ch: line) {
            characterAsBits = change_Character_to_bits_string(ch);
            key = toBinary((rand() % 128 + 128));
            fullKey.append(key);
            ciphertext = code(characterAsBits, key);
            fullCiphertext.append(ciphertext);
        }
    }
}

void encryption_OpenMP(string &characterAsBits, string &fullText, string &key, string &fullKey,
                       string &ciphertext, string &fullCiphertext, vector<string> &fullTextAsArray) {
    #pragma omp parallel
    for (string line: fullTextAsArray)
    {
        for (char ch: line) {
            characterAsBits = change_Character_to_bits_string(ch);
            key = toBinary((rand() % 128 + 128));
            fullKey.append(key);
            ciphertext = code(characterAsBits, key);
            fullCiphertext.append(ciphertext);
        }
    }
}

void run_encryption(encrytption_fun_type fun, string &characterAsBits, string &fullText, string &key, string &fullKey,
                    string &ciphertext,
                    string &fulltextAfterDecode, string &textAfterDecode, string &fullCiphertext,
                    vector<string> &fullTextAsArray) {//  szyfrowanie
    auto start = high_resolution_clock::now();

    fun(characterAsBits, fullText, key, fullKey, ciphertext, fullCiphertext,
                                     fullTextAsArray);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken: " << duration.count() << " us" << endl;

//  deszyfrowanie

    for (int i = 0; i < fullCiphertext.length(); i += 8) {
        textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
        fulltextAfterDecode += bitsToChar(textAfterDecode);
    }

    test(fulltextAfterDecode, fullText);
    fullKey = "";
    fulltextAfterDecode = "";
    fullCiphertext = "";
}
int main(int argc, char *argv[]) {
    omp_set_num_threads(16);

    string characterAsBits;
    string fullText;
    string key;
    string fullKey;
    string ciphertext;
    string textAfterCode;
    string fulltextAfterDecode;
    string textAfterDecode;
    string fullCiphertext;
    srand(time(nullptr));

    string nameOfFile = "alice.txt";

    ifstream myReadFile("" + nameOfFile);
    if (myReadFile.fail()) {
        cout << "My error: not proper file opening" << endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << myReadFile.rdbuf();
    fullText = buffer.str();
    vector<string> fullTextAsArray = splitString(fullText);


    encrytption_fun_type ptr;

    ptr = encryption_without_parallelizing;
    run_encryption(ptr, characterAsBits, fullText, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode,
                   fullCiphertext,
                   fullTextAsArray);


    ptr = encryption_OpenMP;
    run_encryption(ptr, characterAsBits, fullText, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode,
                   fullCiphertext,
                   fullTextAsArray);
    return 0;
}