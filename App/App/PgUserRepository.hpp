// PgUserRepository.hpp
#pragma once
#include "UserRepository.hpp"
#include <libpq-fe.h>
#include <string>
#include <optional>

class PgUserRepository : public UserRepository {
public:
    explicit PgUserRepository(const std::string& connInfo);
    ~PgUserRepository() override;

    void add(const User& user, const std::string& passwordHash) override;
    std::optional<User> findByUsername(const std::string& username) const override;
    std::string getPasswordHash(const std::string& username) const override;  // ← и здесь

private:
    PGconn* conn_;
    void exec(const std::string& sql) const;
    std::string escape(const std::string& s) const;
};
