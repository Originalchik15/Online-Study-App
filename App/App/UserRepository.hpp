// UserRepository.hpp
#pragma once
#include <optional>
#include <string>
#include "User.hpp"

class UserRepository {
public:
    virtual ~UserRepository() = default;

    virtual void add(const User& user, const std::string& passwordHash) = 0;
    virtual std::optional<User> findByUsername(const std::string& username) const = 0;

    // ← Добавляем вот это:
    virtual std::string getPasswordHash(const std::string& username) const = 0;
};
