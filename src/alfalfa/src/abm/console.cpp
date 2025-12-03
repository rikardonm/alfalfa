#include <vector>
#include <string>
#include <map>
#include <cctype>

#include <alfalfa/lib/errors.hpp>
#include <alfalfa/abm/console.hpp>


namespace alfalfa
{
namespace abm
{
namespace console
{

    Console::Console(std::shared_ptr<alfalfa::lib::IStreamSource<std::byte>> source,
                std::shared_ptr<alfalfa::lib::IStreamSink<std::byte>> sink)
        : rx()
        , _dev_source(source)
        , _source_adapter(nullptr)
        , _in_stream(nullptr)
        , _dev_sink(sink)
        , _sink_adapter(nullptr)
        , _out_stream(nullptr)
        , commands()
        , console_is_enabled(true)
    {
        _source_adapter = std::make_unique<alfalfa::lib::StreamSinkSourceAdapter>(nullptr, _dev_source);
        _in_stream = std::make_shared<std::istream>(_source_adapter.get());
        _sink_adapter = std::make_unique<alfalfa::lib::StreamSinkSourceAdapter>(_dev_sink, nullptr);
        _out_stream = std::make_shared<std::ostream>(_sink_adapter.get());
    };

// #if defined(CONFIG_AZAL_CONSOLE_ENABLE)
#if 1

    void Console::AddFunction(std::string name, CommandFunctionTypePtr function)
    {
        commands.emplace(name, function);
    }

    void Console::ConsoleTask()
    {
        auto testchar = _dev_source->ReadOne();
        if (not console_is_enabled)
        {
            // drop packet
            return;
        }
        bool process = false;
        while (testchar.has_value() and not process)
        {
            const auto charin = static_cast<char>(testchar.value());
            switch(charin)
            {
                case '\r':
                    process = true;
                    break;
                case '\n':
                    process = true;
                    break;
                case '\b':
                    if (rx.size())
                    {
                        rx.pop_back();
                    }
                    break;
                default:
                    rx.push_back(charin);
                    break;
            }
            // Echo the character
            _dev_sink->WriteFrom(testchar.value());
            testchar = _dev_source->ReadOne();
        }
        if (process)
        {
            // do something here.
            Process(rx);
            rx.clear();
        }
    }

#else

    void Console::AddFunction(std::string name, CommandFunctionTypePtr function)
    {
    }

    void Console::ConsoleTask()
    {
    }

#endif

    void Console::Help()
    {
        *_out_stream << "Available commands:\n";
        for(auto& cmd : commands)
        {
            *_out_stream << cmd.first.c_str() << "\n";
        }
    }

    void Console::Process(std::vector<char>& inputline)
    {
        std::string command;
        std::vector<std::string> cli;
        auto cp = inputline.begin();
        while(cp != inputline.end())
        {
            // Trim left whitespace
            while(std::isspace(*cp) && cp != inputline.end())
            {
                ++cp;
            }
            // Mark start
            auto ce = cp;
            // Find next whitespace
            while(not std::isspace(*cp) && cp != inputline.end())
            {
                ++cp;
            }
            if (ce != cp)
            {
                if (command.size() != 0)
                {
                    // Mark end by copying string
                    cli.push_back(std::string(ce, cp));
                }
                else
                {
                    command = std::string(ce, cp);
                }
            }
        }

        if (command.size() == 0)
        {
            *_out_stream << "No command in vector!\n";
            return;
        }

        if (command == "help")
        {
            Help();
            return;
        }

        auto cmdptr = commands.find(command);
        if (cmdptr == commands.end())
        {
            *_out_stream << "Command " << command << " not known!\n";
            return;
        }
        /* If the command was found, call it */
        cmdptr->second(*_in_stream, *_out_stream, cli);
    }

}
}
}
