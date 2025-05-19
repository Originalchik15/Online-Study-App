#pragma once

#include <string>
#include <optional>
#include "UserRepository.hpp"
#include "User.hpp"

class AuthenticationService {
public:
    // Конструктор — только объявление
    explicit AuthenticationService(UserRepository& repo);

    // Возвращает авторизованного User или выбрасывает исключение
    User login(const std::string& username,
        const std::string& password);

private:
    UserRepository& repo_;

    // Хэширование пароля
    std::string hashPassword(const std::string& password) const;
};
