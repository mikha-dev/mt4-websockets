#ifndef SAFEVECTOR_HPP
#define SAFEVECTOR_HPP

#include <vector>
#include <mutex>
#include <condition_variable>
#include <string>
using namespace std;

class SafeVector {
public:
	SafeVector() : vec(), mut(), cond() {}
	SafeVector(const SafeVector& orig) : vec(orig.vec), mut(), cond() {}
	~SafeVector() {}

	void insert(string in, const int index);
	void push_back(string in);
	void pop_back();
	size_t size();
	string& operator[](const int index);
	string& back();
	vector<string>::iterator begin();
	vector<string>::iterator end();
	vector<string> toVector();

private:
	vector<string> vec;
	mutex mut;
	condition_variable cond;
};

#endif /* SAFEVECTOR_HPP */