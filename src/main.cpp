#include "felisss_logger/felisss_logger.h"

#include "bot_manager/bot_manager.h"
#include "bot_manager/timer.h"
#include "config_manager/config_manager.h"
#include "database_manager/database_manager.h"

#include <felisss_tattoo/cmake_vars.h>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

static volatile sig_atomic_t done = 0;

static void done_handler(int signum) {
    spdlog::info("Finishing FelisssTattooBot, please wait...");
    done = 1;
}

static bool install_done(const int signum) {
    struct sigaction act;

    memset(&act, 0, sizeof(act));

    sigemptyset(&act.sa_mask);
    act.sa_flags   = 0;
    act.sa_handler = done_handler;
    if (sigaction(signum, &act, NULL) == -1) {
        SPDLOG_CRITICAL("Cannot install signal handlers: {}", strerror(errno));
        return false;
    }
    return true;
}

int main() {
    int app_exit_code = EXIT_SUCCESS;
    try {
        FelisssLogger::init();

        spdlog::info("FelisssTattooBot started...");

        if (!(install_done(SIGINT) && install_done(SIGHUP) && install_done(SIGTERM))) {
            return EXIT_FAILURE;
        }

        auto config_values = ConfigManager::getConfigValues(CONFIG_PATHNAME);
        if (!config_values.has_value()) {
            throw std::runtime_error(fmt::format("Couldn't parse config file {}", CONFIG_PATHNAME));
        }
        const auto token = config_values->token;

        spdlog::info("token: {}", *token);
        if (*token == "<your_bot_token_here>") {
            SPDLOG_CRITICAL("Specify token in {}", CONFIG_PATHNAME);
            return EXIT_FAILURE;
        }

        const auto admin_pass = config_values->admin_pass;
        if (!admin_pass) {
            SPDLOG_CRITICAL("There is no admin_pass in {}", CONFIG_PATHNAME);
            return EXIT_FAILURE;
        }
        spdlog::info("admin_pass: {}", *admin_pass);
        if (*admin_pass == "<admin_password_here>") {
            SPDLOG_CRITICAL("Specify admin_pass in {}", CONFIG_PATHNAME);
            return EXIT_FAILURE;
        }

        BotManager bot(*token, *admin_pass);
        while (!done) {
            bot.poll();
        }
    } catch (const std::exception& e) {
        SPDLOG_CRITICAL("{}", e.what());
        app_exit_code = EXIT_FAILURE;
    }
    spdlog::shutdown();

    return app_exit_code;
}
