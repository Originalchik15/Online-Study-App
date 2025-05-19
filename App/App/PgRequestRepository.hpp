#pragma once

#include "RequestRepository.hpp"
#include "Request.hpp"
#include <libpq-fe.h>
#include <string>
#include <vector>

class PgRequestRepository : public RequestRepository {
public:
    // connInfo — строка подключения к Postgres, например:
    // "host=localhost port=5432 dbname=cources user=postgres password=..."
    explicit PgRequestRepository(const std::string& connInfo);
    ~PgRequestRepository() override;

    void add(const Request& r) override;
    void update(const Request& r) override;
    std::vector<Request> findAllPending() const override;
    Request findById(int id) const override;

private:
    PGconn* conn_;

    void initSchema() const;
    void exec(const std::string& sql) const;
    std::string escape(const std::string& s) const;

    // Преобразование enum ↔ строка
    static std::string typeToString(Request::Type t);
    static Request::Type stringToType(const std::string& s);
    static std::string stateToString(Request::State s);
    static Request::State stringToState(const std::string& s);
};
