#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>

using namespace std::chrono;

using namespace std;

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
    if (fulltextAfterDecode == fullText)
        cout << "Test Pass" << endl;
    else
        cout << "Test Fail" << endl;
}


int main(int argc, char * argv[]) {

    string line;
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
    ifstream myReadFile("../" + nameOfFile);
    if(myReadFile.fail()){
        cout<<"not proper file opening"<<endl;
        exit(1);
    }


    std::stringstream buffer;
    buffer << myReadFile.rdbuf();
    fullText = buffer.str();
    myReadFile.seekg(0);
    fullText = "";
    4;
//  szyfrowanie
    auto start = high_resolution_clock::now();

    while (getline(myReadFile, line)) {
        fullText.append(line);
        for (char ch:line) {
            characterAsBits = change_Character_to_bits_string(ch);
            key = toBinary((rand() % 128 + 128));
            fullKey.append(key);
            ciphertext = code(characterAsBits, key);
            fullCiphertext.append(ciphertext);
        }

    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by program: " << duration.count() << " us" << endl;

//  deszyfrowanie
    int i;
    for (i = 0; i < fullCiphertext.length(); i += 8) {
        textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
        fulltextAfterDecode += bitsToChar(textAfterDecode);
    }



    test(fulltextAfterDecode, fullText);
}