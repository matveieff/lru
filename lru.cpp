//@brief:  Users cache
//@desc:   Simple tool for users LRU based cache demonstration
//@author: Grigory Matveev
//@date:   04.06.16

#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <list>
#include <stdexcept>

// @brief: Generic LRU cache class
template <typename Key, typename Value>
class LRUCache {
public:
    static constexpr uint32_t DEFAULT_CAPACITY = 1 << 10;

private:
    struct Entry {
        Key key;
        Value value;
        Entry(Key k, const Value& v) : key(k), value(v) {}
    };

public:
    explicit LRUCache(const uint32_t capacity = DEFAULT_CAPACITY);

protected:
    Value GetValue(const Key key);
    Value virtual DataSourceRequest(const Key key) const = 0;

private:
    void cacheValue(const Key key, const Value& value);

    uint32_t capacity_;
    std::list<Entry> entries_;

    typedef typename decltype(entries_)::const_iterator ConstEntryIterator;
    std::unordered_map<Key, ConstEntryIterator> key2Entry_;
};

//-----------------------------------------------------------------------------

template<typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(const uint32_t capacity)
    : capacity_(capacity)
{
}

//-----------------------------------------------------------------------------

template<typename Key, typename Value>
Value LRUCache<Key, Value>::GetValue(const Key key) {

    const auto cachedEntryIt = key2Entry_.find(key);

    if (cachedEntryIt == key2Entry_.end())
    {
        const auto value = DataSourceRequest(key);

        if (capacity_ == 0)
            return value;

        cacheValue(key, value);
    } else
    {
        entries_.splice(entries_.begin(), entries_, cachedEntryIt->second);
    }

    return entries_.front().value;

}

//-----------------------------------------------------------------------------

template<typename Key, typename Value>
void LRUCache<Key, Value>::cacheValue(const Key key, const Value& value) {

    if (entries_.size() == capacity_)
    {
        auto outdatedKey = entries_.back().key;
        key2Entry_.erase(outdatedKey);
        entries_.pop_back();
    }

    entries_.push_front(Entry(key, value));
    key2Entry_.insert(make_pair(key, entries_.begin()));
}

//-----------------------------------------------------------------------------

// @brief: User cache specialization
class UserCache : public LRUCache<uint32_t, std::string> {
    typedef std::unordered_map<uint32_t, std::string> DataSource;

public:
    explicit UserCache(const DataSource* dataSource,
        const uint32_t capacity = DEFAULT_CAPACITY) throw (std::runtime_error);

    std::string GetUserById(const uint32_t userID);

private:
    std::string virtual DataSourceRequest(const uint32_t key) const override;

    // @brief: throws an error if user with such userID is not found
    std::string getUserByID(const uint32_t userID) const throw(std::out_of_range);

    const DataSource* dataSource_;
};

//-----------------------------------------------------------------------------

UserCache::UserCache(const DataSource* dataSource, const uint32_t capacity) throw(std::runtime_error)
    : LRUCache(capacity)
    , dataSource_(dataSource)
    {
        if (dataSource_ == nullptr)
            throw std::runtime_error("data source is empty");
    }

//-----------------------------------------------------------------------------

std::string UserCache::GetUserById(const uint32_t userID) {
    return GetValue(userID);
}

//-----------------------------------------------------------------------------

std::string UserCache::DataSourceRequest(const uint32_t key) const {
    return getUserByID(key);
}

//-----------------------------------------------------------------------------

std::string UserCache::getUserByID(const uint32_t userID) const throw(std::out_of_range) {
    if (dataSource_ == nullptr)
        throw std::out_of_range("data source is missed");
    return dataSource_->at(userID);
}

//-----------------------------------------------------------------------------

int main() {

    std::unordered_map<uint32_t, std::string> data;
    data.insert(std::make_pair(1, "Frank Sinatra"));
    data.insert(std::make_pair(5, "Darth Vader"));
    data.insert(std::make_pair(10, "John Lennon"));

    uint32_t testCapacity = 2;
    UserCache userCache(&data, testCapacity);

    uint32_t rID = 0;
    std::cout << "rID: [" << rID++ << "] fill the cache: " << userCache.GetUserById(1) << "\n";
    std::cout << "rID: [" << rID++ << "] fill the cache: " << userCache.GetUserById(5) << "\n";

    data.erase(1);
    std::cout << "rID: [" << rID++ << "] check cached: " << userCache.GetUserById(1) << "\n";

    std::cout << "rID: [" << rID++ << "] fill the cache: " << userCache.GetUserById(10) << "\n";
    std::cout << "rID: [" << rID++ << "] fill the cache: " << userCache.GetUserById(5) << "\n";

    try {
        userCache.GetUserById(1);
    } catch (std::out_of_range& ex) {
        std::cout << "rID: [" << rID++ << "]: expected bad ID " << ex.what() << "\n";
    }
    return 0;
}
