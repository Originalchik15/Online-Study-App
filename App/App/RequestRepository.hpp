#pragma once
#include "Request.hpp"
#include <vector>

class RequestRepository {
public:
	virtual ~RequestRepository() = default;

	// CRUD-операции над заявками
	virtual void add(const Request& r) = 0;
	virtual void update(const Request& r) = 0;
	virtual std::vector<Request> findAllPending() const = 0;
	virtual Request findById(int id) const = 0;
};
