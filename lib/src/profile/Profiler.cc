//
//  Profiler.cc
//  avara3d
//
//  Created by Morgan Davis on 12/6/25.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/profile/Profiler.h"

#include "a3d/log/Log.h"

using namespace std;

namespace a3d {

// [Public Member Functions]

void Profiler::add(Tag tag, chrono::nanoseconds ns) noexcept {
    if (_taggedSamples.contains(tag)) {
        _taggedSamples[tag] = _taggedSamples[tag] + ns;
    }
    else {
        _taggedSamples[tag] = ns;
    }
}

void Profiler::add(const string& key, chrono::nanoseconds ns) noexcept {
    if (_keyedSamples.contains(key)) {
        _keyedSamples[key] = _keyedSamples[key] + ns;
    }
    else {
        _keyedSamples[key] = ns;
    }
}

// void Profiler::subtract(Tag tag, std::chrono::nanoseconds ns) {
//     if (_taggedSamples.contains(tag)) {
//         // ! note, if ns is larger than _taggedSamples[tag], bad things happen.
//         auto newNS = _taggedSamples[tag] - ns;
//         _taggedSamples[tag] = newNS;
//     }
//     else {
//         log::e()("Can't subtract {}ns for {} -- no samples exist yet for tag.", ns,
//                  util::enums::enum_name(tag));
//     }
// }
//
// void Profiler::subtract(const std::string& key, std::chrono::nanoseconds ns) {
//     if (_keyedSamples.contains(key)) {
//         // ! note, if ns is larger than _taggedSamples[tag], bad things happen.
//         auto newNS = _keyedSamples[key] - ns;
//         _keyedSamples[key] = newNS;
//     }
//     else {
//         log::e()("Can't subtract {}ns key '{}' -- no samples exist yet for key.", ns, key);
//     }
// }

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

} // namespace a3d
