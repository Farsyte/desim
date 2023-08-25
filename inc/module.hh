#pragma once

class Module {
    const char* name;

public:
    // Modules in this simulation all have names.
    Module(const char* name);
    virtual ~Module();

    // Some modules will want to obtain pointers to Edge
    // and other objects owned by other modules, which may
    // not be available when this module is constructed,
    // so we require all modules to have a "linked" method
    // that can be called when all external dependencies
    // of the module have been linked.

    virtual void linked() = 0;
};
