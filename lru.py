#!/usr/bin/env python3

from collections import OrderedDict

def lru_cache(capacity):

    def fn_wrapper(dataSourceRequestFn):
        _entries = OrderedDict()

        def wrapper(key):
            if key in _entries:
                _entries.move_to_end(key, last=False)
                return _entries[key]

            value = dataSourceRequestFn(key)
            if len(_entries) == capacity:
                _entries.popitem(last=True)
            _entries[key] = value
            _entries.move_to_end(key, last=False)

            return value
        return wrapper
    return fn_wrapper

data = {1: "Frank Sinatra", 5: "Darth Vader", 10: "John Lennon"}

@lru_cache(2)
def dataSourceRequest(key):
    return data[key]

if __name__ == '__main__':

    print("fill the cache: {}".format(dataSourceRequest(1)))
    print("fill the cache: {}".format(dataSourceRequest(5)))

    del data[1]

    print("check cached: {}".format(dataSourceRequest(1)))

    print("fill the cache: {}".format(dataSourceRequest(10)))
    print("fill the cache: {}".format(dataSourceRequest(5)))

    try:
        dataSourceRequest(1)
    except KeyError:
        print("expected error on bad ID")
