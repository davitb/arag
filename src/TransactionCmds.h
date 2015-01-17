#ifndef __arag__TransactionCmds__
#define __arag__TransactionCmds__

#include "Commands.h"

namespace arag
{
  
    COMMAND_CLASS(MultiCommand, 1, 1);

    COMMAND_CLASS_WITH_CONSTRUCTOR(ExecCommand, 1, 1);

    COMMAND_CLASS_WITH_CONSTRUCTOR(DiscardCommand, 1, 1);
    
    COMMAND_CLASS(WatchCommand, 2, INT_MAX);
    
    COMMAND_CLASS(UnwatchCommand, 1, 1);
};

#endif /* defined(__arag__TransactionCmds__) */
