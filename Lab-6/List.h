#include <cassert>
#include <utility>
#include <new>

template <typename T>
class List
{
    const size_t DEFAULT_CAPACITY = 4;

    T* m_data;
    size_t m_capacity;
    size_t m_count;

public:
    List() : m_data(reinterpret_cast<T*>(new char[DEFAULT_CAPACITY * sizeof(T)])), m_capacity(DEFAULT_CAPACITY), m_count(0)
    {
    }

    List(const List<T>& list) : m_data(reinterpret_cast<T*>(new char[list.m_capacity * sizeof(T)])), m_capacity(list.m_capacity), m_count(list.m_count)
    {
        for (size_t i = 0; i < m_count; i++)
        {
            new (&m_data[i]) T(std::move(list.m_data[i]));
        }
    }

    List(List<T>&& list) : m_data(list.m_data), m_capacity(list.m_capacity), m_count(list.m_count)
    {
        list.m_data     = nullptr;
        list.m_capacity = 0;
        list.m_count    = 0;
    }

    void Add(T&& item)
    {
        if (m_count == m_capacity)
        {
            Grow();
        }

        new (&m_data[m_count++]) T(std::move(item));
    }

    void Add(const T& item)
    {
        Add(std::move(item));
    }

    size_t Count() const
    {
        return m_count;
    }

    T& operator [](size_t index) const
    {
        assert(index < m_count);
        return m_data[index];
    }

    ~List()
    {
        if (m_data != nullptr)
        {
            // Destruct the elements manually.
            for (size_t i = 0; i < m_count; i++)
            {
                m_data[i].~T();
            }

            delete[] reinterpret_cast<char*>(m_data);
        }
    }

private:
    void Grow()
    {
        size_t newCapacity = m_capacity * 2;
        T* newData         = reinterpret_cast<T*>(new char[sizeof(T) * newCapacity]);
        for (size_t i = 0; i < m_count; i++)
        {
            new (&newData[i]) T(std::move(m_data[i]));
        }

        delete[] m_data;
        m_data     = newData;
        m_capacity = newCapacity;
    }
};