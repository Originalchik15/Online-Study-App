#include "PgRequestRepository.hpp"
#include <stdexcept>
#include <sstream>

// --- Преобразователи enum ↔ строка ---
std::string PgRequestRepository::typeToString(Request::Type t) {
    switch (t) {
    case Request::Type::Create: return "Create";
    case Request::Type::Edit:   return "Edit";
    case Request::Type::Delete: return "Delete";
    }
    throw std::logic_error("Unknown Request::Type");
}

Request::Type PgRequestRepository::stringToType(const std::string& s) {
    if (s == "Create") return Request::Type::Create;
    if (s == "Edit")   return Request::Type::Edit;
    if (s == "Delete") return Request::Type::Delete;
    throw std::runtime_error("Invalid request type: " + s);
}

std::string PgRequestRepository::stateToString(Request::State s) {
    switch (s) {
    case Request::State::Pending:   return "Pending";
    case Request::State::Approved:  return "Approved";
    case Request::State::Rejected:  return "Rejected";
    }
    throw std::logic_error("Unknown Request::State");
}

Request::State PgRequestRepository::stringToState(const std::string& s) {
    if (s == "Pending")   return Request::State::Pending;
    if (s == "Approved")  return Request::State::Approved;
    if (s == "Rejected")  return Request::State::Rejected;
    throw std::runtime_error("Invalid request state: " + s);
}

// --- Конструктор / деструктор ---
PgRequestRepository::PgRequestRepository(const std::string& connInfo)
    : conn_(PQconnectdb(connInfo.c_str()))
{
    if (PQstatus(conn_) != CONNECTION_OK) {
        std::string err = PQerrorMessage(conn_);
        PQfinish(conn_);
        throw std::runtime_error("Postgres connect failed: " + err);
    }
    initSchema();
}

PgRequestRepository::~PgRequestRepository() {
    if (conn_) PQfinish(conn_);
}

// --- Создание схемы (если ещё нет) ---
void PgRequestRepository::initSchema() const {
    // 1) Тип ENUM для поля type
    exec(R"(
      DO $$ BEGIN
        IF NOT EXISTS (SELECT 1 FROM pg_type WHERE typname = 'request_type') THEN
          CREATE TYPE request_type AS ENUM ('Create','Edit','Delete');
        END IF;
      END $$;
    )");
    // 2) Таблица requests
    exec(R"(
      CREATE TABLE IF NOT EXISTS requests (
        id              SERIAL PRIMARY KEY,
        course_id       VARCHAR(64),
        type            request_type   NOT NULL,
        new_title       TEXT,
        new_description TEXT,
        requested_by    VARCHAR(64)    NOT NULL,
        status          VARCHAR(16)    NOT NULL DEFAULT 'Pending',
        created_at      TIMESTAMP      NOT NULL DEFAULT now()
      );
    )");
}

// --- Вспомогательные функции ---
void PgRequestRepository::exec(const std::string& sql) const {
    PGresult* res = PQexec(conn_, sql.c_str());
    auto st = PQresultStatus(res);
    if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("SQL error: " + err);
    }
    PQclear(res);
}

std::string PgRequestRepository::escape(const std::string& s) const {
    char* buf = PQescapeLiteral(conn_, s.c_str(), s.size());
    if (!buf) throw std::runtime_error("Escape failed");
    std::string out(buf);
    PQfreemem(buf);
    return out;  // уже обрамлено в одинарные кавычки
}

// --- CRUD для заявок ---
void PgRequestRepository::add(const Request& r) {
    std::ostringstream sql;
    sql << "INSERT INTO requests(course_id,type,new_title,new_description,"
        "requested_by,status,created_at) VALUES ("
        << (r.getCourseId().empty() ? "NULL" : escape(r.getCourseId())) << ", "
        << escape(typeToString(r.getType())) << ", "
        << escape(r.getNewTitle()) << ", "
        << escape(r.getNewDescription()) << ", "
        << escape(r.getRequestedBy()) << ", "
        << escape(stateToString(r.getStatus())) << ", "
        << "now()"
        << ")";
    exec(sql.str());
}

void PgRequestRepository::update(const Request& r) {
    std::ostringstream sql;
    sql << "UPDATE requests SET status="
        << escape(stateToString(r.getStatus()))
        << " WHERE id=" << r.getId();
    exec(sql.str());
}

std::vector<Request> PgRequestRepository::findAllPending() const {
    PGresult* res = PQexec(conn_,
        "SELECT id,course_id,type,new_title,new_description,"
        "requested_by,status,created_at "
        "FROM requests WHERE status='Pending' ORDER BY created_at");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Query failed: " + err);
    }

    std::vector<Request> list;
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        int           id = std::stoi(PQgetvalue(res, i, 0));
        std::string   courseId = PQgetvalue(res, i, 1);
        std::string   typeStr = PQgetvalue(res, i, 2);
        std::string   title = PQgetvalue(res, i, 3);
        std::string   desc = PQgetvalue(res, i, 4);
        std::string   by = PQgetvalue(res, i, 5);
        std::string   stateStr = PQgetvalue(res, i, 6);
        // created_at можем игнорировать или парсить при необходимости

        list.emplace_back(
            id,
            courseId,
            stringToType(typeStr),
            title,
            desc,
            by,
            stringToState(stateStr),
            std::chrono::system_clock::now()
        );
    }
    PQclear(res);
    return list;
}

Request PgRequestRepository::findById(int id) const {
    std::ostringstream q;
    q << "SELECT course_id,type,new_title,new_description,"
        "requested_by,status,created_at "
        "FROM requests WHERE id=" << id;
    PGresult* res = PQexec(conn_, q.str().c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn_);
        PQclear(res);
        throw std::runtime_error("Query failed: " + err);
    }
    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("Request not found: " + std::to_string(id));
    }

    std::string courseId = PQgetvalue(res, 0, 0);
    std::string typeStr = PQgetvalue(res, 0, 1);
    std::string title = PQgetvalue(res, 0, 2);
    std::string desc = PQgetvalue(res, 0, 3);
    std::string by = PQgetvalue(res, 0, 4);
    std::string stateStr = PQgetvalue(res, 0, 5);
    // created_at — не используем прямо

    PQclear(res);
    return Request(
        id,
        courseId,
        stringToType(typeStr),
        title,
        desc,
        by,
        stringToState(stateStr),
        std::chrono::system_clock::now()
    );
}
