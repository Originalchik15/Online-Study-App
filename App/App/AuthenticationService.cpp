#include "AuthenticationService.hpp"
#include <stdexcept>

// Конструктор
AuthenticationService::AuthenticationService(UserRepository& repo)
    : repo_(repo) {
}

// Простое “хэширование” (при желании замените на реальную SHA/BCrypt)
std::string AuthenticationService::hashPassword(const std::string& password) const {
    // для примера просто возвращаем пароль
    return password;
}

User AuthenticationService::login(const std::string& username,
    const std::string& password) {
    // Ищем пользователя в репозитории
    auto optUser = repo_.findByUsername(username);
    if (!optUser) {
        throw std::runtime_error("User not found: " + username);
    }

    // Предположим, что UserRepository умеет возвращать хэш
    // (дополнительно нужно добавить метод в UserRepository и PgUserRepository):
    std::string storedHash = repo_.getPasswordHash(username);
    if (hashPassword(password) != storedHash) {
        throw std::runtime_error("Invalid password");
    }

    return *optUser;
}
