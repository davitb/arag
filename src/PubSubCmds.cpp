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
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair(channel, RedisProtocol::DataType::BULK_STRING),
        bInteger ? make_pair(num, RedisProtocol::DataType::INTEGER) :
        make_pair(num, RedisProtocol::DataType::BULK_STRING)
    };
    
    return arr;
}

static CommandResult::ResultArray prepareResponseWithPattern(const string& type,
                              const string& pattern,
                              const string& channel,
                              const string& msg)
{
    CommandResult::ResultArray arr = {
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair(pattern, RedisProtocol::DataType::BULK_STRING),
        make_pair(channel, RedisProtocol::DataType::BULK_STRING),
        make_pair(msg, RedisProtocol::DataType::BULK_STRING)
    };
    
    return arr;
}

static CommandResult::ResultArray prepareResponseWithNill(const string& type)
{
    CommandResult::ResultArray arr = {
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair("", RedisProtocol::DataType::NILL),
        make_pair("0", RedisProtocol::DataType::INTEGER)
    };
    
    return arr;
}

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
                                    to_string(pubSub.getSubscribersNum(sid))));
    }
    
    return response;
}

static CommandResultPtr unsubscribe(const RedisProtocol::RedisArray& tokens,
                          PubSubMap& pubSub,
                          SessionContext& ctx,
                          const string& type)
{
    CommandResultPtr response(new CommandResult(CommandResult::MULTI_ARRAY_RESPONSE));
    
    if (tokens.size() == 1) {
        // Unscubscribe from all channels
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
                                    to_string(pubSub.getSubscribersNum(sid))));
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
            throw invalid_argument("Invalid args");
        }
        
        return subscribe(mTokens, db.getPubSubMap(), ctx, "subscribe", false);
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PSubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return subscribe(mTokens, db.getPubSubMap(), ctx, "psubscribe", true);
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PublishCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string channel = mTokens[1].first;
        string message = mTokens[2].first;
        
        PubSubMap& pubSub = db.getPubSubMap();
        
        list<PubSubMap::PubSubElement> subscribersList = pubSub.getSubscribers(channel);

        int num = 0;
        
        for (auto subscrs : subscribersList) {
            for (auto sid : subscrs.first) {
                try {
                    ClientSession& session = Arag::instance().getClientSession(sid);
                    
                    string response;
                    
                    if (subscrs.second.first == true) {
                        // This is a pattern based channel
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
                catch (std::invalid_argument& e) {
                    //cout << e.what() << endl;
                    pubSub.removeSubscriber(subscrs.second.second, sid);
                }
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(num), RedisProtocol::DataType::INTEGER));
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr UnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        return unsubscribe(mTokens, db.getPubSubMap(), ctx, "unsubscribe");
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr PUnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return unsubscribe(mTokens, db.getPubSubMap(), ctx, "punsubscribe");
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}
