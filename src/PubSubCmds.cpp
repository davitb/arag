#include "PubSubCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include <iostream>

using namespace std;
using namespace arag;
using namespace arag::command_const;

static string prepareResponse(const string& type,
                              const string& channel,
                              const string& num,
                              int bInteger = true)
{
    RedisProtocol::RedisArray arr = {
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair(channel, RedisProtocol::DataType::BULK_STRING),
        bInteger ? make_pair(num, RedisProtocol::DataType::INTEGER) :
        make_pair(num, RedisProtocol::DataType::BULK_STRING)
    };
    
    return RedisProtocol::serializeArray(arr);
}

static string prepareResponseWithPattern(const string& type,
                              const string& pattern,
                              const string& channel,
                              const string& msg)
{
    RedisProtocol::RedisArray arr = {
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair(pattern, RedisProtocol::DataType::BULK_STRING),
        make_pair(channel, RedisProtocol::DataType::BULK_STRING),
        make_pair(msg, RedisProtocol::DataType::BULK_STRING)
    };
    
    return RedisProtocol::serializeArray(arr);
}

static string prepareResponseWithNill(const string& type)
{
    RedisProtocol::RedisArray arr = {
        make_pair(type, RedisProtocol::DataType::BULK_STRING),
        make_pair("", RedisProtocol::DataType::NILL),
        make_pair("0", RedisProtocol::DataType::INTEGER)
    };
    
    return RedisProtocol::serializeArray(arr);
}

static string subscribe(const RedisProtocol::RedisArray& tokens,
                        InMemoryData& db,
                        SessionContext& ctx,
                        const string& type,
                        bool pattern)
{
    string response;
    for (int i = 1; i < tokens.size(); ++i) {
        int sid = ctx.getSessionID();
        
        db.addSubscriber(tokens[i].first, sid, pattern);
        
        response += prepareResponse(type,
                                    tokens[i].first,
                                    to_string(db.getSubscribersNum(sid)));
    }
    
    return response;
}

static string unsubscribe(const RedisProtocol::RedisArray& tokens,
                          InMemoryData& db,
                          SessionContext& ctx,
                          const string& type)
{
    string response;
    
    if (tokens.size() == 1) {
        // Unscubscribe from all channels
        vector<string> channels = db.unsubscribeFromAllChannels(ctx.getSessionID());
        if (channels.size() == 0) {
            return prepareResponseWithNill(type);
        }
        for (int i = 0; i < channels.size(); ++i) {
            response += prepareResponse(type,
                                        channels[i],
                                        to_string((int)channels.size() - i - 1));
        }
        return response;
    }
    
    for (int i = 1; i < tokens.size(); ++i) {
        int sid = ctx.getSessionID();
        
        db.removeSubscriber(tokens[i].first, sid);
        
        response += prepareResponse(type,
                                    tokens[i].first,
                                    to_string(db.getSubscribersNum(sid)));
    }
    
    return response;
}

//-------------------------------------------------------------------------

string SubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return subscribe(mTokens, db, ctx, "subscribe", false);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string PSubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return subscribe(mTokens, db, ctx, "psubscribe", true);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string PublishCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string channel = mTokens[1].first;
        string message = mTokens[2].first;
        
        list<InMemoryData::PubSubElement> subscribersList = db.getSubscribers(channel);

        int num = 0;
        
        for (auto subscrs : subscribersList) {
            for (auto sid : subscrs.first) {
                try {
                    ClientSession& session = Arag::instance().getClientSession(sid);
                    
                    string response;
                    
                    if (subscrs.second.first == true) {
                        // This is a pattern based channel
                        response = prepareResponseWithPattern("pmessage",
                                                              subscrs.second.second,
                                                              channel,
                                                              message);
                    }
                    else {
                        response = prepareResponse("message",
                                                   channel,
                                                   message,
                                                   false);
                    }
                    
                    session.writeResponse(response);
                    
                    num++;
                }
                catch (std::invalid_argument& e) {
                    //cout << e.what() << endl;
                    db.removeSubscriber(subscrs.second.second, sid);
                }
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(num), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string UnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        return unsubscribe(mTokens, db, ctx, "unsubscribe");
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string PUnsubscribeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return unsubscribe(mTokens, db, ctx, "punsubscribe");
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
