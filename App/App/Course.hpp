#pragma once

#include <string>
#include <vector>

using std::string;
// Класс Course: модель курса с жизненным циклом и материалами
class Course {
public:
    enum class Status { Draft, UnderReview, Published, Archived, Deleted };

    // Конструктор
    Course(const std::string& id,
        const std::string& title,
        const std::string& description);

    // Геттеры
    const std::string& getId() const;
    const std::string& getTitle() const;
    const std::string& getDescription() const;
    Status getStatus() const;
    const std::vector<std::string>& getMaterials() const;

    // Модификация данных
    void setTitle(const std::string& newTitle);
    void setDescription(const std::string& newDescription);
    void addMaterial(const std::string& materialPath);

    // Методы перехода статусов
    void submitForReview();
    void approve();
    void reject();
    void archive();
    void restore();
    void remove();

private:
    std::string id_;
    std::string title_;
    std::string description_;
    std::vector<std::string> materials_;
    Status status_;
};
