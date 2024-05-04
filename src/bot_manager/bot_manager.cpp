#include "bot_manager.h"

#include <database_manager/database_manager_tools.h>
#include <felisss_logger/felisss_logger.h>

#include <algorithm>

TgBot::InlineKeyboardMarkup::Ptr BotManager::mAdminMainMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr BotManager::mMaterialsMenu(new TgBot::InlineKeyboardMarkup);
TgBot::InlineKeyboardMarkup::Ptr BotManager::mChooseMaterialMenu(new TgBot::InlineKeyboardMarkup);

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
    TgBot::InlineKeyboardButton::Ptr materials_button(new TgBot::InlineKeyboardButton);
    materials_button->text         = "Матеріали";
    materials_button->callbackData = "materials";
    mAdminMainMenu->inlineKeyboard.push_back({materials_button});

    TgBot::InlineKeyboardButton::Ptr materials_menu_add_material(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_delete_material(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_modify_material(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_back(new TgBot::InlineKeyboardButton);
    materials_menu_add_material->text            = "Додати матеріал";
    materials_menu_add_material->callbackData    = "add_material";
    materials_menu_delete_material->text         = "Видалити матеріал";
    materials_menu_delete_material->callbackData = "delete_material";
    materials_menu_modify_material->text = "Оновити кількість матеріалу";
    materials_menu_modify_material->callbackData = "modify_material";
    materials_menu_back->text                    = "<< Назад в меню";
    materials_menu_back->callbackData            = "main_menu";

    mMaterialsMenu->inlineKeyboard.push_back(
        {materials_menu_add_material, materials_menu_delete_material});
    mMaterialsMenu->inlineKeyboard.push_back({materials_menu_modify_material});
    mMaterialsMenu->inlineKeyboard.push_back({materials_menu_back});
}

void BotManager::callbackOnStartCommand(const TgBot::Message::Ptr& message) {
    if (!checkIfTelegramIdIsAdmin(message->from->id)) {
        mBotHandler.getApi().sendMessage(
            message->chat->id,
            "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
        return;
    }

    mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0, mAdminMainMenu,
                                     "Markdown");
}

void BotManager::callbackOnMakeMeAdminCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabaseManager.getUserByTelegramId(telegram_id);

    if (checkIfTelegramIdIsAdmin(telegram_id)) {
        mBotHandler.getApi().sendMessage(message->chat->id, "Ти вже адмін");
    } else {
        mDatabaseManager.addAdmin(user.value());
        mBotHandler.getApi().sendMessage(message->chat->id, "Тепер ти адмін");
    }
}

void BotManager::callbackOnMakeMeTattooArtistCommand(const TgBot::Message::Ptr& message) {
    const int64_t telegram_id = message->from->id;
    const auto user           = mDatabaseManager.getUserByTelegramId(telegram_id);

    if (checkIfTelegramIdIsTattooArtist(telegram_id)) {
        mBotHandler.getApi().sendMessage(message->chat->id, "Ти вже тату майстер");
    } else {
        mDatabaseManager.addTattooArtist(user.value());
        mBotHandler.getApi().sendMessage(message->chat->id, "Тепер ти тату майстер");
    }
}

void BotManager::callbackOnAnyMessage(const TgBot::Message::Ptr& message) {
    try {
        insertUserInTableIfNotExists(message);

        spdlog::info("\"{}\" wrote \"{}\"", message->chat->username, message->text);

        if (StringTools::startsWith(message->text, "/")) {
            return;
        }

        if (!checkIfTelegramIdIsAdmin(message->from->id)) {
            mBotHandler.getApi().sendMessage(
                message->chat->id,
                "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        auto client_status = getClientChatStatus(message->from->id);

        if (client_status->do_user_type_material_name) {
            client_status->do_user_type_material_name = false;
            if (!DatabaseManagerTools::validateUserInput(message->text)) {
                mBotHandler.getApi().sendMessage(
                    message->chat->id, "На жаль сталась помилка. Зверніться до адміністратора");
                mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                                 mMaterialsMenu, "Markdown");
                return;
            }
            const auto find_result = mDatabaseManager.getMaterialByName(message->text);
            if (find_result) {
                const auto send_str = fmt::format("Наразі {} {}{} вже в базі даних",
                                                  find_result->name, find_result->count,
                                                  find_result->suffix.value_or(""));
                mBotHandler.getApi().sendMessage(message->chat->id, send_str);
                mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                                 mMaterialsMenu, "Markdown");
                return;
            }

            client_status->do_user_specify_material_suffix = true;
            client_status->updating_material.name          = message->text;

            static TgBot::ReplyKeyboardMarkup::Ptr material_suffix_reply_keyboard(
                new TgBot::ReplyKeyboardMarkup);
            static bool isFirstTime = true;
            if (isFirstTime) {
                isFirstTime = false;
                TgBot::KeyboardButton::Ptr material_suffix_no_suffix_button(
                    new TgBot::KeyboardButton);
                material_suffix_no_suffix_button->text = "Матеріал не має одиниці виміру";
                material_suffix_no_suffix_button->requestContact  = false;
                material_suffix_no_suffix_button->requestLocation = false;
                material_suffix_reply_keyboard->keyboard.push_back(
                    {material_suffix_no_suffix_button});
            }

            mBotHandler.getApi().sendMessage(message->chat->id,
                                             "В чому вимірюється одиниця матеріалу?", false, 0,
                                             material_suffix_reply_keyboard);
        } else if (client_status->do_user_specify_material_suffix) {
            client_status->do_user_specify_material_suffix = false;
            client_status->do_user_specify_material_count  = true;

            if (message->text != "Матеріал не має одиниці виміру") {
                client_status->updating_material.suffix = message->text;
            }
            mBotHandler.getApi().sendMessage(message->chat->id,
                                             "Скільки зараз одиниць матеріалу в салоні?");
        } else if (client_status->do_user_specify_material_count) {
            client_status->do_user_specify_material_count = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                mBotHandler.getApi().sendMessage(
                    message->chat->id, "На жаль сталась помилка. Зверніться до адміністратора");
                mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                                 mMaterialsMenu, "Markdown");
                SPDLOG_ERROR("{}", e.what());
                return;
            }
            if (mDatabaseManager.addMaterial(client_status->updating_material)) {
                const auto send_str = fmt::format(
                    "{} {}{} внесено в базу даних", client_status->updating_material.name,
                    client_status->updating_material.count,
                    client_status->updating_material.suffix.value_or(""));
                mBotHandler.getApi().sendMessage(message->chat->id, send_str);
            } else {
                mBotHandler.getApi().sendMessage(
                    message->chat->id, "На жаль сталась помилка. Зверніться до адміністратора");
            }
            mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                             mMaterialsMenu, "Markdown");
        } else if (client_status->do_user_update_material_count) {
            client_status->do_user_update_material_count = false;
            try {
                client_status->updating_material.count = std::stod(message->text);
            } catch (const std::exception& e) {
                mBotHandler.getApi().sendMessage(
                    message->chat->id, "На жаль сталась помилка. Зверніться до адміністратора");
                SPDLOG_ERROR("{}", e.what());
                mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                                 mMaterialsMenu, "Markdown");
                return;
            }

            if (mDatabaseManager.updateMaterialCountById(
                    client_status->updating_material.id.value(),
                    client_status->updating_material)) {

                const auto send_str = fmt::format(
                    "{} {}{} оновлено в базі даних", client_status->updating_material.name,
                    client_status->updating_material.count,
                    client_status->updating_material.suffix.value_or(""));
                mBotHandler.getApi().sendMessage(message->chat->id, send_str);
            } else {
                mBotHandler.getApi().sendMessage(
                    message->chat->id, "На жаль сталась помилка. Зверніться до адміністратора");
            }

            mBotHandler.getApi().sendMessage(message->chat->id, "Виберіть пункт:", false, 0,
                                             mMaterialsMenu, "Markdown");
        } else {
            mBotHandler.getApi().sendMessage(message->chat->id, "Невідомий статус чату");
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        mBotHandler.getApi().sendMessage(
            message->chat->id,
            "На жаль сталося помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

void BotManager::callbackOnCallbackQuery(const TgBot::CallbackQuery::Ptr& query) {
    try {
        spdlog::info("\"{}\" applied callback with data \"{}\"", query->message->chat->username,
                     query->data);
        if (!checkIfTelegramIdIsAdmin(query->from->id)) {
            mBotHandler.getApi().sendMessage(
                query->from->id,
                "На жаль ви не маєте доступа до цього бота. Зверніться до адміністратора");
            return;
        }

        auto client_status = getClientChatStatus(query->from->id);
        static constexpr std::string_view material_prefix("material_id_");
        if (query->data == "main_menu") {
            mBotHandler.getApi().editMessageText("Виберіть пункт:", query->message->chat->id,
                                                 query->message->messageId, "", "Markdown", false,
                                                 mAdminMainMenu);
        } else if (query->data == "materials") {
            mBotHandler.getApi().editMessageText("Виберіть пункт:", query->message->chat->id,
                                                 query->message->messageId, "", "Markdown", false,
                                                 mMaterialsMenu);
        } else if (query->data == "add_material") {
            mBotHandler.getApi().sendMessage(query->message->chat->id, "Як називається матеріал?");
            client_status->do_user_type_material_name = true;
        } else if (query->data == "delete_material" || query->data == "modify_material") {
            const size_t max_columns_size = 3;
            mChooseMaterialMenu->inlineKeyboard.clear();

            const auto materials = mDatabaseManager.getMaterials();
            size_t i             = 0;
            for (; i < materials.size() - materials.size() % max_columns_size;
                 i += max_columns_size) {
                std::vector<TgBot::InlineKeyboardButton::Ptr> row;
                for (size_t j = 0; j < max_columns_size; ++j) {
                    TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
                    const auto material  = materials.at(i + j);
                    button->text         = fmt::format("{} {}{}", material.name, material.count,
                                                       material.suffix.value_or(""));
                    button->callbackData = fmt::format("material_id_{}", material.id.value());
                    row.push_back(button);
                }
                mChooseMaterialMenu->inlineKeyboard.push_back(row);
            }
            if (i < materials.size()) {
                std::vector<TgBot::InlineKeyboardButton::Ptr> last_row;
                for (; i < materials.size(); ++i) {
                    TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
                    auto material        = materials.at(i);
                    button->text         = fmt::format("{} {}{}", material.name, material.count,
                                                       material.suffix.value_or(""));
                    button->callbackData = fmt::format("{}{}", material_prefix,
                                                       material.id.value());
                    last_row.push_back(button);
                }
                mChooseMaterialMenu->inlineKeyboard.push_back(last_row);
            }

            TgBot::InlineKeyboardButton::Ptr back_button(new TgBot::InlineKeyboardButton);
            back_button->text         = "<< Назад в меню";
            back_button->callbackData = "materials";
            mChooseMaterialMenu->inlineKeyboard.push_back({back_button});

            if (query->data == "delete_material") {
                client_status->do_user_choose_to_delete_material = true;
                client_status->do_user_choose_to_modify_material = false;
            } else if (query->data == "modify_material") {
                client_status->do_user_choose_to_modify_material = true;
                client_status->do_user_choose_to_delete_material = false;
            }

            mBotHandler.getApi().editMessageText("Виберіть пункт:", query->message->chat->id,
                                                 query->message->messageId, "", "Markdown", false,
                                                 mChooseMaterialMenu);
        } else if (StringTools::startsWith(query->data, material_prefix.data())) {
            std::string data_str(query->data);
            data_str.erase(0, material_prefix.length());

            client_status->updating_material.id = std::stoll(data_str);

            if (client_status->do_user_choose_to_modify_material) {
                client_status->do_user_update_material_count = true;

                client_status->updating_material
                    = mDatabaseManager.getMaterialById(client_status->updating_material.id.value())
                          .value();
                mBotHandler.getApi().sendMessage(
                    query->message->chat->id,
                    fmt::format("Введіть нову кількість для {} {}{}",
                                client_status->updating_material.name,
                                client_status->updating_material.count,
                                client_status->updating_material.suffix.value_or("")));
            } else if (client_status->do_user_choose_to_delete_material) {
                if (mDatabaseManager.deleteMaterialById(
                        client_status->updating_material.id.value())) {
                    mBotHandler.getApi().sendMessage(
                        query->message->chat->id,
                        fmt::format("{} {}{} видалено успішно",
                                    client_status->updating_material.name,
                                    client_status->updating_material.count,
                                    client_status->updating_material.suffix.value_or("")));
                } else {
                    mBotHandler.getApi().sendMessage(
                        query->message->chat->id,
                        fmt::format(
                            "На жаль не вдалося видалити {} {}{}. Зверніться до адміністратора!",
                            client_status->updating_material.name,
                            client_status->updating_material.count,
                            client_status->updating_material.suffix.value_or("")));
                }
                mBotHandler.getApi().sendMessage(query->message->chat->id, "Виберіть пункт:", false,
                                                 0, mMaterialsMenu, "Markdown");
            }
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("{}", e.what());
        mBotHandler.getApi().sendMessage(
            query->message->chat->id,
            "На жаль сталося помилка. Спробуйте ще раз або зверніться до адміністратора");
    }
}

std::shared_ptr<ClientChatStatus> BotManager::getClientChatStatus(int64_t client_id) {
    std::shared_ptr<ClientChatStatus> ret_value;
    const auto it = mClientChatStatuses.find(client_id);
    if (it != mClientChatStatuses.end()) {
        ret_value = it->second;
    } else {
        std::shared_ptr<ClientChatStatus> created_ptr(new ClientChatStatus);
        mClientChatStatuses.insert({client_id, created_ptr});
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
