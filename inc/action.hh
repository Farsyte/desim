#pragma once

#include <functional>

typedef std::function<void()> Action;
#define ACTION(body) [=] { body; }
