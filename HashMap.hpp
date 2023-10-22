#ifndef _HASHMAP_HPP_
#define _HASHMAP_HPP_

#include <stdexcept>
#include <functional>
#include <vector>
#define START_CAPACITY 16
#define LOWER_BOUNDARY 0.25
#define UPPER_BOUNDARY 0.75
#define RANGE_ERROR_MSG "Index is out of range!"
#define INVALID_ARG_MSG "Argument is invalid/not in map!"

template <class KeyT, class ValueT>
class HashMap
{
 private:
  int _size; // cur num of objects stored in the map
  int _capacity; // max num of objects that can be stored in the map
  std::vector<std::pair<KeyT, ValueT>>* _map; // array of vectors, each
                                        // vector contains pairs of <key,value>

  /**
   * hashes the given key to an index in the map.
   * @param key KeyT
   * @return int index in the map (vector)
   */
  int hash_function(KeyT key) const
  {
    int converted_key = std::hash<KeyT>{}(key);
    int index = converted_key & (_capacity - 1); // location in map (vector)
    return index;
  }

  /**
   * update the size of the map if it crosses the upper or lower boundary,
   * and rehash the map's elements to new locations.
   * @return true if rehashing finished successfully, false otherwise.
   */
  bool rehash()
  {
    int old_capacity = _capacity; // save old capacity
    auto old_map = _map; // save old map to copy it's elements later
    if (get_load_factor() > UPPER_BOUNDARY)
    {
      while (get_load_factor() > UPPER_BOUNDARY)
      {
        _capacity *= 2;
      }
      auto new_map = new std::vector<std::pair<KeyT, ValueT>> [_capacity];
      _map = new_map;
      _size = 0;
//      _capacity *= 2;
      for (int i = 0; i < old_capacity; i++) // run on hashmap
      {
        for (size_t j = 0; j < old_map[i].size(); j++) // run on vectors
        {
          insert(old_map[i][j].first, old_map[i][j].second);
        }
      }
    }
    if (get_load_factor() < LOWER_BOUNDARY)
    {
      while ((get_load_factor() < LOWER_BOUNDARY) && _capacity > 1)
      {
        _capacity /= 2;
      }
      auto new_map = new std::vector<std::pair<KeyT, ValueT>> [_capacity];
      _map = new_map;
      _size = 0;
      for (int i = 0; i < old_capacity; i++) // run on hashmap
      {
        for (size_t j = 0; j < old_map[i].size(); j++) // run on vectors
        {
            insert(old_map[i][j].first, old_map[i][j].second);
        }
      }
    }
    delete[] old_map;
    return true;
  }

 public:

  /**
   * default constructor
   */
  HashMap() : _size(0), _capacity(START_CAPACITY)
  {
    _map = new std::vector<std::pair<KeyT, ValueT>> [_capacity];
  }

  /**
   * non-default constructor.
   * @param keys std::vector<KeyT>
   * @param values std::vector<ValueT>
   */
  HashMap(std::vector<KeyT> keys, std::vector<ValueT> values) :
  _size(0), _capacity(START_CAPACITY),
  _map(new std::vector<std::pair<KeyT, ValueT>> [_capacity])
  {
    if (keys.size() != values.size())
    {
      throw std::out_of_range(RANGE_ERROR_MSG);
    }
    for (size_t i = 0; i < keys.size(); i++)
    {
      (*this)[keys[i]] = values[i];
    }
  }

  // copy constructor
  HashMap(const HashMap& rhs)
  {
    _map = new std::vector<std::pair<KeyT, ValueT>> [START_CAPACITY];
    _size = rhs.size();
    _capacity = rhs.capacity();
    for (int i = 0; i < rhs._capacity; i++)
    {
      _map[i] = rhs._map[i];
    }
  }

  /**
   * class destructor.
   */
  ~HashMap()
  {
    delete[] _map;
  }

  /**
   * getter for the number of objects currently contained in map.
   * @return int _size
   */
  int size() const
  {
    return _size;
  }

  /**
   * getter for the maximum number of objects the map can hold.
   * @return int _capacity
   */
  int capacity() const
  {
    return _capacity;
  }

  /**
   * checks if the map is empty - if it currently contains no objects.
   * @return true if empty, false if not empty.
   */
  bool empty() const
  {
    return (_size == 0);
  }

  /**
   * inserts a new key and value to the map.
   * uses the hash function in order to hash the key to a vector and saves
   * it's value.
   * @param key KeyT
   * @param value ValueT
   * @return true for success, false if key is already in map
   */
  bool insert(KeyT key, ValueT value)
  {
    if (contains_key(key)) // if key is already in map
    {
      return false;
    }
    else // not in map - insert it:
    {
      int index = hash_function(key); // hashes key to vector in map
      _map[index].push_back(std::make_pair(key, value));
      _size++;
      if ((get_load_factor() > UPPER_BOUNDARY)) // enter function only if
        // capacity is over bounds
      {
        rehash();
      }
    }
    return true;
  }

  /**
   * checks if key is already in map.
   * @param key KeyT
   * @return true if key is in map, false if it isn't yet in the map
   */
  bool contains_key(KeyT key) const
  {
    int index = hash_function(key); // hashes key to some vector in map
    for (size_t i = 0; i < _map[index].size(); i++) // iterate over vector
    {
      if (_map[index][i].first == key)
      {
        return true;
      }
    }
    return false; // did not find key in map
  }

  /**
   * returns the value saved in the key's location in the map.
   * raises an error if the key does not exist in the map.
   * @param key KeyT
   * @return ValueT
   */
  ValueT at(KeyT key) const // read-only
  {
    if (!contains_key (key)) // key not in map
    {
      throw std::invalid_argument (INVALID_ARG_MSG);
    }
    return (*this).operator[] (key); // returns the value using [] operator
  }

  /**
   * returns the value saved in the key's location in the map.
   * raises an error if the key does not exist in the map.
   * @param key KeyT
   * @return ValueT
   */
  ValueT& at(KeyT key) // non-const
  {
    if (!contains_key (key)) // key not in map
    {
      throw std::invalid_argument (INVALID_ARG_MSG);
    }
    return (*this).operator[] (key); // returns the value using [] operator
  }

  /**
   * deletes the key with it's value from the map.
   * @param key KeyT
   * @return true for success in erasing, false for error
   */
  bool erase(KeyT key)
  {
    if (!contains_key(key))
    {
      return false;
    }
    int index = bucket_index(key);
    for (auto it = _map[index].begin(); it < _map[index].end(); it++)
    {
      if ((*it).first == key)
      {
        _map[index].erase(it);
        _size--;
        break;
      }
    }
    if (get_load_factor() < LOWER_BOUNDARY)
    {
      rehash();
    }
    return true;
  }

  /**
   * returns the map's load factor - size / capacity.
   * @return double _size / _capacity
   */
  double get_load_factor() const
  {
    return (double) _size / _capacity;
  }

  int bucket_size(KeyT key) const
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument(INVALID_ARG_MSG);
    }
    int index = hash_function(key);
    return _map[index].size(); // returns vector.size()
  }

  int bucket_index(KeyT key) const
  {
    if (!contains_key (key))
    {
      throw std::invalid_argument(INVALID_ARG_MSG);
    }
    return hash_function(key);
  }

  void clear() // removes all objects from the map
  {
    for (int i = 0; i < _capacity; i++)
    {
      _map[i].clear();
    }
    _size = 0;
  }

  // operators:
  HashMap& operator=(HashMap rhs)
  {
    auto new_map = new std::vector<std::pair<KeyT, ValueT>> [rhs._capacity];
    delete[] _map;
    this->_map = new_map;
    this->_capacity = rhs._capacity;
    this->_size = 0;
    for (int i = 0; i < rhs._capacity; i++) // run on hashmap
    {
      for (size_t j = 0; j < rhs._map[i].size(); j++) // run on vectors
      {
        insert(rhs._map[i][j].first, rhs._map[i][j].second);
      }
    }
    return *this;
  }

  ValueT operator[](KeyT key) const // read-only operator
  {
    if (!contains_key (key))
    {
//      insert (key, ValueT ());
      return ValueT ();
    }
    int index = hash_function (key);
    for (const auto &elem: _map[index])
    {
      if (elem.first == key)
      {
        return elem.second;
      }
    }
    return ValueT(); // just for compilation - function never gets here.
  }

  ValueT& operator[](KeyT key) // non-const operator
  {
    if (!contains_key(key))
    {
      insert(key, ValueT());
      return (*this).at(key);
    }
    int index = hash_function (key);
    for (auto &elem : _map[index])
    {
      if (elem.first == key)
      {
        return elem.second;
      }
    }  // just for compilation - function never gets here.
    throw std::invalid_argument(INVALID_ARG_MSG);
  }

  bool operator==(HashMap rhs) const
  {
    if (rhs.size() != _size)
    {
      return false;
    }
    for (int i = 0; i < _capacity; i++) // run on vectors in lhs map
    {
      for (size_t j = 0; j < _map[i].size (); j++) // run on pairs in vectors
      {
        if (!(rhs.contains_key(_map[i][j].first)))
        { // if key is not in rhs map
          return false;
        } // check if value is the same in both maps
        if (_map[i][j].second != rhs.at ((_map[i][j].first)))
        {
          return false; // not the same value - maps are not equal
        }
      }
    }
    return true; // found equal!
  }

  bool operator!=(HashMap rhs) const
  {
    return (!(this->operator== (rhs)));
  }

  class ConstIterator
  {
   private:
    const HashMap* hash_map_;
    size_t vector_index_;
    size_t pair_index_;

   public:
    typedef std::pair<KeyT, ValueT> value_type;
    typedef const value_type &reference;
    typedef const value_type *pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    // copy constructor:
    ConstIterator (const HashMap* hash_map,
                   size_t vector_index,
                   size_t pair_index)
        : hash_map_ (hash_map),
          vector_index_ (vector_index),
          pair_index_ (pair_index) {}

    ConstIterator &operator++ ()
    {
      const std::vector<std::pair<KeyT, ValueT>>& cur_vector =
          hash_map_->_map[vector_index_];
      if (++pair_index_ >= cur_vector.size ())
      {
        int old_ind = vector_index_;
        pair_index_ = 0;
        vector_index_++;
        while ((hash_map_->_map[vector_index_].size() == 0) &&
        (vector_index_ < hash_map_->_capacity))
        {
          vector_index_++;
        }
        vector_index_ = (vector_index_ == hash_map_->_capacity) ? old_ind + 1
            : vector_index_;
      }
      return *this;
    }

    ConstIterator operator++ (int)
    {
      ConstIterator it = (*this); // default copy-ctr
//      while ((hash_map_->_map[vector_index_].size() == 0) &&
//             (vector_index_ < hash_map_->_capacity))
//      {
      this->operator++ ();
//      }
      return it;
    }

    bool operator== (const ConstIterator &rhs) const
    {
      return (&hash_map_ == &rhs.hash_map_)
             && (vector_index_ == rhs.vector_index_)
             && (pair_index_ == rhs.pair_index_);
    }

    bool operator!= (const ConstIterator &rhs) const
    {
      return !operator== (rhs);
    }

    reference operator* () const
    {
      return hash_map_->_map[vector_index_][pair_index_];
    }

    pointer operator-> () const
    {
      return &(operator* ());
    }
  };

  using const_iterator = ConstIterator;

  const_iterator cbegin () const
  {
    int i = 0;
    while (i < _capacity && _map[i].empty()) {i++;}
    return ConstIterator (this, i, 0);
  }
  const_iterator cend () const
  {
    int end = 0;
    for (int i = 0; i < _capacity; i++)
    {
      end = (!(_map[i].empty())) ? (i + 1) : end;
    }
    end = (end == 0) ? _capacity : end;
    return ConstIterator (this, end, 0);
  }
  const_iterator begin () const
  {
    return cbegin ();
  }
  const_iterator end () const
  {
    return cend ();
  }
};

#endif //_HASHMAP_HPP_
