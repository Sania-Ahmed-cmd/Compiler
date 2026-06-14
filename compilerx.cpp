#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cctype>

using namespace std;

// ---------------- Token ----------------
struct Token {
    string type;
    string value;
};

// ---------------- Tokenizer ----------------
vector<Token> tokenize(string code) {
    vector<Token> tokens;
    int i = 0;

    while (i < code.length()) {

        // Skip whitespace
        if (isspace(code[i])) {
            i++;
            continue;
        }

        // Skip comments
        if (code[i] == '/' && i + 1 < code.length() && code[i + 1] == '/') {
            while (i < code.length() && code[i] != '\n')
                i++;
            continue;
        }

        // Numbers
        if (isdigit(code[i])) {
            string num;

            while (i < code.length() &&
                  (isdigit(code[i]) || code[i] == '.')) {
                num += code[i];
                i++;
            }

            tokens.push_back({"NUMBER", num});
        }

        // Keywords / Identifiers
        else if (isalpha(code[i])) {
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

        // Operators
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

// ---------------- Expression Evaluator ----------------
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
        cout << "Empty expression!" << endl;
        exit(1);
    }

    double result = stod(processed[0]);

    for (int i = 1; i < processed.size(); i += 2) {

        string op = processed[i];
        double val = stod(processed[i + 1]);

        if (op == "+")
            result += val;

        else if (op == "-")
            result -= val;

        else if (op == "*")
            result *= val;

        else if (op == "/") {

            if (val == 0) {
                cout << "Division by zero error!" << endl;
                exit(1);
            }

            result /= val;
        }
    }

    return result;
}

// ---------------- Main ----------------
int main() {

    ifstream file("program.vortex");

    if (!file.is_open()) {
        cout << "Could not open program.vortex" << endl;
        return 1;
    }

    stringstream buffer;
    buffer << file.rdbuf();

    string code = buffer.str();

    vector<Token> tokens = tokenize(code);

    map<string, double> variables;

    for (int i = 0; i < tokens.size(); i++) {

        // let x = 10
        if (tokens[i].type == "LET") {

            if (i + 3 >= tokens.size()) {
                cout << "Invalid variable declaration" << endl;
                return 1;
            }

            string var = tokens[i + 1].value;
            double value = stod(tokens[i + 3].value);

            variables[var] = value;

            i += 3;
        }

        // show expression
        else if (tokens[i].type == "SHOW") {

            vector<string> expr;

            i++;

            while (
                i < tokens.size() &&
                tokens[i].type != "LET" &&
                tokens[i].type != "SHOW"
            ) {
                expr.push_back(tokens[i].value);
                i++;
            }

            i--;

            double result = evaluate(expr, variables);

            cout << "Output: " << result << endl;
        }
    }

    return 0;
}