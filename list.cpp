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
    User() : username(""), password("") {}
    User(std::string username, std::string password) : username(username), password(password) {}

    const std::string& getUsername() const { return username; }

    std::string getPassword() const { return password; }
};

class Task {
private:
    std::string id;
    std::string userId;
    std::string date;
    std::string text;
    bool isDone = false;

public:
    Task(const std::string& userId, const std::string& text) 
        : userId(userId), text(text) {
        id = generate_uuid();
        std::time_t t = std::time(nullptr);
        std::ostringstream oss;
        oss << std::put_time(std::gmtime(&t), "%Y-%m-%d %H:%M:%S");
        date = oss.str();
    }

    Task(const std::string& userId, const std::string& text, const std::string& date) 
        : userId(userId), text(text), date(date) {
        id = generate_uuid();
    }

    std::string getId() const { return id; }
    std::string getUserId() const { return userId; }
    std::string getDate() const { return date; }
    std::string getText() const { return text; }
    bool getIsDone() const { return isDone; }

    void setText(const std::string& newText) { text = newText; }
    void setDone(bool done) { isDone = done; }

private:
    std::string generate_uuid() {
   
        std::ostringstream ss;
        for (int i = 0; i < 32; i++) {
            const char DIGITS[] = "0123456789abcdef";
            ss << DIGITS[rand() % 16];
        }
        return ss.str();
    }
};

class Database {
private:
    std::unordered_map<std::string, User> users;
    std::vector<Task> tasks;

public:
    std::vector<Task> getUserTasks(const std::string& username) {
        std::vector<Task> userTasks;
        for (const Task& task : tasks) {
            if (task.getUserId() == username) {
                userTasks.push_back(task);
            }
        }
        return userTasks;
    }

    bool registerUser(const std::string& username, const std::string& password) {
        if (users.find(username) != users.end()) {
            return false;
        } else {
            users.emplace(username, User(username, password));
            return true;
        }
    }

    User* authenticateUser(const std::string& username, const std::string& password) {
        auto it = users.find(username);
        if (it != users.end() && it->second.getPassword() == password) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    void addTask(const std::string& userId, const std::string& text) {
        static int nextTaskId = 0; 
        tasks.emplace_back(std::to_string(nextTaskId++), userId, text);
    }

    bool deleteTask(const std::string& userId, const std::string& taskId) {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Task& task) {
            return task.getId() == taskId && task.getUserId() == userId;
        });
        if (it != tasks.end()) {
            tasks.erase(it);
            return true;
        }
        return false;
    }

    bool markTaskDone(const std::string& userId, const std::string& taskId) {
        auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Task& task) {
            return task.getId() == taskId && task.getUserId() == userId;
        });
        if (it != tasks.end()) {
            it->setDone(true); 
            return true;
        }
        return false;
    }

    bool updateTask(const std::string& userId, const std::string& taskId, const std::string& newText) {
        for (Task& task : tasks) {
            if (task.getId() == taskId && task.getUserId() == userId) {
                task.setText(newText);
                return true;
            }
        }
        return false;
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
    while ((ch = _getch()) != '\r') { 
        if (ch == '\b') { 
            if (!password.empty()) {
                std::cout << "\b \b"; 
                password.pop_back(); 
            }
        } else {
            std::cout << '*'; 
            password += ch; 
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
    while ((ch = _getch()) != '\r') { 
        if (ch == '\b') { 
            if (!password.empty()) {
                std::cout << "\b \b"; 
                password.pop_back(); 
            }
        } else {
            std::cout << '*'; 
            password += ch; 
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

void markTaskDone(Database& database, User& user) {
    std::string taskId;
    std::cout << "Введите ID выполненного дела: ";
    std::cin >> taskId;

    if (!isValidUUID(taskId)) {
        std::cout << "Некорректный формат UUID. Пожалуйста, введите корректный UUID." << std::endl;
        return;
    }

    if (database.markTaskDone(user.getUsername(), taskId)) {
        std::cout << "Дело успешно отмечено как выполненное!" << std::endl;
    } else {
        std::cout << "Не удалось найти задачу с таким ID или она не принадлежит вам. Пожалуйста, введите корректный ID." << std::endl;
    }
}

void updateTask(Database& database, User& user) {
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

    if (database.updateTask(user.getUsername(), taskId, newText)) {
        std::cout << "Текст задачи успешно обновлён!" << std::endl;
    } else {
        std::cout << "Не удалось найти задачу с таким ID или она не принадлежит вам. Пожалуйста, введите корректный ID." << std::endl;
    }
}

void deleteTask(Database& database, const User& user) {
    std::string taskId;
    while (true) {
        std::cout << "Введите ID задачи, которую хотите удалить: ";
        std::cin >> taskId;
        if (!isValidUUID(taskId)) {
            std::cout << "Некорректный формат UUID. Пожалуйста, введите корректный UUID или введите 'exit' для отмены." << std::endl;
            if (taskId == "exit") return;
            continue;
        }
        try {
            if (database.deleteTask(user.getUsername(), taskId)) {
                std::cout << "Задача успешно удалена!" << std::endl;
                break;
            } else {
                std::cout << "Не удалось найти задачу с таким ID. Пожалуйста, введите корректный ID или 'exit' для отмены." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << std::endl;
            break;
        }
    }
}

void showTasks(Database& database, const User& user) {
    std::vector<Task> userTasks = database.getUserTasks(user.getUsername());
    if (!userTasks.empty()) {
        std::cout << "\nВаши дела:\n";
        std::cout << "ID\t\tДата\t\tДело\n";
        std::cout << "-----------------------------------------------\n";
        for (const auto& task : userTasks) {
            std::cout << task.getId() << "\t" << task.getDate() << "\t" << task.getText() << std::endl;
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
                return choice;  
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
        std::string taskId;
        switch (choice) {
            case 1:
                addTask(database, user);
                break;
            case 2:
                std::cout << "Введите ID задачи для пометки как выполненной: ";
                std::cin >> taskId;
                markTaskDone(database, user); 
                break;
            case 3:
                std::cout << "Введите ID задачи для обновления: ";
                std::cin >> taskId;
                updateTask(database, user);  
                break;
            case 4:
                deleteTask(database, user);  
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
    Database database; 
    User user;    
      
    std::cout << "Добро пожаловать в систему управления задачами!" << std::endl;

    showUserMenu(database, user);

    return 0;
}
