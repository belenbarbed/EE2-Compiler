#ifndef context_hpp
#define context_hpp

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception>


class Context {
public:
	bool regs[32];
	unsigned int SP;													// current offset from original $sp initialisation
	unsigned int fsize_org;
	unsigned int fsize;
	
	int paramNo;
	int varNo;
	int globlVar;
	
	int argsNo;
	int savedNo;
	
	std::unordered_map<std::string, unsigned int> variableBindings;		// in bottom of stack
	std::unordered_map<std::string, unsigned int> dynamicBindings;		// on top of stack (modify SP accordingly)
	
	std::unordered_map<std::string, unsigned int> globalBindings;		// for global variables

	Context();
	Context(Context* c);
	~Context();
	
	std::vector<unsigned int> freeSavedRegisters();
	std::vector<unsigned int> freeTmpRegisters();
	
	// for Expressions
	void setUsed(unsigned int i);
	void setUnused(unsigned int i);
	
	// for Global Variables
	void addGlobal(const std::string *name);
	void deleteGlobal(const std::string *name);
	unsigned int findGlobal(const std::string *name);
	bool isGlobal(const std::string *name);
	
	// for Variables
	void addVariable(const std::string *name);
	void deleteVariable(const std::string *name);
	unsigned int findVariable(const std::string *name);
	bool isVariable(const std::string *name);
	
	// for Dynamicly allocated variables
	void addDynamic(const std::string *name);
	void deleteDynamic(const std::string *name);
	unsigned int findDynamic(const std::string *name);
	bool isDynamic(const std::string *name);
	
	// find where something is and return it's position on the stack
	bool isOnStack(const std::string *name);
	std::string findOnStack(const std::string *name);

};


#endif
