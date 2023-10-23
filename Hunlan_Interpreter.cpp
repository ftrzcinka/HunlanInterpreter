#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <stack>
#include "Hunlan_Interpreter.h"

//HulanString Functions
HunlanString::HunlanString() {}

HunlanString::HunlanString(const std::string& name, const std::string& value) {
	variableName = name;
	variableValue = value;
}

std::string HunlanString::getName() const {
	return variableName;
}

std::string HunlanString::getValue() const {
	return variableValue;
}

void HunlanString::setValue(const std::string& value) {
	variableValue = value;
}

HunlanString::~HunlanString() {}



//HulanNumber Functions
HunlanNumber::HunlanNumber() {
	variableValue = 0;
}

HunlanNumber::HunlanNumber(const std::string& name, const long long value) {
	variableName = name;
	variableValue = value;
}

std::string HunlanNumber::getName() const {
	return variableName;
}

long long HunlanNumber::getValue() const {
	return variableValue;
}

void HunlanNumber::setValue(const long long value) {
	variableValue = value;
}

HunlanNumber::~HunlanNumber() {}




//MyParser Functions
MyParser::MyParser() {
	parsed_Seperators.push_back(' ');
	parsed_Seperators.push_back('\t');
}

MyParser::MyParser(const std::vector<char>& seperators) {
	parsed_Seperators = seperators;
}

std::vector<std::string> MyParser::operator()(std::string& input) {
	std::string tempString = "";
	std::string operatorString = "";
	bool foundLetter = false;
	bool isSeperator = false;
	bool foundQuotes = false;

	line_Input.clear();

	//std::cout << input.length() << std::endl;

	if (input.length() <= 1) {
		line_Input.push_back("");
		return line_Input;
	}

	for (int i = 0; i < input.length(); i++) {
		if (input[i] == '\\' && i < input.length() - 1) {
			++i;
			if (input[i] == '\"') {
				line_Input.clear();
				line_Input.push_back("error");
				return line_Input;
			}
			else if (input[i] == 'n') {
				tempString += '\n';
				++i;
			}
			else if (input[i] == 't') {
				tempString += '\t';
				++i;
			}
		}
		
		if (input[i] == '#')
			break;
		
		isSeperator = false;

		for (int j = 0; j < parsed_Seperators.size(); j++) {
			if (input[i] == parsed_Seperators[j]) {
				isSeperator = true;
				break;
			}
		}

		if (input[i] == '"' && foundQuotes) {
			line_Input.push_back(tempString);
			tempString = "";
			foundLetter = false;
			foundQuotes = false;
		}
		else if (input[i] == '"' && !foundQuotes)
			foundQuotes = true;

		if (foundQuotes) {
			if (input[i] != '"')
				tempString += input[i];
		}
		else if (!isSeperator) {
			if (input[i] == '=' || input[i] == '+' || input[i] == '-' || input[i] == '*') {
				if (tempString != "")
					line_Input.push_back(tempString);

				tempString = input[i];
				line_Input.push_back(tempString);
				tempString = "";
				foundLetter = false;
			}
			else {
				if (input[i] != '"') {
					tempString += input[i];
					foundLetter = true;
				}
			}
		}
		else {
			if (foundLetter && tempString != "") {
				line_Input.push_back(tempString);
				tempString = "";
				foundLetter = false;
			}
		}
	}

	if (!isSeperator && input[input.size() - 1] != '"')
		line_Input.push_back(tempString);
	
	return line_Input;
}

MyParser::~MyParser() {}






//MyInterpreter Functions
MyInterpreter::MyInterpreter() {
	lineNumber = 0;
	std::vector<char>  separators{ ' ', '\t' };
	parser = MyParser(separators);
}

bool MyInterpreter::is_All_Letters(const std::string& str) const {
	for (int i = 0; i < str.length(); i++) {
		if (!isalpha(str[i])) {
			return false;
		}
	}

	return true;
}

bool MyInterpreter::is_All_Numbers(const std::string& str) const {
	for (int i = 0; i < str.length(); i++) {
		if (!isdigit(str[i])) {
			return false;
		}
	}

	return true;
}

void MyInterpreter::addStringVar(const std::string& name, const std::string& value) {
	HunlanString item = HunlanString(name, value);
	string_Variables.push_back(item);
}

void MyInterpreter::addNumberVar(const std::string& name, const long long value) {
	HunlanNumber item = HunlanNumber(name, value);
	number_Variables.push_back(item);
}

bool MyInterpreter::doesStringVarNameExist(const std::string& name) const {
	for (int i = 0; i < string_Variables.size(); i++) {
		if (string_Variables[i].getName() == name) {
			return true;
		}
	}

	return false;
}

bool MyInterpreter::doesNumVarNameExist(const std::string& name) const {
	for (int i = 0; i < number_Variables.size(); i++) {
		if (number_Variables[i].getName() == name) {
			return true;
		}
	}

	return false;
}

int MyInterpreter::findStringVar(const std::string& name) const {
	for (int i = 0; i < string_Variables.size(); i++) {
		if (string_Variables[i].getName() == name) {
			return i;
		}
	}

	return -1;
}

int MyInterpreter::findNumberVar(const std::string& name) const {
	for (int i = 0; i < number_Variables.size(); i++) {
		if (number_Variables[i].getName() == name) {
			return i;
		}
	}

	return -1;
}

std::vector<std::string> MyInterpreter::infixToPostfix() {
	std::vector<std::string> postfixVector;
	std::stack<std::string> strStack;
	int i = 0;
	bool doubleOperators = false;
	bool doubleNumbers = false;

	if (parsed_Line[0] == "NUMBER") {
		i = 3;
	}
	else {
		i = 2;
	}

	strStack.push("bottom"); //without this my program crashes checking the top of an empty stack

	for (i; i < parsed_Line.size(); i++) {
		if (parsed_Line[i] == "*") {
			if (doubleOperators) {
				std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical expression" << std::endl;
				exit(1);
			}
			if (strStack.top() == "*") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			strStack.push(parsed_Line[i]);
			doubleOperators = true;
			doubleNumbers = false;
		}
		else if (parsed_Line[i] == "+") {
			if (doubleOperators) {
				std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical expression" << std::endl;
				exit(1);
			}
			if (strStack.top() == "*") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			else if (strStack.top() == "+") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			else if (strStack.top() == "-") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			strStack.push(parsed_Line[i]);
			doubleOperators = true;
			doubleNumbers = false;
		}
		else if (parsed_Line[i] == "-") {
			if (doubleOperators) {
				std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical expression" << std::endl;
				exit(1);
			}
			if (strStack.top() == "*") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			else if (strStack.top() == "+") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			else if (strStack.top() == "-") {
				postfixVector.push_back(strStack.top());
				strStack.pop();
			}
			strStack.push(parsed_Line[i]);
			doubleOperators = true;
			doubleNumbers = false;
		}
		else if (parsed_Line[i] == "/" || parsed_Line[i] == "(" || parsed_Line[i] == ")") {
			std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical symbol" << std::endl;
			exit(1);
		}
		else if (!is_All_Numbers(parsed_Line[i])) {
			if (doubleNumbers) {
				std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical expression" << std::endl;
				exit(1);
			}
			if (doesNumVarNameExist(parsed_Line[i])) {
				postfixVector.push_back(parsed_Line[i]);
				doubleNumbers = true;
				doubleOperators = false;
			}
			else {
				std::cout << "ERROR On Line " << lineNumber << " due to including STRING literal/variable to mathematical expression" << std::endl;
				exit(1);
			}
		}
		else if(is_All_Numbers(parsed_Line[i])) {
			if (doubleNumbers) {
				std::cout << "ERROR On Line " << lineNumber << " due to improper mathematical expression" << std::endl;
				exit(1);
			}

			postfixVector.push_back(parsed_Line[i]);
			doubleNumbers = true;
			doubleOperators = false;
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper operator or symbol" << std::endl;
			exit(1);
		}
	}

	while (strStack.size() != 1) {
		postfixVector.push_back(strStack.top());
		strStack.pop();
	}

	return postfixVector;
}

long long MyInterpreter::postfixSolver(std::vector<std::string>& postfix_equation) {
	long long num1 = 0;
	long long num2 = 0;
	std::stack<long long> numberStack;

	for (int i = 0; i < postfix_equation.size(); i++) {
		if (!is_All_Numbers(postfix_equation[i])) {
			if (doesNumVarNameExist(postfix_equation[i])) {
				int location = findNumberVar(postfix_equation[i]);
				numberStack.push(number_Variables[location].getValue());
			}
			else if (postfix_equation[i] == "*") {
				num1 = numberStack.top();
				numberStack.pop();
				num2 = numberStack.top();
				numberStack.pop();
				num1 = num2 * num1;
				numberStack.push(num1);
			}
			else if (postfix_equation[i] == "+") {
				num1 = numberStack.top();
				numberStack.pop();
				num2 = numberStack.top();
				numberStack.pop();
				num1 = num2 + num1;
				numberStack.push(num1);
			}
			else if (postfix_equation[i] == "-") {
				num1 = numberStack.top();
				numberStack.pop();
				num2 = numberStack.top();
				numberStack.pop();
				num1 = num2 - num1;
				numberStack.push(num1);
			}
			else {
				std::cout << "ERROR On Line " << lineNumber << " due to including STRING literal/variable to mathematical expression" << std::endl;
				exit(1);
			}
		}
		else if (is_All_Numbers(postfix_equation[i])) {
			num1 = stoll(postfix_equation[i], nullptr, 10);
			numberStack.push(num1);
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper operator or symbol" << std::endl;
			exit(1);
		}
	}

	return numberStack.top();
}

long long MyInterpreter::equationSolver() {
	std::vector<std::string> postfix = infixToPostfix();
	return postfixSolver(postfix);
}

void MyInterpreter::print() const {
	if (doesStringVarNameExist(parsed_Line[1])) {
		int location = findStringVar(parsed_Line[1]);
		std::cout << string_Variables[location].getValue();
	}
	else if (doesNumVarNameExist(parsed_Line[1])) {
		int location = findNumberVar(parsed_Line[1]);
		std::cout << number_Variables[location].getValue();
	}
	else {
		std::cout << parsed_Line[1];
	}
}

void MyInterpreter::stringDeclaration() {
	if (parsed_Line.size() <= 1 || parsed_Line.size() > 4) {
		std::cout << "ERROR On Line " << lineNumber << " due to Incorrect STRING Declaration" << std::endl;
		exit(1);
	}

	if (isdigit(parsed_Line[1][0])) {
		std::cout << "ERROR On Line " << lineNumber << " due to variable name starting with number" << std::endl;
		exit(1);
	}

	if (doesStringVarNameExist(parsed_Line[1])) {
		std::cout << "ERROR On Line " << lineNumber << " due to reusing varaible name" << std::endl;
		exit(1);
	}
	
	if (parsed_Line.size() == 2) {
		addStringVar(parsed_Line[1], "");
		return;
	}
	else if(parsed_Line.size() == 4) {
		if (is_All_Numbers(parsed_Line[3])) {
			std::cout << "ERROR On Line " << lineNumber << " Due to assigning NUMBER value to STRING" << std::endl;
			exit(1);
		}
		
		if (doesStringVarNameExist(parsed_Line[3])) {
			int location = findStringVar(parsed_Line[3]);
			addStringVar(parsed_Line[1], string_Variables[location].getValue());
			return;
		}
		else {
			addStringVar(parsed_Line[1], parsed_Line[3]);
			return;
		}
	}
	else {
		std::cout << "ERROR On Line " << lineNumber << " due improper STRING declaration syntax" << std::endl;
		exit(1);
	}
}

void MyInterpreter::numberDeclaration() {
	if (parsed_Line.size() <= 1) {
		std::cout << "ERROR On Line " << lineNumber << " due to improper NUMBER declaration syntax" << std::endl;
		exit(1);
	}

	if (isdigit(parsed_Line[1][0])) {
		std::cout << "ERROR On Line " << lineNumber << " due to variable name starting with number" << std::endl;
		exit(1);
	}

	if (doesNumVarNameExist(parsed_Line[1])) {
		std::cout << "ERROR On Line " << lineNumber << " variable name already exits" << std::endl;
		exit(1);
	}

	if (parsed_Line.size() == 2) {
		addNumberVar(parsed_Line[1],0);
		return;
	}
	else if (parsed_Line.size() == 4) {
		if (is_All_Numbers(parsed_Line[3])) {
			long long num = stoll(parsed_Line[3], nullptr, 10);
			addNumberVar(parsed_Line[1], num);
			return;
		}
		else {
			if (doesNumVarNameExist(parsed_Line[3])) {
				int location = findNumberVar(parsed_Line[3]);
				addNumberVar(parsed_Line[1], number_Variables[location].getValue());
				return;
			}
			else {
				std::cout << "ERROR On Line " << lineNumber << " NUMBER variable name does not exist";
				std::cout << " or trying to assign STRING to NUMBER variable" << std::endl;
				exit(1);
			}
		}
	}
	else if (parsed_Line.size() > 4) {
		long long result = equationSolver();
		addNumberVar(parsed_Line[1], result);
		return;
	}
	else {
		std::cout << "ERROR On Line " << lineNumber << " due to improper NUMBER variable declaration syntax" << std::endl;
		exit(1);
	}
}

void MyInterpreter::stringAssignment() {
	if (parsed_Line.size() <= 2 || parsed_Line.size() > 3) {
		std::cout << "ERROR On Line " << lineNumber << " due to improper STRING varaible assignment syntax" << std::endl;
		exit(1);
	}
	if (is_All_Numbers(parsed_Line[2])) {
		std::cout << "ERROR On Line " << lineNumber << " due to placing NUMBER as STRING value" << std::endl;
		exit(1);
	}

	int location1 = findStringVar(parsed_Line[0]);

	if (doesStringVarNameExist(parsed_Line[2])) {
		int location2 = findStringVar(parsed_Line[2]);
		string_Variables[location1].setValue(string_Variables[location2].getValue());
	}
	string_Variables[location1].setValue(parsed_Line[2]);
	return;
}

void MyInterpreter::numberAssignment() {
	if (parsed_Line.size() <= 2) {
		std::cout << "ERROR On Line " << lineNumber << " due to improper NUMBER variable assignment syntax" << std::endl;
		exit(1);
	}

	if (parsed_Line.size() == 3) {
		if (is_All_Numbers(parsed_Line[2])) {
			long long num = stoll(parsed_Line[2], nullptr, 10);
			int location = findNumberVar(parsed_Line[0]);
			number_Variables[location].setValue(num);
		}
		else {
			if (doesNumVarNameExist(parsed_Line[2])) {
				int location1 = findNumberVar(parsed_Line[0]);
				int location2 = findNumberVar(parsed_Line[2]);
				number_Variables[location1].setValue(number_Variables[location2].getValue());
			}
			else {
				std::cout << "ERROR On Line " << lineNumber << " NUMBER variable name does not exist";
				std::cout << " or trying to assign STRING to NUMBER variable" << std::endl;
				exit(1);
			}
		}
	}
	else if (parsed_Line.size() > 3) {
		long long result = equationSolver();
		int location = findNumberVar(parsed_Line[0]);
		number_Variables[location].setValue(result);
		return;
	}
	else {
		std::cout << "ERROR On Line " << lineNumber << " due to improper NUMBER variable assignment syntax" << std::endl;
		exit(1);
	}
}

void MyInterpreter::process(std::string& line) {
	++lineNumber;
	
	parsed_Line.clear();
	parsed_Line = parser(line);

	if (parsed_Line.size() < 1)
		return;

	if (parsed_Line.size() == 1) {
		if (parsed_Line[0] == "" || parsed_Line[0] == " " || parsed_Line[0] == "	")
			return;
		else if (parsed_Line[0] == "error") {
			std::cout << "ERROR On Line " << lineNumber << " due to double quotations" << std::endl;
			exit(1);
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper syntax" << std::endl;
			exit(1);
		}
	}

	if (parsed_Line[0] == "PRINT") {
		if (parsed_Line.size() == 2) {
			print();
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper PRINT syntax" << std::endl;
			exit(1);
		}
	}
	else if (parsed_Line[0] == "STRING") {
		if (parsed_Line.size() == 2 || (parsed_Line.size() > 3 && parsed_Line[2] == "=")) {
			stringDeclaration();
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper STRING declaration syntax" << std::endl;
			exit(1);
		}
	}
	else if (parsed_Line[0] == "NUMBER") {
		if (parsed_Line.size() == 2 || (parsed_Line.size() > 3 && parsed_Line[2] == "=")) {
			numberDeclaration();
		}
		else {
			std::cout << "ERROR On Line " << lineNumber << " due to improper NUMBER declaration syntax" << std::endl;
			exit(1);
		}
	}
	else if (doesStringVarNameExist(parsed_Line[0]) && parsed_Line[1] == "=") {
		stringAssignment();
	}
	else if (doesNumVarNameExist(parsed_Line[0]) && parsed_Line[1] == "=") {
		numberAssignment();
	}
	else {
		std::cout << "ERROR On Line " << lineNumber << " due to improper syntax" << std::endl;
		exit(1);
	}
}

MyInterpreter::~MyInterpreter() {}




//main function
int main(int argc, char* argv[]) {
	MyInterpreter engine = MyInterpreter();
	//int argc, char *argv[]
	//"HUNLAN_code.hun"
	//argv[1]
	std::string fileLine = "";
	std::ifstream myFile;
	myFile.open(argv[1]);

	if (myFile.fail()) {
		std::cout << "ERROR Could Not Open File";
		exit(1);
	}

	while (std::getline(myFile, fileLine)) {
		engine.process(fileLine);
	}
	myFile.close();
	return 0;
}