#include <bitset>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <vector>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

typedef void (*encryption_fun_type)(string&, string&, string&,
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
    // ifstream myReadFile(filePath);
    // if (myReadFile.fail()) {
    //     cout << "Error while opening the file - check if the filepath is correct" << endl;
    //     exit(1);
    // }

    // stringstream buffer;
    // buffer << myReadFile.rdbuf();
    // fullText = buffer.str();
    return splitString(fullText);
}

// Splitting the vector depending on processes amount  
template <typename T>
vector<vector<T>> split_vector(const vector<T>& vec, size_t n) {
    vector<vector<T>> result;
    size_t length = vec.size() / n;
    size_t remain = vec.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < min(n, vec.size()); ++i) {
        end += (remain > 0) ? (length + !!(remain--)) : length;

        result.push_back(vector<T>(vec.begin() + begin, vec.begin() + end));

        begin = end;
    }

    return result;
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
    string cleanFullTextAfterDecode = fulltextAfterDecode;

    cleanFullText.erase(remove(cleanFullText.begin(), cleanFullText.end(), '\n'), cleanFullText.cend());
    cleanFullTextAfterDecode.erase(remove(cleanFullTextAfterDecode.begin(), cleanFullTextAfterDecode.end(), '\n'), cleanFullTextAfterDecode.cend());

    if (cleanFullTextAfterDecode == cleanFullText) {
        cout << "Test Pass" << endl;
    } else {
        cout << "Test Fail" << endl;
        cout << fullText << endl << endl;
        cout << cleanFullTextAfterDecode << endl;
    }
}

// Encryption without parallelization
void encryptionWithoutParallelizing(string& characterAsBits, string& key, string& fullKey,
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
string runEncryption(encryption_fun_type fun, string& characterAsBits, string& key, string& fullKey,
                   string& ciphertext, string& fulltextAfterDecode, string& textAfterDecode, string& fullCiphertext,
                   vector<string>& fullTextAsArray) {
    // auto start = high_resolution_clock::now();

    fun(characterAsBits, key, fullKey, ciphertext, fullCiphertext, fullTextAsArray);

    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    // calculation_time = duration.count();
    // cout << "Time taken: " << calculation_time << " us" << endl;

    // Decryption process
    for (size_t i = 0; i < fullCiphertext.length(); i += 8) {
        textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
        fulltextAfterDecode += bitsToChar(textAfterDecode);
    }

    fullKey.clear();
    fullCiphertext.clear();

    return fulltextAfterDecode;
}

int main(int argc, char** argv) {
    int rank, size;
    string characterAsBits, fullText, key, fullKey, ciphertext, textAfterDecode, fulltextAfterDecode, fullCiphertext, finalText;
    encryption_fun_type ptr;
    srand(time(nullptr));

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    
    if (rank == 0) {
        vector<string> fullTextAsArray = readFileAndSplitIntoLines("short.txt");
        vector<vector<string>> split_vec = split_vector(fullTextAsArray, size - 2);

        string concatenatedStrings;
        size_t numElements;

        for (int i = 1; i < size - 1; i++) {
            concatenatedStrings = "";

            for (const auto& str : split_vec[i-1]) {
                concatenatedStrings += str + '\n';
            }

            numElements = concatenatedStrings.size();
            cout << "Number of elements for the process " << i << ": " << numElements << endl;

            MPI_Send(concatenatedStrings.c_str(), numElements + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
    } else if (rank == size - 1) {
        fullText = readTextFile("short.txt");
        cout << "\nFinall process:" << endl;

        for (int i = 1; i < size - 1; i++) {
            char buffer[1000000];
            MPI_Recv(&buffer, 1000000, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);

            finalText += buffer;
        }

        test(finalText, fullText);
    } else {
        ptr = encryptionWithoutParallelizing;

        char buffer[1000000];
        MPI_Recv(&buffer, 1000000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

        string receivedString(buffer);
        vector<string> receivedVector = splitString(receivedString);

        // cout << "\nProcess " << rank << " received:" << endl;

        // for (const auto& str : receivedVector) {
        //     cout << str << " ";
        // }
        // cout << endl;

        string textAfterOperation = runEncryption(ptr, characterAsBits, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, receivedVector);

        // cout << "\nProcess " << rank << " returned after operations:" << endl;
        // cout << textAfterOperation << endl;

        MPI_Send(textAfterOperation.c_str(), textAfterOperation.size() + 1, MPI_CHAR, size - 1, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return(0);
}