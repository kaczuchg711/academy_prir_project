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
        cout << "Test result: Pass" << endl;
    } else {
        cout << "Test result: Fail" << endl;
        cout << cleanFullText << endl << endl;
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

    fun(characterAsBits, key, fullKey, ciphertext, fullCiphertext, fullTextAsArray);

    // Decryption process
    // for (size_t i = 0; i < fullCiphertext.length(); i += 8) {
    //     textAfterDecode = decode(fullCiphertext.substr(i, 8), fullKey.substr(i, 8));
    //     fulltextAfterDecode += bitsToChar(textAfterDecode);
    // }

    fullKey.clear();
    fullCiphertext.clear();

    return fulltextAfterDecode;
}

int main(int argc, char** argv) {
    int rank, size;
    string characterAsBits, fullText, key, fullKey, ciphertext, textAfterDecode, fulltextAfterDecode, fullCiphertext, finalText;
    encryption_fun_type ptr;
    long calculation_time;
    srand(time(nullptr));

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Check if there are at least 2 processes
    if (size < 2) {
        std::cerr << "This program must be run with at least 2 processes.\n";
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    
    if (rank == 0) {
        vector<string> fullTextAsArray = readFileAndSplitIntoLines(argv[1]);
        vector<vector<string>> split_vec = split_vector(fullTextAsArray, size);

        string concatenatedStrings;
        size_t numElements;

        // Start the clock and send start value to the last process
        auto start = high_resolution_clock::now();
        MPI_Send(&start, sizeof(start), MPI_BYTE, size - 1, 2, MPI_COMM_WORLD);

        // Vectors concatenation and sending to the all processes except process number 0
        for (int i = 1; i < size; i++) {
            concatenatedStrings = "";

            for (const auto& str : split_vec[i]) {
                concatenatedStrings += str + '\n';
            }

            numElements = concatenatedStrings.size();
            MPI_Send(concatenatedStrings.c_str(), numElements + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }

        // First vector encryption and sending the result to the last process
        ptr = encryptionWithoutParallelizing;
        string textAfterOperation = runEncryption(ptr, characterAsBits, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, split_vec[0]);

        MPI_Send(textAfterOperation.c_str(), textAfterOperation.size() + 1, MPI_CHAR, size - 1, 1, MPI_COMM_WORLD);
    } else if (rank == size - 1) {
        // Receive the clock start value
        high_resolution_clock::time_point start;
        MPI_Recv(&start, sizeof(start), MPI_BYTE, 0, 2, MPI_COMM_WORLD, &status);

        // Read full text file content
        fullText = readTextFile(argv[1]);

        // Receive the text data from the other processes
        for (int i = 1; i < size - 1; i++) {
            char buffer[5000000];
            MPI_Recv(&buffer, 5000000, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);

            finalText += buffer;
        }

        // Receive the not encrypted text data from the process 0 and do the encryption
        char buffer[5000000];
        MPI_Recv(&buffer, 5000000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

        string receivedString(buffer);
        vector<string> receivedVector = splitString(receivedString);

        ptr = encryptionWithoutParallelizing;
        string textAfterOperation = runEncryption(ptr, characterAsBits, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, receivedVector);
        finalText += textAfterOperation;

        // Receive text data after encryption from the process 0
        MPI_Recv(&buffer, 5000000, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
        finalText = buffer + finalText;

        // Test if the text before and after encryption and decryption are the same
        // test(finalText, fullText);

        // Stop the clock and calculate the duration time
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        calculation_time = duration.count();
        // cout << "Time taken: " << calculation_time/1000. << " ms" << endl;
    } else {
        // Receive the text data from the process 0
        char buffer[5000000];
        MPI_Recv(&buffer, 5000000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

        // Perform encryption and decryption
        string receivedString(buffer);
        vector<string> receivedVector = splitString(receivedString);

        ptr = encryptionWithoutParallelizing;
        string textAfterOperation = runEncryption(ptr, characterAsBits, key, fullKey, ciphertext, fulltextAfterDecode, textAfterDecode, fullCiphertext, receivedVector);

        // Send the text data after operations to the last process
        MPI_Send(textAfterOperation.c_str(), textAfterOperation.size() + 1, MPI_CHAR, size - 1, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return(0);
}