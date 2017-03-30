#include "context.hpp"
using namespace std;


Context::Context() {
	for(int i = 0; i < 32; i++) {
		regs[i] = false;
	}
	regs[0] = true;
	regs[1] = true;
	regs[2] = true;
	regs[3] = true;
	regs[4] = true;
	regs[5] = true;
	regs[6] = true;
	regs[7] = true;
	regs[26] = true;
	regs[27] = true;
	regs[28] = true;
	regs[29] = true;
	regs[30] = true;
	regs[31] = true;
	
	SP = 0;
	fsize = 0;
	fsize_org = 0;
	
	paramNo = 0;
	varNo = 0;
	globlVar = 0;
	
	argsNo = 4;
	savedNo = 8;
}
	
Context::Context(Context* c) {
	for(int i = 0; i < 32; i++) {
		regs[i] = c->regs[i];
	}
	SP = c->SP;
	fsize = c->fsize;
	fsize_org = c->fsize_org;
	paramNo = c->paramNo;
	varNo = c->varNo;
	globlVar = c->globlVar;
	argsNo = c->argsNo;
	savedNo = c->savedNo;
}

Context::~Context() {}

vector<unsigned int> Context::freeSavedRegisters() {
	
	vector<unsigned int> free;
	
	for(int i = 16; i < 24; i++) {
		if(!regs[i]) {
			free.push_back(i);
		}
	}
	
	if(free.empty()) {
		cout << "no free registers" << endl;
		exit(1);
	}
	
	return free;
	
}

vector<unsigned int> Context::freeTmpRegisters() {
	
	vector<unsigned int> free;
	
	for(int i = 8; i < 16; i++) {
		if(!regs[i]) {
			free.push_back(i);
		}
	}
	
	for(int i = 24; i < 26; i++) {
		if(!regs[i]) {
			free.push_back(i);
		}
	}
	
	if(free.empty()) {
		cout << "no free temporary registers" << endl;
		exit(1);
	}
	
	return free;
	
}


// for Expressions
void Context::setUsed(unsigned int i) {
	regs[i] = true;
}
	
void Context::setUnused(unsigned int i) {
	regs[i] = false;
}

// for Global Variables
void Context::addGlobal(const std::string *name) {
	unsigned int loc = globalBindings.size();
	globalBindings.emplace(*name, loc);
}

void Context::deleteGlobal(const std::string *name) {
	globalBindings.erase(*name);
}

unsigned int Context::findGlobal(const std::string *name) {
	
	if(globalBindings.find(*name) == globalBindings.end()) {
		// not found
		cout << "Global variable " << *name << " not found" << endl;
		exit(1);
	} else {
		// found
		return globalBindings.find(*name)->second;
	}
}

bool Context::isGlobal(const std::string *name) {
	
	if(this->globalBindings.find(*name) == this->globalBindings.end()) {
		// not a variable
		return false;	
	} else {
		// is a variable or parameter
		return true;
	}
}

// for Variables
void Context::addVariable(const std::string *name) {
	unsigned int loc = variableBindings.size();
	variableBindings.emplace(*name, loc);
}

void Context::deleteVariable(const std::string *name) {
	variableBindings.erase(*name);
}

unsigned int Context::findVariable(const std::string *name) {
	
	if(variableBindings.find(*name) == variableBindings.end()) {
		// not found
		cout << "Variable " << *name << " not found on stack" << endl;
		exit(1);
	} else {
		// found
		return variableBindings.find(*name)->second;
	}
	
}

bool Context::isVariable(const std::string *name) {
	
	if(this->variableBindings.find(*name) == this->variableBindings.end()) {
		// not a variable
		return false;	
	} else {
		// is a variable or parameter
		return true;
	}
}

// for Dynamicly allocated variables
void Context::addDynamic(const std::string *name) {
	unsigned int loc = dynamicBindings.size();
	dynamicBindings.emplace(*name, loc);
}

void Context::deleteDynamic(const std::string *name) {
	dynamicBindings.erase(*name);
}

unsigned int Context::findDynamic(const std::string *name) {
	
	if(dynamicBindings.find(*name) == dynamicBindings.end()) {
		// not found
		cout << "Variable " << *name << " not found" << endl;
		exit(1);
	} else {
		// found
		return dynamicBindings.find(*name)->second;
	}
}

bool Context::isDynamic(const std::string *name) {
	
	if(this->dynamicBindings.find(*name) == this->dynamicBindings.end()) {
		// not a dynamically-allocated variable
		return false;
	} else {
		// is a dynamically-allocated variable
		return true;
	}
}

// find where something is and return it's position on stack/registers
bool Context::isOnStack(const std::string *name) {
	
	if(this->isVariable(name)) {
		// it's a variable or a parameter
		return true;
		
	} else if (this->isDynamic(name)) {
		// it's a dynamically-allocated variable
		return true;
		
	} else if (this->isGlobal(name)) {
		// it's a global variable
		return true;
		
	} else {
		// it's not on the stack
		return false;
	}
}

std::string Context::findOnStack(const std::string *name) {
	
	if(this->isVariable(name)) {
		// it's a variable or a parameter
		int loc = this->findVariable(name);
		return std::to_string(loc * -4) + "($fp)";
		
	} else if (this->isDynamic(name)) {
		// it's a dynamically-allocated variable
		int dynNo = dynamicBindings.size();
		unsigned int loc = this->findDynamic(name);
		return std::to_string((dynNo - 1 - loc)*8) + "($sp)";
		
	} else if (this->isGlobal(name)) {
		// it's a global variable
		//return *name + "($gp)";
		return "%got(" + *name + ")($28)";
		
	}
	
	return "something went wrong with variable allocation";
}
