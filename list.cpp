#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>
#include <windows.h>
#include <conio.h>
#include <limits>
#include <regex>
#include <string>

class User {
private:
    std::string username;
    std::string password;

public:
    User(std::string username, std::string password) : username(username), password(password) {}

    std::string getUsername() const { return username; }

    std::string getPassword() const { return password; }
};

class Task {
private:
    std::string id;
    std::string userId;
    std::string date;
    std::string text;

public:
    Task(std::string userId, std::string text) : userId(userId), text(text) {
        id = generate_uuid();
        std::time_t t = std::time(nullptr);    
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S");
        date = oss.str();
    }

    std::string getId() const { return id; }

    std::string getUserId() const { return userId; }

    std::string getDate() const { return date; }

    std::string getText() const { return text; }

    void setText(std::string newText) { text = newText; }

private:
    std::string generate_uuid() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        const char* c = "0123456789abcdef";
        std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
        for (auto& r : uuid) {
            if (r == 'x') {
                r = c[dis(gen)];
            } else if (r == 'y') {
                r = c[(dis(gen) & 0x3) | 0x8];
            }
        }
        return uuid;
    }
};

class Database {
private:
    std::unordered_map<std::string, User> users;
    std::vector<Task> tasks;

public:
    bool registerUser(std::string username, std::string password) {
        if (users.find(username) != users.end()) {
            return false;
        } else {
            users.emplace(username, User(username, password));
            return true;
        }
    }

    User* authenticateUser(std::string username, std::string password) {
        auto it = users.find(username);
        if (it != users.end() && it->second.getPassword() == password) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    void addTask(std::string userId, std::string text) {
        tasks.emplace_back(userId, text);
    }

    bool deleteTask(std::string taskId) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if (it->getId() == taskId) {
                tasks.erase(it);
                return true;
            }
        }
        return false;
    }

    bool markTaskDone(std::string taskId) {
        for (auto& task : tasks) {
            if (task.getId() == taskId) {
                task.setText(task.getText() + " +DONE+");
                return true;
            }
        }
        return false;
    }

    bool updateTask(std::string taskId, std::string newText) {
        for (auto& task : tasks) {
            if (task.getId() == taskId) {
                task.setText(newText);
                return true;
            }
        }
        return false;
    }

    std::vector<Task> getUserTasks(std::string userId) {
        std::vector<Task> userTasks;
        for (const auto& task : tasks) {
            if (task.getUserId() == userId) {
                userTasks.push_back(task);
            }
        }
        return userTasks;
    }
};

bool isValidUUID(const std::string& uuid) {
    std::regex uuidRegex("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$");
    return std::regex_match(uuid, uuidRegex);
}

void printMainMenu() {
    std::cout << "\nГлавное меню:" << std::endl;
    std::cout << "1. Войти" << std::endl;
    std::cout << "2. Зарегистрироваться" << std::endl;
    std::cout << "3. Выйти" << std::endl;
    std::cout << "Выберите действие: ";
}

void printUserMenu() {
    std::cout << "\nОсновное меню:" << std::endl;
    std::cout << "1. Добавить дело" << std::endl;
    std::cout << "2. Пометить дело как выполненное" << std::endl;
    std::cout << "3. Изменить текст задачи" << std::endl;
    std::cout << "4. Удалить задачу" << std::endl;
    std::cout << "5. Просмотреть дела" << std::endl;
    std::cout << "6. Выйти из аккаунта" << std::endl;
    std::cout << "7. Закрыть приложение" << std::endl;
    std::cout << "Выберите действие: ";
}

void showUserMenu(Database& database, User& user);

void login(Database& database) {
    std::string username, password;
    std::cout << "Введите имя пользователя: ";
    std::cin >> username;
    std::cout << "Введите пароль: ";
    char ch;
    password = "";
    while ((ch = _getch()) != '\r') { // Считываем символы без отображения
        if (ch == '\b') { // Если нажата клавиша Backspace
            if (!password.empty()) {
                std::cout << "\b \b"; // Стираем символ с консоли
                password.pop_back(); // Удаляем последний символ из строки пароля
            }
        } else {
            std::cout << '*'; // Выводим символ * вместо введенного символа
            password += ch; // Добавляем символ к паролю
        }
    }
    std::cout << std::endl;
    User* user = database.authenticateUser(username, password);
    if (user != nullptr) {
        std::cout << "Аутентификация прошла успешно!" << std::endl;
        showUserMenu(database, *user);
    } else {
        std::cout << "Неверное имя пользователя или пароль." << std::endl;
    }
}

void registerUser(Database& database) {
    std::string username, password;
    std::cout << "Введите имя пользователя: ";
    std::cin >> username;
    std::cout << "Введите пароль: ";
    char ch;
    password = "";
    while ((ch = _getch()) != '\r') { // Считываем символы без отображения
        if (ch == '\b') { // Если нажата клавиша Backspace
            if (!password.empty()) {
                std::cout << "\b \b"; // Стираем символ с консоли
                password.pop_back(); // Удаляем последний символ из строки пароля
            }
        } else {
            std::cout << '*'; // Выводим символ * вместо введенного символа
            password += ch; // Добавляем символ к паролю
        }
    }
    std::cout << std::endl;
    if (database.registerUser(username, password)) {
        std::cout << "Пользователь успешно зарегистрирован!" << std::endl;
    } else {
        std::cout << "Пользователь с таким именем уже существует." << std::endl;
    }
}

void addTask(Database& database, User& user) {
    std::string text;
    std::cout << "Введите новое дело, которое предстоит выполнить: ";
    std::cin.ignore();
    std::getline(std::cin, text);
    database.addTask(user.getUsername(), text);
    std::cout << "Дело успешно добавлено в ежедневник!" << std::endl;
}

void markTaskDone(Database& database) {
    std::string taskId;
    std::cout << "Введите ID выполненного дела: ";
    std::cin >> taskId;

    if (!isValidUUID(taskId)) {
        std::cout << "Некорректный формат UUID. Пожалуйста, введите корректный UUID." << std::endl;
        return;
    }

    if (database.markTaskDone(taskId)) {
        std::cout << "Дело успешно отмечено как выполненное!" << std::endl;
    } else {
        std::cout << "Не удалось найти задачу с таким ID. Пожалуйста, введите корректный ID." << std::endl;
    }
}

void updateTask(Database& database) {
    std::string taskId, newText;
    std::cout << "Введите ID задачи, которую хотите изменить: ";
    std::cin >> taskId;
    std::cout << "Введите новый текст задачи: ";
    std::cin.ignore();
    std::getline(std::cin, newText);

    if (!isValidUUID(taskId)) {
        std::cout << "Некорректный формат UUID. Пожалуйста, введите корректный UUID." << std::endl;
        return;
    }

    if (database.updateTask(taskId, newText)) {
        std::cout << "Текст задачи успешно обновлён!" << std::endl;
    } else {
        std::cout << "Не удалось найти задачу с таким ID. Пожалуйста, введите корректный ID." << std::endl;
    }
}

void deleteTask(Database& database) {
    std::string taskId;
    std::cout << "Введите ID задачи, которую хотите удалить: ";
    std::cin >> taskId;

    if (!isValidUUID(taskId)) {
        std::cout << "Некорректный формат UUID. Пожалуйста, введите корректный UUID." << std::endl;
        return;
    }

    if (database.deleteTask(taskId)) {
        std::cout << "Дело успешно удалено из ежедневника!" << std::endl;
    } else {
        std::cout << "Не удалось найти задачу с таким ID. Пожалуйста, введите корректный ID." << std::endl;
    }
}

void showTasks(Database& database, User& user) {
    std::vector<Task> userTasks = database.getUserTasks(user.getUsername());
    if (!userTasks.empty()) {
        std::cout << "\nВаши дела:" << std::endl;
        for (const auto& task : userTasks) {
            std::cout << "ID: " << task.getId() << ", Дата: " << task.getDate() << ", Дело: " << task.getText() << std::endl;
        }
    } else {
        std::cout << "У вас нет ни одного дела." << std::endl;
    }
}

int getValidInput(int min, int max) {
    int choice;
    while (true) {
        if (std::cin >> choice) {
            if (choice >= min && choice <= max) {
                return choice;  // Возврат корректного значения
            } else {
                std::cout << "Введённое число вне допустимого диапазона. Попробуйте снова.\n";
            }
        } else {
            std::cout << "Некорректный ввод. Пожалуйста, введите число.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

void showUserMenu(Database& database, User& user) {
    bool loggedIn = true;
    while (loggedIn) {
        printUserMenu();
        int choice = getValidInput(1, 7);
        switch (choice) {
            case 1:
                addTask(database, user);
                break;
            case 2:
                markTaskDone(database);
                break;
            case 3:
                updateTask(database);
                break;
            case 4:
                deleteTask(database);
                break;
            case 5:
                showTasks(database, user);
                break;
            case 6:
                std::cout << "Выход из аккаунта..." << std::endl;
                loggedIn = false;
                break;
            case 7:
                std::cout << "Закрытие приложения..." << std::endl;
                exit(0);
            default:
                std::cout << "Некорректный выбор. Пожалуйста, выберите снова." << std::endl;
        }
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    Database database;
    bool running = true;
    while (running) {
        printMainMenu();
        int choice = getValidInput(1, 3);
        switch (choice) {
            case 1:
                login(database);
                break;
            case 2:
                registerUser(database);
                break;
            case 3:
                std::cout << "До свидания!" << std::endl;
                running = false;
                break;
            default:
                std::cout << "Некорректный выбор. Пожалуйста, выберите снова." << std::endl;
        }
    }
    return 0;
}   
