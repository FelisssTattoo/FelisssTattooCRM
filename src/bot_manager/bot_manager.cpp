#include "bot_manager.h"

#include <database_manager/database_manager_tools.h>
#include <felisss_logger/felisss_logger.h>

#include <algorithm>

TgBot::InlineKeyboardButton::Ptr BotManager::mBackButton(new TgBot::InlineKeyboardButton);

TgBot::InlineKeyboardMarkup::Ptr BotManager::mMainMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr BotManager::mMaterialsMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr BotManager::mChooseMaterialMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr
    BotManager::mChooseMaterialAlarmUserMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr
    BotManager::mConfigureMaterialCriticalAmountMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr
    BotManager::mChooseCriticalAmountMaterialMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr
    BotManager::mChooseCriticalAmountMaterialMenuToAdd(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr
    BotManager::mChooseCriticalAmountMaterialMenuToUpdateDelete(new TgBot::InlineKeyboardMarkup);

BotManager::BotManager(const std::string& token) :
    mToken(token), mBotHandler(token), mLongPoll(mBotHandler), mDatabaseManager(DATABASE_PATHNAME) {
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
        "makemeadmin",
        std::bind(&BotManager::callbackOnMakeMeAdminCommand, this, std::placeholders::_1));

    mBotHandler.getEvents().onCommand(
        "makemetattooartist",
        std::bind(&BotManager::callbackOnMakeMeTattooArtistCommand, this, std::placeholders::_1));

    mBotHandler.getEvents().onAnyMessage(
        std::bind(&BotManager::callbackOnAnyMessage, this, std::placeholders::_1));

    mBotHandler.getEvents().onCallbackQuery(
        std::bind(&BotManager::callbackOnCallbackQuery, this, std::placeholders::_1));
}

void BotManager::initMenus() {
    mBackButton->text         = "<< Назад в меню";
    mBackButton->callbackData = "back_button";

    TgBot::InlineKeyboardButton::Ptr materials_button(new TgBot::InlineKeyboardButton);
    materials_button->text         = "Матеріали";
    materials_button->callbackData = "materials";
    mMainMenu->inlineKeyboard.push_back({materials_button});

    TgBot::InlineKeyboardButton::Ptr materials_menu_add_material(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_delete_material(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_modify_material(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_modify_alarm_users(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_material_critical_amount(
        new TgBot::InlineKeyboardButton);

    materials_menu_add_material->text            = "Додати матеріал";
    materials_menu_add_material->callbackData    = "add_material";
    materials_menu_delete_material->text         = "Видалити матеріал";
    materials_menu_delete_material->callbackData = "delete_material";
    materials_menu_modify_material->text = "Оновити кількість матеріалу";
    materials_menu_modify_material->callbackData = "modify_material";
    materials_menu_modify_alarm_users->text = "Налаштування користувачів із сповіщеннями";
    materials_menu_modify_alarm_users->callbackData = "modify_alarm_users";
    materials_menu_material_critical_amount->text
        = "Налаштування критичної кількості для матеріалів";
    materials_menu_material_critical_amount->callbackData
        = "materials_menu_material_critical_amount";

    TgBot::InlineKeyboardButton::Ptr configure_material_critical_amount_add(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr configure_material_critical_amount_delete(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr configure_material_critical_amount_modify(
        new TgBot::InlineKeyboardButton);
    configure_material_critical_amount_add->text = "Додати критичну кількість для матеріалу";
    configure_material_critical_amount_add->callbackData = "configure_material_critical_amount_add";
    configure_material_critical_amount_delete->text = "Видали критичну кількість для матеріалу";
    configure_material_critical_amount_delete->callbackData
        = "configure_material_critical_amount_delete";
    configure_material_critical_amount_modify->text = "Змінити критичну кількість для матеріалу";
    configure_material_critical_amount_modify->callbackData
        = "configure_material_critical_amount_modify";
    mConfigureMaterialCriticalAmountMenu->inlineKeyboard.push_back(
        {configure_material_critical_amount_add});
    mConfigureMaterialCriticalAmountMenu->inlineKeyboard.push_back(
        {configure_material_critical_amount_delete});
    mConfigureMaterialCriticalAmountMenu->inlineKeyboard.push_back(
        {configure_material_critical_amount_modify});
    mConfigureMaterialCriticalAmountMenu->inlineKeyboard.push_back({mBackButton});

    mMaterialsMenu->inlineKeyboard.push_back(
        {materials_menu_add_material, materials_menu_delete_material});
    mMaterialsMenu->inlineKeyboard.push_back({materials_menu_modify_material});
    mMaterialsMenu->inlineKeyboard.push_back({materials_menu_modify_alarm_users});
    mMaterialsMenu->inlineKeyboard.push_back({materials_menu_material_critical_amount});
    mMaterialsMenu->inlineKeyboard.push_back({mBackButton});
}

void BotManager::sendMessage(const TgBot::Message::Ptr& recv_message, const std::string& message) {
    try {
        mBotHandler.getApi().sendMessage(recv_message->chat->id, message, false, 0, nullptr,
                                         PARSE_MODE.data());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
    }
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

void BotManager::sendCurrentMenu(const TgBot::Message::Ptr& recv_message) {
    const auto chat_status = getClientChatStatus(recv_message);
    auto menu_msg          = getMenuMessage(chat_status->current_menu);
    if (menu_msg) {
        sendMenuWithMessage(recv_message, chat_status->current_menu, *menu_msg);
    } else {
        sendMessage(recv_message,
                    "На жаль сталася помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

void BotManager::editCurrentMenu(const TgBot::Message::Ptr& recv_message) {
    const auto chat_status = getClientChatStatus(recv_message);
    const auto menu_msg    = getMenuMessage(chat_status->current_menu);
    if (menu_msg) {
        editMenuWithMessage(recv_message, chat_status->current_menu, *menu_msg);
    } else {
        sendMessage(recv_message,
                    "На жаль сталася помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

void BotManager::callbackOnStartCommand(const TgBot::Message::Ptr& message) {
    if (!checkIfTelegramIdIsAdmin(message->from->id)) {
        sendMessage(message,
                    "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
        return;
    }

    const auto chat_status    = getClientChatStatus(message);
    chat_status->current_menu = mMainMenu;
    sendCurrentMenu(message);
}

void BotManager::callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabaseManager.getUserByTelegramId(telegram_id);

    if (checkIfTelegramIdIsAdmin(telegram_id)) {
        sendMessage(message, "Ти вже адмін");
    } else {
        mDatabaseManager.addAdmin(user.value());
        sendMessage(message, "Тепер ти адмін");
    }
}

void BotManager::callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabaseManager.getUserByTelegramId(telegram_id);

    if (checkIfTelegramIdIsTattooArtist(telegram_id)) {
        sendMessage(message, "Ти вже тату майстер");
    } else {
        mDatabaseManager.addTattooArtist(user.value());
        sendMessage(message, "Тепер ти тату майстер");
    }
}

void BotManager::callbackOnAnyMessage(const TgBot::Message::Ptr& message) {
    try {
        insertUserInTableIfNotExists(message);

        spdlog::info("{} wrote \"{}\"", formUserInfoStr(message->from), message->text);

        if (StringTools::startsWith(message->text, "/")) {
            return;
        }

        if (!checkIfTelegramIdIsAdmin(message->from->id)) {
            sendMessage(message,
                        "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        if (!DatabaseManagerTools::validateUserInput(message->text)) {
            sendMessage(message, ERROR_MESSAGE.data());
            sendCurrentMenu(message);
            return;
        }

        const auto client_status = getClientChatStatus(message);

        if (client_status->do_user_type_material_name) {
            client_status->do_user_type_material_name = false;
            const auto find_result = mDatabaseManager.getMaterialByName(message->text);
            if (find_result) {
                const auto send_str = fmt::format("Наразі {} вже в базі даних",
                                                  formMaterialInfoStr(*find_result));
                sendMessage(message, send_str);
                sendCurrentMenu(message);
                return;
            }

            client_status->do_user_specify_material_suffix = true;
            client_status->updating_material.name          = message->text;

            static TgBot::ReplyKeyboardMarkup::Ptr material_suffix_reply_keyboard(
                new TgBot::ReplyKeyboardMarkup);
            static bool isFirstTime = true;
            if (isFirstTime) {
                isFirstTime                                     = false;
                material_suffix_reply_keyboard->isPersistent    = false;
                material_suffix_reply_keyboard->oneTimeKeyboard = true;

                TgBot::KeyboardButton::Ptr material_suffix_no_suffix_button(
                    new TgBot::KeyboardButton);
                material_suffix_no_suffix_button->text = "Матеріал не має одиниці виміру";
                material_suffix_no_suffix_button->requestContact  = false;
                material_suffix_no_suffix_button->requestLocation = false;
                material_suffix_reply_keyboard->keyboard.push_back(
                    {material_suffix_no_suffix_button});
            }
            sendMenuWithMessage(message, material_suffix_reply_keyboard,
                                "В чому вимірюється одиниця матеріалу?");
        } else if (client_status->do_user_specify_material_suffix) {
            client_status->do_user_specify_material_suffix = false;
            client_status->do_user_specify_material_count  = true;

            if (message->text != "Матеріал не має одиниці виміру") {
                client_status->updating_material.suffix = message->text;
            }
            sendMessage(message, "Скільки зараз одиниць матеріалу в салоні?");
        } else if (client_status->do_user_specify_material_count) {
            client_status->do_user_specify_material_count = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                sendCurrentMenu(message);
                SPDLOG_ERROR("{}", e.what());
                return;
            }
            if (mDatabaseManager.addMaterial(client_status->updating_material)) {
                const auto send_str = fmt::format(
                    "{} внесено в базу даних",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(message, send_str);
            } else {
                sendMessage(message, ERROR_MESSAGE.data());
            }
            sendCurrentMenu(message);
        } else if (client_status->do_user_update_material_count) {
            client_status->do_user_update_material_count = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                sendMessage(message, ERROR_MESSAGE.data());
                SPDLOG_ERROR("{}", e.what());
                sendCurrentMenu(message);
                return;
            }

            if (mDatabaseManager.updateMaterialCountById(
                    client_status->updating_material.id.value(),
                    client_status->updating_material)) {

                const auto send_str = fmt::format(
                    "{} оновлено в базі даних",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(message, send_str);
            } else {
                sendMessage(message, ERROR_MESSAGE.data());
            }

            updateChooseMaterialMenu();
            sendCurrentMenu(message);
        } else if (client_status->do_user_choose_material_critical_amount_to_add) {
            client_status->do_user_choose_material_critical_amount_to_add = false;
            try {
                const auto new_critical_amount = std::stoll(message->text);
                if (mDatabaseManager.addMaterialCriticalAmount(client_status->updating_material,
                                                               new_critical_amount)) {
                    const auto material = mDatabaseManager
                                              .getMaterialById(
                                                  client_status->updating_material.id.value())
                                              .value();
                    sendMessage(message,
                                fmt::format("Для {} оновлено критичну кількість - {}",
                                            formMaterialInfoStr(material), new_critical_amount));
                    updateChooseMaterialCriticalAmountToAdd();
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
        } else if (client_status->do_user_type_material_critical_amount_to_update) {
            client_status->do_user_type_material_critical_amount_to_update = false;
            try {
                MaterialCriticalAmountTable::MaterialCriticalAmountRow new_critical_amount = {
                    .critical_amount = std::stoll(message->text)};
                if (mDatabaseManager.updateMaterialCriticalAmountById(
                        client_status->updating_material.id.value(), new_critical_amount)) {
                    const auto material = mDatabaseManager
                                              .getMaterialById(
                                                  client_status->updating_material.id.value())
                                              .value();
                    sendMessage(message, fmt::format("Для {} оновлено критичну кількість - {}",
                                                     formMaterialInfoStr(material),
                                                     new_critical_amount.critical_amount));
                    updateChooseMaterialCriticalAmountToUpdateDelete();
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
        if (!checkIfTelegramIdIsAdmin(query->from->id)) {
            sendMessage(query->message,
                        "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        const auto client_status = getClientChatStatus(query->message);
        if (query->data == "back_button") {
            client_status->clearAllProperties();
            client_status->current_menu = returnPreviousMenu(client_status->current_menu);
            editCurrentMenu(query->message);
        } else if (query->data == "main_menu") {
            client_status->current_menu = mMainMenu;
            editCurrentMenu(query->message);
        } else if (query->data == "materials") {
            client_status->current_menu = mMaterialsMenu;
            editCurrentMenu(query->message);
        } else if (query->data == "add_material") {
            sendMessage(query->message, "Як називається матеріал?");
            client_status->do_user_type_material_name = true;
        } else if (query->data == "delete_material" || query->data == "modify_material") {
            updateChooseMaterialMenu();

            if (query->data == "delete_material") {
                client_status->do_user_choose_to_delete_material = true;
                client_status->do_user_choose_to_modify_material = false;
            } else if (query->data == "modify_material") {
                client_status->do_user_choose_to_modify_material = true;
                client_status->do_user_choose_to_delete_material = false;
            }

            client_status->current_menu = mChooseMaterialMenu;
            editCurrentMenu(query->message);
        } else if (StringTools::startsWith(query->data, CHOOSE_MATERIAL_PREFIX.data())) {
            std::string data_str(query->data);
            data_str.erase(0, CHOOSE_MATERIAL_PREFIX.length());

            client_status->updating_material.id = std::stoll(data_str);
            const auto material_id              = client_status->updating_material.id.value();
            client_status->updating_material
                = mDatabaseManager.getMaterialById(material_id).value();

            if (client_status->do_user_choose_to_modify_material) {
                client_status->do_user_update_material_count = true;
                const auto send_message                      = fmt::format(
                    "Введіть нову кількість для {}",
                    formMaterialInfoStr(client_status->updating_material));
                sendMessage(query->message, send_message);
            } else if (client_status->do_user_choose_to_delete_material) {
                if (mDatabaseManager.deleteMaterialById(material_id)) {
                    const auto send_message = fmt::format(
                        "{} видалено успішно",
                        formMaterialInfoStr(client_status->updating_material));
                    sendMessage(query->message, send_message);
                } else {
                    const auto send_message = fmt::format(
                        "На жаль не вдалося видалити {}. Зверніться до адміністратора!",
                        formMaterialInfoStr(client_status->updating_material));
                    sendMessage(query->message, send_message);
                }
                updateChooseMaterialMenu();
                sendCurrentMenu(query->message);
            } else if (client_status->do_user_choose_material_critical_amount_to_add) {
                sendMessage(query->message,
                            fmt::format("Введіть критичну кількість для {}",
                                        formMaterialInfoStr(client_status->updating_material)));
            } else if (client_status->do_user_choose_material_critical_amount_to_update) {
                client_status->do_user_type_material_critical_amount_to_update = true;
                const auto critical_amount = mDatabaseManager.getMaterialCriticalAmountByMaterialId(
                    material_id);
                sendMessage(query->message,
                            fmt::format("Введіть нову критичного кількість для {}, минула: {}",
                                        formMaterialInfoStr(client_status->updating_material),
                                        (*critical_amount).critical_amount));
            } else if (client_status->do_user_choose_material_critical_amount_to_delete) {
                const auto critical_amount = mDatabaseManager.getMaterialCriticalAmountByMaterialId(
                    material_id);
                if (mDatabaseManager.deleteMaterialCriticalAmountByMaterialId(material_id)) {
                    sendMessage(query->message,
                                fmt::format("Прибрано критичну кількість {} для {}",
                                            (*critical_amount).critical_amount,
                                            formMaterialInfoStr(client_status->updating_material)));
                } else {
                    sendMessage(query->message, ERROR_MESSAGE.data());
                }
                updateChooseMaterialCriticalAmountToUpdateDelete();
                sendCurrentMenu(query->message);
            }
        } else if (query->data == "modify_alarm_users") {
            updateChooseMaterialAlarmUserMenu();
            client_status->current_menu = mChooseMaterialAlarmUserMenu;
            editCurrentMenu(query->message);
        } else if (StringTools::startsWith(query->data, CHOOSE_MATERIAL_ALARM_USER_PREFIX.data())) {
            std::string data_str(query->data);
            data_str.erase(0, CHOOSE_MATERIAL_ALARM_USER_PREFIX.length());

            const auto switch_alarm_user_id = std::stoll(data_str);
            const auto material_alarm_users = mDatabaseManager.getMaterialAlarmUsers();
            const bool isAlarmOn            = std::any_of(material_alarm_users.begin(),
                                                          material_alarm_users.end(),
                                                          [switch_alarm_user_id](const auto& alarm_user) {
                                                   return (switch_alarm_user_id == alarm_user.id);
                                               });
            const auto users                = mDatabaseManager.getUsers();
            const auto found_user           = std::find_if(users.begin(), users.end(),
                                                           [switch_alarm_user_id](const auto& user) {
                                                     return (user.id == switch_alarm_user_id);
                                                 });
            bool is_operation_okay          = false;
            if (isAlarmOn) {
                if (mDatabaseManager.deleteMaterialAlarmUserById(switch_alarm_user_id)) {
                    sendMessage(query->message, fmt::format("{} прибрано із системи сповіщень",
                                                            formUserInfoStr(*found_user)));
                    is_operation_okay = true;
                }
            } else {
                if (mDatabaseManager.addMaterialAlarmUser(*found_user)) {
                    sendMessage(query->message, fmt::format("{} додано до системи сповіщень",
                                                            formUserInfoStr(*found_user)));
                    is_operation_okay = true;
                }
            }
            if (!is_operation_okay) {
                sendMessage(query->message, "На жаль сталася помилка. Спробуйте ще раз або "
                                            "зверніться до адміністартора");
            }
            updateChooseMaterialAlarmUserMenu();
            sendCurrentMenu(query->message);
        } else if (query->data == "materials_menu_material_critical_amount") {
            client_status->current_menu = mConfigureMaterialCriticalAmountMenu;
            editCurrentMenu(query->message);
        } else if (query->data == "configure_material_critical_amount_add"
                   || query->data == "configure_material_critical_amount_modify"
                   || query->data == "configure_material_critical_amount_delete") {
            if (query->data == "configure_material_critical_amount_add") {
                updateChooseMaterialCriticalAmountToAdd();
                client_status->current_menu = mChooseCriticalAmountMaterialMenuToAdd;
                client_status->do_user_choose_material_critical_amount_to_add = true;
            } else if (query->data == "configure_material_critical_amount_modify") {
                updateChooseMaterialCriticalAmountToUpdateDelete();
                client_status->current_menu = mChooseCriticalAmountMaterialMenuToUpdateDelete;
                client_status->do_user_choose_material_critical_amount_to_update = true;
            } else if (query->data == "configure_material_critical_amount_delete") {
                updateChooseMaterialCriticalAmountToUpdateDelete();
                client_status->current_menu = mChooseCriticalAmountMaterialMenuToUpdateDelete;
                client_status->do_user_choose_material_critical_amount_to_delete = true;
            }
            editCurrentMenu(query->message);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        sendMessage(query->message,
                    "На жаль сталося помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

bool BotManager::checkIfTelegramIdIsAdmin(std::int64_t telegram_id) {
    const auto all_admins  = mDatabaseManager.getAdmins();
    const auto found_admin = std::find_if(all_admins.begin(), all_admins.end(),
                                          [telegram_id](const UsersTable::UserRow& user_row) {
                                              return (user_row.telegram_id == telegram_id);
                                          });
    return (found_admin != all_admins.end());
}

bool BotManager::checkIfTelegramIdIsTattooArtist(std::int64_t telegram_id) {
    const auto all_tattoo_artists = mDatabaseManager.getTattooArtists();
    const auto found_admin = std::find_if(all_tattoo_artists.begin(), all_tattoo_artists.end(),
                                          [telegram_id](const UsersTable::UserRow& user_row) {
                                              return (user_row.telegram_id == telegram_id);
                                          });
    return (found_admin != all_tattoo_artists.end());
}

void BotManager::updateChooseMaterialMenu() {
    const size_t max_columns_size = 1;
    mChooseMaterialMenu->inlineKeyboard.clear();

    const auto materials = mDatabaseManager.getMaterials();
    size_t i             = 0;
    for (; i < materials.size() - materials.size() % max_columns_size; i += max_columns_size) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        for (size_t j = 0; j < max_columns_size; ++j) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            const auto material  = materials.at(i + j);
            button->text         = fmt::format("{}", formMaterialInfoStr(material));
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            row.push_back(button);
        }
        mChooseMaterialMenu->inlineKeyboard.push_back(row);
    }
    if (i < materials.size()) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> last_row;
        for (; i < materials.size(); ++i) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            auto material        = materials.at(i);
            button->text         = fmt::format("{}", formMaterialInfoStr(material));
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX, material.id.value());
            last_row.push_back(button);
        }
        mChooseMaterialMenu->inlineKeyboard.push_back(last_row);
    }

    mChooseMaterialMenu->inlineKeyboard.push_back({mBackButton});
}

void BotManager::updateChooseMaterialCriticalAmountToAdd() {
    mChooseCriticalAmountMaterialMenuToAdd->inlineKeyboard.clear();
    const auto materials = mDatabaseManager.getMaterials();
    for (const auto& material : materials) {
        const auto critical_amount_material
            = mDatabaseManager.getMaterialCriticalAmountByMaterialId(material.id.value());
        if (!critical_amount_material) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = formMaterialInfoStr(material);
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            mChooseCriticalAmountMaterialMenuToAdd->inlineKeyboard.push_back({button});
        }
    }
    mChooseCriticalAmountMaterialMenuToAdd->inlineKeyboard.push_back({mBackButton});
}

void BotManager::updateChooseMaterialCriticalAmountToUpdateDelete() {
    mChooseCriticalAmountMaterialMenuToUpdateDelete->inlineKeyboard.clear();
    const auto materials = mDatabaseManager.getMaterials();
    for (const auto& material : materials) {
        const auto critical_amount_material
            = mDatabaseManager.getMaterialCriticalAmountByMaterialId(material.id.value());
        if (critical_amount_material) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text = fmt::format("{} - критична кількість: {}", formMaterialInfoStr(material),
                                       (*critical_amount_material).critical_amount);
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            mChooseCriticalAmountMaterialMenuToUpdateDelete->inlineKeyboard.push_back({button});
        }
    }
    mChooseCriticalAmountMaterialMenuToUpdateDelete->inlineKeyboard.push_back({mBackButton});
}

void BotManager::updateChooseMaterialAlarmUserMenu() {
    mChooseMaterialAlarmUserMenu->inlineKeyboard.clear();

    const auto admins               = mDatabaseManager.getAdmins();
    const auto material_alarm_users = mDatabaseManager.getMaterialAlarmUsers();
    for (const auto& admin : admins) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        const bool isAlarmOn = std::any_of(material_alarm_users.begin(), material_alarm_users.end(),
                                           [&admin](const auto& alarm_user) {
                                               return (admin.id.value() == alarm_user.id.value());
                                           });
        button->text         = fmt::format("{} - {} сповіщення", formUserInfoStr(admin),
                                   (isAlarmOn) ? ("вимкнути") : ("ввімкнути"));
        button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_ALARM_USER_PREFIX.data(),
                                           admin.id.value());
        mChooseMaterialAlarmUserMenu->inlineKeyboard.push_back({button});
    }

    mChooseMaterialAlarmUserMenu->inlineKeyboard.push_back({mBackButton});
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

std::shared_ptr<ClientChatStatus>
BotManager::getClientChatStatus(const TgBot::Message::Ptr& message) {
    std::shared_ptr<ClientChatStatus> ret_value;
    const auto it = mClientChatStatuses.find(message->chat->id);
    if (it != mClientChatStatuses.end()) {
        ret_value = it->second;
    } else {
        std::shared_ptr<ClientChatStatus> created_ptr(new ClientChatStatus);
        created_ptr->current_menu = mMainMenu;
        mClientChatStatuses.insert({message->chat->id, created_ptr});
        ret_value = created_ptr;
    }

    return ret_value;
}

void BotManager::insertUserInTableIfNotExists(const TgBot::Message::Ptr& message) {
    const auto found_user = mDatabaseManager.getUserByTelegramId(message->from->id);
    if (!found_user) {
        mDatabaseManager.addUser(scrapUserDataFromMessage(message));
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

std::optional<std::string>
BotManager::getMenuMessage(const TgBot::InlineKeyboardMarkup::Ptr& menu) {
    static constexpr std::string_view choose_option_str("Виберіть пункт:");
    std::string ret_message;
    if (menu == mMainMenu) {
        ret_message = choose_option_str;
    } else if (menu == mMaterialsMenu) {
        ret_message.assign("Матеріали в салоні:\n");
        for (const auto& item : mDatabaseManager.getMaterials()) {
            ret_message.append(fmt::format(" - {}\n", formMaterialInfoStr(item)));
        }
        ret_message.append(choose_option_str);
    } else if (menu == mChooseMaterialMenu) {
        ret_message = choose_option_str;
    } else if (menu == mChooseMaterialAlarmUserMenu) {
        ret_message = "Виберіть адміна:";
    } else if (menu == mConfigureMaterialCriticalAmountMenu) {
        ret_message.assign("Поточні налаштування:\n");
        for (const auto& material_row : mDatabaseManager.getMaterials()) {
            ret_message.append(fmt::format(" - {} - ", formMaterialInfoStr(material_row)));
            const auto material_critical_amount
                = mDatabaseManager.getMaterialCriticalAmountByMaterialId(material_row.id.value());

            if (material_critical_amount) {
                ret_message.append(fmt::format("критична кількість: {}\n",
                                               material_critical_amount.value().critical_amount));
            } else {
                ret_message.append("критична кількість не налаштована\n");
            }
        }
    } else if (menu == mChooseCriticalAmountMaterialMenuToAdd) {
        ret_message = choose_option_str;
    } else if (menu == mChooseCriticalAmountMaterialMenuToUpdateDelete) {
        ret_message = choose_option_str;
    } else {
        SPDLOG_ERROR("Нема відповідного меню");
        return {};
    }
    return ret_message;
}

TgBot::InlineKeyboardMarkup::Ptr
BotManager::returnPreviousMenu(const TgBot::InlineKeyboardMarkup::Ptr& current_menu) {
    if (current_menu == mMainMenu) {
        SPDLOG_WARN("Prevoius of mMainMenu is mMainMenu");
        return mMainMenu;
    } else if (current_menu == mMaterialsMenu) {
        return mMainMenu;
    } else if (current_menu == mChooseMaterialMenu) {
        return mMaterialsMenu;
    } else if (current_menu == mChooseMaterialAlarmUserMenu) {
        return mMaterialsMenu;
    } else if (current_menu == mConfigureMaterialCriticalAmountMenu) {
        return mMaterialsMenu;
    } else if (current_menu == mChooseCriticalAmountMaterialMenu) {
        return mConfigureMaterialCriticalAmountMenu;
    } else if (current_menu == mChooseCriticalAmountMaterialMenuToAdd) {
        return mConfigureMaterialCriticalAmountMenu;
    } else if (current_menu == mChooseCriticalAmountMaterialMenuToUpdateDelete) {
        return mConfigureMaterialCriticalAmountMenu;
    } else {
        SPDLOG_ERROR("Not valid current menu");
        return mMainMenu;
    }
}
