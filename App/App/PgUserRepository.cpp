// PgUserRepository.cpp
#include "PgUserRepository.hpp"
#include <stdexcept>
#include <sstream>

PgUserRepository::PgUserRepository(const std::string& connInfo)
    : conn_(PQconnectdb(connInfo.c_str())) {
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::string err = PQerrorMessage(conn_);
        PQfinish(conn_);
        throw std::runtime_error("Postgres connect failed: " + err);
    }
}

PgUserRepository::~PgUserRepository() {
    if (conn_) PQfinish(conn_);
}

void PgUserRepository::exec(const std::string& sql) const {
    PGresult* res = PQexec(conn_, sql.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("SQL error: " + err);
    }
    PQclear(res);
}

std::string PgUserRepository::escape(const std::string& s) const {
    char* buf = PQescapeLiteral(conn_, s.c_str(), s.size());
    std::string out(buf);
    PQfreemem(buf);
    return out;
}

void PgUserRepository::add(const User& user, const std::string& passwordHash) {
    std::ostringstream sql;
    sql << "INSERT INTO users(id,username,password_hash,role) VALUES ("
        << escape(user.getId()) << ","
        << escape(user.getName()) << ","
        << escape(passwordHash) << ","
        << escape(User::roleToString(user.getRole()))
        << ")";
    exec(sql.str());
}

std::optional<User> PgUserRepository::findByUsername(const std::string& username) const {
    auto q = "SELECT name, role FROM users WHERE username=" + escape(username);
    PGresult* res = PQexec(conn_, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return std::nullopt;
    }
    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }
    std::string name = PQgetvalue(res, 0, 0);
    std::string role = PQgetvalue(res, 0, 1);
    PQclear(res);
    return User(username, name, User::stringToRole(role));
}

std::string PgUserRepository::getPasswordHash(const std::string& username) const {
    auto q = "SELECT password_hash FROM users WHERE username=" + escape(username);
    PGresult* res = PQexec(conn_, q.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Cannot get password hash: " + err);
    }
    std::string hash = PQgetvalue(res, 0, 0);
    PQclear(res);
    return hash;
}
