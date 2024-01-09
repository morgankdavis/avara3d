//
//  main.cpp
//	avara-engine
//
//  Created by Morgan Davis on 12/02/23.
//  Copyright © 2023 Morgan K Davis. All rights reserved.
//


#include "Sandbox.h"


using namespace ae;
using namespace std;


int main(int argc, const char* argv[]) {

	auto args = vector<string>();
	for (int i=0; i<argc; ++i) {
		args.push_back(argv[i]);
	}

	sandbox::Sandbox().run(args);
}
