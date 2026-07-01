//
//  string.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_STRING_H
#define AVARA3D_UTIL_STRING_H

#include <string>
#include <vector>

namespace a3d {
	class Node;
}

namespace a3d::util::string {

	void 						Replace(std::string& str,
										const std::string& oldStr,
										const std::string& newStr);
	std::vector<std::string>	Split(const std::string& s,
									  std::string delim);
	std::string 				Lowercase(const std::string& s);

	// put somewhere else?

	std::string TreeString(const Node& root);
	std::string DateTimeString();

#ifdef A3D_POSIX
	std::string StackTraceString(unsigned dropFunctions = 0);
#endif
}

#endif //AVARA3D_UTIL_STRING_H
