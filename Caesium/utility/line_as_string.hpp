#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define __LINE_AS_STRING__ TOSTRING(__LINE__)
