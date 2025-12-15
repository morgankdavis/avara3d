//
//  Profiler.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/profiling/Profiler.h"

#include "magic_enum.hpp"

#include "a3d/diagnostic/log/Log.h"

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

void Profiler::subtract(Tag tag, std::chrono::nanoseconds ns) {
	if (_taggedSamples.contains(tag)) {
		auto newNS = _taggedSamples[tag] - ns;
		A3D_LOG_W("Profiler tag {} time is negative.", magic_enum::enum_name(tag));
		_taggedSamples[tag] = newNS;
	}
	else {
		_taggedSamples[tag] = ns;
	}
}
void Profiler::subtract(const std::string& key, std::chrono::nanoseconds ns) {
	if (_keyedSamples.contains(key)) {
		auto newNS = _keyedSamples[key] - ns;
		A3D_LOG_W("Profiler key {} time is negative.", key);
		_keyedSamples[key] = newNS;
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
