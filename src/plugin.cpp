#include "plugin.hpp"

void CORE_Process(size_t arg);

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

        bool idaapi run(size_t arg)
        {
            CORE_Process(arg);
            return true;
        }
    }
}

plugin_t PLUGIN = {
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
