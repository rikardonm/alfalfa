#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include <tuple>

#include <alfalfa/lib/types/streams.hpp>


namespace alfalfa
{
namespace abm
{
namespace console
{
    using StdIn = alfalfa::lib::IStreamSource<std::byte>;
    using StdOut = alfalfa::lib::IStreamSink<std::byte>;
    using Arguments=std::vector<std::string>&;
    using CommandFunctionTypePtr = std::function<void(std::istream&, std::ostream&, Arguments&&)>;

    class Console
    {
    public:
        Console(std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> source,
                std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> sink);

        void ConsoleTask();
        void AddFunction(std::string name, CommandFunctionTypePtr function);

        template<typename T>
        void AddFunction(
            std::string name,
            void (T::*method)(std::istream&, std::ostream&, Arguments&&), std::shared_ptr<T> object)
        {
            AddFunction(name, std::bind(method, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }

        void DisableConsole()
        {
            console_is_enabled = false;
        }

        std::tuple<std::shared_ptr<std::istream>, std::shared_ptr<std::ostream>> GetStdStreams() const
        {
            return std::make_tuple(_in_stream, _out_stream);
        }

    private:
        std::vector<char> rx;
        std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> _dev_source;
        std::unique_ptr<alfalfa::lib::StreamSinkSourceAdapter> _source_adapter;
        std::shared_ptr<std::istream> _in_stream;

        std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> _dev_sink;
        std::unique_ptr<alfalfa::lib::StreamSinkSourceAdapter> _sink_adapter;
        std::shared_ptr<std::ostream> _out_stream;

        std::map<std::string, CommandFunctionTypePtr> commands;
        bool console_is_enabled;

        void Process(std::vector<char>& inputline);
        void Help();
    };

}
}
}
