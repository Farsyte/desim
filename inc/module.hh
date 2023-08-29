#pragma once

class Module {
public:
    Module(const char* name);

    // Some modules will want to obtain pointers to Edge
    // and other objects owned by other modules, which may
    // not be available when this module is constructed,
    // so we require all modules to have a "linked" method
    // that can be called when all external dependencies
    // of the module have been linked.

    virtual void linked() = 0;

    const char* name;
};
