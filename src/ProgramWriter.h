#pragma once

namespace pic {

class ProgramWriter {
    virtual void EraseProgram() = 0;
    virtual void EraseData() = 0;
    virtual void EraseFull() = 0;
    virtual void WriteConfiguration() = 0;
    virtual void WriteProgram() = 0;
    virtual void WriteData() = 0;
    virtual void WriteFull() = 0;
    virtual void RewriteFull() = 0;
};

}
