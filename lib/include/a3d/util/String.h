//
//  String.h
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTIL_STRING_H
#define AVARA3D_UTIL_STRING_H

#include <string>
#include <string_view>
#include <vector>

namespace a3d {

    class Node;

}

namespace a3d::util::string {

    void                     Replace(std::string& str, const std::string& oldStr, const std::string& newStr);
    std::vector<std::string> Split(const std::string& s, std::string delim);
    std::string              Uppercase(std::string_view s);
    std::string              Lowercase(std::string_view s);

    // put somewhere else?

    std::string              Tree(const Node& root);
    std::string              Timestamp();

}

#endif // AVARA3D_UTIL_STRING_H
