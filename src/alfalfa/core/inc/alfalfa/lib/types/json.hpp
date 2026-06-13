#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <optional>

#include <nlohmann/json.hpp>


inline void ToJson(const int8_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const uint8_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const int16_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const uint16_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const int32_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const uint32_t& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const float& value, std::stringstream& ss)
{
    ss << value;
}

inline void ToJson(const double& value, std::stringstream& ss)
{
    ss << value;
}

class JsonKeyRAII
{
    public:
        JsonKeyRAII(std::string& dst, const std::string& name) : _dst(dst)
        {
            _dst.append(1, '"');
            _dst.append(name);
            _dst.append("\":");
        }

        JsonKeyRAII(std::string& dst, const char* name) : _dst(dst)
        {
            _dst.append(1, '"');
            _dst.append(name);
            _dst.append("\":");
        }

        template<std::size_t N>
        JsonKeyRAII(std::string& dst, const char name[N]) : _dst(dst)
        {
            _dst.append(1, '"');
            _dst.append(name, N);
            _dst.append("\":");
        }

        ~JsonKeyRAII()
        {
            _dst.append(1, ',');
        }
    private:
        std::string& _dst;
};

class _StreamJsonBase
{
    public:
        void Stream(std::string&& rvalue)
        {
            _dst.append(1, '"');
            _dst.append(rvalue);
            _dst.append(1, '"');
        }

        void Stream(const bool& value)
        {
            if (value)
            {
                _dst.append("true");
            }
            else
            {
                _dst.append("false");
            }
        }

        void Stream(const int8_t& value)
        {
            /* Although we may think that a const char is a letter... our damn use of uint8_t ends up here. */
            _dst.append(std::to_string(value));
        }

        void Stream(const uint8_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const int16_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const uint16_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const int32_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const uint32_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const int64_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(const uint64_t& value)
        {
            _dst.append(std::to_string(value));
        }

        void Stream(char* const& value)
        {
            _dst.append(1, '"');
            _dst.append(value);
            _dst.append(1, '"');
        }

        void Stream(const std::string& value)
        {
            _dst.append(1, '"');
            _dst.append(value);
            _dst.append(1, '"');
        }

        template<std::size_t N>
        void Stream(const char (&value)[N])
        {
            _dst.append(1, '"');
            _dst.append(value);
            _dst.append(1, '"');
        }

        void Stream(const nlohmann::json& value)
        {
            _dst.append(1, '"');
            _dst.append(value.dump());
            _dst.append(1, '"');
        }

        void StreamSerializedObject(const std::string& value)
        {
            _dst.append(value);
        }

        void StreamSerializedObject(const std::string&& value)
        {
            _dst.append(value);
        }

        void StreamSerializedObject(const std::string::iterator& begin, const std::string::iterator& end)
        {
            std::copy(begin, end, std::back_inserter(_dst));
        }

        /**
         * @brief Get an RAII object that creates a key in the JSON object and appends a colon.
         *
         * The emmitted text will be: `"key": <yield>,`
         *
         * @tparam T
         * @param key
         * @return JsonKeyRAII
         */
        template<typename T>
        JsonKeyRAII MakeKey(T key)
        {
            return JsonKeyRAII(ValueRef(), key);
        }

    protected:

        _StreamJsonBase(_StreamJsonBase& other) : _dst(other._dst) {}
        _StreamJsonBase(std::string& value) : _dst(value) {};

        void PopTrailingComma()
        {
            if (*_dst.rbegin() == ',')
            {
                _dst.pop_back();
            }
        }

        void PushToDst(const char value)
        {
            _dst.append(1, value);
        }

        void PushToDst(const std::string& value)
        {
            _dst.append(value);
        }

        void PushToDst(const char* value)
        {
            _dst.append(value);
        }

        template<std::size_t N>
        void PushToDst(const char (&value)[N], const std::size_t size)
        {
            _dst.append(size, value);
        }

        std::string& ValueRef()
        {
            return _dst;
        }

    private:
        std::string& _dst;
};

class StreamJsonObject : public _StreamJsonBase
{
    public:
        using _StreamJsonBase::Stream;

        StreamJsonObject(StreamJsonObject& other) : _StreamJsonBase(other)
        {
            PushToDst('{');
        }

        ~StreamJsonObject()
        {
            PopTrailingComma();
            PushToDst('}');
        }

        template<typename T>
        void Stream(const char* key, const T& value)
        {
            auto x = MakeKey(key);
            Stream(value);
        }

        template<typename T>
        void Stream(const std::string& key, const T& value)
        {
            auto x = MakeKey(key);
            Stream(value);
        }

        template<typename T>
        void Stream(const T& value)
        {
            StreamJson(*this, value);
        }

        template<typename T>
        void Stream(const std::optional<T>& value)
        {
            if (value.has_value())
            {
                Stream(value.value());
            }
            else
            {
                PushToDst("null");
            }
        }

        /* Sequence types */
        template<typename T, std::size_t N>
        void Stream(const std::array<T, N>& value)
        {
            PushToDst('[');
            for (const auto& it : value)
            {
                Stream(it);
                PushToDst(',');
            }
            PopTrailingComma();
            PushToDst(']');
        }

        template<typename T>
        void Stream(const std::vector<T>& value)
        {
            PushToDst('[');
            for (const auto& it : value)
            {
                Stream(it);
                PushToDst(',');
            }
            PopTrailingComma();
            PushToDst(']');
        }

    protected:
        StreamJsonObject(std::string& value) : _StreamJsonBase(value)
        {
            PushToDst("{");
        }
};

class NamedStreamJsonObject : private JsonKeyRAII, public StreamJsonObject
{
    public:
        template<typename T>
        NamedStreamJsonObject(StreamJsonObject& other, T key)
        : JsonKeyRAII(other.MakeKey(key)), StreamJsonObject(other)
        {}
};

class StreamJsonObjectRoot : public StreamJsonObject
{
    public:
        using StreamJsonObject::Stream;
        using StreamJsonObject::ValueRef;

        StreamJsonObjectRoot(std::string& value)
        : StreamJsonObject(value)
        {
        }
};
