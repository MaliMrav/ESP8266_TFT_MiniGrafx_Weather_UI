#include "DataSourceManager.h"

bool DataSourceManager::add(IDataSource& source)
{
    if (count_ >= MAX_SOURCES) return false;
    sources_[count_++] = &source;
    return true;
}

void DataSourceManager::begin()
{
    for (uint8_t i = 0; i < count_; i++)
        sources_[i]->begin();
}

void DataSourceManager::loop()
{
    for (uint8_t i = 0; i < count_; i++)
        sources_[i]->loop();
}
