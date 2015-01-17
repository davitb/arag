#ifndef __arag__PubSubCmds__
#define __arag__PubSubCmds__

#include "Commands.h"

namespace arag
{
    
    COMMAND_CLASS(SubscribeCommand, 2, INT_MAX);

    COMMAND_CLASS(PSubscribeCommand, 2, INT_MAX);
    
    COMMAND_CLASS(PublishCommand, 3, 3);
    
    COMMAND_CLASS(UnsubscribeCommand, 1, INT_MAX);
    
    COMMAND_CLASS(PUnsubscribeCommand, 1, INT_MAX);

};


#endif /* defined(__arag__PubSubCmds__) */
