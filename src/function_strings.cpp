#include "function_strings.hpp"
#include "ida_sdk.hpp"
#include <pro.h>
#include <vector>
#include <ranges>
#include <optional>

namespace momo
{
    namespace
    {
        constexpr size_t MIN_STR_SIZE = 4;
        constexpr size_t MAX_LINE_STR_COUNT = 10;
        constexpr size_t MAX_COMMENT = 764;

        std::string trim(std::string_view str)
        {
            auto trimmed = str                                                                         //
                           | std::views::drop_while([](unsigned char ch) { return std::isspace(ch); }) //
                           | std::views::reverse                                                       //
                           | std::views::drop_while([](unsigned char ch) { return std::isspace(ch); }) //
                           | std::views::reverse;
            return {trimmed.begin(), trimmed.end()};
        }

        std::optional<std::string> resolve_string_at_address(ea_t address)
        {
            xrefblk_t xb;
            if (!xb.first_from(address, XREF_DATA))
            {
                return std::nullopt;
            }

            // TODO: Support more string types.
            const auto str_type = static_cast<int32>(get_str_type(xb.to));
            if (str_type != STRTYPE_C)
            {
                return std::nullopt;
            }

            qstring buffer{};
            const auto len = get_max_strlit_length(xb.to, str_type, ALOPT_IGNHEADS);
            if (len < MIN_STR_SIZE)
            {
                return std::nullopt;
            }

            get_strlit_contents(&buffer, xb.to, len, str_type);

            std::string_view buffer_view(buffer.c_str(), buffer.size());
            auto trimmed_str = trim(buffer_view);

            if (trimmed_str.size() < MIN_STR_SIZE)
            {
                return std::nullopt;
            }

            return {std::move(trimmed_str)};
        }

        std::set<std::string> find_strings_in_function(func_t* function)
        {
            std::set<std::string> found_strings{};

            func_item_iterator_t it(function);

            do
            {
                ea_t currentEA = it.current();
                auto str = resolve_string_at_address(currentEA);

                if (str.has_value())
                {
                    found_strings.insert(std::move(*str));
                }
            } while (it.next_addr() && found_strings.size() < MAX_LINE_STR_COUNT);

            return found_strings;
        }

        bool has_comment(func_t* function)
        {
            if (!function)
            {
                return false;
            }

            qstring str{};
            get_func_cmt(&str, function, true);

            return !str.empty();
        }

        std::string build_final_comment(const std::set<std::string>& strings)
        {
            std::string comment{};

            for (const auto& s : strings)
            {
                if (comment.empty())
                {
                    comment = "STR: ";
                }
                else
                {
                    comment += ", ";
                }

                comment += "\"";
                comment += s;
                comment += "\"";

                if (comment.size() > MAX_COMMENT)
                {
                    break;
                }
            }

            return comment;
        }

        void associate_strings_to_function(func_t* function)
        {
            if (!function || function->size() < 8 || has_comment(function))
            {
                return;
            }

            const auto found_strings = find_strings_in_function(function);
            if (found_strings.empty())
            {
                return;
            }

            const auto comment = build_final_comment(found_strings);
            set_func_cmt(function, comment.c_str(), true);
        }

        template <typename Functor>
        void process_functions(const Functor& func)
        {
            const auto function_count = get_func_qty();
            for (size_t i = 0; i < function_count; i++)
            {
                msg("Processing function %u of %u\r", i + 1, function_count);

                auto* function = getn_func(i);
                func(function);
            }
        }
    }

    void associate_strings_to_functions()
    {
        process_functions(associate_strings_to_function);
    }
}
