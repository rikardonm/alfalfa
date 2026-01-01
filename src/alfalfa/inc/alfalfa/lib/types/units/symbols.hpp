#pragma once

#include <ratio>


namespace Units
{
    /* TODO: this needs to be renamed, or its members.
        When doing that, we would create another type for Fahrenheit, for example.
        Should celsius and fahrenheit derive from a common "temperature" class?
        Or just have declared cross conversion operations? */
    enum class PhysicalQuantity
    {
        // Time
        Second,
        // Frequency
        Hertz,
        // Pressure
        Pascal,
        // Temperature
        Celsius,
        // Potential
        Volt,
        // Volume
        Liter,
        // ?
        RelativeHumidity,
        // Length
        Meter,
        // Speed
        MeterPerSecond,
        // Current
        Ampere,
        // Power
        Watt,
        // Mass
        Gram
    };

    inline constexpr const char* GetSymbol(PhysicalQuantity unit)
    {
        switch(unit)
        {
            /* Time */
            case PhysicalQuantity::Second:              return "s";
            /* Frequency */
            case PhysicalQuantity::Hertz:               return "Hz";
            /* Pressure */
            case PhysicalQuantity::Pascal:              return "Pa";
            /* Temperature */
            case PhysicalQuantity::Celsius:             return "C";
            /* Electrical */
            case PhysicalQuantity::Volt:                return "V";
            /* Volume */
            case PhysicalQuantity::Liter:               return "L";
            /* Humidity */
            case PhysicalQuantity::RelativeHumidity:    return "%";
            /* Length */
            case PhysicalQuantity::Meter:               return "m";
            /* Speed */
            case PhysicalQuantity::MeterPerSecond:      return "m/s";
            /* Current */
            case PhysicalQuantity::Ampere:              return "A";
            /* Power */
            case PhysicalQuantity::Watt:                return "W";
            /* Mass */
            case PhysicalQuantity::Gram:                return "g";
            /* Unknown */
            default:                                    return "?";
        };
    };

    /** @brief Unitless representations was too big of a name. */
    enum class UnitlessSymbol
    {
        None,
        Percent,
        Ppm,
        Bpm,
    };

    namespace Multiples
    {
        const char Tera = 'T';
        const char Giga = 'G';
        const char Mega = 'M';
        const char Kilo = 'k';
        const char Hecto = 'h';
        // Unity
        const char Milli = 'm';
        const char Micro = 'u';
        const char Nano = 'n';
        const char Pico = 'p';

        inline constexpr char GetLetter(int power)
        {
            switch(power)
            {
                case 12:        return Tera;
                case 9:         return Giga;
                case 6:         return Mega;
                case 3:         return Kilo;
                case 2:         return Hecto;
                // case 0:         return " ";
                case -3:        return Milli;
                case -6:        return Micro;
                case -9:        return Nano;
                case -12:       return Pico;
                default:        return '?';
            };
        }

        inline constexpr double GetMultiplier(const char letter)
        {
            switch(letter)
            {
                case Tera:       return 1e12;
                case Giga:       return 1e9;
                case Mega:       return 1e6;
                case Kilo:       return 1e3;
                case Hecto:       return 1e2;
                case Milli:       return 1e-3;
                case Micro:       return 1e-6;
                case Nano:       return 1e-9;
                case Pico:       return 1e-12;
                default:        return 1.0;
            }
        }

        template<std::intmax_t Nx = 1, std::intmax_t Dx = 1>
        constexpr char GetLetter(std::ratio<Nx, Dx> ratio)
        {
            // static_assert(std::_Is_ratio_v<RATIO>, "RATIO must be an std::ratio type.");
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::tera>)
            {
                return Tera;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::giga>)
            {
                return Giga;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::mega>)
            {
                return Mega;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::kilo>)
            {
                return Kilo;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::hecto>)
            {
                return Hecto;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::milli>)
            {
                return Milli;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::micro>)
            {
                return Micro;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::nano>)
            {
                return Nano;
            }
            if (std::ratio_equal_v<std::ratio<Nx, Dx>, std::pico>)
            {
                return Pico;
            }
            return '?';
        }

        inline std::string GetLettersUnitless(UnitlessSymbol symbol)
        {
            switch(symbol)
            {
                case UnitlessSymbol::None:      return "";
                case UnitlessSymbol::Percent:   return "%";
                case UnitlessSymbol::Ppm:       return "ppm";
                case UnitlessSymbol::Bpm:       return "bpm";
                default:                        return "?";
            }
        }

        inline constexpr double GetMultiplierUnitless(UnitlessSymbol symbol)
        {
            switch(symbol)
            {
                case UnitlessSymbol::None:      return 1.0;
                case UnitlessSymbol::Percent:   return 1e2;
                case UnitlessSymbol::Ppm:       return 1e6;
                case UnitlessSymbol::Bpm:       return 1e9;
                default:                        return 0.0;
            }
        }

    }


} /* namespace Units */
