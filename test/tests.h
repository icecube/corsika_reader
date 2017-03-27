#pragma once
#undef NDEBUG
#include <corsika/RawStream.h>
#include <corsika/Block.h>
#include <corsika/ShowerFile.h>
#include <corsika/Units.h>
#include <corsika/Index.h>
#include <string>
#include <vector>
#define ENSURE_EQUAL(a, b, ...) assert(a == b)
using namespace corsika;

void test_low_high(const char* dir);
void test_file(const char* directory);
void test_rawstream(const char* directory);
void test_index();
