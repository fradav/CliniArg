#pragma once

/**
 * @brief Singleton class
 * 
 * @tparam T 
 */
template<class T>
class Singleton {
public:
    static T& instance();

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

protected:
    Singleton() {}
};

/**
 * @brief return the singleton instance
 * 
 * @tparam T 
 * @return T& 
 */
template<class T>
T& Singleton<T>::instance()
{
    static T instance{};
    return instance;
}