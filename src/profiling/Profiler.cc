
#include "a3d/profiling/Profiler.h"

using namespace a3d;
using namespace std;

//Profiler& Profiler::Instance() {
//	static Profiler inst;
//	return inst;
//}

void Profiler::add(Tag tag, std::chrono::nanoseconds ns) {
	_taggedSamples.insert({tag, ns});
}

void Profiler::add(string key, std::chrono::nanoseconds ns) {
	_keyedSamples.insert({key, ns});
}

std::chrono::nanoseconds Profiler::time(Tag tag) const {
	std::chrono::nanoseconds ns;
	for (auto& item : _taggedSamples) {
		if (get<0>(item) == tag) ns += get<1>(item);
	}
	return ns;
}

std::chrono::nanoseconds Profiler::time(string key) const {
	std::chrono::nanoseconds ns;
	for (auto& item : _keyedSamples) {
		if (get<0>(item) == key) ns += get<1>(item);
	}
	return ns;
}

void Profiler::reset() {
	_taggedSamples.clear();
	_keyedSamples.clear();
}
