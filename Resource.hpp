#pragma once
#include <memory>
#include <mutex>

template <class T>
class Resource
{
    Resource<T>(const Resource<T> &) = delete;
    Resource<T> &operator=(const Resource<T> &) = delete;
    static std::unique_ptr<Resource<T>> m_ins;
    static std::once_flag m_once;
    Resource<T>() = default;

  public:
    virtual ~Resource<T>() = default;
    static Resource<T> &getInstance()
    {
        std::call_once(m_once, []() {
            m_ins.reset(new T);
        });
        return *m_ins.get();
    }
};