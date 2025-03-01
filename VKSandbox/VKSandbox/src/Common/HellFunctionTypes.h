#pragma once
#include "File/FileFormats.h"
#include <string>

using NewFileCallback = void(*)(const std::string&);
using OpenFileCallback = void(*)(const std::string&);
//using OpenFileCallback = void(*)(FileFormat, const std::string&);