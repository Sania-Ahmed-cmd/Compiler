#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype>

using namespace std;

// ---------------- Ttoken struct ----------------
struct Token {
    string type;
    string value;
};

// ---------------- tokenizer ----------------
vector<Token> tokenize(string code) {
    vector<Token> tokens;
    int i = 0;

    while (i < code.length()) {
        if (isspace(code[i])) {
            i++;
            continue;
        }

        // Numbers
        if (isdigit(code[i])) {
            string num;
            while (i < code.length() && isdigit(code[i])) {
                num += code[i];
                i++;
            }
            tokens.push_back({"NUMBER", num});
        }

        // Identifiers / keywords
        else if (isalpha(code[i])) {
            string word;
            while (i < code.length() && isalnum(code[i])) {
                word += code[i];
                i++;
            }

            if (word == "let")
                tokens.push_back({"LET", word});
            else if (word == "show")
                tokens.push_back({"SHOW", word});
            else
                tokens.push_back({"IDENTIFIER", word});
        }

        // Operators
        else {
            char ch = code[i];

            if (ch == '=') tokens.push_back({"EQUALS", "="});
            else if (ch == '+') tokens.push_back({"PLUS", "+"});
            else if (ch == '-') tokens.push_back({"MINUS", "-"});
            else if (ch == '*') tokens.push_back({"MULTIPLY", "*"});
            else if (ch == '/') tokens.push_back({"DIVIDE", "/"});
            else {
                cout << "Unexpected character: " << ch << endl;
                exit(1);
            }
            i++;
        }
    }

    return tokens;
}

// ---------------- interpreter ----------------
int evaluate(vector<string> expr, map<string, int>& vars) {
    vector<string> processed;

    for (auto& token : expr) {
        if (vars.count(token))
            processed.push_back(to_string(vars[token]));
        else
            processed.push_back(token);
    }

    // VERY simple evaluation (left to right)
    int result = stoi(processed[0]);

    for (int i = 1; i < processed.size(); i += 2) {
        string op = processed[i];
        int val = stoi(processed[i + 1]);

        if (op == "+") result += val;
        else if (op == "-") result -= val;
        else if (op == "*") result *= val;
        else if (op == "/") result /= val;
    }

    return result;
}

// ---------------- main ----------------
int main() {
    ifstream file("program.vortex");
    stringstream buffer;
    buffer << file.rdbuf();
    string code = buffer.str();

    vector<Token> tokens = tokenize(code);

    map<string, int> variables;

    for (int i = 0; i < tokens.size(); i++) {

        // let x = 5
        if (tokens[i].type == "LET") {
            string var = tokens[i + 1].value;
            int value = stoi(tokens[i + 3].value);
            variables[var] = value;
            i += 3;
        }

        // show x + y
        else if (tokens[i].type == "SHOW") {
            vector<string> expr;
            i++;

            while (i < tokens.size() && tokens[i].type != "LET" && tokens[i].type != "SHOW") {
                expr.push_back(tokens[i].value);
                i++;
            }

            i--; // step back
            int result = evaluate(expr, variables);
            cout << "Output will be " << result << endl;
        }
    }

    return 0;
}


