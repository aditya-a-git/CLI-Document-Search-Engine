#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"

using namespace std;

string convertPdf(string filename, int docId) {
    string newfile = "temp_" + to_string(docId) + ".txt";
    int ret =
        system(("pdftotext \"" + filename + "\" \"" + newfile + "\"").c_str());

    if (ret) {
        throw runtime_error(
            "pdftotext failed or not installed in your system!");
    }

    return newfile;
}

string convertDocx(string filename, int docId) {
    string newfile = "temp_" + to_string(docId) + ".txt";
    int ret =
        system(("pandoc \"" + filename + "\" -t plain -o \"" + newfile + "\"")
                   .c_str());

    if (ret) {
        throw runtime_error("Pandoc failed or not installed in your system!");
    }

    return newfile;
}

string normalize(const string &word) {
    string result;

    for (char c : word) {
        if (isalnum(c)) {
            result += tolower(c);
        }
    }

    return result;
}

void indexFile(unordered_map<string, unordered_map<int, vector<int>>> &index,
               int docId, const string filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening the file: " << filename << endl;
        return;
    }

    string line;
    int lineNo = 0;

    while (getline(file, line)) {
        stringstream words(line);
        lineNo++;
        string word;

        while (words >> word) {
            word = normalize(word);
            if (!word.empty()) {
                index[word][docId].push_back(lineNo);
            }
        }
    }
}

void processFile(unordered_map<string, unordered_map<int, vector<int>>> &index,
                 int docId, vector<string> &filenames) {
    if (filenames[docId].ends_with(".pdf")) {
        string temp = convertPdf(filenames[docId], docId);
        indexFile(index, docId, temp);
        filenames[docId] = temp;
        return;
    }

    if (filenames[docId].ends_with(".docx")) {
        string temp = convertDocx(filenames[docId], docId);
        indexFile(index, docId, temp);
        filenames[docId] = temp;
        return;
    }

    indexFile(index, docId, filenames[docId]);
}

unordered_map<int, set<int>>
search(unordered_map<string, unordered_map<int, vector<int>>> &index,
       const string query) {
    unordered_map<int, set<int>> result;

    for (const auto p : index) {
        if (p.first.starts_with(query)) {
            for (const auto id : p.second) {
                result[id.first].insert(id.second.begin(), id.second.end());
            }
        }
    }

    return result;
}

string extractLine(int docId, int resLineNo, const vector<string> filenames) {
    ifstream file(filenames[docId]);
    int lineNo = 0;
    string line;

    while (lineNo != resLineNo) {
        getline(file, line);
        lineNo++;
    }

    return line;
}

vector<string> wrapText(string text) {
    vector<string> wrapped;
    int width = 50;

    for (int i = 0; i < text.size(); i += width) {
        wrapped.push_back(text.substr(i, width));
    }

    return wrapped;
}

string cleanText(string content) {
    string cleaned = "";

    for (char c : content) {
        if (c >= 32 && c <= 126) {
            cleaned += c;
        }
    }

    int i = 0;
    while (i < cleaned.size() && cleaned[i] == ' ') {
        i++;
    }

    return cleaned.substr(i);
}

void display(const unordered_map<int, set<int>> &result,
             const vector<string> &filenames, const vector<string> ogFnames) {

    cout << BOLD << GREEN << "\t\t\t\tSEARCH RESULTS" << RESET << endl;
    cout << BOLD << YELLOW << left << setw(30) << "DOCUMENT NAME" << setw(10)
         << "LINE" << "CONTENT" << RESET << endl;
    cout << string(90, '-') << endl;

    for (auto it : result) {
        for (int i : it.second) {
            string content = extractLine(it.first, i, filenames);
            content = cleanText(content);

            cout << left << setw(30) << ogFnames[it.first] << setw(10) << i;

            vector<string> wrapped = wrapText(content);

            for (int j = 0; j < wrapped.size(); j++) {
                if (!j) {
                    cout << wrapped[j] << endl;
                } else {
                    cout << left << setw(40) << "" << wrapped[j] << endl;
                }
            }

            cout << string(90, '-') << endl;
        }
    }
}

void removeTempFiles(vector<string> filenames) {
    for (string name : filenames) {
        if (name.starts_with("temp")) {
            remove(name.c_str());
        }
    }
}

int main() {
    unordered_map<string, unordered_map<int, vector<int>>> index;
    vector<string> filenames;
    cout << "\033[2J\033[H";

    cout << BOLD << "\n==========================================\n";
    cout << CYAN << "\tCLI DOCUMENT SEARCH ENGINE\n" << RESET;
    cout << BOLD << "==========================================\n\n" << RESET;

    cout << "NOTE: This app depends on external CLI tools, pdftotext and pandoc for processing PDF and DOCX files." << endl << endl;

    cout << "Enter filenames (Enter * to exit):" << endl;
    string filename;
    getline(cin, filename);

    while (filename != "*") {
        filenames.push_back(filename);
        cin >> filename;
    }

    if (filenames.empty()) {
        cout << "No files provided" << endl;
        return 1;
    }

    vector<string> ogFnames = filenames;

    for (int i = 0; i < filenames.size(); i++) {
        processFile(index, i, filenames);
    }

    cout << "Indexing Complete!" << endl;

    while (true) {
        string query;
        cout << "Enter word to search (Enter // to exit): ";
        cin >> query;

        if (query == "//") {
            break;
        }

        query = normalize(query);
        unordered_map<int, set<int>> result = search(index, query);

        if (result.empty()) {
            cout << RED << "Entered word doesn't exist in provided files"
                 << RESET << endl;
            continue;
        }

        display(result, filenames, ogFnames);
    }

    removeTempFiles(filenames);
    return 0;
}