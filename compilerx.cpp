#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype>

using namespace std;

struct Token {
    string type;
    string value;
};

vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    int i = 0;

    while (i < code.length()) {

        if (code[i] == '\n') {
            tokens.push_back({"NEWLINE", "\\n"});
            i++;
            continue;
        }

        if (isspace(code[i])) {
            i++;
            continue;
        }

        if (code[i] == '/' &&
            i + 1 < code.length() &&
            code[i + 1] == '/') {

            while (i < code.length() && code[i] != '\n')
                i++;

            continue;
        }

        if (isdigit(code[i]) || code[i] == '.') {

            string num;
            bool decimalFound = false;

            while (i < code.length() &&
                   (isdigit(code[i]) || code[i] == '.')) {

                if (code[i] == '.') {

                    if (decimalFound) {
                        cout << "Invalid number format\n";
                        exit(1);
                    }

                    decimalFound = true;
                }

                num += code[i];
                i++;
            }

            tokens.push_back({"NUMBER", num});
        }

        else if (isalpha(code[i]) || code[i] == '_') {

            string word;

            while (i < code.length() &&
                  (isalnum(code[i]) || code[i] == '_')) {

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

        else {

            char ch = code[i];

            if (ch == '=')
                tokens.push_back({"EQUALS", "="});

            else if (ch == '+')
                tokens.push_back({"PLUS", "+"});

            else if (ch == '-')
                tokens.push_back({"MINUS", "-"});

            else if (ch == '*')
                tokens.push_back({"MULTIPLY", "*"});

            else if (ch == '/')
                tokens.push_back({"DIVIDE", "/"});

            else {
                cout << "Unexpected character: " << ch << endl;
                exit(1);
            }

            i++;
        }
    }

    return tokens;
}

double evaluate(vector<string> expr,
                map<string, double>& vars) {

    vector<string> processed;

    for (auto& token : expr) {

        if (vars.count(token))
            processed.push_back(to_string(vars[token]));
        else
            processed.push_back(token);
    }

    if (processed.empty()) {
        cout << "Empty expression\n";
        exit(1);
    }

    double result = stod(processed[0]);

    for (int i = 1; i < processed.size(); i += 2) {

        string op = processed[i];

        if (i + 1 >= processed.size()) {
            cout << "Invalid expression\n";
            exit(1);
        }

        double val = stod(processed[i + 1]);

        if (op == "+")
            result += val;

        else if (op == "-")
            result -= val;

        else if (op == "*")
            result *= val;

        else if (op == "/") {

            if (val == 0) {
                cout << "Division by zero\n";
                exit(1);
            }

            result /= val;
        }
    }

    return result;
}

int main() {

    ifstream file("program.vortex");

    if (!file.is_open()) {
        cout << "Could not open program.vortex\n";
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    string code = buffer.str();

    vector<Token> tokens = tokenize(code);

    map<string, double> variables;

    int i = 0;

    while (i < tokens.size()) {

        if (tokens[i].type == "NEWLINE") {
            i++;
            continue;
        }

        if (tokens[i].type == "LET") {

            if (i + 3 >= tokens.size()) {
                cout << "Invalid declaration\n";
                return 1;
            }

            string varName = tokens[i + 1].value;

            if (tokens[i + 2].type != "EQUALS") {
                cout << "Expected '='\n";
                return 1;
            }

            double value = stod(tokens[i + 3].value);

            variables[varName] = value;

            i += 4;
        }

        else if (tokens[i].type == "SHOW") {

            vector<string> expr;
            i++;

            while (i < tokens.size() &&
                   tokens[i].type != "NEWLINE") {

                expr.push_back(tokens[i].value);
                i++;
            }

            cout << "Output: "
                 << evaluate(expr, variables)
                 << endl;
        }

        else {
            i++;
        }
    }

    return 0;
}