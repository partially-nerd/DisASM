#include "./fs.hpp"

using namespace std;

string readFileAsync(string file) {
  ifstream File(file);

  if (!File.is_open()) {
    cerr << file << " not found." << endl;
  }

  string retval = "";

  while (!File.eof()) {
    string line;
    getline(File, line);
    retval += line + "\n";
  }

  File.close();

  return retval;
}

void trim(string &s) {
  int start = 0;
  int end = s.size() - 1;

  while (start <= end && s[start] == ' ')
    start++;

  while (end >= start && s[end] == ' ')
    end--;

  s = s.substr(start, end - start + 1);
}

string readFileCleanAsync(string file) {
  ifstream File(file);

  if (!File.is_open()) {
    cerr << file << " not found." << endl;
  }

  string retval = "";

  while (!File.eof()) {
    string line;
    getline(File, line);
    if (line == "") {
      continue;
    }
    trim(line);
    retval += line + "\n";
  }

  File.close();

  return retval;
}
