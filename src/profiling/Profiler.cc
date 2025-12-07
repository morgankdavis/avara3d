//
//  Profiler.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/profiling/Profiler.h"

using namespace a3d;
using namespace std;

/// Public Member Functions ///

void Profiler::add(Tag tag, chrono::nanoseconds ns) {
	if (_taggedSamples.contains(tag)) {
		_taggedSamples[tag] = _taggedSamples[tag] + ns;
	}
	else {
		_taggedSamples[tag] = ns;
	}
}

void Profiler::add(const string& key, chrono::nanoseconds ns) {
	if (_keyedSamples.contains(key)) {
		_keyedSamples[key] = _keyedSamples[key] + ns;
	}
	else {
		_keyedSamples[key] = ns;
	}
}

chrono::nanoseconds Profiler::time(Tag tag) {
	if (_taggedSamples.contains(tag)) {
		return _taggedSamples[tag];
	}
	return chrono::nanoseconds(0);
}

chrono::nanoseconds Profiler::time(const string& key) {
	if (_keyedSamples.contains(key)) {
		return _keyedSamples[key];
	}
	return chrono::nanoseconds(0);
}

void Profiler::reset() {
	_taggedSamples.clear();
	_keyedSamples.clear();
}
