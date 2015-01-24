#include "InMemoryData.h"

using namespace arag;
using namespace std;

InMemoryData::InMemoryData()
{
    _keyMap.subscribeMap(mStringMap);
    _keyMap.subscribeMap(mSetMap);
    _keyMap.subscribeMap(mSortedSetMap);
    _keyMap.subscribeMap(mListMap);
    _keyMap.subscribeMap(mHLLMap);
    _keyMap.subscribeMap(_hashMap);
}


