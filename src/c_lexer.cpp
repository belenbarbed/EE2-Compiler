#include "lexer.hpp"

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

#include <fstream>

using namespace std;

typedef unordered_map<std::string, unsigned> histogram_type;

struct json_obj {

	string text   = "";
	string class_ = "";
	
	int streamLine = 0;
	string sourceFile = "";

	int sourceLine = 0;
	int sourceCol  = 0;

	string to_print(){
		stringstream out;
		out << "{\"Text\":\"" << text << "\",  \"Class\":\"" << class_ << "\",  \"StreamLine\": " << streamLine << ",  \"SourceFile\":\"" << sourceFile << "\",  \"SourceLine\": " << sourceLine << ",  \"SourceCol\": " << sourceCol << "},";
		return out.str();
	}

};


/* 	HOW TO TEST

save desired input in text file input.in in same directory
	make histogram
	./histogram<input.in

Notes:
	git add remote spec https://github.com/LangProc/langproc-2016-cw.git
	git pull spec master


preprocessing c
	cpp hello_world.c


*/


// Define the instance of the variable that is declared in the header
TokenValue yylval;

int main() {

	vector<json_obj> saved;
	json_obj tmp;
	histogram_type invalids;
	//vector<string> invs;
	int nInv = 0;

	while(1) {
		TokenType type=(TokenType)yylex();

		if(type==TTNone){
			break; // No more tokens
		
		}else {

			tmp.streamLine = yylval.strLine;
			tmp.sourceLine = yylval.srcLine;
			tmp.sourceFile = *yylval.srcFile;
			tmp.sourceCol = yylval.srcCol;
			tmp.text = *yylval.wordValue;
			delete yylval.wordValue;
			delete yylval.srcFile;


			if(type==TTConstant){
				tmp.class_ = "Constant";

			}else if(type==TTKeyword){
				tmp.class_ = "Keyword";
		
			}else if(type==TTIdentifier){
				tmp.class_ = "Identifier";
		
			}else if(type==TTStringLiteral){
				tmp.class_ = "StringLiteral";
		
			}else if(type==TTOperator){
				tmp.class_ = "Operator";
		
			}else if(type==TTInvalid){

				tmp.class_ = "Invalid";
				invalids.emplace(*yylval.wordValue, 0);
				invalids[*yylval.wordValue] += 1;
				//invs.push_back(*yylval.wordValue);
				nInv++;
			}else{
				assert(0);
				return 1;
			}
			
			saved.push_back(tmp);
		}
	}


	// print vector in JSON format
	cout << "[" << endl;
	for(int i = 0; i < saved.size(); i++) {
		cout << saved[i].to_print() << endl;
		//cout << saved[i].text << endl;
	}
	cout << "{}" << endl;
	cout << "]" << endl;

	cout << endl;

	// DEBUGGING SHIT
	cout << "This file generated " << saved.size() << " JSON dictionaries" << endl;
	cout << "This file has " << nInv << " invalid tokens:" << endl;
	
	//
	vector<pair<string,double> > sorted(invalids.begin(), invalids.end());

	std::sort(sorted.begin(), sorted.end(), [](const pair<string,double> &a, const pair<string,double> &b){
		if(a.second > b.second)
			return true;
		if(a.second < b.second)
			return false;
		return a.first < b.first;
	});

	auto it=sorted.begin();
	while(it!=sorted.end()){
		string name=it->first;
		unsigned count=it->second;
		cout << "\"" << name <<  "\" " << count << endl;
		++it;
	}
	//

	//for (int i = 0; i < invs.size(); i++) {
	//	cout << invs[i] << endl;
	//}
	
	/*
	ofstream myfile;
	myfile.open("output.txt");
	myfile << "[" << endl;
	for(int i = 0; i < saved.size(); i++) {
		myfile << saved[i].to_print() << endl;
	}
	myfile << "{}" << endl;
	myfile << "]" << endl;
	myfile.close();
	*/
	
	return 0;
}

