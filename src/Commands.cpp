#include <sstream>
#include "Commands.h"

using namespace std;
using namespace cache_server;

int Command::convertToInt(std::string val)
{
    try {
        size_t idx = 0;
        int intVal = std::stoi(val, &idx);
        if (idx != val.length()) {
            throw invalid_argument("Must be a number");
        }
        
        return intVal;
    }
    catch (invalid_argument& e) {
    }
    
    throw invalid_argument("Must be a number");
}

void Command::insertNewWords(vector<string>& tokens, string line)
{
    istringstream iss(line);
    
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(tokens));
}

vector<string> Command::parse(std::string line)
{
    vector<string> tokens;
    size_t start = 0;
    size_t end = 0;
    
    while (true) {
        end = line.find("\"", end);
        if (end == string::npos) {
            insertNewWords(tokens, line.substr(start));
            break;
        }
        if (end != 0) {
            insertNewWords(tokens, line.substr(start, end - start - 1));
        }
        start = end;
        end = line.find("\"", end + 1);
        if (end == string::npos) {
            throw invalid_argument("Invalid command");
        }
        tokens.push_back(line.substr(start + 1, end - start - 1));
        start = ++end;
        if (end == line.length()) {
            break;
        }
    }
    
    if (tokens.size() == 0) {
        throw invalid_argument("Invalid command");
    }
    
    return tokens;
}

shared_ptr<Command> Command::createCommand(string cmdline)
{
    vector<string> tokens = parse(cmdline);
    Command* pCmd = nullptr;
    
    if (tokens.size() == 0) {
        throw invalid_argument("Invalid Command");
    }
    
    if (tokens[0] == "SET") {
        pCmd = new SetCommand();
    }
    else
    if (tokens[0] == "GET") {
        pCmd = new GetCommand();
    }
    else
    if (tokens[0] == "GETSET") {
        pCmd = new GetSetCommand();
    }
    else
    if (tokens[0] == "APPEND") {
        pCmd = new AppendCommand();
    }
    else
    if (tokens[0] == "INCR") {
        pCmd = new IncrCommand();
    }
    else
    if (tokens[0] == "GETRANGE") {
        pCmd = new GetRangeCommand();
    }
    else
    if (tokens[0] == "MGET") {
        pCmd = new MGetCommand();
    }
    else
    if (tokens[0].substr(0, string(CMD_INTERNAL_PREFIX).length()) == CMD_INTERNAL_PREFIX) {
        pCmd = new InternalCommand();
    }
    else {
        throw invalid_argument("Invalid Command");
    }
    
    pCmd->setTokens(tokens);
    return shared_ptr<Command>(pCmd);
}

string Command::stringsToString(const vector<string>& vec)
{
    string out;
    for (string str : vec) {
        out += str + "\n";
    }
    return out;
}

//-------------------------------------------------------------------------

void Command::setTokens(const std::vector<std::string> &tokens)
{
    mTokens = tokens;
}

string Command::getCommandName() const
{
    if (mTokens.size() == 0) {
        return "";
    }
    
    return mTokens[0];
}


//-------------------------------------------------------------------------

string SetCommand::execute(CSMap& map)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    string key = mTokens[1];
    string val = mTokens[2];
    string expType = "";
    string expVal = "";
    int intVal = 0;
    
    if (cmdNum == Consts::MAX_ARG_NUM) {
        expType = mTokens[3];
        expVal = mTokens[4];

        if (expType != "EX") {
            throw invalid_argument("Invalid args");
        }

        intVal = convertToInt(expVal);
    }
    
    map.set(key, val, intVal);
    
    return to_string(map.get(mTokens[1]).length());
}

//-------------------------------------------------------------------------

string GetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    return map.get(mTokens[1]);
}

//-------------------------------------------------------------------------

string GetSetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    string key = mTokens[1];
    string val = mTokens[2];
    string expType = "";
    string expVal = "";
    int intVal = 0;
    
    if (cmdNum == Consts::MAX_ARG_NUM) {
        expType = mTokens[3];
        expVal = mTokens[4];

        if (expType != "EX") {
            throw invalid_argument("Invalid args");
        }
        
        intVal = convertToInt(expVal);
    }
    
    return map.getset(key, val, intVal);
}

//-------------------------------------------------------------------------

string AppendCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    string key = mTokens[1];
    string val = mTokens[2];
    string expType = "";
    string expVal = "";
    int intVal = 0;
    
    if (cmdNum == Consts::MAX_ARG_NUM) {
        expType = mTokens[3];
        expVal = mTokens[4];

        if (expType != "EX") {
            throw invalid_argument("Invalid args");
        }
        
        intVal = convertToInt(expVal);
    }
    
    return to_string(map.append(key, val, intVal));
}

//-------------------------------------------------------------------------

string IncrCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    string key = mTokens[1];
    
    return to_string(map.incr(key));
}

//-------------------------------------------------------------------------

string GetRangeCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }
    
    string key = mTokens[1];
    string start = mTokens[2];
    string end = mTokens[3];
    
    return map.getRange(key, convertToInt(start), convertToInt(end));
}

//-------------------------------------------------------------------------

string MGetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
        throw invalid_argument("Invalid args");
    }

    vector<string> keys(mTokens.begin() + 1, mTokens.end());

    return stringsToString(map.mget(keys));
}
