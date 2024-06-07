#include "bot_manager.h"

#include "consts.h"

#include <database_manager/database_manager_tools.h>
#include <felisss_logger/felisss_logger.h>

#include <algorithm>

TgBot::ReplyKeyboardMarkup::Ptr BotManager::mSkipMenu(new TgBot::ReplyKeyboardMarkup);

BotManager::BotManager(const std::string_view& token, const std::string_view& admin_pass) :
    mToken(token),
    mAdminPass(admin_pass),
    mBotHandler(token.data()),
    mLongPoll(mBotHandler),
    mDatabase(new DatabaseManager(DB_PATHNAME)) {
    init();
}

void BotManager::poll() {
    mLongPoll.start();
}

void BotManager::init() {
    initMenus();

    mBotHandler.getEvents().onCommand(
        "start", std::bind(&BotManager::callbackOnStartCommand, this, std::placeholders::_1));

    mBotHandler.getEvents().onCommand(
        "owner", std::bind(&BotManager::callbackOnOwnerCommand, this, std::placeholders::_1));

    mBotHandler.getEvents().onCommand(
        mAdminPass.data(),
        std::bind(&BotManager::callbackOnMakeMeAdminCommand, this, std::placeholders::_1));

    mBotHandler.getEvents().onAnyMessage(
        std::bind(&BotManager::callbackOnAnyMessage, this, std::placeholders::_1));

    mBotHandler.getEvents().onCallbackQuery(
        std::bind(&BotManager::callbackOnCallbackQuery, this, std::placeholders::_1));

    mAlarmMessageTimer.setCallback(std::bind(&BotManager::sendMaterialAlarms, this));
    mAlarmMessageTimer.setSingleShot(true);
    mAlarmMessageTimer.setInterval(mAlarmMessageDelayMinutes * 60 * 1000);

    mSessionReminderTimer.setCallback(
        std::bind(&BotManager::sendSessionReminderIfNessessory, this));
    mSessionReminderTimer.setInterval(60 * 1000);
    mSessionReminderTimer.startOrReset();

    spdlog::info("Bot: {}(@{})", mBotHandler.getApi().getMe()->firstName,
                 mBotHandler.getApi().getMe()->username);
}

void BotManager::initMenus() {
    mSkipMenu->isPersistent    = false;
    mSkipMenu->oneTimeKeyboard = true;
    mSkipMenu->resizeKeyboard  = true;

    TgBot::KeyboardButton::Ptr skip_button(new TgBot::KeyboardButton);
    skip_button->text            = SKIP_BUTTON_TEXT.data();
    skip_button->requestContact  = false;
    skip_button->requestLocation = false;
    mSkipMenu->keyboard.push_back({skip_button});
}

void BotManager::sendMessage(std::int64_t telegram_id, const std::string& message) {
    try {
        mBotHandler.getApi().sendMessage(telegram_id, message, false, 0, nullptr,
                                         PARSE_MODE.data());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void BotManager::sendMessage(const TgBot::Message::Ptr& recv_message, const std::string& message) {
    sendMessage(getTelegramIdFromQuery(recv_message), message);
}

void BotManager::sendMessage(const TgBot::CallbackQuery::Ptr& query, const std::string& message) {
    sendMessage(getTelegramIdFromQuery(query), message);
}

void BotManager::sendMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                                     const TgBot::GenericReply::Ptr& menu,
                                     const std::string& message) {
    mBotHandler.getApi().sendMessage(recv_message->chat->id, message, false, 0, menu,
                                     PARSE_MODE.data());
}

void BotManager::editMenuWithMessage(const TgBot::Message::Ptr& recv_message,
                                     const TgBot::InlineKeyboardMarkup::Ptr& menu,
                                     const std::string& message) {
    try {
        mBotHandler.getApi().editMessageText(message, recv_message->chat->id,
                                             recv_message->messageId, "", PARSE_MODE.data(), false,
                                             menu);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
}

void BotManager::sendCurrentMenu(const TgBot::Message::Ptr& message) {
    const auto chat_status = getClientChatStatus(getTelegramIdFromQuery(message));
    const auto menu_msg    = chat_status->getMenuMessage();
    sendMenuWithMessage(message, chat_status->getMenu(), menu_msg);
}

void BotManager::editCurrentMenu(const TgBot::Message::Ptr& message) {
    const auto chat_status = getClientChatStatus(getTelegramIdFromQuery(message));
    const auto menu_msg    = chat_status->getMenuMessage();
    editMenuWithMessage(message, chat_status->getMenu(), menu_msg);
}

void BotManager::sendCurrentMenu(const TgBot::CallbackQuery::Ptr& query) {
    const auto chat_status = getClientChatStatus(getTelegramIdFromQuery(query));
    const auto menu_msg    = chat_status->getMenuMessage();
    sendMenuWithMessage(query->message, chat_status->getMenu(), menu_msg);
}

void BotManager::editCurrentMenu(const TgBot::CallbackQuery::Ptr& query) {
    const auto chat_status = getClientChatStatus(getTelegramIdFromQuery(query));
    const auto menu_msg    = chat_status->getMenuMessage();
    editMenuWithMessage(query->message, chat_status->getMenu(), menu_msg);
}

void BotManager::callbackOnStartCommand(const TgBot::Message::Ptr& message) {
    if (!mDatabase->checkIfTelegramIdIsAdmin(message->from->id)
        && !mDatabase->checkIfTelegramIdIsTattooArtist(message->from->id)) {
        sendMessage(message,
                    "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
        return;
    }

    const auto chat_status = getClientChatStatus(getTelegramIdFromQuery(message));
    chat_status->setMenu(Menus::MAIN_MENU);
    sendCurrentMenu(message);
}

void BotManager::callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabase->getUserByTelegramId(telegram_id);

    if (mDatabase->checkIfTelegramIdIsAdmin(telegram_id)) {
        sendMessage(message, "Ти вже адмін");
    } else {
        mDatabase->addAdmin(user.value());
        sendMessage(message, "Тепер ти адмін");
    }
}

void BotManager::callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabase->getUserByTelegramId(telegram_id);

    if (mDatabase->checkIfTelegramIdIsTattooArtist(telegram_id)) {
        sendMessage(message, "Ти вже тату майстер");
    } else {
        mDatabase->addTattooArtist(user.value());
        sendMessage(message, "Тепер ти тату майстер");
    }
}

void BotManager::callbackOnOwnerCommand(const TgBot::Message::Ptr& message) {
    sendMessage(message, "Я бот пана Дмитра (@TENK_28)");
}

void BotManager::callbackOnAnyMessage(const TgBot::Message::Ptr& message) {
    try {
        insertUserInTableIfNotExists(message);

        spdlog::info("{} wrote \"{}\"", formUserInfoStr(message->from), message->text);

        if (StringTools::startsWith(message->text, "/")) {
            return;
        }

        if (!mDatabase->checkIfTelegramIdIsAdmin(message->from->id)
            && !mDatabase->checkIfTelegramIdIsTattooArtist(message->from->id)) {
            sendMessage(message,
                        "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        const auto client_status = getClientChatStatus(getTelegramIdFromQuery(message));
        if (!validateMessageAndSendErrorWithMenu(message)) {
            client_status->clearAllProperties();
            return;
        }

        if (client_status->at(ChatStatuses::DO_USER_TYPE_MATERIAL_NAME)) {
            client_status->at(ChatStatuses::DO_USER_TYPE_MATERIAL_NAME) = false;
            const auto find_result = mDatabase->getMaterialByName(message->text);
            if (find_result) {
                const auto send_str = fmt::format("Наразі {} вже в базі даних",
                                                  formMaterialInfoStr(*find_result));
                sendMessage(message, send_str);
                sendCurrentMenu(message);
                return;
            }

            client_status->updating_material.name                            = message->text;
            client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_SUFFIX) = true;
            sendMenuWithMessage(message, mSkipMenu, "В чому вимірюється одиниця матеріалу?");
        } else if (client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_SUFFIX)) {
            client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_SUFFIX) = false;
            if (message->text != SKIP_BUTTON_TEXT) {
                client_status->updating_material.suffix = message->text;
            }

            client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_COUNT) = true;
            sendMessage(message, "Скільки зараз одиниць матеріалу в салоні?");
        } else if (client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_COUNT)) {
            client_status->at(ChatStatuses::DO_USER_SPECIFY_MATERIAL_COUNT) = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                sendCurrentMenu(message);
                SPDLOG_ERROR("{}", e.what());
                return;
            }
            if (mDatabase->addMaterial(client_status->updating_material)) {
                const auto send_str = fmt::format(
                    "{} внесено в базу даних",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(message, send_str);
            } else {
                sendMessage(message, ERROR_MESSAGE.data());
            }
            sendCurrentMenu(message);

        } else if (client_status->at(ChatStatuses::DO_USER_UPDATE_MATERIAL_COUNT)) {
            client_status->at(ChatStatuses::DO_USER_UPDATE_MATERIAL_COUNT) = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                SPDLOG_ERROR("{}", e.what());
                sendCurrentMenu(message);
                return;
            }

            if (mDatabase->updateMaterialCountById(client_status->updating_material.id.value(),
                                                   client_status->updating_material)) {

                const auto send_str = fmt::format(
                    "{} оновлено в базі даних",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(message, send_str);
                scheduleCriticalAmountMessageIfNessessory();
            } else {
                sendMessage(message, ERROR_MESSAGE.data());
            }
            client_status->returnToPreviousMenu();
            sendCurrentMenu(message);
        } else if (client_status->at(
                       ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_ADD)) {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_ADD) = false;
            try {
                const auto new_critical_amount = std::stoll(message->text);
                if (mDatabase->addMaterialCriticalAmount(client_status->updating_material,
                                                         new_critical_amount)) {
                    const auto material
                        = mDatabase->getMaterialById(client_status->updating_material.id.value())
                              .value();
                    sendMessage(message,
                                fmt::format("Для {} оновлено критичну кількість - {}",
                                            formMaterialInfoStr(material), new_critical_amount));
                    sendCurrentMenu(message);
                } else {
                    sendMessage(message, ERROR_MESSAGE.data());
                }
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                SPDLOG_ERROR("{}", e.what());
                sendCurrentMenu(message);
                return;
            }
        } else if (client_status->at(
                       ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE)) {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE)
                = false;
            try {
                MaterialCriticalAmountTable::MaterialCriticalAmountRow new_critical_amount = {
                    .critical_amount = std::stoll(message->text)};
                if (mDatabase->updateMaterialCriticalAmountById(
                        client_status->updating_material.id.value(), new_critical_amount)) {
                    const auto material
                        = mDatabase->getMaterialById(client_status->updating_material.id.value())
                              .value();
                    sendMessage(message, fmt::format("Для {} оновлено критичну кількість - {}",
                                                     formMaterialInfoStr(material),
                                                     new_critical_amount.critical_amount));
                    sendCurrentMenu(message);
                } else {
                    sendMessage(message, ERROR_MESSAGE.data());
                }
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                SPDLOG_ERROR("{}", e.what());
                sendCurrentMenu(message);
                return;
            }
        } else if (client_status->at(ChatStatuses::DO_USER_TYPE_DATE)) {
            client_status->at(ChatStatuses::DO_USER_TYPE_DATE) = false;

            if (!DatabaseManagerTools::isValidDateTimeFormat(message->text)) {
                sendMessage(message, "Неправильний формат дати");
                sendCurrentMenu(message);
                return;
            }
            auto tp = DatabaseManagerTools::convertStrToTimePoint(message->text);
            if (!tp.has_value()) {
                sendMessage(message, "Неправильно зазначена дата");
                sendCurrentMenu(message);
                return;
            }
            client_status->session_row.date_time
                = DatabaseManagerTools::convertTimePointToStr(*tp).value();

            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_FOR_SESSION) = true;

            client_status->setMenu(Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_CUSTOMER);
            sendCurrentMenu(message);
        } else if (client_status->at(ChatStatuses::DO_USER_TYPE_USER_NAME)) {
            client_status->user_row.name                               = message->text;
            client_status->at(ChatStatuses::DO_USER_TYPE_USER_NAME)    = false;
            client_status->at(ChatStatuses::DO_USER_TYPE_USER_SURNAME) = true;
            sendMenuWithMessage(message, mSkipMenu, "Введіть прізвище(можна пропустити):");
        } else if (client_status->at(ChatStatuses::DO_USER_TYPE_USER_SURNAME)) {
            if (message->text != SKIP_BUTTON_TEXT) {
                client_status->user_row.surname = message->text;
            }
            client_status->at(ChatStatuses::DO_USER_TYPE_USER_SURNAME) = false;
            if (mDatabase->addUser(client_status->user_row)) {
                sendMessage(message, fmt::format("{} додано в базу даних",
                                                 formUserInfoStr(client_status->user_row)));
            } else {
                sendMessage(message, fmt::format("Не вдалося {} додати в базу даних",
                                                 formUserInfoStr(client_status->user_row)));
            }
            sendCurrentMenu(message);
        } else {
            sendMessage(message, "Невідомий статус чату");
            sendCurrentMenu(message);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        sendMessage(message,
                    "На жаль сталося помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

void BotManager::callbackOnCallbackQuery(const TgBot::CallbackQuery::Ptr& query) {
    try {
        spdlog::info("{} applied callback with data \"{}\"", formUserInfoStr(query->from),
                     query->data);
        if (!mDatabase->checkIfTelegramIdIsAdmin(getTelegramIdFromQuery(query))
            && !mDatabase->checkIfTelegramIdIsTattooArtist(getTelegramIdFromQuery(query))) {
            sendMessage(query,
                        "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        const auto client_status = getClientChatStatus(getTelegramIdFromQuery(query));
        if (query->data == "back_button") {
            client_status->clearAllProperties();
            client_status->returnToPreviousMenu();
            editCurrentMenu(query);
        } else if (query->data == "main_menu") {
            client_status->setMenu(Menus::MAIN_MENU);
            editCurrentMenu(query);
        } else if (query->data == "materials") {
            client_status->setMenu(Menus::MATERIALS_MENU);
            editCurrentMenu(query);
        } else if (query->data == "add_material") {
            sendMessage(query, "Як називається матеріал?");
            client_status->at(ChatStatuses::DO_USER_TYPE_MATERIAL_NAME) = true;
        } else if (query->data == "delete_material" || query->data == "modify_material") {
            if (query->data == "delete_material") {
                client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_DELETE_MATERIAL) = true;
                client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_MODIFY_MATERIAL) = false;
                client_status->setMenu(Menus::MATERIALS_DELETE_MATERIAL_MENU);
            } else if (query->data == "modify_material") {
                client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_MODIFY_MATERIAL) = true;
                client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_DELETE_MATERIAL) = false;
                client_status->setMenu(Menus::MATERIALS_UPDATE_MATERIAL_MENU);
            }

            editCurrentMenu(query);
        } else if (StringTools::startsWith(query->data, CHOOSE_MATERIAL_PREFIX.data())) {
            std::string data_str(query->data);
            data_str.erase(0, CHOOSE_MATERIAL_PREFIX.length());

            client_status->updating_material.id = std::stoll(data_str);
            const auto material_id              = client_status->updating_material.id.value();
            client_status->updating_material    = mDatabase->getMaterialById(material_id).value();

            if (client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_MODIFY_MATERIAL)) {
                client_status->at(ChatStatuses::DO_USER_UPDATE_MATERIAL_COUNT) = true;
                const auto send_message                                        = fmt::format(
                    "Введіть нову кількість для {}",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(query, send_message);
            } else if (client_status->at(ChatStatuses::DO_USER_CHOOSE_TO_DELETE_MATERIAL)) {
                mDatabase->deleteMaterialCriticalAmountByMaterialId(material_id);

                if (mDatabase->deleteMaterialById(material_id)) {
                    const auto send_message = fmt::format(
                        "{} видалено успішно",
                        formMaterialInfoStr(client_status->updating_material));
                    sendMessage(query, send_message);
                } else {
                    const auto send_message = fmt::format(
                        "На жаль не вдалося видалити {}. Зверніться до адміністратора!",
                        formMaterialInfoStr(client_status->updating_material));
                    sendMessage(query, send_message);
                }
                client_status->returnToPreviousMenu();
                sendCurrentMenu(query);
            } else if (client_status->at(
                           ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_ADD)) {
                sendMessage(query,
                            fmt::format("Введіть критичну кількість для {}",
                                        formMaterialInfoStr(client_status->updating_material)));
            } else if (client_status->at(
                           ChatStatuses::DO_USER_TYPE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE)) {
                client_status->at(ChatStatuses::DO_USER_TYPE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE)
                    = true;
                const auto critical_amount = mDatabase->getMaterialCriticalAmountByMaterialId(
                    material_id);
                sendMessage(query,
                            fmt::format("Введіть нову критичного кількість для {}, минула: {}",
                                        formMaterialInfoStr(client_status->updating_material),
                                        (*critical_amount).critical_amount));
            } else if (client_status->at(
                           ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_DELETE)) {
                const auto critical_amount = mDatabase->getMaterialCriticalAmountByMaterialId(
                    material_id);
                if (mDatabase->deleteMaterialCriticalAmountByMaterialId(material_id)) {
                    sendMessage(query,
                                fmt::format("Прибрано критичну кількість {} для {}",
                                            (*critical_amount).critical_amount,
                                            formMaterialInfoStr(client_status->updating_material)));
                } else {
                    sendMessage(query, ERROR_MESSAGE.data());
                }
                sendCurrentMenu(query);
            }
        } else if (query->data == "modify_alarm_users") {
            client_status->setMenu(Menus::MATERIALS_CHOOSE_USER_CRITICAL_ALARM_MENU);
            editCurrentMenu(query);
        } else if (StringTools::startsWith(query->data, CHOOSE_MATERIAL_ALARM_USER_PREFIX.data())) {
            std::string data_str(query->data);
            data_str.erase(0, CHOOSE_MATERIAL_ALARM_USER_PREFIX.length());

            const auto switch_alarm_user_id = std::stoll(data_str);
            const auto material_alarm_users = mDatabase->getMaterialAlarmUsers();
            const bool isAlarmOn            = std::any_of(material_alarm_users.begin(),
                                                          material_alarm_users.end(),
                                                          [switch_alarm_user_id](const auto& alarm_user) {
                                                   return (switch_alarm_user_id == alarm_user.id);
                                               });
            const auto users                = mDatabase->getUsers();
            const auto found_user           = std::find_if(users.begin(), users.end(),
                                                           [switch_alarm_user_id](const auto& user) {
                                                     return (user.id == switch_alarm_user_id);
                                                 });
            bool is_operation_okay          = false;
            if (isAlarmOn) {
                if (mDatabase->deleteMaterialAlarmUserByUserId(switch_alarm_user_id)) {
                    sendMessage(query, fmt::format("{} прибрано із системи сповіщень",
                                                   formUserInfoStr(*found_user)));
                    is_operation_okay = true;
                }
            } else {
                if (mDatabase->addMaterialAlarmUser(*found_user)) {
                    sendMessage(query, fmt::format("{} додано до системи сповіщень",
                                                   formUserInfoStr(*found_user)));
                    is_operation_okay = true;
                }
            }
            if (!is_operation_okay) {

                SPDLOG_ERROR("Не вдалося {} сповіщення для {}",
                             (isAlarmOn) ? ("прибрати") : ("додати"), formUserInfoStr(*found_user));
                sendMessage(query, ERROR_MESSAGE.data());
            }
            sendCurrentMenu(query);
        } else if (query->data == "materials_menu_material_critical_amount") {
            client_status->setMenu(Menus::CRITICAL_MATERIALS_MENU);
            editCurrentMenu(query);
        } else if (query->data == "configure_material_critical_amount_add"
                   || query->data == "configure_material_critical_amount_modify"
                   || query->data == "configure_material_critical_amount_delete") {
            if (query->data == "configure_material_critical_amount_add") {
                client_status->setMenu(Menus::CRITICAL_MATERIALS_MENU_ADD);
                client_status->at(ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_ADD)
                    = true;
            } else if (query->data == "configure_material_critical_amount_modify") {
                client_status->setMenu(Menus::CRITICAL_MATERIALS_MENU_UPDATE);
                client_status->at(ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_UPDATE)
                    = true;
            } else if (query->data == "configure_material_critical_amount_delete") {
                client_status->setMenu(Menus::CRITICAL_MATERIALS_MENU_DELETE);
                client_status->at(ChatStatuses::DO_USER_CHOOSE_MATERIAL_CRITICAL_AMOUNT_TO_DELETE)
                    = true;
            }
            editCurrentMenu(query);
        } else if (query->data == "sessions") {
            client_status->setMenu(Menus::SESSIONS_MENU);
            editCurrentMenu(query);
        } else if (query->data == "add_session") {
            client_status->session_row = SessionsTable::SessionRow{};
            client_status->setMenu(Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_TATTOO_ARTIST);
            editCurrentMenu(query);
        } else if (query->data == "delete_session") {
            client_status->setMenu(Menus::SESSIONS_MENU_DELETE_SESSION);
            editCurrentMenu(query);
        } else if (StringTools::startsWith(query->data, CHOOSE_TATTOO_ARTIST_PREFIX.data())) {
            try {
                std::string data_str(query->data);
                data_str.erase(0, CHOOSE_TATTOO_ARTIST_PREFIX.length());

                std::int64_t tattoo_artist_id = std::stoll(data_str);

                sendMessage(query, "Відправ дату сеансу у форматі dd/mm/year, а також "
                                   "додай hh:mm якщо відомий час:");
                client_status->session_row.tattoo_artist_id        = tattoo_artist_id;
                client_status->at(ChatStatuses::DO_USER_TYPE_DATE) = true;
            } catch (const std::exception& e) {
                SPDLOG_ERROR("{}", e.what());
                sendMessage(query, ERROR_MESSAGE.data());
            }
        } else if (StringTools::startsWith(query->data, CHOOSE_USER_PREFIX.data())) {
            try {
                std::string data_str(query->data);
                data_str.erase(0, CHOOSE_USER_PREFIX.length());

                if (client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_FOR_SESSION)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_FOR_SESSION) = false;
                    if (data_str != "no") {
                        client_status->session_row.user_id = std::stoll(data_str);
                    }

                    const auto tattoo_artist = *mDatabase->getUserById(
                        client_status->session_row.tattoo_artist_id);
                    std::optional<UsersTable::UserRow> customer;
                    if (client_status->session_row.user_id) {
                        customer = mDatabase->getUserById(*client_status->session_row.user_id);
                    }

                    if (mDatabase->addSession(client_status->session_row)) {
                        sendMessage(query,
                                    fmt::format("{} додано в бази даних",
                                                formSessionInfoStr(client_status->session_row,
                                                                   tattoo_artist, customer)));
                    } else {
                        sendMessage(query,
                                    fmt::format("не вдалося додади {} в бази даних",
                                                formSessionInfoStr(client_status->session_row,
                                                                   tattoo_artist, customer)));
                    }
                    client_status->setMenu(Menus::SESSIONS_MENU);
                    sendCurrentMenu(query);
                } else if (client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_TO_DELETE)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_TO_DELETE) = false;

                    if (data_str != "no") {
                        const auto user     = mDatabase->getUserById(std::stoll(data_str)).value();
                        const auto user_str = formUserInfoStr(user);
                        if (mDatabase->deleteUser(user.id.value())) {
                            sendMessage(
                                query,
                                fmt::format("Користувача {} було видалено з бази даних", user_str));
                        } else {
                            sendMessage(
                                query,
                                fmt::format("Користувача {} не вдалося видалити з бази даних",
                                            user_str));
                        }
                    }

                    client_status->setMenu(Menus::USERS_MENU);
                    sendCurrentMenu(query);
                } else if (client_status->at(
                               ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_ADD)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_ADD)
                        = false;
                    const auto user_id = std::stoll(data_str);
                    const auto user    = mDatabase->getUserById(user_id).value();
                    if (mDatabase->addTattooArtist(user)) {
                        sendMessage(query,
                                    fmt::format("Користувача {} успішно додано до тату майстрів",
                                                formUserInfoStr(user)));
                    } else {
                        sendMessage(query, fmt::format("Не вдалося додати {} до тату майстрів",
                                                       formUserInfoStr(user)));
                    }
                    client_status->setMenu(Menus::USER_RIGHTS_MENU);
                    sendCurrentMenu(query);
                } else if (client_status->at(
                               ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_DELETE)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_DELETE)
                        = false;
                    const auto user_id = std::stoll(data_str);
                    const auto user    = mDatabase->getUserById(user_id).value();
                    if (mDatabase->deleteTattooArtistByUserId(user.id.value())) {
                        sendMessage(query,
                                    fmt::format("Користувача {} успішно видалено з тату майстрів",
                                                formUserInfoStr(user)));
                    } else {
                        sendMessage(query, fmt::format("Не вдалося видалити {} з тату майстрів",
                                                       formUserInfoStr(user)));
                    }
                    client_status->setMenu(Menus::USER_RIGHTS_MENU);
                    sendCurrentMenu(query);

                } else if (client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_ADD)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_ADD) = false;
                    const auto user_id = std::stoll(data_str);
                    const auto user    = mDatabase->getUserById(user_id).value();
                    if (mDatabase->addAdmin(user)) {
                        sendMessage(query, fmt::format("Користувача {} успішно додано до адмінів",
                                                       formUserInfoStr(user)));
                    } else {
                        sendMessage(query, fmt::format("Не вдалося додати {} до адмінів",
                                                       formUserInfoStr(user)));
                    }
                    client_status->setMenu(Menus::USER_RIGHTS_MENU);
                    sendCurrentMenu(query);

                } else if (client_status->at(
                               ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_DELETE)) {
                    client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_DELETE)
                        = false;
                    const auto user_id = std::stoll(data_str);
                    const auto user    = mDatabase->getUserById(user_id).value();
                    if (mDatabase->deleteAdminByUserId(user.id.value())) {
                        sendMessage(query, fmt::format("Користувача {} успішно видалено з адмінів",
                                                       formUserInfoStr(user)));
                    } else {
                        sendMessage(query, fmt::format("Не вдалося видалити {} з адмінів",
                                                       formUserInfoStr(user)));
                    }
                    client_status->setMenu(Menus::USER_RIGHTS_MENU);
                    sendCurrentMenu(query);
                }
            } catch (const std::exception& e) {
                SPDLOG_ERROR("{}", e.what());
                sendMessage(query, ERROR_MESSAGE.data());
            }
        } else if (StringTools::startsWith(query->data, CHOOSE_SESSION_PREFIX.data())) {
            try {
                std::string data_str(query->data);
                data_str.erase(0, CHOOSE_SESSION_PREFIX.length());

                const auto session_id = std::stoll(data_str);

                const auto session_to_delete = *mDatabase->getSessionById(session_id);

                const auto tattoo_artist = *mDatabase->getUserById(
                    session_to_delete.tattoo_artist_id);
                std::optional<UsersTable::UserRow> customer;
                if (session_to_delete.user_id) {
                    customer = mDatabase->getUserById(*session_to_delete.user_id);
                }
                if (mDatabase->deleteSession(session_id)) {
                    sendMessage(query, fmt::format("{} видалено з бази даних",
                                                   formSessionInfoStr(session_to_delete,
                                                                      tattoo_artist, customer)));
                } else {
                    sendMessage(query, fmt::format("не вдалося видалити {} з бази даних",
                                                   formSessionInfoStr(session_to_delete,
                                                                      tattoo_artist, customer)));
                }
                client_status->setMenu(Menus::SESSIONS_MENU);
                sendCurrentMenu(query);
            } catch (const std::exception& e) {
                SPDLOG_ERROR("{}", e.what());
                sendMessage(query, ERROR_MESSAGE.data());
            }
        } else if (query->data == "users") {
            client_status->setMenu(Menus::USERS_MENU);
            editCurrentMenu(query);
        } else if (query->data == "add_user") {
            client_status->clearAllProperties();
            client_status->at(ChatStatuses::DO_USER_TYPE_USER_NAME) = true;
            sendMessage(query, "Введіть імя користувача");
        } else if (query->data == "delete_user") {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_TO_DELETE) = true;
            client_status->setMenu(Menus::USERS_MENU_DELETE_USER);
            editCurrentMenu(query);
        } else if (query->data == "users_rights") {
            client_status->setMenu(Menus::USER_RIGHTS_MENU);
            editCurrentMenu(query);
        } else if (query->data == "user_rights_tattoo_artist_add") {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_ADD) = true;
            client_status->setMenu(Menus::USER_RIGHTS_MENU_ADD_TATTOO_ARTIST);
            editCurrentMenu(query);
        } else if (query->data == "user_rights_tattoo_artist_delete") {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_TATTOO_ARTIST_DELETE) = true;
            client_status->setMenu(Menus::USER_RIGHTS_MENU_DELETE_TATTOO_ARTIST);
            editCurrentMenu(query);
        } else if (query->data == "user_rights_admin_add") {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_ADD) = true;
            client_status->setMenu(Menus::USER_RIGHTS_MENU_ADD_ADMIN);
            editCurrentMenu(query);
        } else if (query->data == "user_rights_admin_delete") {
            client_status->at(ChatStatuses::DO_USER_CHOOSE_USER_RIGHTS_ADMIN_DELETE) = true;
            client_status->setMenu(Menus::USER_RIGHTS_MENU_DELETE_ADMIN);
            editCurrentMenu(query);
        } else {
            SPDLOG_ERROR("Callback \"{}\" is not valid", query->data);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        sendMessage(query, ERROR_MESSAGE.data());
    }
}

std::string BotManager::formUserInfoStr(const TgBot::User::Ptr& user) {
    UsersTable::UserRow user_row;
    user_row.name = user->firstName;
    if (user->lastName.length()) {
        user_row.surname = user->lastName;
    }
    if (user->username.length()) {
        user_row.telegram = user->username;
    }

    return formUserInfoStr(user_row);
}

std::string BotManager::formUserInfoStr(const UsersTable::UserRow& user) {
    std::string ret_str(user.name);
    if (user.surname) {
        ret_str.append(fmt::format(" {}", user.surname.value()));
    }
    if (user.telegram) {
        ret_str.append(fmt::format("(@{})", user.telegram.value()));
    }

    return ret_str;
}

std::string BotManager::formMaterialInfoStr(const MaterialsTable::MaterialRow& material_row) {
    return fmt::format("{} {}{}", material_row.name, material_row.count,
                       material_row.suffix.value_or(""));
}

std::string BotManager::formSessionInfoStr(const SessionsTable::SessionRow& row,
                                           const UsersTable::UserRow& tattoo_artist,
                                           std::optional<UsersTable::UserRow>& customer) {
    std::string ret_str;
    try {
        ret_str = fmt::format("сеанс у {} на {}", formUserInfoStr(tattoo_artist), row.date_time);
        if (row.user_id.has_value() && customer.has_value()) {
            ret_str.append(fmt::format(" для {}", formUserInfoStr(customer.value())));
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
    return ret_str;
}

void BotManager::sendMaterialAlarms() {
    const auto users              = mDatabase->getMaterialAlarmUsers();
    const auto critical_materials = mDatabase->getCriticalMaterials();

    if (critical_materials.size()) {
        std::string message("Критична кількість наступних матеріалів:\n");
        for (const auto& critical_material : critical_materials) {
            const auto material_critical_amount = mDatabase->getMaterialCriticalAmountByMaterialId(
                critical_material.id.value());
            message.append(fmt::format(
                " - {} має критичну кількість {} <= {}\n", formMaterialInfoStr(critical_material),
                critical_material.count, material_critical_amount.value().critical_amount));
        }
        for (const auto& user : users) {
            sendMessage(user.telegram_id.value(), message);
            spdlog::info("Scheduled alarm message sent to {}", formUserInfoStr(user));
        }
    }
}

void BotManager::scheduleCriticalAmountMessageIfNessessory() {
    const auto critical_materials = mDatabase->getCriticalMaterials();
    if (critical_materials.size()) {
        spdlog::info("Scheduled alarm message for {} minute", mAlarmMessageDelayMinutes);
        mAlarmMessageTimer.startOrReset();
    } else {
        mAlarmMessageTimer.stop();
    }
}

void BotManager::sendSessionReminderIfNessessory() {
    const auto sessions = mDatabase->getSessionsInFuture();
    if (sessions.size()) {
        const auto now_tm = DatabaseManagerTools::convertTimePointToTm(
            std::chrono::system_clock::now());

        for (const auto& session : sessions) {
            const auto session_tp
                = DatabaseManagerTools::convertStrToTimePoint(session.date_time).value();
            std::tm session_tm = DatabaseManagerTools::convertTimePointToTm(session_tp);

            // reminder the hour before
            if (session_tm.tm_hour != 00) {
                if (now_tm.tm_mday == session_tm.tm_mday && now_tm.tm_mon == session_tm.tm_mon
                    && now_tm.tm_yday == session_tm.tm_yday) {
                    if (now_tm.tm_hour == session_tm.tm_hour - 1
                        && now_tm.tm_min == session_tm.tm_min) {
                        sendSessionReminder(session);
                    }
                }
            }

            std::tm the_day_before_session_tm = DatabaseManagerTools::convertTimePointToTm(
                session_tp - std::chrono::hours(24));

            if (now_tm.tm_mday == the_day_before_session_tm.tm_mday
                && now_tm.tm_mon == the_day_before_session_tm.tm_mon
                && now_tm.tm_yday == the_day_before_session_tm.tm_yday) {
                if (now_tm.tm_hour == 18 && now_tm.tm_min == 0) {
                    sendSessionReminder(session);
                }
            }
        }
    }
}

void BotManager::sendSessionReminder(const SessionsTable::SessionRow& row) {
    const auto tattoo_artist = *mDatabase->getUserById(row.tattoo_artist_id);
    std::string message(fmt::format("Нагадування: в тебе сеанс на {}", row.date_time));
    if (row.user_id) {
        const auto customer = *mDatabase->getUserById(*row.user_id);
        message.append(fmt::format(" для {}", formUserInfoStr(customer)));
    }

    spdlog::info("Sending session reminder for {}, at {}", formUserInfoStr(tattoo_artist),
                 row.date_time);
    sendMessage(tattoo_artist.telegram_id.value(), message);
}

std::int64_t BotManager::getTelegramIdFromQuery(const TgBot::CallbackQuery::Ptr& query) {
    return query->from->id;
}

std::int64_t BotManager::getTelegramIdFromQuery(const TgBot::Message::Ptr& message) {
    return message->from->id;
}

std::shared_ptr<ClientChatStatus> BotManager::getClientChatStatus(std::int64_t telegram_id) {
    std::shared_ptr<ClientChatStatus> ret_value;
    const auto it = mClientChatStatuses.find(telegram_id);
    if (it != mClientChatStatuses.end()) {
        ret_value = it->second;
    } else {
        SPDLOG_INFO("Inserting telegram_id: {}", telegram_id);
        std::shared_ptr<ClientChatStatus> created_ptr(new ClientChatStatus(telegram_id, mDatabase));

        mClientChatStatuses.insert({telegram_id, created_ptr});
        ret_value = created_ptr;
    }

    return ret_value;
}

void BotManager::insertUserInTableIfNotExists(const TgBot::Message::Ptr& message) {
    const auto found_user = mDatabase->getUserByTelegramId(message->from->id);
    if (!found_user) {
        mDatabase->addUser(scrapUserDataFromMessage(message));
    }
}

UsersTable::UserRow BotManager::scrapUserDataFromMessage(const TgBot::Message::Ptr& message) {
    UsersTable::UserRow user_row{};
    user_row.telegram_id = message->from->id;
    user_row.name        = message->chat->firstName;
    if (message->chat->lastName.length()) {
        user_row.surname = message->chat->lastName;
    }
    if (message->chat->username.length()) {
        user_row.telegram = message->chat->username;
    }

    return user_row;
}

bool BotManager::validateMessageAndSendErrorWithMenu(const TgBot::Message::Ptr& message) {
    bool is_okay = true;
    if (!DatabaseManagerTools::validateUserInput(message->text)) {
        sendMessage(message, "Помічені заборонені знаки: \"^['\";%]*$\"");
        is_okay = false;
    }

    if (is_okay && message->animation) {
        sendMessage(message, "Повідомлення не може бути анімацією");
        is_okay = false;
    }

    if (is_okay && message->audio) {
        sendMessage(message, "Повідомлення не може бути аудіо");
        is_okay = false;
    }

    if (is_okay && message->contact) {
        sendMessage(message, "Повідомлення не може бути контактом");
        is_okay = false;
    }

    if (is_okay && message->document) {
        sendMessage(message, "Повідомлення не може бути документом");
        is_okay = false;
    }

    if (is_okay && message->game) {
        sendMessage(message, "Повідомлення не може бути грою");
        is_okay = false;
    }

    if (is_okay && message->location) {
        sendMessage(message, "Повідомлення не може бути локацією");
        is_okay = false;
    }

    if (is_okay && !message->text.length()) {
        sendMessage(message, "Повідомлення не може бути пустим");
        is_okay = false;
    }

    if (!is_okay) {
        sendCurrentMenu(message);
    }

    return is_okay;
}
