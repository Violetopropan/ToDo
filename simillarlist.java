import java.util.Scanner;
import java.util.UUID;
import java.time.ZoneOffset;
import java.time.ZonedDateTime;
import java.util.ArrayList;
import java.util.HashMap;

class User {
    private String username;
    private String password;

    public User(String username, String password) {
        this.username = username;
        this.password = password;
    }

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }
}

class Task {
    private String id;
    private String userId;
    private ZonedDateTime date;
    private String text;

    public Task(String userId, String text) {
        this.id = UUID.randomUUID().toString();
        this.userId = userId;
        this.date = ZonedDateTime.now(ZoneOffset.UTC);
        this.text = text;
    }

    public String getId() {
        return id;
    }

    public String getUserId() {
        return userId;
    }

    public ZonedDateTime getDate() {
        return date;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }
}

class Database {
    private HashMap<String, User> users;
    private ArrayList<Task> tasks;

    public Database() {
        this.users = new HashMap<>();
        this.tasks = new ArrayList<>();
    }

    public boolean registerUser(String username, String password) {
        if (users.containsKey(username)) {
            return false;
        } else {
            users.put(username, new User(username, password));
            return true;
        }
    }

    public User authenticateUser(String username, String password) {
        User user = users.get(username);
        if (user != null && user.getPassword().equals(password)) {
            return user;
        } else {
            return null;
        }
    }

    public void addTask(String userId, String text) {
        Task task = new Task(userId, text);
        tasks.add(task);
    }

    public boolean deleteTask(String taskId) {
        for (Task task : tasks) {
            if (task.getId().equals(taskId)) {
                tasks.remove(task);
                return true;
            }
        }
        return false;
    }

    public boolean markTaskDone(String taskId) {
        for (Task task : tasks) {
            if (task.getId().equals(taskId)) {
                task.setText(task.getText() + " +DONE+");
                return true;
            }
        }
        return false;
    }

    public boolean updateTask(String taskId, String newText) {
        for (Task task : tasks) {
            if (task.getId().equals(taskId)) {
                task.setText(newText);
                return true;
            }
        }
        return false;
    }

    public ArrayList<Task> getUserTasks(String userId) {
        ArrayList<Task> userTasks = new ArrayList<>();
        for (Task task : tasks) {
            if (task.getUserId().equals(userId)) {
                userTasks.add(task);
            }
        }
        return userTasks;
    }
}

public class TaskManager {
    private static Scanner scanner = new Scanner(System.in);
    private static Database database = new Database();
    private static User currentUser = null;

    public static void main(String[] args) {
        boolean running = true;
        while (running) {
            printMainMenu();
            String choice = scanner.nextLine();
            switch (choice) {
                case "1":
                    login();
                    break;
                case "2":
                    register();
                    break;
                case "3":
                    System.out.println("До свидания!");
                    running = false;
                    break;
                default:
                    System.out.println("Некорректный выбор. Пожалуйста, выберите снова.");
            }
        }
    }

    private static void printMainMenu() {
        System.out.println("\nГлавное меню:");
        System.out.println("1. Войти");
        System.out.println("2. Зарегистрироваться");
        System.out.println("3. Выйти");
        System.out.print("Выберите действие: ");
    }

    private static void login() {
        System.out.print("Введите имя пользователя: ");
        String username = scanner.nextLine();
        System.out.print("Введите пароль: ");
        String password = scanner.nextLine();
        currentUser = database.authenticateUser(username, password);
        if (currentUser != null) {
            System.out.println("Аутентификация прошла успешно!");
            showUserMenu();
        } else {
            System.out.println("Неверное имя пользователя или пароль.");
        }
    }

    private static void register() {
        System.out.print("Введите имя пользователя: ");
        String username = scanner.nextLine();
        System.out.print("Введите пароль: ");
        String password = scanner.nextLine();
        if (database.registerUser(username, password)) {
            System.out.println("Пользователь успешно зарегистрирован!");
        } else {
            System.out.println("Пользователь с таким именем уже существует.");
        }
    }

    private static void showUserMenu() {
        boolean loggedIn = true;
        while (loggedIn) {
            printUserMenu();
            String choice = scanner.nextLine();
            switch (choice) {
                case "1":
                    addTask();
                    break;
                case "2":
                    markTaskDone();
                    break;
                case "3":
                    updateTask();
                    break;
                case "4":
                    deleteTask();
                    break;
                case "5":
                    showTasks();
                    break;
                case "6":
                    currentUser = null;
                    System.out.println("Выход из аккаунта...");
                    loggedIn = false;
                    break;
                case "7":
                    System.out.println("Закрытие приложения...");
                    System.exit(0);
                default:
                    System.out.println("Некорректный выбор. Пожалуйста, выберите снова.");
            }
        }
    }

    private static void printUserMenu() {
        System.out.println("\nОсновное меню:");
        System.out.println("1. Добавить дело");
        System.out.println("2. Пометить дело как выполненное");
        System.out.println("3. Изменить текст задачи");
        System.out.println("4. Удалить задачу");
        System.out.println("5. Просмотреть дела");
        System.out.println("6. Выйти из аккаунта");
        System.out.println("7. Закрыть приложение");
        System.out.print("Выберите действие: ");
    }

    private static void addTask() {
        System.out.print("Введите новое дело, которое предстоит выполнить: ");
        String text = scanner.nextLine();
        database.addTask(currentUser.getUsername(), text);
        System.out.println("Дело успешно добавлено в ежедневник!");
    }

    private static void markTaskDone() {
        System.out.print("Введите ID выполненного дела: ");
        String taskId = scanner.nextLine();
        if (database.markTaskDone(taskId)) {
            System.out.println("Дело успешно отмечено как выполненное!");
        } else {
            System.out.println("Некорректный ID задачи. Пожалуйста, введите корректный.");
        }
    }

    private static void updateTask() {
        System.out.print("Введите ID задачи, которую хотите изменить: ");
        String taskId = scanner.nextLine();
        System.out.print("Введите новый текст задачи: ");
        String newText = scanner.nextLine();
        if (database.updateTask(taskId, newText)) {
            System.out.println("Текст задачи успешно обновлён!");
        } else {
            System.out.println("Некорректный ID задачи. Пожалуйста, введите корректный.");
        }
    }

    private static void deleteTask() {
        System.out.print("Введите ID задачи, которую хотите удалить: ");
        String taskId = scanner.nextLine();
        if (database.deleteTask(taskId)) {
            System.out.println("Дело успешно удалено из ежедневника!");
        } else {
            System.out.println("Некорректный ID задачи. Пожалуйста, введите корректный.");
        }
    }

    private static void showTasks() {
        ArrayList<Task> userTasks = database.getUserTasks(currentUser.getUsername());
        if (!userTasks.isEmpty()) {
            System.out.println("\nВаши дела:");
            for (Task task : userTasks) {
                System.out.println("ID: " + task.getId() + ", Дата: " + task.getDate() + ", Дело: " + task.getText());
            }
        } else {
            System.out.println("У вас нет ни одного дела.");
        }
    }
}
