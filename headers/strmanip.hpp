#pragma once

#include <string>
#include <sstream>
#include <algorithm>

// The only member of the class data is a string, which is usually a number in
// the string representation. The functionality of the class includes various
// transformations with a string that contains a number.
class strmanip final
{
private:
    std::string m_str;

protected:
    inline void copy_obj(const strmanip &__obj) { m_str = __obj.m_str; }
    inline void clear_obj(strmanip &__obj) { __obj.m_str.clear(); }

public:
    explicit strmanip() : m_str("") {}
    explicit strmanip(const std::string &__str) : m_str(__str){};
    explicit strmanip(const strmanip &__obj) { copy_obj(__obj); }
    explicit strmanip(strmanip &&__rhs);
    virtual ~strmanip() = default;

    inline std::string get_str() { return m_str; }
    inline void set_str(const std::string &__str) { m_str = __str; }

    strmanip &operator=(const std::string &__str) noexcept;
    strmanip &operator=(const strmanip &__obj);
    strmanip &operator=(strmanip &&__rhs) noexcept;
    friend std::ostream &operator<<(std::ostream &__os, const strmanip &__obj);
    friend std::istream &operator>>(std::istream &__is, strmanip &__obj);
    friend bool operator==(const strmanip &__lhs, const strmanip &__rhs);
    friend bool operator==(const strmanip &__lhs, const char *__rhs);
    strmanip &operator+=(const char *__str);

    inline void remove_endl() noexcept;
    template <typename T>
    T to_number() const noexcept;
    inline std::string to_string() const noexcept;
    inline const char *to_c_str() const noexcept;

    inline bool is_less_or_64symbols() const noexcept;
    inline bool is_digits() const noexcept;
    inline bool is_32multiple() const noexcept;
    inline bool is_3more_digits() const noexcept;

    inline virtual void clear() noexcept;
    inline size_t length() const noexcept;
    uint sum_of_digits() const noexcept;
    void sort_descending() noexcept;

    // Replacing all even digits in string to 'KB' symbols
    void even_to_KB() noexcept;
};

strmanip::strmanip(strmanip &&__rhs)
{
    copy_obj(__rhs);
    clear_obj(__rhs);
}

strmanip &strmanip::operator=(const std::string &__str) noexcept
{
    this->m_str = __str;
    return *this;
}

strmanip &strmanip::operator=(const strmanip &__obj)
{
    if (this == &__obj)
        return *this;

    copy_obj(__obj);
    return *this;
}

strmanip &strmanip::operator=(strmanip &&__rhs) noexcept
{
    if (this == &__rhs)
        return *this;

    clear_obj(*this);
    copy_obj(__rhs);
    clear_obj(__rhs);

    return *this;
}

std::ostream &operator<<(std::ostream &__os, const strmanip &__obj)
{
    __os << __obj.m_str;
    return __os;
}

std::istream &operator>>(std::istream &__is, strmanip &__obj)
{
    __is >> __obj.m_str;
    return __is;
}

bool operator==(const strmanip &__lhs, const strmanip &__rhs)
{
    return (__lhs.to_string() == __rhs.to_string()) ? true : false;
}

bool operator==(const strmanip &__lhs, const char *__rhs)
{
    return (strcmp(__lhs.to_c_str(), __rhs));
}

strmanip &strmanip::operator+=(const char *__str)
{
    m_str += __str;
    return *this;
}

void strmanip::remove_endl() noexcept
{
    std::erase_if(m_str, [](auto ch)
                  { return (ch == '\n'); });
}

template <typename T>
T strmanip::to_number() const noexcept
{
    std::istringstream iss(m_str);

    T num{};
    iss >> num;

    return num;
}

std::string strmanip::to_string() const noexcept
{
    return m_str;
}

const char *strmanip::to_c_str() const noexcept
{
    return m_str.c_str();
}

bool strmanip::is_less_or_64symbols() const noexcept
{
    return m_str.length() <= 64UL ? true : false;
}

bool strmanip::is_digits() const noexcept
{
    return std::all_of(m_str.cbegin(), m_str.cend(), ::isdigit);
}

bool strmanip::is_32multiple() const noexcept
{
    return (to_number<int>() % 32 == 0) ? true : false;
}

bool strmanip::is_3more_digits() const noexcept
{
    return (is_digits() && m_str.length() > 2UL) ? true : false;
}

void strmanip::clear() noexcept
{
    m_str.clear();
}

size_t strmanip::length() const noexcept
{
    return m_str.length();
}

uint strmanip::sum_of_digits() const noexcept
{
    uint sum{0};

    for (auto it = m_str.cbegin(); it != m_str.cend(); ++it)
    {
        if (isdigit(*it))
            sum += static_cast<uint>(*it - '0');
    }

    return sum;
}

void strmanip::sort_descending() noexcept
{
    std::sort(m_str.begin(), m_str.end());
    std::reverse(m_str.begin(), m_str.end());
}

void strmanip::even_to_KB() noexcept
{
    for (size_t index{0UL}; index < m_str.length(); index++)
    {
        if (m_str.at(index) % 2 == 0)
        {
            m_str.erase(index, 1UL);
            m_str.insert(index++, "KB");
        }
    }
}
