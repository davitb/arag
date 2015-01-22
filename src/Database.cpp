#include "Database.h"
#include "Config.h"

using namespace arag;
using namespace std;

//---------------------------------------------------

Database& Database::instance()
{
    static Database sDB(Config::DATABASE_COUNT);
    return sDB;
}

Database::Database(int count)
{
    mDatabases = vector<InMemoryData>(count);
}

InMemoryData& Database::get(int index)
{
    if (index >= mDatabases.size()) {
        throw EWrongDBIndex();
    }
    
    return mDatabases[index];
}

void Database::flush(int index)
{
    if (index >= mDatabases.size()) {
        throw EWrongDBIndex();
    }
    
    if (index == FLUSH_ALL) {
        for (int i = 0; i < mDatabases.size(); ++i) {
            mDatabases[i].flush();
        }
    }
    else {
        mDatabases[index].flush();
    }
}
