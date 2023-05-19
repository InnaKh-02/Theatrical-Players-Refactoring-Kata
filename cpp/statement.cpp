#include "statement.h"

#include <iostream>
#include <sstream>
#include <iomanip>

class comma_numpunct : public std::numpunct<char>
{
  protected:
    virtual char do_thousands_sep() const override
    {
        return ',';
    }

    virtual std::string do_grouping() const override
    {
        return "\03";
    }
};

static std::string performance_types[2] = {"tragedy", "comedy"};

std::string statement(
    const nlohmann::json& invoice,
    const nlohmann::json& plays)
{
    float total_amount = 0;
    int volume_credits = 0;

    const std::locale comma_locale(std::locale(), new comma_numpunct());

    std::stringstream result;
    result.imbue(comma_locale);
    result.precision(2);
    result << "Statement for " << invoice["customer"].get<std::string>() << '\n';

    for( const nlohmann::json& perf : invoice["performances"])
    {
        float this_amount = 0;
        const nlohmann::json& play = plays[perf["playID"].get<std::string>()];
        if (play["type"] == performance_types[0])
        {
            this_amount = (perf["audience"] > 30) ? (1000 * perf["audience"].get<int>() + 10000) : 40000;
        }

        else if (play["type"] == performance_types[1])
        {
            this_amount = (perf["audience"] > 20) ? (500 * perf["audience"].get<int>()) : (30000 + 300 * perf["audience"].get<int>());
        }

        else
        {
            throw std::domain_error("unknown type: " + play["type"].get<std::string>());
        }

        volume_credits += (play["type"] == performance_types[1]) ? (std::max(perf["audience"].get<int>() - 30, 0)) : (perf["audience"].get<int>() / 5);

       result << " " << play["name"].get<std::string>() << ": " << "$" << std::fixed << (this_amount/100) <<
            " (" << perf["audience"] << " seats)\n";
        total_amount += this_amount;
    }

    result << "Amount owed is " << "$" << std::fixed << (total_amount/100.0f) << "\n";
    result << "You earned " << volume_credits << " credits";
    return result.str();
}
