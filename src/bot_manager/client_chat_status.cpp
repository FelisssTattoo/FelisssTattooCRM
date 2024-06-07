#include "client_chat_status.h"

#include "bot_manager.h"
#include "consts.h"

#include <felisss_logger/felisss_logger.h>

TgBot::InlineKeyboardButton::Ptr ClientChatStatus::mBackButton(new TgBot::InlineKeyboardButton);

ClientChatStatus::ClientChatStatus(std::int64_t telegram_id,
                                   std::weak_ptr<DatabaseManager> database) :
    mMenu(new TgBot::InlineKeyboardMarkup),
    mTelegramId(telegram_id),
    mDatabase(std::move(database)) {
    mBackButton->text         = "<< Назад в меню";
    mBackButton->callbackData = "back_button";

    setMenu(Menus::MAIN_MENU);
}

void ClientChatStatus::clearAllProperties() {
    session_row = {};
    user_row    = {};

    mChatStatuses.reset();
}

void ClientChatStatus::setMenu(Menus menu) {
    mCurrentMenu = menu;
}

TgBot::InlineKeyboardMarkup::Ptr ClientChatStatus::getMenu() {
    updateCurrentMenu();
    return mMenu;
}

std::string ClientChatStatus::getMenuMessage() {
    static constexpr std::string_view choose_option_str("Виберіть пункт:");
    std::string ret_message;
    if (mCurrentMenu == Menus::MAIN_MENU) {
        ret_message.assign(choose_option_str);
    } else if (mCurrentMenu == Menus::MATERIALS_MENU) {
        ret_message.assign("Матеріали в салоні:\n");
        for (const auto& item : mDatabase.lock()->getMaterials()) {
            ret_message.append(fmt::format(" - {}\n", BotManager::formMaterialInfoStr(item)));
        }
        ret_message.append(choose_option_str);
    } else if (mCurrentMenu == Menus::SESSIONS_MENU) {
        const auto sessions = mDatabase.lock()->getSessionsInFuture();
        if (sessions.size()) {
            ret_message.assign("Сеанси:\n");
            for (const auto& session : sessions) {
                const auto tattoo_artist = *mDatabase.lock()->getUserById(session.tattoo_artist_id);
                std::optional<UsersTable::UserRow> customer;
                if (session.user_id) {
                    customer = *mDatabase.lock()->getUserById(*session.user_id);
                }
                ret_message.append(fmt::format(
                    " - {}\n", BotManager::formSessionInfoStr(session, tattoo_artist, customer)));
            }
        }
        ret_message.append(choose_option_str);
    } else if (mCurrentMenu == Menus::MATERIALS_UPDATE_MATERIAL_MENU) {
        ret_message.assign("Виберіть матеріал на оновлення:");
    } else if (mCurrentMenu == Menus::MATERIALS_DELETE_MATERIAL_MENU) {
        ret_message.assign("Виберіть матеріал на видалення:");
    } else if (mCurrentMenu == Menus::MATERIALS_CHOOSE_USER_CRITICAL_ALARM_MENU) {
        ret_message.assign("Виберіть користувача, щоб перемкнути сповіщення:");
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU) {
        ret_message.assign("Поточні налаштування:\n");
        for (const auto& material_row : mDatabase.lock()->getMaterials()) {
            ret_message.append(
                fmt::format(" - {} - ", BotManager::formMaterialInfoStr(material_row)));
            const auto material_critical_amount
                = mDatabase.lock()->getMaterialCriticalAmountByMaterialId(material_row.id.value());

            if (material_critical_amount) {
                ret_message.append(fmt::format("критична кількість: {}\n",
                                               material_critical_amount.value().critical_amount));
            } else {
                ret_message.append("критична кількість не налаштована\n");
            }
        }
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_ADD) {
        ret_message.assign("Виберіть матеріал, щоб додати критичну кількість:");
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_DELETE) {
        ret_message.assign("Виберіть матеріал, щоб прибрати критичну кількість:");
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_UPDATE) {
        ret_message.assign("Виберіть матеріал, щоб оновити критичну кількість:");
    } else if (mCurrentMenu == Menus::SESSIONS_MENU) {
        const auto sessions = mDatabase.lock()->getSessionsInFuture();
        if (sessions.size()) {
            ret_message.assign("Сеанси:\n");
            for (const auto& session : sessions) {
                const auto tattoo_artist = *mDatabase.lock()->getUserById(session.tattoo_artist_id);
                std::optional<UsersTable::UserRow> customer;
                if (session.user_id) {
                    customer = *mDatabase.lock()->getUserById(*session.user_id);
                }
                ret_message.append(fmt::format(
                    " - {}\n", BotManager::formSessionInfoStr(session, tattoo_artist, customer)));
            }
        }
        ret_message.append(choose_option_str);
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_TATTOO_ARTIST) {
        ret_message.assign("Виберіть тату майстра на сеанс:");
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_CUSTOMER) {
        ret_message.assign("Виберіть клієнта на сеанс(можна пропустити):");
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_DELETE_SESSION) {
        ret_message.assign("Виберіть сеанс, щоб видалити:");
    } else if (mCurrentMenu == Menus::USERS_MENU) {
        const auto users = mDatabase.lock()->getUsers();
        if (users.size()) {
            ret_message.assign("Користувачі в системі:\n");
            for (const auto& user : users) {
                ret_message.append(fmt::format("- {}\n", BotManager::formUserInfoStr(user)));
            }
        }
        ret_message.append(choose_option_str);
    } else if (mCurrentMenu == Menus::USERS_MENU_DELETE_USER) {
        ret_message.assign("Виберіть користувача, щоб видалити:");
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU) {
        const auto& tattoo_artists = mDatabase.lock()->getTattooArtists();
        if (tattoo_artists.size()) {
            ret_message += "Тату майстри:\n";
            for (const auto& tattoo_artist : tattoo_artists) {
                ret_message += fmt::format(" - {}\n", BotManager::formUserInfoStr(tattoo_artist));
            }
        }
        const auto& admins = mDatabase.lock()->getAdmins();
        if (admins.size()) {
            ret_message += "Адміни:\n";
            for (const auto& admin : admins) {
                ret_message += fmt::format(" - {}\n", BotManager::formUserInfoStr(admin));
            }
        }
        ret_message += choose_option_str;
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_TATTOO_ARTIST) {
        ret_message.assign("Виберіть користувача, щоб зробити його тату майстром:");
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_TATTOO_ARTIST) {
        ret_message.assign("Виберіть користувача, щоб прибрати його з тату майстрів:");
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_ADMIN) {
        ret_message.assign("Виберіть користувача, щоб зробити його адміном:");
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_ADMIN) {
        ret_message.assign("Виберіть користувача, щоб прибрати його з адмінів:");
    } else {
        SPDLOG_ERROR("No text for menu {}", static_cast<int>(mCurrentMenu));
        ret_message.assign("Тут має бути текст меню!!!");
    }

    return ret_message;
}

void ClientChatStatus::returnToPreviousMenu() {
    Menus menu = Menus::MAIN_MENU;

    if (mCurrentMenu == Menus::MATERIALS_UPDATE_MATERIAL_MENU
        || mCurrentMenu == Menus::MATERIALS_DELETE_MATERIAL_MENU
        || mCurrentMenu == Menus::MATERIALS_CHOOSE_USER_CRITICAL_ALARM_MENU
        || mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU) {
        menu = Menus::MATERIALS_MENU;
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_ADD
               || mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_DELETE
               || mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_UPDATE) {
        menu = Menus::CRITICAL_MATERIALS_MENU;
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_TATTOO_ARTIST
               || mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_CUSTOMER
               || mCurrentMenu == Menus::SESSIONS_MENU_DELETE_SESSION) {
        menu = Menus::SESSIONS_MENU;
    } else if (mCurrentMenu == Menus::USERS_MENU_DELETE_USER) {
        menu = Menus::USERS_MENU;
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_TATTOO_ARTIST
               || mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_TATTOO_ARTIST
               || mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_ADMIN
               || mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_ADMIN) {
        menu = Menus::USER_RIGHTS_MENU;
    }

    setMenu(menu);
}

std::bitset<static_cast<int>(ChatStatuses::CHAT_STATUSES_SIZE)>::reference
ClientChatStatus::operator[](ChatStatuses chat_status_flag) {
    return mChatStatuses[static_cast<int>(chat_status_flag)];
}

std::bitset<static_cast<int>(ChatStatuses::CHAT_STATUSES_SIZE)>::reference
ClientChatStatus::at(ChatStatuses chat_status_flag) {
    return (*this)[chat_status_flag];
}

void ClientChatStatus::updateCurrentMenu() {
    if (mCurrentMenu == Menus::MAIN_MENU) {
        setMainMenu();
    } else if (mCurrentMenu == Menus::MATERIALS_MENU) {
        setMaterialsMenu();
    } else if (mCurrentMenu == Menus::MATERIALS_DELETE_MATERIAL_MENU) {
        setMaterialsDeleteMaterialMenu();
    } else if (mCurrentMenu == Menus::MATERIALS_UPDATE_MATERIAL_MENU) {
        setMaterialsUpdateMaterialMenu();
    } else if (mCurrentMenu == Menus::MATERIALS_CHOOSE_USER_CRITICAL_ALARM_MENU) {
        setMaterialsChooseUserCriticalAlarmMenu();
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU) {
        setCriticalMaterialsMenu();
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_ADD) {
        setCriticalMaterialsAddMenu();
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_DELETE) {
        setCriticalMaterialsDeleteMenu();
    } else if (mCurrentMenu == Menus::CRITICAL_MATERIALS_MENU_UPDATE) {
        setCriticalMaterialsUpdateMenu();
    } else if (mCurrentMenu == Menus::SESSIONS_MENU) {
        setSessionsMenu();
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_TATTOO_ARTIST) {
        setSessionsAddSessionChooseTattooArtistMenu();
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_ADD_SESSION_CHOOSE_CUSTOMER) {
        setSessionsAddSessionChooseCustomerMenu();
    } else if (mCurrentMenu == Menus::SESSIONS_MENU_DELETE_SESSION) {
        setSessionsDeleteSessionMenu();
    } else if (mCurrentMenu == Menus::USERS_MENU) {
        setUsersMenu();
    } else if (mCurrentMenu == Menus::USERS_MENU_DELETE_USER) {
        setUsersDeleteUserMenu();
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU) {
        setUserRightsMenu();
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_TATTOO_ARTIST) {
        setUserRightsAddTattooArtistMenu();
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_TATTOO_ARTIST) {
        setUserRightsDeleteTattooArtistMenu();
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_ADD_ADMIN) {
        setUserRightsAddAdminMenu();
    } else if (mCurrentMenu == Menus::USER_RIGHTS_MENU_DELETE_ADMIN) {
        setUserRightsDeleteAdminMenu();
    } else {
        SPDLOG_ERROR("Menu is not supported: enum number {}", static_cast<int>(mCurrentMenu));
    }
}

void ClientChatStatus::setMainMenu() {
    mMenu->inlineKeyboard.clear();

    TgBot::InlineKeyboardButton::Ptr materials_button(new TgBot::InlineKeyboardButton);
    materials_button->text         = "Матеріали";
    materials_button->callbackData = "materials";
    TgBot::InlineKeyboardButton::Ptr sessions_button(new TgBot::InlineKeyboardButton);
    sessions_button->text         = "Сеанси";
    sessions_button->callbackData = "sessions";
    TgBot::InlineKeyboardButton::Ptr users_button(new TgBot::InlineKeyboardButton);
    users_button->text         = "Користувачі";
    users_button->callbackData = "users";

    mMenu->inlineKeyboard.push_back({materials_button});
    mMenu->inlineKeyboard.push_back({sessions_button});
    mMenu->inlineKeyboard.push_back({users_button});

    if (mDatabase.lock()->checkIfTelegramIdIsAdmin(mTelegramId)) {
        TgBot::InlineKeyboardButton::Ptr users_rights_button(new TgBot::InlineKeyboardButton);
        users_rights_button->text         = "Налаштування доступу";
        users_rights_button->callbackData = "users_rights";
        mMenu->inlineKeyboard.push_back({users_rights_button});
    }
}

void ClientChatStatus::setMaterialsMenu() {
    mMenu->inlineKeyboard.clear();

    TgBot::InlineKeyboardButton::Ptr materials_menu_add_material(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr MATERIALS_DELETE_MATERIAL_MENU(
        new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr materials_menu_modify_material(
        new TgBot::InlineKeyboardButton);

    materials_menu_add_material->text            = "Додати матеріал";
    materials_menu_add_material->callbackData    = "add_material";
    MATERIALS_DELETE_MATERIAL_MENU->text         = "Видалити матеріал";
    MATERIALS_DELETE_MATERIAL_MENU->callbackData = "delete_material";
    materials_menu_modify_material->text = "Оновити кількість матеріалу";
    materials_menu_modify_material->callbackData = "modify_material";

    mMenu->inlineKeyboard.push_back({materials_menu_add_material, MATERIALS_DELETE_MATERIAL_MENU});
    mMenu->inlineKeyboard.push_back({materials_menu_modify_material});

    if (mDatabase.lock()->checkIfTelegramIdIsAdmin(mTelegramId)) {
        TgBot::InlineKeyboardButton::Ptr materials_menu_modify_alarm_users(
            new TgBot::InlineKeyboardButton);
        TgBot::InlineKeyboardButton::Ptr materials_menu_material_critical_amount(
            new TgBot::InlineKeyboardButton);

        materials_menu_modify_alarm_users->text = "Налаштування користувачів із сповіщеннями";
        materials_menu_modify_alarm_users->callbackData = "modify_alarm_users";
        materials_menu_material_critical_amount->text
            = "Налаштування критичної кількості для матеріалів";
        materials_menu_material_critical_amount->callbackData
            = "materials_menu_material_critical_amount";

        mMenu->inlineKeyboard.push_back({materials_menu_modify_alarm_users});
        mMenu->inlineKeyboard.push_back({materials_menu_material_critical_amount});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setMaterialsDeleteMaterialMenu() {
    mMenu->inlineKeyboard.clear();

    const auto materials = mDatabase.lock()->getMaterials();
    for (const auto& material : materials) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = fmt::format("{}", BotManager::formMaterialInfoStr(material));
        button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                           material.id.value());
        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setMaterialsUpdateMaterialMenu() {
    mMenu->inlineKeyboard.clear();

    const auto materials = mDatabase.lock()->getMaterials();
    for (const auto& material : materials) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = fmt::format("{}", BotManager::formMaterialInfoStr(material));
        button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                           material.id.value());
        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setMaterialsChooseUserCriticalAlarmMenu() {
    mMenu->inlineKeyboard.clear();

    const auto admins               = mDatabase.lock()->getAdmins();
    const auto material_alarm_users = mDatabase.lock()->getMaterialAlarmUsers();
    for (const auto& admin : admins) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        const bool isAlarmOn = std::any_of(material_alarm_users.begin(), material_alarm_users.end(),
                                           [&admin](const auto& alarm_user) {
                                               return (admin.id.value() == alarm_user.id.value());
                                           });
        button->text = fmt::format("{} - {} сповіщення", BotManager::formUserInfoStr(admin),
                                   (isAlarmOn) ? ("вимкнути") : ("ввімкнути"));
        button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_ALARM_USER_PREFIX.data(),
                                           admin.id.value());
        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setCriticalMaterialsMenu() {
    mMenu->inlineKeyboard.clear();

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
    mMenu->inlineKeyboard.push_back({configure_material_critical_amount_add});
    mMenu->inlineKeyboard.push_back({configure_material_critical_amount_delete});
    mMenu->inlineKeyboard.push_back({configure_material_critical_amount_modify});
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setCriticalMaterialsAddMenu() {
    mMenu->inlineKeyboard.clear();
    const auto materials = mDatabase.lock()->getMaterials();
    for (const auto& material : materials) {
        const auto critical_amount_material
            = mDatabase.lock()->getMaterialCriticalAmountByMaterialId(material.id.value());
        if (!critical_amount_material) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = BotManager::formMaterialInfoStr(material);
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            mMenu->inlineKeyboard.push_back({button});
        }
    }
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setCriticalMaterialsDeleteMenu() {
    mMenu->inlineKeyboard.clear();
    const auto materials = mDatabase.lock()->getMaterials();
    for (const auto& material : materials) {
        const auto critical_amount_material
            = mDatabase.lock()->getMaterialCriticalAmountByMaterialId(material.id.value());
        if (critical_amount_material) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = fmt::format("{} - критична кількість: {}",
                                               BotManager::formMaterialInfoStr(material),
                                               (*critical_amount_material).critical_amount);
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            mMenu->inlineKeyboard.push_back({button});
        }
    }
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setCriticalMaterialsUpdateMenu() {
    mMenu->inlineKeyboard.clear();
    const auto materials = mDatabase.lock()->getMaterials();
    for (const auto& material : materials) {
        const auto critical_amount_material
            = mDatabase.lock()->getMaterialCriticalAmountByMaterialId(material.id.value());
        if (critical_amount_material) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = fmt::format("{} - критична кількість: {}",
                                               BotManager::formMaterialInfoStr(material),
                                               (*critical_amount_material).critical_amount);
            button->callbackData = fmt::format("{}{}", CHOOSE_MATERIAL_PREFIX.data(),
                                               material.id.value());
            mMenu->inlineKeyboard.push_back({button});
        }
    }
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setSessionsMenu() {
    mMenu->inlineKeyboard.clear();

    TgBot::InlineKeyboardButton::Ptr add_session_bt(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr delete_session_bt(new TgBot::InlineKeyboardButton);
    add_session_bt->text            = "Додати сеанс";
    add_session_bt->callbackData    = "add_session";
    delete_session_bt->text         = "Скасувати сеанс";
    delete_session_bt->callbackData = "delete_session";

    mMenu->inlineKeyboard.push_back({add_session_bt, delete_session_bt});
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setSessionsAddSessionChooseTattooArtistMenu() {
    mMenu->inlineKeyboard.clear();

    const auto tattoo_artists = mDatabase.lock()->getTattooArtists();
    for (const auto& tattoo_artist : tattoo_artists) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = BotManager::formUserInfoStr(tattoo_artist);
        button->callbackData = fmt::format("{}{}", CHOOSE_TATTOO_ARTIST_PREFIX.data(),
                                           tattoo_artist.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setSessionsAddSessionChooseCustomerMenu() {
    mMenu->inlineKeyboard.clear();
    TgBot::InlineKeyboardButton::Ptr no_user_bt(new TgBot::InlineKeyboardButton);
    no_user_bt->text         = "Без користувача";
    no_user_bt->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), "no");
    mMenu->inlineKeyboard.push_back({no_user_bt});

    const auto users = mDatabase.lock()->getUsers();
    for (const auto& user : users) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = BotManager::formUserInfoStr(user);
        button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), user.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setSessionsDeleteSessionMenu() {
    mMenu->inlineKeyboard.clear();

    const auto sessions = mDatabase.lock()->getSessionsInFuture();
    for (const auto& session : sessions) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        const auto tattoo_artist = *mDatabase.lock()->getUserById(session.tattoo_artist_id);
        std::optional<UsersTable::UserRow> customer;
        if (session.user_id) {
            customer = mDatabase.lock()->getUserById(*session.user_id);
        }
        button->text         = BotManager::formSessionInfoStr(session, tattoo_artist, customer);
        button->callbackData = fmt::format("{}{}", CHOOSE_SESSION_PREFIX.data(),
                                           session.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUsersMenu() {
    mMenu->inlineKeyboard.clear();

    TgBot::InlineKeyboardButton::Ptr add_user_bt(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr delete_user_bt(new TgBot::InlineKeyboardButton);
    add_user_bt->text            = "Додати користувача";
    add_user_bt->callbackData    = "add_user";
    delete_user_bt->text         = "Видалити користувача";
    delete_user_bt->callbackData = "delete_user";
    mMenu->inlineKeyboard.push_back({add_user_bt, delete_user_bt});
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUsersDeleteUserMenu() {
    mMenu->inlineKeyboard.clear();

    const auto users = mDatabase.lock()->getUsers();
    for (const auto& user : users) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = BotManager::formUserInfoStr(user);
        button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), user.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUserRightsMenu() {
    mMenu->inlineKeyboard.clear();

    TgBot::InlineKeyboardButton::Ptr user_rights_tattoo_artist_add(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr user_rights_tattoo_artist_delete(
        new TgBot::InlineKeyboardButton);
    user_rights_tattoo_artist_add->text            = "Додати тату майстра";
    user_rights_tattoo_artist_add->callbackData    = "user_rights_tattoo_artist_add";
    user_rights_tattoo_artist_delete->text         = "Прибрати тату майстра";
    user_rights_tattoo_artist_delete->callbackData = "user_rights_tattoo_artist_delete";
    TgBot::InlineKeyboardButton::Ptr user_rights_admin_add(new TgBot::InlineKeyboardButton);
    TgBot::InlineKeyboardButton::Ptr user_rights_admin_delete(new TgBot::InlineKeyboardButton);
    user_rights_admin_add->text            = "Додати Адміна (Обережно)";
    user_rights_admin_add->callbackData    = "user_rights_admin_add";
    user_rights_admin_delete->text         = "Прибрати Адміна";
    user_rights_admin_delete->callbackData = "user_rights_admin_delete";
    mMenu->inlineKeyboard.push_back(
        {user_rights_tattoo_artist_add, user_rights_tattoo_artist_delete});
    mMenu->inlineKeyboard.push_back({user_rights_admin_add, user_rights_admin_delete});
    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUserRightsAddTattooArtistMenu() {
    mMenu->inlineKeyboard.clear();

    const auto users          = mDatabase.lock()->getUsers();
    const auto tattoo_artists = mDatabase.lock()->getTattooArtists();
    for (const auto& user : users) {
        const auto found_tattoo_artist = std::find_if(tattoo_artists.begin(), tattoo_artists.end(),
                                                      [&user](const auto& tattoo_artist) {
                                                          return (user.id == tattoo_artist.id);
                                                      });
        if (found_tattoo_artist == tattoo_artists.end()) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = BotManager::formUserInfoStr(user);
            button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), user.id.value());
            mMenu->inlineKeyboard.push_back({button});
        }
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUserRightsDeleteTattooArtistMenu() {
    mMenu->inlineKeyboard.clear();

    const auto tattoo_artists = mDatabase.lock()->getTattooArtists();
    for (const auto& tattoo_artist : tattoo_artists) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = BotManager::formUserInfoStr(tattoo_artist);
        button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(),
                                           tattoo_artist.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUserRightsAddAdminMenu() {
    mMenu->inlineKeyboard.clear();

    const auto users  = mDatabase.lock()->getUsers();
    const auto admins = mDatabase.lock()->getAdmins();
    for (const auto& user : users) {
        const auto found_admin = std::find_if(admins.begin(), admins.end(),
                                              [&user](const auto& admin) {
                                                  return (user.id == admin.id);
                                              });
        if (found_admin == admins.end()) {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
            button->text         = BotManager::formUserInfoStr(user);
            button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), user.id.value());
            mMenu->inlineKeyboard.push_back({button});
        }
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}

void ClientChatStatus::setUserRightsDeleteAdminMenu() {
    mMenu->inlineKeyboard.clear();

    const auto admins = mDatabase.lock()->getAdmins();
    for (const auto& admin : admins) {
        TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);
        button->text         = BotManager::formUserInfoStr(admin);
        button->callbackData = fmt::format("{}{}", CHOOSE_USER_PREFIX.data(), admin.id.value());

        mMenu->inlineKeyboard.push_back({button});
    }

    mMenu->inlineKeyboard.push_back({mBackButton});
}
