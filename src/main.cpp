#include "felisss_logger/felisss_logger.h"

#include "bot_manager/bot_manager.h"
#include "config_manager/config_manager.h"
#include "database_manager/database_manager.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include "bot_manager/timer.h"

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

        if (!(install_done(SIGINT) && install_done(SIGHUP) && install_done(SIGTERM))) {
            return EXIT_FAILURE;
        }

        auto config_values = ConfigManager::getConfigValues();
        auto token         = config_values.token;
        if (!token) {
            SPDLOG_CRITICAL("Token not specified in env");
            return EXIT_FAILURE;
        }

        spdlog::info("Token: {}", *token);
        if (*token == "<your_bot_token_here>") {
            SPDLOG_CRITICAL("Specify token in config.json");
            return EXIT_FAILURE;
        }

        BotManager bot(*token);
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
