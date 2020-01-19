#include "stdafx.h"
#include <string>
#include <utility>
#include "safe_vector.hpp"

void SafeVector::insert(string in, const int index)
{
	lock_guard<mutex> lock(mut);
	vec[index] = move(in);
	cond.notify_one();
}

void SafeVector::push_back(string in)
{
	lock_guard<mutex> lock(mut);
	vec.push_back(move(in));
	cond.notify_one();
}

void SafeVector::pop_back()
{
	lock_guard<mutex> lock(mut);
	vec.pop_back();
	cond.notify_one();
}

string& SafeVector::back()
{
	return vec.back();
}

size_t SafeVector::size() {
	return vec.size();
}

string& SafeVector::operator[](const int index)
{
	return vec[index];
}

vector<string>::iterator SafeVector::begin()
{
	return vec.begin();
}

vector<string>::iterator SafeVector::end()
{
	return vec.end();
}

vector<string> SafeVector::toVector()
{
	return vec;
}