//
//  StdOutLogSink.cc
//  avara3d
//
//  Created by Morgan Davis on 11/9/23.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/log/sink/StdOutLogSink.h"

#include <iostream>

#ifdef A3D_WINDOWS
    #include <windows.h>
    #undef ERROR // see note at LOG_LEVEL
#endif

using namespace a3d;
using namespace std;

// [Public Lifecycle Functions]

StdOutLogSink::StdOutLogSink() {
    // on Windows, output often just kinda stops unless you manually flush it...
    // (or until more new stuff arrives).
#ifdef A3D_WINDOWS
    //setvbuf(stdout, nullptr, _IOLBF, 0); // line-buffered -- crashes?
    //setvbuf(stdout, nullptr, _IONBF, 0); // unbuffered (SLOW)
    cout.setf(ios::unitbuf); // flush after every insertion (not AS slow?)
#endif
}

StdOutLogSink::~StdOutLogSink() {
    flush();
}

// [Public Member Functions]

void StdOutLogSink::flush() {

    // also flushes cout
    // https://stackoverflow.com/questions/6027034/why-cerr-flushes-the-buffer-of-cout
    cerr.flush();
}

// [Internal Member Functions]

void StdOutLogSink::write(const string& output, Log::Level level) {

    if (static_cast<underlying_type<Log::Level>::type>(level)
        >= static_cast<underlying_type<Log::Level>::type>(Log::Level::Error)) {
        cerr << output;
    }
    else {
        cout << output;
    }

#ifdef A3D_WINDOWS
    OutputDebugStringA((const char*) output.c_str()); // this broke with C++20.  trying to include windows.h ^^
#endif
}
