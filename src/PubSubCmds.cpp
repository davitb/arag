#include "PubSubCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include <iostream>

using namespace std;
using namespace arag;
using namespace arag::command_const;

static CommandResult::ResultArray prepareResponse(const string& type,
                              const string& channel,
                              const string& num,
                              int bInteger = true)
{
    CommandResult::ResultArray arr = {
        make_pair(type, RedisProtocol::BULK_STRING),
        make_pair(channel, RedisProtocol::BULK_STRING),
        bInteger ? make_pair(num, RedisProtocol::INTEGER) :
        make_pair(num, RedisProtocol::BULK_STRING)
    };
    
    return arr;
}

static CommandResult::ResultArray prepareResponseWithPattern(const string& type,
                              const string& pattern,
                              const string& channel,
                              const string& msg)
{
    CommandResult::ResultArray arr = {
        make_pair(type, RedisProtocol::BULK_STRING),
        make_pair(pattern, RedisProtocol::BULK_STRING),
        make_pair(channel, RedisProtocol::BULK_STRING),
        make_pair(msg, RedisProtocol::BULK_STRING)
    };
    
    return arr;
}

static CommandResult::ResultArray prepareResponseWithNill(const string& type)
{
    CommandResult::ResultArray arr = {
        make_pair(type, RedisProtocol::BULK_STRING),
        make_pair("", RedisProtocol::NILL),
        make_pair("0", RedisProtocol::INTEGER)
    };
    
    return arr;
}

// Subscriber to N channels
static CommandResultPtr subscribe(const RedisProtocol::RedisArray& tokens,
                        PubSubMap& pubSub,
                        SessionContext& ctx,
                        const string& type,
                        bool pattern)
{
    CommandResultPtr response(new CommandResult(CommandResult::MULTI_ARRAY_RESPONSE));
    for (int i = 1; i < tokens.size(); ++i) {
        int sid = ctx.getSessionID();
        
        pubSub.addSubscriber(tokens[i].first, sid, pattern);
        
        response->appendToArray(prepareResponse(type,
                                    tokens[i].first,
                                    to_string(pubSub.getSubscriptionsNum(sid))));
    }
    
    return response;
}

// Unsubscriber from N channels
static CommandResultPtr unsubscribe(const RedisProtocol::RedisArray& tokens,
                          PubSubMap& pubSub,
                          SessionContext& ctx,
                          const string& type)
{
    CommandResultPtr response(new CommandResult(CommandResult::MULTI_ARRAY_RESPONSE));
    
    // If no channel provided - unscubscribe from all channels
    if (tokens.size() == 1) {
        vector<string> channels = pubSub.unsubscribeFromAllChannels(ctx.getSessionID());
        if (channels.size() == 0) {
            return CommandResultPtr(new CommandResult(prepareResponseWithNill(type)));
        }
        for (int i = 0; i < channels.size(); ++i) {
            response->appendToArray(prepareResponse(type,
                                        channels[i],
                                        to_string((int)channels.size() - i - 1)));
        }
        return response;
    }
    
    for (int i = 1; i < tokens.size(); ++i) {
        int sid = ctx.getSessionID();
        
        pubSub.removeSubscriber(tokens[i].first, sid);
        
        response->appendToArray(prepareResponse(type,
                                    tokens[i].first,
                                    to_string(pubSub.getSubscriptionsNum(sid))));
    }
    
    return response;
}

//-------------------------------------------------------------------------

CommandResultPtr SubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        return subscribe(mTokens, db.getPubSubMap(), ctx, "subscribe", false);
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PSubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        return subscribe(mTokens, db.getPubSubMap(), ctx, "psubscribe", true);
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PublishCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }

        const string& channel = mTokens[1].first;
        const string& message = mTokens[2].first;
        
        PubSubMap& pubSub = db.getPubSubMap();
        
        // Get all subscribers for this channel
        // FIXME. this needs an optimization
        list<PubSubMap::PubSubElement> subscribersList = pubSub.getSubscribers(channel);

        int num = 0;
        
        for (auto subscrs : subscribersList) {
            for (auto sid : subscrs.first) {
                try {
                    ClientSession& session = Arag::instance().getClientSession(sid);
                    
                    string response;
                    
                    // Check if it's a pattern based channel
                    if (subscrs.second.first == true) {
                        CommandResult res = prepareResponseWithPattern("pmessage",
                                                                       subscrs.second.second,
                                                                       channel,
                                                                       message);
                        response = res.toRedisResponse();
                    }
                    else {
                        response = CommandResult(prepareResponse("message",
                                                   channel,
                                                   message,
                                                   false)).toRedisResponse();
                    }
                    
                    session.writeResponse(response);
                    
                    num++;
                }
                catch (AragException& e) {
                    //cout << e.what() << endl;
                    pubSub.removeSubscriber(subscrs.second.second, sid);
                }
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(num), RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr UnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }

        return unsubscribe(mTokens, db.getPubSubMap(), ctx, "unsubscribe");
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PUnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        return unsubscribe(mTokens, db.getPubSubMap(), ctx, "punsubscribe");
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
