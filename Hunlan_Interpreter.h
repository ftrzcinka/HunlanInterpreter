#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <stack>

class HunlanString {
public:
	HunlanString();
	HunlanString(const std::string& name, const std::string& value);

	std::string getName() const;
	std::string getValue() const;

	void setValue(const std::string& value);

	~HunlanString();

private:
	std::string variableName;
	std::string variableValue;
};

class HunlanNumber {
public:
	HunlanNumber();
	HunlanNumber(const std::string& name, const long long value);

	std::string getName()const;
	long long getValue()const;

	void setValue(const long long value);

	~HunlanNumber();

private:
	std::string variableName;
	long long variableValue;
};

class MyParser {
public:
	MyParser();
	MyParser(const std::vector<char>& seperators);

	std::vector<std::string> operator()(std::string& input);

	~MyParser();

private:
	std::vector<char> parsed_Seperators;
	std::vector<std::string> line_Input;
};

class MyInterpreter {
public:
	MyInterpreter();

	bool is_All_Letters(const std::string& str) const;
	bool is_All_Numbers(const std::string& str) const;

	void addStringVar(const std::string& name, const std::string& value);
	void addNumberVar(const std::string& name, const long long value);

	bool doesStringVarNameExist(const std::string& name) const;
	bool doesNumVarNameExist(const std::string& name) const;

	int findStringVar(const std::string& name) const;
	int findNumberVar(const std::string& name) const;

	std::vector<std::string> infixToPostfix();
	long long postfixSolver(std::vector<std::string>& postfix_equation);
	long long equationSolver();
	
	void print() const;
	void stringDeclaration();
	void numberDeclaration();
	void stringAssignment();
	void numberAssignment();
	
	void process(std::string& line);

	~MyInterpreter();
private:
	MyParser parser;
	
	std::vector<std::string> parsed_Line;
	std::vector<HunlanString> string_Variables;
	std::vector<HunlanNumber> number_Variables;

	int lineNumber;
};