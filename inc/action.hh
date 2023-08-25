#pragma once

#include <functional>

// The "Action" type is the common method used in
// this project for representing a bit of code to
// be run.

typedef std::function<void()> Action;

// The "ACTION" macro wraps up a statement or statement
// list into an Action for passing to a service function.

#define ACTION(body) [=] { body; }
