#include "fs/fs.c++"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define maxStackSize 50

vector<string> split(string s) {
  s += ", ";
  vector<string> ret;
  char prev_ch = ',';
  int last_enc = 0;

  for (int i = 0; i < s.size(); i++) {
    char ch = s[i];

    if (ch == ' ' && prev_ch == ',') {
      ret.push_back(s.substr(last_enc, i - last_enc - 1));
      // cout << s.substr(last_enc, i - last_enc - 1) << endl;
      last_enc = i + 1;
    }

    prev_ch = ch;
  }

  return ret;
}

int allocSize(string s) {
  if (s == "int") {
    return 1;
  }

  if (s == "str") {
    return 255;
  }

  if (s == "fn") {
    return 1;
  }

  if (s == "lbl") {
    return 1;
  }

  cerr << "Unknown Data Type" << endl;

  return 0;
}

struct keyVal {
  string key = "";
  int val = 0;

  keyVal(string k = "", int v = 0) : key(k), val(v) {}
};

int getPositionWhereKeyOccursInDict(string key, vector<keyVal> dict) {
  for (int i = 0; i < dict.size(); i++) {
    keyVal p = dict[i];
    if (p.key == key) {
      return i;
    }
  }

  return -1;
}

void expandP2(vector<string> &p1, vector<string> &p2) {
  if (p2.size() == 1) {
    string val = p2[0];
    p2.pop_back();
    for (string _ : p1) {
      p2.push_back(val);
    }
  }
}

void checkSizeP1P2(vector<string> &p1, vector<string> &p2) {
  if (p1.size() != p2.size()) {
    cerr << "Syntax error: size mismatch." << endl;
    return;
  }
}

bool canBeInt(const std::string &s) {
  try {
    size_t pos;
    std::stoi(s, &pos);
    return pos == s.size();
  } catch (...) {
    return false;
  }
}

string scopeName(vector<string> &currentStack) {
  string n = "";
  for (auto scope : currentStack) {
    n += scope + ".";
  }
  return n;
}

vector<string> keys(const vector<keyVal> &dict) {
  vector<string> ret;
  for (auto var : dict) {
    ret.push_back(var.key);
  }
  return ret;
}

void decv(string varName, string varDataType, vector<keyVal> &memory,
          vector<string> &currentStack) {
  string var = scopeName(currentStack) + varName;
  if (getPositionWhereKeyOccursInDict(var, memory) != -1) {
    if (varDataType != "fn") {
      cerr << "Attempting to redeclare variable." << endl;
    }
    return;
  }

  memory.resize(memory.size() + 1);
  memory.back().key = var;
  memory.resize(memory.size() + allocSize(varDataType) - 1);
}

string getMostRecentStackWhereVariableExists(string varName,
                                             vector<keyVal> &memory,
                                             vector<string> currentStack) {
  vector<string> symbols = keys(memory);
  int maxIterations = currentStack.size();

  for (int i = 0; i < maxIterations; i++) {
    string vName = scopeName(currentStack) + varName;

    if (count(symbols.begin(), symbols.end(), vName) != 0) {
      return vName;
    }

    if (currentStack.size() != 0) {
      currentStack.pop_back();
    }
  }

  cerr << "Variable doesn't exists in any scope." << endl;
  return "";
}

int getValueOfSymbol(string a, vector<keyVal> &memory,
                     vector<string> &currentStack) {
  int valA;
  if (canBeInt(a)) {
    valA = stoi(a);
  } else {
    string valAName =
        getMostRecentStackWhereVariableExists(a, memory, currentStack);
    valA = memory[getPositionWhereKeyOccursInDict(valAName, memory)].val;
  }
  return valA;
}

void set(string variable, string value, vector<keyVal> &memory,
         vector<string> &currentStack) {
  string var =
      getMostRecentStackWhereVariableExists(variable, memory, currentStack);
  int val = getValueOfSymbol(value, memory, currentStack);
  int indexInMemory = getPositionWhereKeyOccursInDict(var, memory);
  memory[indexInMemory].val = val;
}

void add(string a, string b, string storeAt, vector<keyVal> &memory,
         vector<string> &currentStack) {
  int valA = getValueOfSymbol(a, memory, currentStack);
  int valB = getValueOfSymbol(b, memory, currentStack);
  string sum = to_string(valA + valB);
  set(storeAt, sum, memory, currentStack);
}

void printMemory(vector<keyVal> &memory) {
  for (auto kv : memory) {
    cout << kv.key << " = " << kv.val << endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    cerr << "No input file received." << endl;
    return 1;
  }

  string file = argv[1];
  string code = readFileCleanAsync(file);

  istringstream stream(code);
  string line;
  int lNo = 0;

  vector<vector<string>> instructionSet;

  // tokenization
  while (getline(stream, line)) {
    line += " ";

    char prev_ch;
    int last_enc = 0;

    if (line[0] == '/' && line[1] == '/') {
      continue;
    }

    vector<string> tokens;

    for (int i = 0; i < line.size(); i++) {
      char ch = line[i];

      if (ch == ' ' && prev_ch != ',') {
        tokens.push_back(line.substr(last_enc, i - last_enc));
        last_enc = i + 1;
      }

      prev_ch = ch;
    }

    instructionSet.push_back(tokens);
  }

  vector<keyVal> memory; // where symbols will be stored
  vector<string> currentStack;
  vector<int> resizeMemoryToThisOnEndF;
  vector<int> moveToThisOnEndF;
  int i = 0;
  currentStack.push_back("");
  // execution pass

  bool skip = false;
  bool DEBUG = false;
  string previousCommand;
  string scopeNameOfOutOfFunctionDefinition;
  vector<string> functionParameters;

  int PC = 0;
  while (PC != -1) {
    auto instruction = instructionSet[PC];
    string command = instruction[0];
    PC++;
    i++;

    if (DEBUG) {
      cout << "--------------------------------------------------------------"
           << endl;
      cout << "i = " << i << endl;
      cout << "PC = " << PC - 1 << endl;
      cout << "Stack size = " << currentStack.size() << endl;
      cout << "Performing " << command << endl;
      cout << "Memory before pass: " << endl;
      printMemory(memory);

      if (i >= maxStackSize) {
        return 1;
      }
    }

    if (currentStack.size() > maxStackSize) {
      cerr << "Stack too deep." << endl;
      return 1;
    }

    if (command == "decf") {
      auto fnName = instruction[1];
      auto p1 = split(instruction[2]);
      auto p2 = split(instruction[3]);
      expandP2(p1, p2);
      checkSizeP1P2(p1, p2);

      decv(fnName, "fn", memory, currentStack);
      set(fnName, to_string(PC - 1), memory, currentStack);
      resizeMemoryToThisOnEndF.push_back(memory.size());

      if (previousCommand != "call") {
        skip = true;
        scopeNameOfOutOfFunctionDefinition = scopeName(currentStack);
        currentStack.push_back(fnName);

        continue;
      }

      currentStack.push_back(fnName);

      checkSizeP1P2(p1, functionParameters);

      for (int p = 0; p < p1.size(); p++) {
        string variable = p1[p];
        string type = p2[p];

        decv(variable, type, memory, currentStack);
        set(variable, functionParameters[p], memory, currentStack);
      }
    }

    else if (command == "endf") {
      currentStack.pop_back();
      memory.resize(resizeMemoryToThisOnEndF.back());
      resizeMemoryToThisOnEndF.pop_back();

      if (scopeName(currentStack) == scopeNameOfOutOfFunctionDefinition) {
        skip = false;
      }

      if (moveToThisOnEndF.size() != 0) {
        PC = moveToThisOnEndF.back();
        moveToThisOnEndF.pop_back();
      }
    }

    if (skip) {
      continue;
    }

    if (command == "decv") {
      auto p1 = split(instruction[1]);
      auto p2 = split(instruction[2]);
      expandP2(p1, p2);
      checkSizeP1P2(p1, p2);

      for (int p = 0; p < p1.size(); p++) {
        string variable = p1[p];
        string type = p2[p];

        decv(variable, type, memory, currentStack);
      }
    }

    else if (command == "call") {
      auto fn = instruction[1];
      functionParameters.clear();

      for (auto var : split(instruction[2])) {
        functionParameters.push_back(
            to_string(getValueOfSymbol(var, memory, currentStack)));
      }

      auto fnIsAt = getValueOfSymbol(fn, memory, currentStack);

      if (DEBUG) {
        cout << "Call found " << fn << " at " << fnIsAt << endl;
        cout << "Passing parameters ";

        for (auto var : functionParameters) {
          cout << var << ", ";
        }

        cout << endl;
      }

      moveToThisOnEndF.push_back(PC);
      PC = fnIsAt;
    }

    else if (command == "decl") {
      auto lName = instruction[1];
      auto offset = getValueOfSymbol(instruction[2], memory, currentStack);

      decv(lName, "lbl", memory, currentStack);
      set(lName, to_string(PC + offset), memory, currentStack);
    }

    else if (command == "cmpe") {
      auto p1 = split(instruction[1]);
      auto p2 = split(instruction[2]);
      expandP2(p1, p2);
      checkSizeP1P2(p1, p2);

      for (int p = 0; p < p1.size(); p++) {
        int valA = getValueOfSymbol(p1[p], memory, currentStack);
        int valB = getValueOfSymbol(p2[p], memory, currentStack);

        if (valA == valB) {
          PC = getValueOfSymbol(instruction[3], memory, currentStack);
        }
      }
    }

    else if (command == "set") {
      auto p1 = split(instruction[1]);
      auto p2 = split(instruction[2]);
      expandP2(p1, p2);
      checkSizeP1P2(p1, p2);

      for (int p = 0; p < p1.size(); p++) {
        string variable = p1[p];
        string value = p2[p];

        set(variable, value, memory, currentStack);
      }
    }

    else if (command == "out") {
      auto p1 = split(instruction[1]);

      for (int p = 0; p < p1.size(); p++) {
        int value = getValueOfSymbol(p1[p], memory, currentStack);
        cout << value;
      }
    }

    else if (command == "outch") {
      auto p1 = split(instruction[1]);

      for (int p = 0; p < p1.size(); p++) {
        int value = getValueOfSymbol(p1[p], memory, currentStack);
        cout << static_cast<char>(value);
      }
    }

    else if (command == "inc") {
      auto p1 = split(instruction[1]);

      for (int p = 0; p < p1.size(); p++) {
        int value = getValueOfSymbol(p1[p], memory, currentStack);
        set(p1[p], to_string(value + 1), memory, currentStack);
      }
    }

    else if (command == "add") {
      auto p1 = split(instruction[1]);
      auto p2 = split(instruction[2]);
      auto p3 = split(instruction[3]);
      expandP2(p1, p2);
      checkSizeP1P2(p1, p2);
      checkSizeP1P2(p1, p3);

      for (int p = 0; p < p1.size(); p++) {
        string a = p1[p];
        string b = p2[p];
        string storeAt = p3[p];

        add(a, b, storeAt, memory, currentStack);
      }
    }

    else if (command == "exit") {
      PC = -1;
    }

    if (DEBUG) {
      cout << "Memory after pass: " << endl;
      printMemory(memory);
    }

    previousCommand = command;
  }

  cout << endl;

  return 0;
}
