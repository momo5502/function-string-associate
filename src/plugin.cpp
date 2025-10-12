#include "ida_sdk.hpp"
#include "function_strings.hpp"

namespace momo
{
    namespace
    {
        namespace plugin
        {
            const char* name = "Function String Associate";

            plugmod_t* idaapi initialize()
            {
                return PLUGIN_OK;
            }

            void idaapi terminate()
            {
            }

            bool idaapi run(size_t /*arg*/)
            {
                associate_strings_to_functions();
                return true;
            }

            plugin_t create()
            {
                return {
                    .version = IDP_INTERFACE_VERSION,
                    .flags = PLUGIN_UNL,
                    .init = plugin::initialize,
                    .term = plugin::terminate,
                    .run = plugin::run,
                    .comment = plugin::name,
                    .help = plugin::name,
                    .wanted_name = plugin::name,
                    .wanted_hotkey = nullptr,
                };
            }
        }
    }
}

plugin_t PLUGIN = momo::plugin::create();
