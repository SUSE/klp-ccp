#ifndef COW_SET_HH
#define COW_SET_HH

#include <set>
#include <iterator>
#include <type_traits>
#include <cassert>

namespace suse
{
  namespace cp
  {
    template<typename Key, typename Compare = std::less<Key>,
	     typename Allocator = std::allocator<Key> >
    class cow_set
    {
    private:
      typedef std::set<Key, Compare, Allocator> _set_type;

    public:
      typedef typename _set_type::key_type key_type;
      typedef typename _set_type::value_type value_type;
      typedef typename _set_type::size_type size_type;
      typedef typename _set_type::difference_type difference_type;
      typedef typename _set_type::key_compare key_compare;
      typedef typename _set_type::value_compare value_compare;
      typedef typename _set_type::allocator_type allocator_type;
      typedef typename _set_type::reference reference;
      typedef typename _set_type::const_reference const_reference;
      typedef typename _set_type::pointer pointer;
      typedef typename _set_type::const_pointer const_pointer;

      class const_iterator
	: public std::iterator<std::bidirectional_iterator_tag,
			       typename std::add_const<value_type>::type>
      {
      public:
	const_iterator(const const_iterator &it);

	const_iterator(const_iterator &&it);

	~const_iterator() noexcept;

	const_iterator& operator=(const const_iterator &rhs);

	const_iterator& operator=(const_iterator &&rhs);

	bool operator==(const const_iterator &rhs) const noexcept;
	bool operator!=(const const_iterator &rhs) const noexcept;
	const_iterator& operator++();
	const_iterator operator++(int);
	const_iterator& operator--();
	const_iterator operator--(int);

	typename const_iterator::pointer operator->() const;
	typename const_iterator::reference operator*() const;

      private:
	friend class cow_set;

	const_iterator() noexcept;

	const_iterator(typename _set_type::const_iterator &&backing_it);


	typename _set_type::const_iterator*
	_backing_it() noexcept
	{
	  assert(_backing_it_valid);
	  return (reinterpret_cast<typename _set_type::const_iterator*>
		  (&_backing_it_storage));
	}

	const typename _set_type::const_iterator*
	_backing_it() const noexcept
	{
	  assert(_backing_it_valid);
	  return (reinterpret_cast<const typename _set_type::const_iterator*>
		  (&_backing_it_storage));
	}

	typename
	std::aligned_storage<sizeof(typename _set_type::const_iterator),
			     alignof(typename _set_type::const_iterator)>:: type
	  _backing_it_storage;
	bool _backing_it_valid;
      };

      typedef const_iterator iterator;

      cow_set() noexcept;

      cow_set(const cow_set &cs) noexcept;

      cow_set(cow_set &&cs) noexcept;

      ~cow_set() noexcept;

      cow_set& operator=(const cow_set &rhs) noexcept;
      cow_set& operator=(cow_set &&rhs) noexcept;

      const_iterator cbegin() const;
      const_iterator cend() const;

      iterator begin()
      { return cbegin(); }

      iterator end()
      { return cend(); }

      bool empty() const;
      size_type count(const Key &key) const;

      const_iterator find(const Key &key) const;

      std::pair<iterator,bool> insert(const value_type &value);
      std::pair<iterator,bool> insert(value_type &&value);
      iterator insert(const const_iterator&, const value_type &value);
      iterator insert(const const_iterator&, value_type &&value);

      template<typename InputIt>
      void insert(InputIt &&first, InputIt &&last);

      void clear() noexcept;

    private:
      void _do_cow();

      struct _cow_set_data
      {
	_cow_set_data();
	_cow_set_data(const _cow_set_data &csi);

	std::size_t _refcount;
	_set_type _set;
      };

      _cow_set_data *_data;
    };

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    _cow_set_data::_cow_set_data()
      : _refcount(1)
    {}

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    _cow_set_data::_cow_set_data(const _cow_set_data &csi)
      : _refcount(1), _set(csi._set)
    {}


    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    const_iterator::const_iterator() noexcept
      : _backing_it_valid(false)
    {}

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::const_iterator::
    const_iterator(typename _set_type::const_iterator &&backing_it)
      : _backing_it_valid(true)
    {
      new (_backing_it())
	typename _set_type::const_iterator(std::move(backing_it));
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    const_iterator::const_iterator(const const_iterator &it)
      : _backing_it_valid(it._backing_it_valid)
    {
      if (_backing_it_valid)
	new (_backing_it())
	  typename _set_type::const_iterator(*it._backing_it());
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    const_iterator::const_iterator(const_iterator &&it)
      : _backing_it_valid(it._backing_it_valid)
    {
      if (_backing_it_valid)
	new (_backing_it())
	  typename _set_type::const_iterator(std::move(*it._backing_it()));
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::
    const_iterator::~const_iterator() noexcept
    {
      if (_backing_it_valid)
	_backing_it()->_set_type::const_iterator::~const_iterator();
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator&
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator=(const const_iterator &rhs)
    {
      if (this == &rhs)
	return *this;

      if (_backing_it_valid)
	_backing_it()->_set_type::const_iterator::~const_iterator();

      _backing_it_valid = rhs._backing_it_valid;
      if (_backing_it_valid)
	new (_backing_it())
	  typename _set_type::const_iterator(*rhs._backing_it());

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator&
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator=(const_iterator &&rhs)
    {
      if (this == &rhs)
	return *this;

      if (_backing_it_valid)
	_backing_it()->_set_type::const_iterator::~const_iterator();

      _backing_it_valid = rhs._backing_it_valid;
      if (_backing_it_valid)
	new (_backing_it())
	  typename _set_type::const_iterator(std::move(*rhs._backing_it()));

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    bool
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator==(const const_iterator &rhs) const noexcept
    {
      if (_backing_it_valid != rhs._backing_it_valid)
	return false;
      else if (!_backing_it_valid)
	return true;

      return *_backing_it() == *rhs._backing_it();
    }

    template<typename Key, typename Compare, typename Allocator>
    bool
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator!=(const const_iterator &rhs) const noexcept
    {
      return !(*this == rhs);
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator&
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator++()
    {
      assert(_backing_it_valid);
      ++(*_backing_it());

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator++(int)
    {
      const_iterator r(*this);

      assert(_backing_it_valid);
      ++(*_backing_it());

      return r;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator&
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator--()
    {
      assert(_backing_it_valid);
      --(*_backing_it());

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator--(int)
    {
      const_iterator r(*this);

      assert(_backing_it_valid);
      --(*_backing_it());

      return r;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator::pointer
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator->() const
    {
      assert(_backing_it_valid);
      return _backing_it()->operator->();
    }

    template<typename Key, typename Compare, typename Allocator>
    typename  cow_set<Key, Compare, Allocator>::const_iterator::reference
    cow_set<Key, Compare, Allocator>::
    const_iterator::operator*() const
    {
      assert(_backing_it_valid);
      return _backing_it()->operator*();
    }


    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::cow_set() noexcept
      : _data(nullptr)
    {}

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::cow_set(const cow_set &cs) noexcept
      : _data(cs._data)
    {
      if (_data)
	++_data->_refcount;
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::cow_set(cow_set &&cs) noexcept
      : _data(cs._data)

    {
      cs._data = nullptr;
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>::~cow_set() noexcept
    {
      if (_data && !--_data->_refcount) {
	delete _data;
      }
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>&
    cow_set<Key, Compare, Allocator>::operator=(const cow_set &rhs) noexcept
    {
      if (this == &rhs)
	return *this;

      if (_data && !--_data->_refcount)
	delete _data;

      _data = rhs._data;
      if (_data)
	++_data->_refcount;

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    cow_set<Key, Compare, Allocator>&
    cow_set<Key, Compare, Allocator>::operator=(cow_set &&rhs) noexcept
    {
      if (this == &rhs)
	return *this;

      if (_data && !--_data->_refcount)
	delete _data;

      _data = rhs._data;
      rhs._data = nullptr;

      return *this;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator
    cow_set<Key, Compare, Allocator>::cbegin() const
    {
      if (_data)
	return const_iterator(_data->_set.cbegin());
      return const_iterator();
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator
    cow_set<Key, Compare, Allocator>::cend() const
    {
      if (_data)
	return const_iterator(_data->_set.cend());
      return const_iterator();
    }

    template<typename Key, typename Compare, typename Allocator>
    bool
    cow_set<Key, Compare, Allocator>::empty() const
    {
      return (!_data || _data->_set.empty());
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::size_type
    cow_set<Key, Compare, Allocator>::count(const Key &key) const
    {
      if (!_data)
	return 0;

      return _data->_set.count(key);
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::const_iterator
    cow_set<Key, Compare, Allocator>::find(const Key &key) const
    {
      if (!_data)
	return cend();

      return const_iterator(_data->_set.find(key));
    }

    template<typename Key, typename Compare, typename Allocator>
    std::pair<typename cow_set<Key, Compare, Allocator>::iterator,bool>
    cow_set<Key, Compare, Allocator>::insert(const value_type &value)
    {
      if (_data) {
	auto it = find(value);
	if (it != cend())
	  return std::make_pair(it, false);
      }

      _do_cow();

      return std::make_pair(const_iterator(_data->_set.insert(value).first),
			    true);
    }

    template<typename Key, typename Compare, typename Allocator>
    std::pair<typename cow_set<Key, Compare, Allocator>::iterator,bool>
    cow_set<Key, Compare, Allocator>::insert(value_type &&value)
    {
      if (_data) {
	auto it = find(value);
	if (it != cend())
	  return std::make_pair(it, false);
      }

      _do_cow();

      return std::make_pair(const_iterator(_data->_set.
					   insert(std::move(value)).first),
			    true);
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::iterator
    cow_set<Key, Compare, Allocator>::insert(const const_iterator&,
					     const value_type &value)
    {
      return insert(value).first;
    }

    template<typename Key, typename Compare, typename Allocator>
    typename cow_set<Key, Compare, Allocator>::iterator
    cow_set<Key, Compare, Allocator>::insert(const const_iterator&,
					     value_type &&value)
    {
      return insert(std::move(value)).first;
    }

    template<typename Key, typename Compare, typename Allocator>
    template<typename InputIt>
    void cow_set<Key, Compare, Allocator>::insert(InputIt &&first,
						  InputIt &&last)
    {
      if (first == last)
	return;

      _do_cow();

      _data->_set.insert(std::forward<InputIt>(first),
			 std::forward<InputIt>(last));
    }

    template<typename Key, typename Compare, typename Allocator>
    void cow_set<Key, Compare, Allocator>::clear() noexcept
    {
      if (_data && !--_data->_refcount)
	delete _data;

      _data = nullptr;
    }

    template<typename Key, typename Compare, typename Allocator>
    void cow_set<Key, Compare, Allocator>::_do_cow()
    {
      if (!_data) {
	_data = new _cow_set_data();
	return;
      }

      if (_data->_refcount != 1) {
	assert(_data->_refcount > 1);
	auto _new_data = new _cow_set_data(*_data);
	--_data->_refcount;
	_data = _new_data;
      }
    }
  }
}

#endif
