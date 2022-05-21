#pragma once

namespace pic {

class ProgramReader {
public:
    virtual void ReadConfiguration() = 0;
    virtual void ReadProgram() = 0;
    virtual void ReadData() = 0;
    virtual void ReadFull() = 0;
};

}
