//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 10/15/17.
//  Copyright © 2017 Morgan K Davis. All rights reserved.
//


#include "Example.h"


using namespace ae;
using namespace std;


int main(int argc, const char* argv[]) {

	auto args = vector<string>();
	for (int i=0; i<argc; ++i) {
		args.push_back(argv[i]);
	}

	example::Example().run(args);
}
